# Arduino ↔ MainWindow — Code Qt complet

---

## 1. Membres (mainwindow.h)

```cpp
// Arduino
Arduino A;           // instance unique (arduino.h)
QByteArray arduinoData;  // buffer de réception

// Slots déclarés
void on_btnExpedier_clicked();   // toggle 🚗 → affiche btnSaisir + btnExpedierAction
void expedierActionArduino();    // OUTPUT → envoie '1' ou '0' à l'Arduino
void recevoir_donnee();          // INPUT  ← lit les données du port série
```

---

## 2. Connexions dans le constructeur (~ligne 425)

```cpp
// Bouton 🚗 — toggle
connect(ui->btnExpedier, &QPushButton::clicked,
        this, &MainWindow::on_btnExpedier_clicked);

// Bouton ⌨ Saisir — indicateur uniquement (saisie réelle via keypad Arduino)
connect(ui->btnSaisir, &QPushButton::clicked, this, [this]() {
    QMessageBox::information(this, "Saisie Arduino",
        "Utilisez le clavier 16 touches connecté à l'Arduino\n"
        "pour saisir l'ID de la commande.\n\n"
        "Appuyez sur # pour confirmer.");
    ui->btnSaisir->setVisible(false);
    ui->btnExpedierAction->setVisible(false);
    ui->btnExpedier->setVisible(true);
});

// Bouton 🚚 Expédier — OUTPUT Arduino
connect(ui->btnExpedierAction, &QPushButton::clicked,
        this, &MainWindow::expedierActionArduino);
```

---

## 3. Initialisation connexion série (~ligne 440)

```cpp
{
    int ret = A.connection()->connect_arduino();
    switch (ret) {
    case 0:
        qDebug() << "✅ Arduino connecté :" << A.connection()->getPortName();
        QObject::connect(A.connection()->getSerial(), SIGNAL(readyRead()),
                         this, SLOT(recevoir_donnee()));
        break;
    case 1:
        qDebug() << "⚠️ Arduino trouvé mais non ouvert";
        break;
    default:
        qDebug() << "❌ Arduino non trouvé";
        break;
    }
}
```

---

## 4. Toggle boutons — `on_btnExpedier_clicked()`

```cpp
void MainWindow::on_btnExpedier_clicked()
{
    ui->btnExpedier->setVisible(false);
    ui->btnSaisir->setVisible(true);
    ui->btnExpedierAction->setVisible(true);
}
```

---

## 5. OUTPUT — `expedierActionArduino()`

Déclenché par `btnExpedierAction` (🚚 Expédier).

```cpp
void MainWindow::expedierActionArduino()
{
    // Guard : ligne sélectionnée obligatoire
    int row = ui->productionTable->currentRow();
    if (row < 0 || ui->productionTable->selectedItems().isEmpty()) {
        QMessageBox::warning(this, "Attention",
            "Veuillez sélectionner une commande dans le tableau !");
        return;  // boutons restent visibles
    }

    // Fermer les boutons
    ui->btnSaisir->setVisible(false);
    ui->btnExpedierAction->setVisible(false);
    ui->btnExpedier->setVisible(true);

    QString ref    = cellText(ui->productionTable, row, 1);  // col 1 = RÉFÉRENCE
    QString statut = cellText(ui->productionTable, row, 8);  // col 8 = STATUT

    // CAS 1 : statut == "Terminé" ✅
    if (statut == "Terminé") {
        if (A.connection()->getSerial()->isOpen())
            A.write_to_arduino("1");
        else
            qDebug() << "⚠️ Arduino non connecté — envoi ignoré";

        QSqlQuery update;
        update.prepare("UPDATE COMMANDES SET STATUT = 'En livraison' WHERE REFERENCE = :ref");
        update.bindValue(":ref", ref);
        if (!update.exec())
            qDebug() << "Erreur UPDATE:" << update.lastError().text();

        QMessageBox::information(this, "Expédition lancée",
            "Référence    : " + ref    + "\n"
            "Statut avant : Terminé\n"
            "Statut après : En livraison\n"
            "Arduino reçoit : 1\n"
            "Servos         : Démarrés !");

        loadProductionData();

    // CAS 2 : statut != "Terminé" ❌
    } else {
        if (A.connection()->getSerial()->isOpen())
            A.write_to_arduino("0");

        QMessageBox::critical(this, "Expédition impossible",
            "Référence     : " + ref    + "\n"
            "Statut actuel : " + statut + "\n"
            "Statut requis : Terminé\n"
            "Arduino reçoit : 0\n"
            "Servos         : Bloqués !");
    }
}
```

