# Code Expédition — mainwindow.cpp

## Flux complet

```
Keypad saisit ID + D
    ├─ ID introuvable       → '2' → Arduino "ID invalide / Ressaisir + D"
    ├─ Déjà En livraison    → '2' → Arduino "Deja expedie"
    └─ ID trouvé            → UPDATE STATUT = 'Terminé'
                            → '3' → Arduino "ID OK - Cliquer Expedier"
                            → ligne sélectionnée dans tableau
                            → notification Qt

Clic bouton Expédier
    ├─ Arduino non connecté → tentative reconnexion automatique
    ├─ Aucune ligne sélectionnée → warning
    ├─ statut ≠ "Terminé"   → message d'erreur, moteurs NE bougent PAS
    └─ statut == "Terminé"  → '1' → moteurs se déclenchent
                            → UPDATE STATUT = 'En livraison' + DATE_LIVRAISON
                            → loadProductionData()
                            → QMessageBox confirmation
```

---

## 1. Connexion du bouton + style (dans setupConnections)

```cpp
connect(ui->btnExpedier, &QPushButton::clicked, this, &MainWindow::expedierActionArduino);
ui->btnExpedier->setStyleSheet(
    "QPushButton { background-color:#8D6E63; color:white; border:none; border-radius:10px;"
    " padding:10px 20px; font-family:Arial,sans-serif; font-size:12px; font-weight:bold; }"
    "QPushButton:hover { background-color:#A0826D; }"
    "QPushButton:pressed { background-color:#6E473B; padding:11px 20px 9px 20px; }");
```

---

## 2. setupArduinoPointage()

Initialise la connexion Arduino au démarrage, crée l'indicateur de connexion et le simulateur keypad.

```cpp
void MainWindow::setupArduinoPointage()
{
    // Indicateur connexion — overlay coin supérieur droit, visible sur page Production
    m_arduinoIndicator = new QLabel(this);
    m_arduinoIndicator->setFixedSize(160, 24);
    m_arduinoIndicator->setAlignment(Qt::AlignCenter);
    m_arduinoIndicator->setStyleSheet(
        "QLabel { border-radius:12px; font-size:11px; font-weight:bold; color:white; }");
    m_arduinoIndicator->setVisible(false);
    m_arduinoIndicator->raise();

    // Utilise ArduinoConnection pour détecter automatiquement le port (VID/PID)
    int result = m_arduino.connection()->connect_arduino();

    if (result == 0) {
        m_serialArduino = m_arduino.connection()->getSerial();
        connect(m_serialArduino, &QSerialPort::readyRead,
                this, &MainWindow::recevoir_donnee);
        m_arduinoIndicator->setText("● Arduino connecté");
        m_arduinoIndicator->setStyleSheet(
            "QLabel { border-radius:12px; font-size:11px; font-weight:bold; "
            "color:white; background:#27AE60; }");
        qDebug() << "✅ Arduino connecté sur" << m_arduino.connection()->getPortName();
    } else {
        m_serialArduino = nullptr;
        m_arduinoIndicator->setText("● Non connecté");
        m_arduinoIndicator->setStyleSheet(
            "QLabel { border-radius:12px; font-size:11px; font-weight:bold; "
            "color:white; background:#E74C3C; }");
        qDebug() << "⚠️ Arduino non connecté (code:" << result << ")";
    }

    // Timer fin de journée → marquer absents à 23h59
    m_timerAbsences = new QTimer(this);
    connect(m_timerAbsences, &QTimer::timeout, [this]() {
        QTime now = QTime::currentTime();
        if (now.hour() == 23 && now.minute() == 59)
            m_pointage.marquerAbsentsJournee();
    });
    m_timerAbsences->start(60000);

    // Simulateur keypad Qt (inséré dans la page Production)
    setupKeypadSimulator();
}
```

---

## 3. recevoir_donnee()

Lit les données du port série ligne par ligne et appelle `traiterMessageArduino()`.

```cpp
void MainWindow::recevoir_donnee()
{
    if (!m_serialArduino) return;
    static QByteArray buffer;
    buffer += m_arduino.read_from_arduino();

    // Traiter chaque ligne complète (terminée par \n)
    while (buffer.contains('\n')) {
        int idx = buffer.indexOf('\n');
        QString msg = QString::fromUtf8(buffer.left(idx)).trimmed();
        buffer.remove(0, idx + 1);
        if (!msg.isEmpty())
            traiterMessageArduino(msg);
    }
}
```

---

## 4. traiterMessageArduino()

Traite tous les messages reçus de l'Arduino (keypad physique et simulateur Qt).

