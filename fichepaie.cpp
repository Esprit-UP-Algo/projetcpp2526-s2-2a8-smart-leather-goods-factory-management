#include "fichepaie.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QPrinter>
#include <QPainter>
#include <QDesktopServices>
#include <QUrl>
#include <QFileInfo>
#include <QDir>
#include <QComboBox>
#include <QScrollArea>
#include <QFrame>

FichePaieDialog::FichePaieDialog(const QString &matricule, const QString &nom,
                                 const QString &prenom, const QString &cin,
                                 const QString &poste, const QString &departement,
                                 const QDate &dateEmbauche, QWidget *parent)
    : QDialog(parent)
    , m_matricule(matricule)
    , m_nom(nom)
    , m_prenom(prenom)
    , m_cin(cin)
    , m_poste(poste)
    , m_departement(departement)
    , m_dateEmbauche(dateEmbauche)
{
    setupUI();
    applyStyles();
    setWindowTitle("Fiche de Paie - " + nom + " " + prenom);
    setMinimumSize(850, 700);
    resize(900, 750);
}

void FichePaieDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 25, 30, 25);
    mainLayout->setSpacing(20);
    
    // Titre
    QLabel *title = new QLabel("Génération de Fiche de Paie");
    title->setObjectName("dialogTitle");
    title->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(title);
    
    // Scroll area pour le contenu
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    QWidget *scrollContent = new QWidget();
    scrollContent->setStyleSheet("background-color: #FAF5F0;"); // Fond beige
    QVBoxLayout *contentLayout = new QVBoxLayout(scrollContent);
    contentLayout->setSpacing(15);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    
    // Informations employé
    QGroupBox *infoGroup = new QGroupBox("Informations Employé");
    infoGroup->setObjectName("groupBox");
    QGridLayout *infoLayout = new QGridLayout(infoGroup);
    infoLayout->setHorizontalSpacing(15);
    infoLayout->setVerticalSpacing(10);
    
    infoLayout->addWidget(new QLabel("Matricule:"), 0, 0, Qt::AlignRight);
    infoLayout->addWidget(new QLabel(m_matricule), 0, 1);
    infoLayout->addWidget(new QLabel("Nom complet:"), 0, 2, Qt::AlignRight);
    infoLayout->addWidget(new QLabel(m_nom + " " + m_prenom), 0, 3);
    infoLayout->addWidget(new QLabel("CIN:"), 1, 0, Qt::AlignRight);
    infoLayout->addWidget(new QLabel(m_cin), 1, 1);
    infoLayout->addWidget(new QLabel("Poste:"), 1, 2, Qt::AlignRight);
    infoLayout->addWidget(new QLabel(m_poste), 1, 3);
    infoLayout->addWidget(new QLabel("Département:"), 2, 0, Qt::AlignRight);
    infoLayout->addWidget(new QLabel(m_departement), 2, 1, 1, 3);
    infoLayout->setColumnStretch(1, 1);
    infoLayout->setColumnStretch(3, 1);
    contentLayout->addWidget(infoGroup);
    
    // Période
    QGroupBox *periodeGroup = new QGroupBox("Période");
    periodeGroup->setObjectName("groupBox");
    QHBoxLayout *periodeLayout = new QHBoxLayout(periodeGroup);
    periodeLayout->setSpacing(15);
    
    QComboBox *moisCombo = new QComboBox();
    moisCombo->setObjectName("moisCombo");
    moisCombo->addItems({"Janvier", "Février", "Mars", "Avril", "Mai", "Juin",
                        "Juillet", "Août", "Septembre", "Octobre", "Novembre", "Décembre"});
    moisCombo->setCurrentIndex(QDate::currentDate().month() - 1);
    
    QComboBox *anneeCombo = new QComboBox();
    anneeCombo->setObjectName("anneeCombo");
    int currentYear = QDate::currentDate().year();
    for (int i = currentYear - 2; i <= currentYear + 1; i++) {
        anneeCombo->addItem(QString::number(i));
    }
    anneeCombo->setCurrentText(QString::number(currentYear));
    
    periodeLayout->addWidget(new QLabel("Mois:"));
    periodeLayout->addWidget(moisCombo, 1);
    periodeLayout->addSpacing(20);
    periodeLayout->addWidget(new QLabel("Année:"));
    periodeLayout->addWidget(anneeCombo, 1);
    periodeLayout->addStretch(2);
    contentLayout->addWidget(periodeGroup);
    
    // Gains et Retenues côte à côte
    QHBoxLayout *gainsRetenuesLayout = new QHBoxLayout();
    gainsRetenuesLayout->setSpacing(15);
    
    // Gains (gauche)
    QGroupBox *gainsGroup = new QGroupBox("Gains");
    gainsGroup->setObjectName("groupBox");
    QFormLayout *gainsLayout = new QFormLayout(gainsGroup);
    gainsLayout->setLabelAlignment(Qt::AlignRight);
    gainsLayout->setHorizontalSpacing(10);
    gainsLayout->setVerticalSpacing(12);
    
    salaireBrutSpin = new QDoubleSpinBox();
    salaireBrutSpin->setRange(0, 999999);
    salaireBrutSpin->setDecimals(3);
    salaireBrutSpin->setSuffix(" TND");
    salaireBrutSpin->setValue(1500.000);
    salaireBrutSpin->setMinimumWidth(150);
    
    primeRendementSpin = new QDoubleSpinBox();
    primeRendementSpin->setRange(0, 999999);
    primeRendementSpin->setDecimals(3);
    primeRendementSpin->setSuffix(" TND");
    primeRendementSpin->setMinimumWidth(150);
    
    primeAncienneteSpin = new QDoubleSpinBox();
    primeAncienneteSpin->setRange(0, 999999);
    primeAncienneteSpin->setDecimals(3);
    primeAncienneteSpin->setSuffix(" TND");
    primeAncienneteSpin->setMinimumWidth(150);
    
    primeAutresSpin = new QDoubleSpinBox();
    primeAutresSpin->setRange(0, 999999);
    primeAutresSpin->setDecimals(3);
    primeAutresSpin->setSuffix(" TND");
    primeAutresSpin->setMinimumWidth(150);
    
    gainsLayout->addRow("Salaire Brut *:", salaireBrutSpin);
    gainsLayout->addRow("Prime Rendement:", primeRendementSpin);
    gainsLayout->addRow("Prime Ancienneté:", primeAncienneteSpin);
    gainsLayout->addRow("Autres Primes:", primeAutresSpin);
    gainsRetenuesLayout->addWidget(gainsGroup, 1);
    
    // Retenues (droite)
    QGroupBox *retenuesGroup = new QGroupBox("Retenues et Avances");
    retenuesGroup->setObjectName("groupBox");
    QFormLayout *retenuesLayout = new QFormLayout(retenuesGroup);
    retenuesLayout->setLabelAlignment(Qt::AlignRight);
    retenuesLayout->setHorizontalSpacing(10);
    retenuesLayout->setVerticalSpacing(12);
    
    retenuesSpin = new QDoubleSpinBox();
    retenuesSpin->setRange(0, 999999);
    retenuesSpin->setDecimals(3);
    retenuesSpin->setSuffix(" TND");
    retenuesSpin->setMinimumWidth(150);
    
    avancesSpin = new QDoubleSpinBox();
    avancesSpin->setRange(0, 999999);
    avancesSpin->setDecimals(3);
    avancesSpin->setSuffix(" TND");
    avancesSpin->setMinimumWidth(150);
    
    retenuesLayout->addRow("Retenues:", retenuesSpin);
    retenuesLayout->addRow("Avances:", avancesSpin);
    retenuesLayout->addRow("", new QWidget());
    retenuesLayout->addRow("", new QWidget());
    gainsRetenuesLayout->addWidget(retenuesGroup, 1);
    contentLayout->addLayout(gainsRetenuesLayout);
    
    // Calculs automatiques
    QGroupBox *calculsGroup = new QGroupBox("Calculs Automatiques");
    calculsGroup->setObjectName("groupBox");
    QFormLayout *calculsLayout = new QFormLayout(calculsGroup);
    calculsLayout->setLabelAlignment(Qt::AlignRight);
    calculsLayout->setHorizontalSpacing(10);
    calculsLayout->setVerticalSpacing(12);
    
    cnssEdit = new QLineEdit();
    cnssEdit->setReadOnly(true);
    cnssEdit->setStyleSheet("background-color: #F5F5F5; font-weight: bold;");
    cnssEdit->setMinimumWidth(150);
    
    irppEdit = new QLineEdit();
    irppEdit->setReadOnly(true);
    irppEdit->setStyleSheet("background-color: #F5F5F5; font-weight: bold;");
    irppEdit->setMinimumWidth(150);
    
    totalGainsEdit = new QLineEdit();
    totalGainsEdit->setReadOnly(true);
    totalGainsEdit->setStyleSheet("background-color: #E8F5E9; font-weight: bold; color: #2E7D32;");
    totalGainsEdit->setMinimumWidth(150);
    
    totalRetenuesEdit = new QLineEdit();
    totalRetenuesEdit->setReadOnly(true);
    totalRetenuesEdit->setStyleSheet("background-color: #FFEBEE; font-weight: bold; color: #C62828;");
    totalRetenuesEdit->setMinimumWidth(150);
    
    netAPayerEdit = new QLineEdit();
    netAPayerEdit->setReadOnly(true);
    netAPayerEdit->setStyleSheet("background-color: #FFF3E0; font-weight: bold; font-size: 14px; color: #E65100;");
    netAPayerEdit->setMinimumWidth(150);
    
    calculsLayout->addRow("CNSS Salarié (9.18%):", cnssEdit);
    calculsLayout->addRow("IRPP:", irppEdit);
    calculsLayout->addRow("Total Gains:", totalGainsEdit);
    calculsLayout->addRow("Total Retenues:", totalRetenuesEdit);
    calculsLayout->addRow("NET À PAYER:", netAPayerEdit);
    contentLayout->addWidget(calculsGroup);
    
    // Note
    QLabel *noteLabel = new QLabel("* Champs obligatoires");
    noteLabel->setObjectName("noteLabel");
    contentLayout->addWidget(noteLabel);
    
    scrollArea->setWidget(scrollContent);
    mainLayout->addWidget(scrollArea, 1);
    
    // Boutons en bas
    QWidget *buttonWidget = new QWidget();
    QHBoxLayout *btnLayout = new QHBoxLayout(buttonWidget);
    btnLayout->setContentsMargins(0, 10, 0, 0);
    btnLayout->setSpacing(10);
    
    QPushButton *calculerBtn = new QPushButton("🔢 Calculer");
    calculerBtn->setObjectName("calculateButton");
    calculerBtn->setMinimumSize(120, 38);
    
    QPushButton *genererBtn = new QPushButton("📄 Générer PDF");
    genererBtn->setObjectName("generateButton");
    genererBtn->setMinimumSize(140, 38);
    
    QPushButton *annulerBtn = new QPushButton("✕ Annuler");
    annulerBtn->setObjectName("cancelButton");
    annulerBtn->setMinimumSize(120, 38);
    
    btnLayout->addStretch();
    btnLayout->addWidget(calculerBtn);
    btnLayout->addWidget(genererBtn);
    btnLayout->addWidget(annulerBtn);
    mainLayout->addWidget(buttonWidget);
    
    // Connexions
    connect(calculerBtn, &QPushButton::clicked, this, &FichePaieDialog::calculerSalaire);
    connect(genererBtn, &QPushButton::clicked, this, &FichePaieDialog::genererPDF);
    connect(annulerBtn, &QPushButton::clicked, this, &QDialog::reject);
    
    // Calcul automatique
    connect(salaireBrutSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &FichePaieDialog::calculerSalaire);
    connect(primeRendementSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &FichePaieDialog::calculerSalaire);
    connect(primeAncienneteSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &FichePaieDialog::calculerSalaire);
    connect(primeAutresSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &FichePaieDialog::calculerSalaire);
    connect(retenuesSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &FichePaieDialog::calculerSalaire);
    connect(avancesSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &FichePaieDialog::calculerSalaire);
    
    calculerSalaire();
}

