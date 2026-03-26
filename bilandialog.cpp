#include "bilandialog.h"
#include "connection.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDate>
#include <QLocale>
#include <QProgressBar>
#include <QSizePolicy>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

// ── Palette de couleurs de l'interface ───────────────────────────────────────
static const QString BG_PAGE   = "#FAF5F0";  // Fond général (beige clair)
static const QString BG_CARD   = "#FFFFFF";  // Fond des cartes KPI
static const QString BROWN     = "#8D6E63";  // Couleur principale (marron)
static const QString BROWN_LT  = "#A0826D";  // Marron clair (hover, sous-titres)
static const QString DARK_TXT  = "#291C0E";  // Texte foncé
static const QString BORDER    = "#E8DDD5";  // Bordures des cartes

/** @brief Formate un double en monnaie locale française (ex: "1 234,56 DT"). */
static QString fmt(double v) {
    return QLocale(QLocale::French).toString(v, 'f', 2) + " DT";
}

// ─────────────────────────────────────────────────────────────────────────────
BilanDialog::BilanDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Bilan Financier — CUIREA");
    setMinimumSize(820, 640);
    setStyleSheet(QString("QDialog { background: %1; }").arg(BG_PAGE));
    setupUI();    // Construction de l'interface
    loadData();   // Chargement des données depuis la DB
}