```cpp
void MainWindow::traiterMessageArduino(const QString &msg)
{
    qDebug() << "Arduino msg:" << msg;

    // ── INPUT:xxx — saisie en cours, miroir LCD ──────────────────────────
    if (msg.startsWith("INPUT:")) {
        QString saisi = msg.mid(6);
        m_keypadBuffer = saisi;
        if (m_lcdLigne2) m_lcdLigne2->setText("Saisie: " + saisi);
        return;
    }

    // ── CLEAR — réinitialiser LCD ────────────────────────────────────────
    if (msg == "CLEAR") {
        m_keypadBuffer.clear();
        if (m_lcdLigne1) m_lcdLigne1->setText("Systeme pret");
        if (m_lcdLigne2) m_lcdLigne2->setText("Saisir ID + #");
        return;
    }

    // ── '1' — confirmation servos (ignorer) ─────────────────────────────
    if (msg == "1") return;

    // ── ID:xxx — marquer Terminé en BDD, attendre clic Expédier pour les moteurs
    if (msg.startsWith("ID:")) {
        QString id = msg.mid(3).trimmed();

        QSqlQuery q(Connection::instance()->getDatabase());
        q.prepare("SELECT STATUT FROM COMMANDES "
                  "WHERE REFERENCE = :id OR TO_CHAR(ID_COMMANDE) = :id2");
        q.bindValue(":id",  id);
        q.bindValue(":id2", id);

        bool found = q.exec() && q.next();
        QString statutActuel = found ? q.value("STATUT").toString() : "";
        qDebug() << "[Arduino] Statut BDD pour" << id << ":" << statutActuel;

        if (!found) {
            // ID introuvable → répondre immédiatement
            if (m_serialArduino && m_serialArduino->isOpen())
                m_arduino.write_to_arduino("2");
            if (m_lcdLigne1) m_lcdLigne1->setText("ID invalide");
            if (m_lcdLigne2) m_lcdLigne2->setText("Ressaisir + D");
            return;
        }

        // ID trouvé → UPDATE STATUT = 'Terminé' (sauf si déjà En livraison/Annulé)
        if (statutActuel.contains("livraison", Qt::CaseInsensitive) ||
            statutActuel.contains("Annul",     Qt::CaseInsensitive)) {
            qDebug() << "[Arduino] Statut" << statutActuel << "— déjà expédié";
            if (m_serialArduino && m_serialArduino->isOpen())
                m_arduino.write_to_arduino("2");
            if (m_lcdLigne1) m_lcdLigne1->setText("Deja expedie");
            if (m_lcdLigne2) m_lcdLigne2->setText(id);
            return;
        }

        QSqlQuery upd(Connection::instance()->getDatabase());
        upd.prepare("UPDATE COMMANDES SET STATUT = 'Termin\u00e9' "
                    "WHERE REFERENCE = :id OR TO_CHAR(ID_COMMANDE) = :id2");
        upd.bindValue(":id",  id);
        upd.bindValue(":id2", id);
        if (!upd.exec())
            qDebug() << "[Arduino] UPDATE échoué:" << upd.lastError().text();
        else
            qDebug() << "[Arduino] UPDATE OK — lignes:" << upd.numRowsAffected();

        // Si port fermé → tenter reconnexion avant d'envoyer '3'
        if (!m_serialArduino || !m_serialArduino->isOpen()) {
            qDebug() << "[Arduino] Port fermé — tentative reconnexion...";
            int ret = m_arduino.connection()->connect_arduino();
            if (ret == 0) {
                m_serialArduino = m_arduino.connection()->getSerial();
                connect(m_serialArduino, &QSerialPort::readyRead,
                        this, &MainWindow::recevoir_donnee);
                m_arduinoIndicator->setText("● Arduino connecté");
                m_arduinoIndicator->setStyleSheet(
                    "QLabel { border-radius:12px; font-size:11px; font-weight:bold; "
                    "color:white; background:#27AE60; }");
                qDebug() << "[Arduino] Reconnecté sur" << m_arduino.connection()->getPortName();
            }
        }

        // Envoyer '3' → Arduino "ID OK - Cliquer Expedier"
        if (m_serialArduino && m_serialArduino->isOpen()) {
            m_arduino.write_to_arduino("3");
            qDebug() << "[Arduino] '3' envoyé pour" << id;
        } else {
            qDebug() << "[Arduino] ⚠️ Port toujours fermé — '3' non envoyé !";
        }

        // Mettre à jour miroir LCD Qt
        if (m_lcdLigne1) m_lcdLigne1->setText("ID OK");
        if (m_lcdLigne2) m_lcdLigne2->setText("Cliquer Expedier");

        loadProductionData();
        m_keypadBuffer.clear();

        // Sélectionner automatiquement la ligne dans le tableau
        for (int r = 0; r < ui->productionTable->rowCount(); ++r) {
            if (cellText(ui->productionTable, r, 1) == id ||
                cellText(ui->productionTable, r, 0) == id) {
                ui->productionTable->selectRow(r);
                break;
            }
        }

        NotificationWidget::show(
            "✅ Commande prête",
            "Référence " + id + " — statut Terminé. Cliquez Expédier pour lancer les moteurs.",
            NotificationWidget::Success
        );
        return;
    }
}
```