void FichePaieDialog::calculerSalaire()
{
    double salaireBrut = salaireBrutSpin->value();
    double primeRendement = primeRendementSpin->value();
    double primeAnciennete = primeAncienneteSpin->value();
    double primeAutres = primeAutresSpin->value();
    double retenues = retenuesSpin->value();
    double avances = avancesSpin->value();
    
    double cnss = calculerCNSS(salaireBrut);
    double salaireImposable = salaireBrut - cnss;
    double irpp = calculerIRPP(salaireImposable);
    
    double totalGains = salaireBrut + primeRendement + primeAnciennete + primeAutres;
    double totalRetenues = cnss + irpp + retenues + avances;
    double netAPayer = totalGains - totalRetenues;
    
    cnssEdit->setText(QString::number(cnss, 'f', 3) + " TND");
    irppEdit->setText(QString::number(irpp, 'f', 3) + " TND");
    totalGainsEdit->setText(QString::number(totalGains, 'f', 3) + " TND");
    totalRetenuesEdit->setText(QString::number(totalRetenues, 'f', 3) + " TND");
    netAPayerEdit->setText(QString::number(netAPayer, 'f', 3) + " TND");
}

double FichePaieDialog::calculerCNSS(double salaireBrut) const
{
    return salaireBrut * 0.0918;
}