---

## 6. INPUT — `recevoir_donnee()`

Déclenché automatiquement par `QSerialPort::readyRead()`.

```cpp
void MainWindow::recevoir_donnee()
{
    arduinoData = A.read_from_arduino();
    QString msg = QString(arduinoData).trimmed();
    qDebug() << "Reçu depuis Arduino :" << msg;

    // ── Protocole "ID:xxx" — saisie clavier 16 touches ──────────────────────
    if (msg.startsWith("ID:")) {
        QString id = msg.mid(3);  // extrait l'ID après "ID:"

        QSqlQuery q;
        q.prepare("SELECT STATUT FROM COMMANDES WHERE ID = :id");
        q.bindValue(":id", id);
        q.exec();

        if (q.next()) {
            QString statut = q.value(0).toString();
            if (statut == "Terminé") {
                // Commande déjà terminée → activer servos
                A.connection()->getSerial()->write("1");

                QSqlQuery update;
                update.prepare("UPDATE COMMANDES SET STATUT = 'En livraison' WHERE ID = :id");
                update.bindValue(":id", id);
                update.exec();
                loadProductionData();
                qDebug() << "✅ Commande" << id << "→ En livraison";
            } else {
                // Statut incorrect
                A.connection()->getSerial()->write("0");
                QMessageBox::warning(this, "Erreur",
                    "Commande " + id + " — Statut : " + statut +
                    "\nStatut requis : Terminé");
            }
        } else {
            // ID introuvable en BDD
            A.connection()->getSerial()->write("0");
            QMessageBox::critical(this, "Erreur", "Commande introuvable : " + id);
        }

    // ── Protocole "INPUT:xxx" — affichage saisie en cours ───────────────────
    } else if (msg.startsWith("INPUT:")) {
        qDebug() << "Saisie en cours :" << msg.mid(6);

    // ── Protocole "CLEAR" — effacement saisie ───────────────────────────────
    } else if (msg == "CLEAR") {
        qDebug() << "Saisie effacée";
    }
}
```

---

## 7. Flux complet

```
PHASE 1 — Keypad Arduino (INPUT)
  Saisie ID sur clavier 16 touches
  Arduino envoie "INPUT:x"  → Qt : qDebug saisie en cours
  Arduino envoie "ID:xxx"   → Qt cherche en BDD
    ✅ ID trouvé + statut Terminé  → Qt écrit '1' → servos démarrent
                                   → BDD : statut → "En livraison"
    ❌ ID trouvé + mauvais statut  → Qt écrit '0' → erreur
    ❌ ID introuvable              → Qt écrit '0' → erreur
  Arduino envoie "CLEAR"    → Qt : qDebug effacé

PHASE 2 — Bouton Expédier Qt (OUTPUT)
  Clic 🚗 → affiche ⌨ Saisir + 🚚 Expédier
  Clic ⌨ Saisir → message info "utilisez le keypad" → referme
  Clic 🚚 Expédier :
    ❌ aucune ligne sélectionnée → warning, boutons restent ouverts
    ✅ ligne sélectionnée :
        statut == "Terminé"  → Qt écrit '1' → servos → BDD "En livraison"
        statut != "Terminé"  → Qt écrit '0' → erreur Arduino
```

---

## 8. Fichiers concernés

| Fichier | Rôle |
|---------|------|
| `arduinoconnection.h/.cpp` | Détection port, connexion/déconnexion, `getSerial()`, `getPortName()` |
| `arduino.h/.cpp` | `write_to_arduino()`, `read_from_arduino()`, accès à `ArduinoConnection` |
| `mainwindow.h` | Déclaration `Arduino A`, `arduinoData`, slots |
| `mainwindow.cpp` | Init connexion, toggle boutons, OUTPUT, INPUT |
| `arduino_sketch/arduino_sketch.ino` | Sketch Arduino : keypad, servos bras+bloqueur, ... erreur |