---

## 5. expedierActionArduino()

Déclenché par le clic sur le bouton "🚚 Expédier".

```cpp
void MainWindow::expedierActionArduino()
{
    // CAS 1 : Arduino non connecté → tentative reconnexion
    if (!m_serialArduino || !m_serialArduino->isOpen()) {
        int ret = m_arduino.connection()->connect_arduino();
        if (ret == 0) {
            m_serialArduino = m_arduino.connection()->getSerial();
            connect(m_serialArduino, &QSerialPort::readyRead,
                    this, &MainWindow::recevoir_donnee);
            m_arduinoIndicator->setText("● Arduino connecté");
            m_arduinoIndicator->setStyleSheet(
                "QLabel { border-radius:12px; font-size:11px; font-weight:bold; "
                "color:white; background:#27AE60; }");
        } else {
            QMessageBox::warning(this, "Expédition impossible",
                "Impossible d'expédier. Vérifiez la connexion série.");
            return;
        }
    }

    // CAS 2 : aucune ligne sélectionnée
    int row = ui->productionTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Attention", "Veuillez sélectionner une commande.");
        return;
    }

    QString ref    = cellText(ui->productionTable, row, 1);
    QString statut = cellText(ui->productionTable, row, 8);
    QString mail   = cellText(ui->productionTable, row, 10);

    qDebug() << "[Expedier] ref=" << ref << "statut=" << statut
             << "statut.size=" << statut.size()
             << "hex=" << statut.toUtf8().toHex();

    // CAS 3 : statut != Terminé
    bool estTermine = (statut.trimmed().normalized(QString::NormalizationForm_C)
                       == QString("Termin\u00e9").normalized(QString::NormalizationForm_C));

    if (!estTermine) {
        QString msgErr;
        if (statut.contains("livraison", Qt::CaseInsensitive))
            msgErr = QString("Référence : %1\nCette commande est déjà en livraison.").arg(ref);
        else
            msgErr = QString("Référence     : %1\nStatut actuel : %2\n"
                             "La commande doit être Terminé pour être expédiée.").arg(ref, statut);
        QMessageBox::critical(this, "Expédition impossible", msgErr);
        return;
    }

    // CAS 4 : statut == Terminé → déclencher les moteurs puis mettre En livraison
    qDebug() << "[Expedier] Envoi '1' → moteurs pour" << ref;
    m_arduino.write_to_arduino("1");  // moteurs se déclenchent sur l'Arduino
    qDebug() << "[Expedier] '1' envoyé";

    QString dateAujourdhui = QDate::currentDate().toString("dd/MM/yyyy");
    QSqlQuery upd(Connection::instance()->getDatabase());
    upd.prepare("UPDATE COMMANDES SET STATUT = 'En livraison', "
                "DATE_LIVRAISON = TO_DATE(:dl, 'DD/MM/YYYY') "
                "WHERE REFERENCE = :ref");
    upd.bindValue(":dl",  dateAujourdhui);
    upd.bindValue(":ref", ref);
    if (!upd.exec())
        qDebug() << "Erreur UPDATE expédition:" << upd.lastError().text();

    loadProductionData();

    // LCD miroir Qt → "Expedition OK" pendant 2s puis retour accueil
    if (m_lcdLigne1) m_lcdLigne1->setText("Expedition OK");
    if (m_lcdLigne2) m_lcdLigne2->setText(ref);

    QTimer::singleShot(2000, this, [this]() {
        if (m_lcdLigne1) m_lcdLigne1->setText("Systeme pret");
        if (m_lcdLigne2) m_lcdLigne2->setText("Saisir ID + #");
    });

    QMessageBox::information(this, "Expédition lancée",
        QString("Référence : %1\nClient    : %2\n"
                "Statut    : En livraison\nDate      : %3")
            .arg(ref, mail.isEmpty() ? "—" : mail, dateAujourdhui));
}
```

---

## 6. setupKeypadSimulator()