// ── Construction de l'interface ───────────────────────────────────────────────
void BilanDialog::setupUI()
{
    auto *rootLay = new QVBoxLayout(this);
    rootLay->setContentsMargins(0, 0, 0, 0);
    rootLay->setSpacing(0);

    // Zone scrollable pour le contenu principal
    auto *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setStyleSheet("QScrollArea { background: " + BG_PAGE + "; border: none; }"
                          "QScrollBar:vertical { width: 8px; background: #EDE0D8; border-radius: 4px; }"
                          "QScrollBar::handle:vertical { background: " + BROWN + "; border-radius: 4px; }");

    auto *content = new QWidget();
    content->setStyleSheet("background: " + BG_PAGE + ";");
    auto *mainLay = new QVBoxLayout(content);
    mainLay->setContentsMargins(28, 28, 28, 28);
    mainLay->setSpacing(20);

    // ── En-tête dégradé avec titre et date ──────────────────────────────────
    auto *header = new QFrame();
    header->setStyleSheet(
        "QFrame { background: qlineargradient(x1:0,y1:0,x2:1,y2:0,"
        "stop:0 #8D6E63, stop:1 #A0826D);"
        "border-radius: 12px; }");
    auto *hLay = new QVBoxLayout(header);
    hLay->setContentsMargins(24, 18, 24, 18);
    hLay->setSpacing(4);

    auto *hTitle = new QLabel("BILAN FINANCIER");
    hTitle->setStyleSheet("font-size: 22px; font-weight: bold; color: white; letter-spacing: 2px;");
    hTitle->setAlignment(Qt::AlignCenter);

    auto *hSub = new QLabel("CUIREA — Usine de Cuir");
    hSub->setStyleSheet("font-size: 12px; color: #FFF0E6;");
    hSub->setAlignment(Qt::AlignCenter);

    // Label de date mis à jour dans loadData()
    m_lblDate = new QLabel();
    m_lblDate->setStyleSheet("font-size: 11px; color: #FFE8D6;");
    m_lblDate->setAlignment(Qt::AlignCenter);

    hLay->addWidget(hTitle);
    hLay->addWidget(hSub);
    hLay->addWidget(m_lblDate);
    mainLay->addWidget(header);

    // ── Titre de section KPI ─────────────────────────────────────────────────
    auto *kpiTitle = new QLabel("Indicateurs Clés");
    kpiTitle->setStyleSheet(QString("font-size: 13px; font-weight: bold; color: %1;").arg(BROWN));
    mainLay->addWidget(kpiTitle);

    // ── Grille 2 cartes KPI ──────────────────────────────────────────────────
    auto *kpiGrid = new QGridLayout();
    kpiGrid->setSpacing(14);

    // Labels de valeur — initialisés vides, remplis par loadData()
    m_lblCA    = new QLabel("—");
    m_lblMoyen = new QLabel("—");

    // Style commun des valeurs KPI
    auto styleVal = [](QLabel *l, const QString &color) {
        l->setStyleSheet(QString("font-size: 20px; font-weight: bold; color: %1;").arg(color));
        l->setAlignment(Qt::AlignCenter);
        l->setMinimumHeight(32);
    };
    styleVal(m_lblCA,    BROWN);
    styleVal(m_lblMoyen, "#5C8FA8");

    kpiGrid->addWidget(makeKpiCard("💰", "Chiffre d'Affaires", m_lblCA,    BROWN),    0, 0);
    kpiGrid->addWidget(makeKpiCard("📊", "Moyenne / Commande", m_lblMoyen, "#5C8FA8"), 0, 1);
    mainLay->addLayout(kpiGrid);

    // ── Section répartition CA par priorité ──────────────────────────────────
    auto *prioSection = new QFrame();
    prioSection->setStyleSheet(
        QString("QFrame { background: %1; border-radius: 12px; border: 1px solid %2; }")
        .arg(BG_CARD, BORDER));
    auto *prioLay = new QVBoxLayout(prioSection);
    prioLay->setContentsMargins(20, 18, 20, 18);
    prioLay->setSpacing(14);

    auto *prioTitle = new QLabel("Répartition du CA par Priorité");
    prioTitle->setStyleSheet(
        QString("font-size: 13px; font-weight: bold; color: %1; border: none;").arg(BROWN));
    prioLay->addWidget(prioTitle);

    // Séparateur horizontal
    auto *sep = new QFrame();
    sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet(QString("color: %1; border: none; border-top: 1px solid %1;").arg(BORDER));
    prioLay->addWidget(sep);

    // Labels de montant par priorité — mis à jour dans loadData()
    m_lblBasse   = new QLabel("—");
    m_lblNormale = new QLabel("—");
    m_lblUrgente = new QLabel("—");

    // Barres de progression proportionnelles au CA de chaque priorité
    m_barBasse   = new QFrame(); m_barBasse->setFixedHeight(8);
    m_barNormale = new QFrame(); m_barNormale->setFixedHeight(8);
    m_barUrgente = new QFrame(); m_barUrgente->setFixedHeight(8);

    // Fonction locale pour ajouter une ligne priorité (icône + nom + barre + montant)
    auto addPrioRow = [&](const QString &icon, const QString &label,
                          QLabel *valLbl, QFrame *bar, const QString &color)
    {
        auto *row = new QHBoxLayout();
        row->setSpacing(10);

        auto *iconLbl = new QLabel(icon);
        iconLbl->setFixedWidth(22);
        iconLbl->setStyleSheet("font-size: 15px; border: none;");

        auto *nameLbl = new QLabel(label);
        nameLbl->setFixedWidth(80);
        nameLbl->setStyleSheet(
            QString("font-size: 12px; font-weight: bold; color: %1; border: none;").arg(DARK_TXT));

        // Barre colorée dont la largeur est proportionnelle au CA
        bar->setStyleSheet(
            QString("QFrame { background: %1; border-radius: 4px; }").arg(color));
        bar->setMinimumWidth(0);
        bar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        valLbl->setStyleSheet(
            QString("font-size: 12px; font-weight: bold; color: %1; border: none;").arg(color));
        valLbl->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        valLbl->setFixedWidth(160);

        row->addWidget(iconLbl);
        row->addWidget(nameLbl);
        row->addWidget(bar, 1);
        row->addWidget(valLbl);
        prioLay->addLayout(row);
    };

    addPrioRow("🟢", "Basse",   m_lblBasse,   m_barBasse,   "#66BB6A");
    addPrioRow("🟡", "Normale", m_lblNormale, m_barNormale, "#FFA726");
    addPrioRow("🔴", "Urgente", m_lblUrgente, m_barUrgente, "#EF5350");

    // Séparateur bas + total commandes
    auto *sepBot = new QFrame();
    sepBot->setFrameShape(QFrame::HLine);
    sepBot->setStyleSheet(QString("border: none; border-top: 1px solid %1;").arg(BORDER));
    prioLay->addWidget(sepBot);

    m_lblNbTotal = new QLabel();
    m_lblNbTotal->setStyleSheet(
        QString("font-size: 11px; color: %1; border: none;").arg(BROWN_LT));
    m_lblNbTotal->setAlignment(Qt::AlignRight);
    prioLay->addWidget(m_lblNbTotal);

    mainLay->addWidget(prioSection);
    mainLay->addStretch();

    scroll->setWidget(content);
    rootLay->addWidget(scroll);

    // ── Barre de boutons en bas de la fenêtre ────────────────────────────────
    auto *btnBar = new QFrame();
    btnBar->setStyleSheet(
        "QFrame { background: white; border-top: 1px solid " + BORDER + "; }");
    auto *btnLay = new QHBoxLayout(btnBar);
    btnLay->setContentsMargins(20, 12, 20, 12);
    btnLay->setSpacing(10);

    auto *btnExport = new QPushButton("📥 Exporter CSV");
    btnExport->setStyleSheet(
        "QPushButton { background: #6D9B6A; color: white; border: none;"
        "border-radius: 7px; padding: 9px 28px; font-size: 12px; font-weight: bold; }"
        "QPushButton:hover { background: #7DB87A; }");

    auto *btnClose = new QPushButton("Fermer");
    btnClose->setStyleSheet(
        "QPushButton { background: " + BROWN + "; color: white; border: none;"
        "border-radius: 7px; padding: 9px 28px; font-size: 12px; font-weight: bold; }"
        "QPushButton:hover { background: " + BROWN_LT + "; }");

    connect(btnExport, &QPushButton::clicked, this, &BilanDialog::exportCSV);
    connect(btnClose,  &QPushButton::clicked, this, &QDialog::accept);

    btnLay->addStretch();
    btnLay->addWidget(btnExport);
    btnLay->addWidget(btnClose);
    rootLay->addWidget(btnBar);
}