double FichePaieDialog::calculerIRPP(double salaireImposable) const
{
    double irpp = 0.0;
    if (salaireImposable <= 5000) {
        irpp = 0;
    } else if (salaireImposable <= 20000) {
        irpp = (salaireImposable - 5000) * 0.26;
    } else if (salaireImposable <= 30000) {
        irpp = 3900 + (salaireImposable - 20000) * 0.28;
    } else if (salaireImposable <= 50000) {
        irpp = 6700 + (salaireImposable - 30000) * 0.32;
    } else {
        irpp = 13100 + (salaireImposable - 50000) * 0.35;
    }
    return irpp;
}

DonneesPaie FichePaieDialog::getDonneesPaie() const
{
    DonneesPaie donnees;
    donnees.matricule = m_matricule;
    donnees.nom = m_nom;
    donnees.prenom = m_prenom;
    donnees.cin = m_cin;
    donnees.poste = m_poste;
    donnees.departement = m_departement;
    donnees.dateEmbauche = m_dateEmbauche;
    donnees.salaireBrut = salaireBrutSpin->value();
    donnees.primeRendement = primeRendementSpin->value();
    donnees.primeAnciennete = primeAncienneteSpin->value();
    donnees.primeAutres = primeAutresSpin->value();
    donnees.retenues = retenuesSpin->value();
    donnees.avances = avancesSpin->value();
    
    QComboBox *moisCombo = findChild<QComboBox*>("moisCombo");
    if (moisCombo) {
        donnees.mois = moisCombo->currentText();
    }
    QComboBox *anneeCombo = findChild<QComboBox*>("anneeCombo");
    if (anneeCombo) {
        donnees.annee = anneeCombo->currentText().toInt();
    }
    return donnees;
}