Crée le simulateur keypad Qt (miroir LCD + grille 4x4) inséré dans la page Production.

```cpp
void MainWindow::setupKeypadSimulator()
{
    if (m_lcdLigne1 != nullptr) return; // guard anti-doublon

    QWidget *prodTable = ui->productionTable;
    QWidget *parent = prodTable->parentWidget();
    if (!parent) return;
    QVBoxLayout *pageLayout = qobject_cast<QVBoxLayout*>(parent->layout());
    if (!pageLayout) return;

    QGroupBox *grp = new QGroupBox("Simulateur Keypad Arduino", parent);
    grp->setStyleSheet(
        "QGroupBox { border:2px solid #8D6E63; border-radius:8px; margin-top:8px; "
        "font-weight:bold; color:#8D6E63; }"
        "QGroupBox::title { subcontrol-origin:margin; left:10px; padding:0 4px; }");
    QHBoxLayout *grpLay = new QHBoxLayout(grp);
    grpLay->setSpacing(12);

    // Miroir LCD (fond noir, texte vert)
    QFrame *lcd = new QFrame(grp);
    lcd->setFixedSize(220, 56);
    lcd->setStyleSheet("background:#000; border:2px solid #333; border-radius:4px;");
    QVBoxLayout *lcdLay = new QVBoxLayout(lcd);
    lcdLay->setContentsMargins(8, 4, 8, 4);
    lcdLay->setSpacing(2);

    m_lcdLigne1 = new QLabel("Systeme pret", lcd);
    m_lcdLigne1->setStyleSheet(
        "color:#00FF00; font-family:'Courier New'; font-size:12px; font-weight:bold;");
    m_lcdLigne2 = new QLabel("Saisir ID + #", lcd);
    m_lcdLigne2->setStyleSheet(
        "color:#00FF00; font-family:'Courier New'; font-size:12px;");
    lcdLay->addWidget(m_lcdLigne1);
    lcdLay->addWidget(m_lcdLigne2);
    grpLay->addWidget(lcd);

    // Grille 4x4 keypad
    QGridLayout *grid = new QGridLayout();
    grid->setSpacing(4);
    const QStringList keys = {"1","2","3","A","4","5","6","B",
                               "7","8","9","C","*","0","#","D"};
    QString btnStyle =
        "QPushButton { background:#5D4037; color:white; border:none; border-radius:4px; "
        "font-size:13px; font-weight:bold; min-width:32px; min-height:32px; }"
        "QPushButton:hover { background:#8D6E63; }"
        "QPushButton:pressed { background:#3E2723; }";

    for (int i = 0; i < 16; ++i) {
        QPushButton *btn = new QPushButton(keys[i], grp);
        btn->setStyleSheet(btnStyle);
        const QString key = keys[i];
        connect(btn, &QPushButton::clicked, this, [this, key]() {
            if (key == "#") {
                traiterMessageArduino("ID:" + m_keypadBuffer);
                m_keypadBuffer.clear();
            } else if (key == "*") {
                traiterMessageArduino("CLEAR");
            } else {
                m_keypadBuffer += key;
                traiterMessageArduino("INPUT:" + m_keypadBuffer);
            }
        });
        grid->addWidget(btn, i / 4, i % 4);
    }
    grpLay->addLayout(grid);
    grpLay->addStretch();

    // Insérer juste avant productionTable dans le layout
    int insertPos = 0;
    for (int i = 0; i < pageLayout->count(); ++i) {
        QLayoutItem *item = pageLayout->itemAt(i);
        if (item && item->widget() == ui->productionTable) {
            insertPos = i;
            break;
        }
    }
    pageLayout->insertWidget(insertPos, grp);
}
```

---

## Protocole série Qt ↔ Arduino

| Qt envoie | Signification | Réaction Arduino |
|---|---|---|
| `'1'` | Expédier (statut Terminé confirmé) | Séquence moteurs + LCD "Expedition OK" |
| `'2'` | ID invalide ou déjà expédié | LCD "ID invalide / Ressaisir + D" |
| `'3'` | ID OK, attendre clic Expédier | LCD "ID OK - Cliquer Expedier" |
| `'0'` | Erreur générale | LCD "!! ERREUR !!" 2s |

| Arduino envoie | Signification | Traitement Qt |
|---|---|---|
| `INPUT:xxx` | Saisie en cours | Mise à jour miroir LCD ligne 2 |
| `ID:xxx` | ID confirmé (touche D) | SELECT + UPDATE BDD + envoi '2' ou '3' |
| `CLEAR` | Reset (touche *) | Reset miroir LCD Qt |
| `1` | Confirmation fin moteurs | Ignoré |