// ── Chargement des données depuis la base de données ─────────────────────────
void BilanDialog::loadData()
{
    // Afficher la date de génération
    m_lblDate->setText(
        QString("Généré le %1").arg(QDate::currentDate().toString("dd MMMM yyyy")));

    QSqlQuery q(Connection::instance()->getDatabase());

    // ── KPI globaux : CA total et nombre de commandes ────────────────────────
    q.prepare(
        "SELECT "
        "  NVL(SUM(MONTANT), 0) AS ca_total, "
        "  COUNT(*)             AS nb "
        "FROM COMMANDES"
    );

    double ca = 0;
    int    nb = 0;

    if (q.exec() && q.next()) {
        ca = q.value(0).toDouble();
        nb = q.value(1).toInt();
    } else {
        qDebug() << "BilanDialog KPI query error:" << q.lastError().text();
    }

    // Calcul de la moyenne (évite la division par zéro)
    double moyen = (nb > 0) ? (ca / nb) : 0.0;

    m_lblCA->setText(fmt(ca));
    m_lblMoyen->setText(fmt(moyen));

    // ── Répartition CA par priorité ──────────────────────────────────────────
    // GROUP BY sur UPPER(TRIM(PRIORITE)) pour normaliser les valeurs
    q.prepare(
        "SELECT UPPER(TRIM(PRIORITE)), NVL(SUM(MONTANT), 0), COUNT(*) "
        "FROM COMMANDES "
        "WHERE PRIORITE IS NOT NULL "
        "GROUP BY UPPER(TRIM(PRIORITE))"
    );

    double mBasse = 0, mNormale = 0, mUrgente = 0;
    int    nBasse = 0, nNormale = 0, nUrgente = 0;

    if (q.exec()) {
        while (q.next()) {
            QString prio = q.value(0).toString().trimmed();
            double  mont = q.value(1).toDouble();
            int     cnt  = q.value(2).toInt();
            qDebug() << "BilanDialog priorite:" << prio << "montant:" << mont;
            if      (prio == "BASSE")   { mBasse   = mont; nBasse   = cnt; }
            else if (prio == "NORMALE") { mNormale = mont; nNormale = cnt; }
            else if (prio == "URGENTE") { mUrgente = mont; nUrgente = cnt; }
            else if (prio == "HAUTE")   { mUrgente += mont; nUrgente += cnt; } // alias
        }
    } else {
        qDebug() << "BilanDialog priorite query error:" << q.lastError().text();
    }

    // Calcul du total pour les proportions des barres (évite division par zéro)
    double total = mBasse + mNormale + mUrgente;
    if (total <= 0) total = 1;

    // Largeur max des barres = 300px, minimum 4px pour rester visible
    int maxW = 300;
    m_barBasse->setFixedWidth(qMax(4, (int)(mBasse   / total * maxW)));
    m_barNormale->setFixedWidth(qMax(4, (int)(mNormale / total * maxW)));
    m_barUrgente->setFixedWidth(qMax(4, (int)(mUrgente / total * maxW)));

    // Mise à jour des labels montant + nombre de commandes
    m_lblBasse->setText(  QString("%1  (%2 cmd)").arg(fmt(mBasse)).arg(nBasse));
    m_lblNormale->setText(QString("%1  (%2 cmd)").arg(fmt(mNormale)).arg(nNormale));
    m_lblUrgente->setText(QString("%1  (%2 cmd)").arg(fmt(mUrgente)).arg(nUrgente));

    m_lblNbTotal->setText(QString("Total : %1 commande(s)").arg(nb));
}