void FichePaieDialog::genererPDF()
{
    DonneesPaie donnees = getDonneesPaie();
    QString dossier = QDir::currentPath() + "/FichesPaie";
    QDir().mkpath(dossier);
    QString nomFichier = QString("FichePaie_%1_%2_%3.pdf")
                        .arg(m_matricule).arg(donnees.mois).arg(donnees.annee);
    QString cheminComplet = dossier + "/" + nomFichier;
    
    if (FichePaiePDF::genererPDF(donnees, cheminComplet)) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Succès");
        msgBox.setText("Fiche de paie générée avec succès !");
        msgBox.setInformativeText("Voulez-vous ouvrir le fichier ?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        msgBox.setIcon(QMessageBox::Information);
        
        if (msgBox.exec() == QMessageBox::Yes) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(cheminComplet));
        }
        accept();
    } else {
        QMessageBox::critical(this, "Erreur", "Erreur lors de la génération du PDF.");
    }
}

void FichePaieDialog::applyStyles()
{
    setStyleSheet(
        "QDialog { background-color: #FAF5F0; }"
        "QLabel#dialogTitle { color: #291C0E; font-family: 'Times New Roman', serif; "
        "font-size: 20px; font-weight: bold; padding: 10px; }"
        "QGroupBox { "
        "   color: #291C0E; "
        "   font-family: Arial, sans-serif; "
        "   font-size: 13px; "
        "   font-weight: bold; "
        "   border: 2px solid #BCAAA4; "
        "   border-radius: 8px; "
        "   margin-top: 10px; "
        "   padding-top: 15px; "
        "   background-color: #FFF8F0; "
        "}"
        "QGroupBox::title { "
        "   subcontrol-origin: margin; "
        "   subcontrol-position: top left; "
        "   padding: 5px 10px; "
        "   background-color: #FAF5F0; "
        "   border-radius: 4px; "
        "}"
        "QLabel { color: #291C0E; font-family: Arial, sans-serif; font-size: 12px; }"
        "QLineEdit, QDoubleSpinBox { "
        "   background-color: white; "
        "   color: #291C0E; "
        "   border: 2px solid #BCAAA4; "
        "   border-radius: 6px; "
        "   padding: 8px; "
        "   font-family: Arial, sans-serif; "
        "   font-size: 11px; "
        "   min-height: 25px; "
        "}"
        "QComboBox { "
        "   background-color: white; "
        "   color: #291C0E; "
        "   border: 2px solid #BCAAA4; "
        "   border-radius: 6px; "
        "   padding: 8px; "
        "   padding-right: 30px; "
        "   font-family: Arial, sans-serif; "
        "   font-size: 11px; "
        "   min-height: 25px; "
        "}"
        "QComboBox:hover { "
        "   border: 2px solid #8D6E63; "
        "}"
        "QComboBox::drop-down { "
        "   subcontrol-origin: padding; "
        "   subcontrol-position: top right; "
        "   width: 25px; "
        "   border-left: 1px solid #BCAAA4; "
        "   background-color: #E7DDD1; "
        "}"
        "QComboBox::down-arrow { "
        "   image: none; "
        "   border-left: 4px solid transparent; "
        "   border-right: 4px solid transparent; "
        "   border-top: 6px solid #8D6E63; "
        "   width: 0px; "
        "   height: 0px; "
        "}"
        "QComboBox QAbstractItemView { "
        "   background-color: white; "
        "   color: #291C0E; "
        "   border: 2px solid #8D6E63; "
        "   selection-background-color: #8D6E63; "
        "   selection-color: white; "
        "   padding: 5px; "
        "}"
        "QLineEdit:focus, QDoubleSpinBox:focus, QComboBox:focus { "
        "   border: 2px solid #8D6E63; "
        "}"
        "QDoubleSpinBox::up-button, QDoubleSpinBox::down-button { "
        "   background-color: #E7DDD1; "
        "   border: 1px solid #BCAAA4; "
        "   width: 16px; "
        "}"
        "QDoubleSpinBox::up-button:hover, QDoubleSpinBox::down-button:hover { "
        "   background-color: #BCAAA4; "
        "}"
        "QPushButton#calculateButton { "
        "   background-color: #2196F3; "
        "   color: white; "
        "   border: none; "
        "   border-radius: 8px; "
        "   padding: 10px 20px; "
        "   font-size: 12px; "
        "   font-weight: bold; "
        "}"
        "QPushButton#calculateButton:hover { background-color: #1976D2; }"
        "QPushButton#generateButton { "
        "   background-color: #8D6E63; "
        "   color: white; "
        "   border: none; "
        "   border-radius: 8px; "
        "   padding: 10px 20px; "
        "   font-size: 12px; "
        "   font-weight: bold; "
        "}"
        "QPushButton#generateButton:hover { background-color: #A0826D; }"
        "QPushButton#cancelButton { "
        "   background-color: #E7DDD1; "
        "   color: #291C0E; "
        "   border: 1px solid #BCAAA4; "
        "   border-radius: 8px; "
        "   padding: 10px 20px; "
        "   font-size: 12px; "
        "   font-weight: bold; "
        "}"
        "QPushButton#cancelButton:hover { background-color: #F0E6DA; }"
        "QLabel#noteLabel { color: #8D6E63; font-size: 10px; font-style: italic; }"
        "QScrollArea { "
        "   border: none; "
        "   background-color: transparent; "
        "}"
    );
}