// ── Fabrique de carte KPI ─────────────────────────────────────────────────────
QFrame *BilanDialog::makeKpiCard(const QString &icon, const QString &title,
                                  QLabel *valLabel, const QString &color,
                                  const QString &subtitle)
{
    auto *card = new QFrame();
    card->setStyleSheet(
        QString("QFrame { background: %1; border-radius: 12px;"
                "border: 1px solid %2; }").arg(BG_CARD, BORDER));
    card->setMinimumHeight(110);

    auto *lay = new QVBoxLayout(card);
    lay->setContentsMargins(18, 16, 18, 16);
    lay->setSpacing(6);

    // Ligne icône + titre
    auto *topRow = new QHBoxLayout();
    auto *iconLbl = new QLabel(icon);
    iconLbl->setStyleSheet("font-size: 20px; border: none;");
    auto *titleLbl = new QLabel(title);
    titleLbl->setStyleSheet(
        QString("font-size: 11px; color: %1; font-weight: bold; border: none;").arg(BROWN_LT));
    topRow->addWidget(iconLbl);
    topRow->addWidget(titleLbl);
    topRow->addStretch();
    lay->addLayout(topRow);

    // Séparateur coloré sous le titre
    auto *sep = new QFrame();
    sep->setFixedHeight(2);
    sep->setStyleSheet(QString("background: %1; border: none;").arg(color));
    lay->addWidget(sep);

    // Le label valeur est passé depuis l'extérieur et mis à jour par loadData()
    valLabel->setParent(card);
    lay->addWidget(valLabel);

    if (!subtitle.isEmpty()) {
        auto *subLbl = new QLabel(subtitle);
        subLbl->setStyleSheet("font-size: 10px; color: #999; border: none;");
        lay->addWidget(subLbl);
    }

    return card;
}

// ── Export CSV ────────────────────────────────────────────────────────────────
void BilanDialog::exportCSV()
{
    // Demander à l'utilisateur où sauvegarder le fichier
    QString fn = QFileDialog::getSaveFileName(
        this, "Exporter le bilan",
        QString("Bilan_CUIREA_%1.csv").arg(QDate::currentDate().toString("yyyy-MM-dd")),
        "CSV (*.csv)");
    if (fn.isEmpty()) return;

    QFile file(fn);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Erreur", "Impossible d'écrire le fichier.");
        return;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    // En-tête du fichier CSV
    out << "BILAN FINANCIER — CUIREA\n";
    out << "Généré le;" << QDate::currentDate().toString("dd/MM/yyyy") << "\n\n";

    // Section KPI
    out << "Indicateur;Valeur\n";
    out << "Chiffre d'Affaires;" << m_lblCA->text() << "\n";
    out << "Moyenne / Commande;" << m_lblMoyen->text() << "\n\n";

    // Section répartition par priorité — relecture DB pour données brutes
    out << "Priorité;Montant;Commandes\n";
    QSqlQuery q(Connection::instance()->getDatabase());
    q.prepare(
        "SELECT UPPER(TRIM(PRIORITE)), NVL(SUM(MONTANT),0), COUNT(*) "
        "FROM COMMANDES WHERE PRIORITE IS NOT NULL "
        "GROUP BY UPPER(TRIM(PRIORITE)) ORDER BY 1"
    );
    if (q.exec()) {
        while (q.next()) {
            out << q.value(0).toString() << ";"
                << QString::number(q.value(1).toDouble(), 'f', 2) << ";"
                << q.value(2).toInt() << "\n";
        }
    }

    file.close();
    QMessageBox::information(this, "Succès",
        QString("Bilan exporté :\n%1").arg(fn));
}