bool FichePaiePDF::genererPDF(const DonneesPaie &donnees, const QString &cheminFichier)
{
    QPrinter printer(QPrinter::ScreenResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(cheminFichier);
    printer.setPageSize(QPageSize::A4);
    printer.setPageMargins(QMarginsF(15, 15, 15, 15), QPageLayout::Millimeter);
    
    QPainter painter;
    if (!painter.begin(&printer)) {
        return false;
    }
    
    int pageWidth = printer.width();
    int pageHeight = printer.height();
    int margin = 100;
    int y = 100;
    
    QFont titleFont("Times New Roman", 18, QFont::Bold);
    QFont headerFont("Arial", 12, QFont::Bold);
    QFont normalFont("Arial", 9);
    QFont smallFont("Arial", 8);
    
    QColor colorPrimary(141, 110, 99);
    QColor colorSecondary(188, 170, 164);
    QColor colorText(41, 28, 14);
    
    // === EN-TÊTE AVEC LOGO CENTRÉ ===
    int startY = 0;
    
    // Logo centré en haut
    QPixmap logo(":/logo.png");
    if (!logo.isNull()) {
        int logoHeight = 90;
        int logoWidth = logo.width() * logoHeight / logo.height();
        int logoX = (pageWidth - logoWidth) / 2;
        painter.drawPixmap(logoX, startY, logoWidth, logoHeight, logo);
        startY += logoHeight + 15;
    }
    
    // Titre CUIREA centré sous le logo
    painter.setPen(colorPrimary);
    painter.setFont(titleFont);
    QRect titleRect(margin, startY, pageWidth - 2*margin, 30);
    painter.drawText(titleRect, Qt::AlignCenter, "CUIREA");
    
    y = startY + 60;
    
    // === TITRE FICHE DE PAIE ===
    painter.setFont(headerFont);
    painter.setPen(colorPrimary);
    QString titreFiche = QString("FICHE DE PAIE - %1 %2").arg(donnees.mois).arg(donnees.annee);
    painter.drawText(margin, y, titreFiche);
    y += 60;
    
    // === BLOC EMPLOYÉ (Tableau 2 colonnes) ===
    painter.setFont(headerFont);
    painter.setPen(colorPrimary);
    painter.drawText(margin, y, "INFORMATIONS EMPLOYÉ");
    y += 40;
    
    painter.setFont(normalFont);
    painter.setPen(colorText);
    
    int tableWidth = pageWidth - 2*margin;
    int colWidth = tableWidth / 2;
    int rowHeight = 32;
    
    // En-tête tableau avec fond beige
    painter.fillRect(margin, y, tableWidth, rowHeight, QBrush(QColor(139, 90, 70)));
    painter.setPen(QPen(colorText, 2));
    painter.drawRect(margin, y, tableWidth, rowHeight);
    painter.drawLine(margin + colWidth, y, margin + colWidth, y + rowHeight);
    
    painter.setPen(Qt::white);
    painter.setFont(headerFont);
    painter.drawText(margin + 15, y + 21, "Matricule");
    painter.drawText(margin + colWidth + 15, y + 21, "Nom Complet");
    y += rowHeight;
    
    // Ligne 1: Matricule | Nom
    painter.setPen(colorText);
    painter.setFont(normalFont);
    painter.fillRect(margin, y, tableWidth, rowHeight, QBrush(QColor(250, 245, 240)));
    painter.drawRect(margin, y, tableWidth, rowHeight);
    painter.drawLine(margin + colWidth, y, margin + colWidth, y + rowHeight);
    painter.drawText(margin + 15, y + 21, donnees.matricule);
    painter.drawText(margin + colWidth + 15, y + 21, donnees.nom + " " + donnees.prenom);
    y += rowHeight;
    
    // En-tête ligne 2
    painter.fillRect(margin, y, tableWidth, rowHeight, QBrush(QColor(139, 90, 70)));
    painter.drawRect(margin, y, tableWidth, rowHeight);
    painter.drawLine(margin + colWidth, y, margin + colWidth, y + rowHeight);
    painter.setPen(Qt::white);
    painter.setFont(headerFont);
    painter.drawText(margin + 15, y + 21, "CIN");
    painter.drawText(margin + colWidth + 15, y + 21, "Poste");
    y += rowHeight;
    
    // Ligne 2: CIN | Poste
    painter.setPen(colorText);
    painter.setFont(normalFont);
    painter.fillRect(margin, y, tableWidth, rowHeight, QBrush(QColor(250, 245, 240)));
    painter.drawRect(margin, y, tableWidth, rowHeight);
    painter.drawLine(margin + colWidth, y, margin + colWidth, y + rowHeight);
    painter.drawText(margin + 15, y + 21, donnees.cin);
    painter.drawText(margin + colWidth + 15, y + 21, donnees.poste);
    y += rowHeight;
    
    // En-tête ligne 3
    painter.fillRect(margin, y, tableWidth, rowHeight, QBrush(QColor(139, 90, 70)));
    painter.drawRect(margin, y, tableWidth, rowHeight);
    painter.drawLine(margin + colWidth, y, margin + colWidth, y + rowHeight);
    painter.setPen(Qt::white);
    painter.setFont(headerFont);
    painter.drawText(margin + 15, y + 21, "Département");
    painter.drawText(margin + colWidth + 15, y + 21, "Date d'Embauche");
    y += rowHeight;
    
    // Ligne 3: Département | Date embauche
    painter.setPen(colorText);
    painter.setFont(normalFont);
    painter.fillRect(margin, y, tableWidth, rowHeight, QBrush(QColor(250, 245, 240)));
    painter.drawRect(margin, y, tableWidth, rowHeight);
    painter.drawLine(margin + colWidth, y, margin + colWidth, y + rowHeight);
    painter.drawText(margin + 15, y + 21, donnees.departement);
    painter.drawText(margin + colWidth + 15, y + 21, donnees.dateEmbauche.toString("dd/MM/yyyy"));
    y += rowHeight + 60;
    
    // === CALCULS ===
    double cnss = calculerCNSS(donnees.salaireBrut);
    double salaireImposable = donnees.salaireBrut - cnss;
    double irpp = calculerIRPP(salaireImposable);
    double totalGains = donnees.salaireBrut + donnees.primeRendement +
                       donnees.primeAnciennete + donnees.primeAutres;
    double totalRetenues = cnss + irpp + donnees.retenues + donnees.avances;
    double netAPayer = totalGains - totalRetenues;
    
    // === SECTION GAINS ===
    painter.setFont(headerFont);
    painter.setPen(colorPrimary);
    painter.drawText(margin, y, "GAINS");
    y += 40;
    
    painter.setFont(normalFont);
    painter.setPen(colorText);
    
    int tableW = pageWidth - 2*margin;
    int rowH = 32;
    
    // En-tête tableau GAINS
    painter.fillRect(margin, y, tableW, rowH, QBrush(QColor(139, 90, 70)));
    painter.setPen(QPen(colorText, 2));
    painter.drawRect(margin, y, tableW, rowH);
    painter.setPen(Qt::white);
    painter.setFont(headerFont);
    painter.drawText(margin + 15, y + 21, "Désignation");
    painter.drawText(pageWidth - margin - 200, y + 21, "Montant (TND)");
    y += rowH;
    
    // Lignes gains avec alternance de couleurs
    painter.setFont(normalFont);
    painter.setPen(colorText);
    bool alternate = false;
    
    // Salaire Brut
    painter.fillRect(margin, y, tableW, rowH, alternate ? QBrush(QColor(250, 245, 240)) : QBrush(Qt::white));
    painter.drawRect(margin, y, tableW, rowH);
    painter.drawText(margin + 15, y + 21, "Salaire Brut");
    painter.drawText(pageWidth - margin - 200, y + 21, QString::number(donnees.salaireBrut, 'f', 3));
    y += rowH;
    alternate = !alternate;
    
    if (donnees.primeRendement > 0) {
        painter.fillRect(margin, y, tableW, rowH, alternate ? QBrush(QColor(250, 245, 240)) : QBrush(Qt::white));
        painter.drawRect(margin, y, tableW, rowH);
        painter.drawText(margin + 15, y + 21, "Prime de Rendement");
        painter.drawText(pageWidth - margin - 200, y + 21, QString::number(donnees.primeRendement, 'f', 3));
        y += rowH;
        alternate = !alternate;
    }
    
    if (donnees.primeAnciennete > 0) {
        painter.fillRect(margin, y, tableW, rowH, alternate ? QBrush(QColor(250, 245, 240)) : QBrush(Qt::white));
        painter.drawRect(margin, y, tableW, rowH);
        painter.drawText(margin + 15, y + 21, "Prime d'Ancienneté");
        painter.drawText(pageWidth - margin - 200, y + 21, QString::number(donnees.primeAnciennete, 'f', 3));
        y += rowH;
        alternate = !alternate;
    }
    
    if (donnees.primeAutres > 0) {
        painter.fillRect(margin, y, tableW, rowH, alternate ? QBrush(QColor(250, 245, 240)) : QBrush(Qt::white));
        painter.drawRect(margin, y, tableW, rowH);
        painter.drawText(margin + 15, y + 21, "Autres Primes");
        painter.drawText(pageWidth - margin - 200, y + 21, QString::number(donnees.primeAutres, 'f', 3));
        y += rowH;
    }
    
    // Total gains
    painter.setFont(headerFont);
    painter.fillRect(margin, y, tableW, 36, QBrush(QColor(232, 245, 233)));
    painter.setPen(QPen(QColor(46, 125, 50), 3));
    painter.drawRect(margin, y, tableW, 36);
    painter.setPen(QColor(46, 125, 50));
    painter.drawText(margin + 15, y + 24, "TOTAL GAINS");
    painter.drawText(pageWidth - margin - 200, y + 24, QString::number(totalGains, 'f', 3) + " TND");
    y += 70;
    
    // === SECTION RETENUES ===
    painter.setFont(headerFont);
    painter.setPen(colorPrimary);
    painter.drawText(margin, y, "RETENUES");
    y += 40;
    
    painter.setFont(normalFont);
    painter.setPen(colorText);
    
    // En-tête tableau RETENUES
    painter.fillRect(margin, y, tableW, rowH, QBrush(QColor(139, 90, 70)));
    painter.setPen(QPen(colorText, 2));
    painter.drawRect(margin, y, tableW, rowH);
    painter.setPen(Qt::white);
    painter.setFont(headerFont);
    painter.drawText(margin + 15, y + 21, "Désignation");
    painter.drawText(pageWidth - margin - 200, y + 21, "Montant (TND)");
    y += rowH;
    
    // Lignes retenues avec alternance
    painter.setFont(normalFont);
    painter.setPen(colorText);
    alternate = false;
    
    // CNSS
    painter.fillRect(margin, y, tableW, rowH, alternate ? QBrush(QColor(250, 245, 240)) : QBrush(Qt::white));
    painter.drawRect(margin, y, tableW, rowH);
    painter.drawText(margin + 15, y + 21, "CNSS Salarié (9.18%)");
    painter.drawText(pageWidth - margin - 200, y + 21, QString::number(cnss, 'f', 3));
    y += rowH;
    alternate = !alternate;
    
    // IRPP
    painter.fillRect(margin, y, tableW, rowH, alternate ? QBrush(QColor(250, 245, 240)) : QBrush(Qt::white));
    painter.drawRect(margin, y, tableW, rowH);
    painter.drawText(margin + 15, y + 21, "IRPP");
    painter.drawText(pageWidth - margin - 200, y + 21, QString::number(irpp, 'f', 3));
    y += rowH;
    alternate = !alternate;
    
    if (donnees.retenues > 0) {
        painter.fillRect(margin, y, tableW, rowH, alternate ? QBrush(QColor(250, 245, 240)) : QBrush(Qt::white));
        painter.drawRect(margin, y, tableW, rowH);
        painter.drawText(margin + 15, y + 21, "Autres Retenues");
        painter.drawText(pageWidth - margin - 200, y + 21, QString::number(donnees.retenues, 'f', 3));
        y += rowH;
        alternate = !alternate;
    }
    
    if (donnees.avances > 0) {
        painter.fillRect(margin, y, tableW, rowH, alternate ? QBrush(QColor(250, 245, 240)) : QBrush(Qt::white));
        painter.drawRect(margin, y, tableW, rowH);
        painter.drawText(margin + 15, y + 21, "Avances");
        painter.drawText(pageWidth - margin - 200, y + 21, QString::number(donnees.avances, 'f', 3));
        y += rowH;
    }
    
    // Total retenues
    painter.setFont(headerFont);
    painter.fillRect(margin, y, tableW, 36, QBrush(QColor(255, 235, 238)));
    painter.setPen(QPen(QColor(198, 40, 40), 3));
    painter.drawRect(margin, y, tableW, 36);
    painter.setPen(QColor(198, 40, 40));
    painter.drawText(margin + 15, y + 24, "TOTAL RETENUES");
    painter.drawText(pageWidth - margin - 200, y + 24, QString::number(totalRetenues, 'f', 3) + " TND");
    y += 70;
    
    // === NET À PAYER ===
    QFont netFont("Arial", 14, QFont::Bold);
    painter.setFont(netFont);
    painter.fillRect(margin, y, tableW, 55, QBrush(QColor(255, 243, 224)));
    painter.setPen(QPen(colorPrimary, 4));
    painter.drawRect(margin, y, tableW, 55);
    painter.setPen(colorPrimary);
    painter.drawText(margin + 20, y + 36, "NET À PAYER");
    
    QFont netAmountFont("Arial", 16, QFont::Bold);
    painter.setFont(netAmountFont);
    painter.drawText(pageWidth - margin - 280, y + 36, QString::number(netAPayer, 'f', 3) + " TND");
    y += 100;
    
    // === SIGNATURES ===
    painter.setFont(normalFont);
    painter.setPen(colorText);
    
    int sigWidth = 250;
    int sigSpacing = (tableW - 2*sigWidth) / 3;
    
    painter.drawText(margin + sigSpacing, y, "Signature Employeur");
    painter.drawText(pageWidth - margin - sigSpacing - sigWidth + 50, y, "Signature Employé");
    
    painter.setPen(QPen(colorSecondary, 2));
    painter.drawLine(margin + sigSpacing, y + 70, margin + sigSpacing + sigWidth, y + 70);
    painter.drawLine(pageWidth - margin - sigSpacing - sigWidth, y + 70, 
                    pageWidth - margin - sigSpacing, y + 70);
    y += 100;
    
    // === PIED DE PAGE ===
    painter.setFont(smallFont);
    painter.setPen(colorSecondary);
    QString piedPage = QString("Document confidentiel - Généré le %1 - CUIREA")
                      .arg(QDate::currentDate().toString("dd/MM/yyyy"));
    QRect piedRect(margin, y, tableW, 30);
    painter.drawText(piedRect, Qt::AlignCenter, piedPage);
    
    y += 25;
    QRect piedRect2(margin, y, tableW, 30);
    painter.drawText(piedRect2, Qt::AlignCenter, 
                    "Ce document est strictement personnel et confidentiel");
    
    painter.end();
    return true;
}

double FichePaiePDF::calculerCNSS(double salaireBrut)
{
    return salaireBrut * 0.0918;
}

double FichePaiePDF::calculerIRPP(double salaireImposable)
{
    double irpp = 0.0;
    if (salaireImposable <= 5000) {
        irpp = 0;
    } else if (salaireImposable <= 20000) {
        irpp = (salaireImposable - 5000) * 0.26;
    } else if (salaireImposable <= 30000) {
        irpp = 3900 + (salaireImposable - 20000) * 0.28;
    } else if (salaireImposable <= 50000) {
        irpp = 6700 + (salaireImposable - 30000) * 0.32;
    } else {
        irpp = 13100 + (salaireImposable - 50000) * 0.35;
    }
    return irpp;
}