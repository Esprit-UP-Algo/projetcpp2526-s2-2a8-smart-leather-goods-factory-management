#include "productdialog.h"
#include <QMessageBox>
#include <QDateTime>
#include <QDebug>

ProductDialog::ProductDialog(QWidget *parent, DialogMode mode)
    : QDialog(parent), currentMode(mode),
      scrollArea(nullptr), scrollWidget(nullptr), formLayout(nullptr),
      idLabel(nullptr), idValue(nullptr), referenceEdit(nullptr), nomEdit(nullptr),
      categorieCombo(nullptr), typeEdit(nullptr), couleurEdit(nullptr), dimensionsEdit(nullptr),
      prixSpin(nullptr), coutSpin(nullptr), stockSpin(nullptr), statutCombo(nullptr),
      descriptionEdit(nullptr), refValue(nullptr), nomValue(nullptr), prixValue(nullptr),
      warningLabel(nullptr), confirmLabel(nullptr), buttonLayout(nullptr),
      confirmButton(nullptr), cancelButton(nullptr)
{
    qDebug() << "ProductDialog constructor - mode:" << mode;
    
    setModal(true);
    setMinimumSize(700, 600);
    
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    contentFrame = new QFrame(this);
    contentLayout = new QVBoxLayout(contentFrame);
    
    // Adjust margins based on mode
    if (mode == StatisticsMode) {
        contentLayout->setContentsMargins(20, 20, 20, 20);
    } else {
        contentLayout->setContentsMargins(30, 30, 30, 30);
    }
    contentLayout->setSpacing(20);
    
    titleLabel = new QLabel(contentFrame);
    titleLabel->setAlignment(Qt::AlignCenter);
    contentLayout->addWidget(titleLabel);
    
    qDebug() << "Before switch statement";
    
    switch (currentMode) {
        case AddMode:
            qDebug() << "Setting up AddMode";
            setWindowTitle("Créer un Nouveau Produit");
            setupAddMode();
            break;
        case EditMode:
            qDebug() << "Setting up EditMode";
            setWindowTitle("Modifier le Produit");
            setupEditMode();
            break;
        case DeleteMode:
            qDebug() << "Setting up DeleteMode";
            setWindowTitle("Supprimer le Produit");
            setupDeleteMode();
            break;
        case ViewMode:
            qDebug() << "Setting up ViewMode";
            setWindowTitle("Détails du Produit");
            setupViewMode();
            break;
        case StatisticsMode:
            qDebug() << "Setting up StatisticsMode";
            setWindowTitle("Statistiques des Produits");
            setupStatisticsMode();
            qDebug() << "StatisticsMode setup complete";
            break;
    }
    
    mainLayout->addWidget(contentFrame);
    applyStyles();
    
    qDebug() << "ProductDialog constructor complete";
}

ProductDialog::~ProductDialog()
{
}

void ProductDialog::setupAddMode()
{
    titleLabel->setText("Nouveau produit");
    titleLabel->setObjectName("dialogTitle");
    
    // Two column layout
    QHBoxLayout *columnsLayout = new QHBoxLayout();
    columnsLayout->setSpacing(40);
    
    // LEFT COLUMN
    QFormLayout *leftForm = new QFormLayout();
    leftForm->setLabelAlignment(Qt::AlignRight);
    leftForm->setHorizontalSpacing(14);
    leftForm->setVerticalSpacing(12);
    
    referenceEdit = new QLineEdit();
    generateReference();
    leftForm->addRow("Référence :", referenceEdit);
    
    nomEdit = new QLineEdit();
    nomEdit->setPlaceholderText("Obligatoire");
    leftForm->addRow("Nom * :", nomEdit);
    
    categorieCombo = new QComboBox();
    categorieCombo->addItems({"Sacs", "Chaussures", "Ceintures", "Portefeuilles", 
                              "Vestes", "Gants", "Accessoires"});
    leftForm->addRow("Catégorie * :", categorieCombo);
    
    typeEdit = new QLineEdit();
    typeEdit->setPlaceholderText("Cuir véritable");
    leftForm->addRow("Type :", typeEdit);
    
    couleurEdit = new QLineEdit();
    couleurEdit->setPlaceholderText("Marron");
    leftForm->addRow("Couleur :", couleurEdit);
    
    dimensionsEdit = new QLineEdit();
    dimensionsEdit->setPlaceholderText("30x25x10 cm");
    leftForm->addRow("Dimensions :", dimensionsEdit);
    
    // RIGHT COLUMN
    QFormLayout *rightForm = new QFormLayout();
    rightForm->setLabelAlignment(Qt::AlignRight);
    rightForm->setHorizontalSpacing(14);
    rightForm->setVerticalSpacing(12);
    
    prixSpin = new QDoubleSpinBox();
    prixSpin->setMaximum(999999.99);
    prixSpin->setSuffix(QString::fromUtf8(" TND"));
    rightForm->addRow("Prix Unitaire * :", prixSpin);
    
    coutSpin = new QDoubleSpinBox();
    coutSpin->setMaximum(999999.99);
    coutSpin->setSuffix(QString::fromUtf8(" TND"));
    rightForm->addRow(QString::fromUtf8("Coût Fabrication * :"), coutSpin);
    
    stockSpin = new QSpinBox();
    stockSpin->setMaximum(999999);
    rightForm->addRow("Stock Initial * :", stockSpin);
    
    statutCombo = new QComboBox();
    statutCombo->addItems({"Disponible", "En Production", "Obsolète"});
    rightForm->addRow("Statut * :", statutCombo);
    
    descriptionEdit = new QTextEdit();
    descriptionEdit->setMaximumHeight(100);
    descriptionEdit->setPlaceholderText("Description du produit...");
    rightForm->addRow("Description :", descriptionEdit);
    
    columnsLayout->addLayout(leftForm);
    columnsLayout->addLayout(rightForm);
    
    contentLayout->addLayout(columnsLayout);
    
    // Required fields note
    QLabel *noteLabel = new QLabel("* Champs obligatoires", contentFrame);
    noteLabel->setObjectName("noteLabel");
    noteLabel->setStyleSheet("color: #8D6E63; font-family: Arial, sans-serif; font-size: 10px; font-style: italic;");
    contentLayout->addWidget(noteLabel);
    
    buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    cancelButton = new QPushButton("Annuler", contentFrame);
    confirmButton = new QPushButton("Enregistrer", contentFrame);
    
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(confirmButton);
    contentLayout->addLayout(buttonLayout);
    
    connect(confirmButton, &QPushButton::clicked, this, &ProductDialog::onConfirmClicked);
    connect(cancelButton, &QPushButton::clicked, this, &ProductDialog::onCancelClicked);
}

void ProductDialog::setupEditMode()
{
    titleLabel->setText("Modifier le produit");
    titleLabel->setObjectName("dialogTitle");
    
    // Two column layout
    QHBoxLayout *columnsLayout = new QHBoxLayout();
    columnsLayout->setSpacing(40);
    
    // LEFT COLUMN
    QFormLayout *leftForm = new QFormLayout();
    leftForm->setLabelAlignment(Qt::AlignRight);
    leftForm->setHorizontalSpacing(14);
    leftForm->setVerticalSpacing(12);
    
    idLabel = new QLabel("ID :");
    idValue = new QLabel("-");
    leftForm->addRow(idLabel, idValue);
    
    referenceEdit = new QLineEdit();
    leftForm->addRow("Référence :", referenceEdit);
    
    nomEdit = new QLineEdit();
    nomEdit->setPlaceholderText("Obligatoire");
    leftForm->addRow("Nom * :", nomEdit);
    
    categorieCombo = new QComboBox();
    categorieCombo->addItems({"Sacs", "Chaussures", "Ceintures", "Portefeuilles", 
                              "Vestes", "Gants", "Accessoires"});
    leftForm->addRow("Catégorie * :", categorieCombo);
    
    typeEdit = new QLineEdit();
    typeEdit->setPlaceholderText("Cuir véritable");
    leftForm->addRow("Type :", typeEdit);
    
    couleurEdit = new QLineEdit();
    couleurEdit->setPlaceholderText("Marron");
    leftForm->addRow("Couleur :", couleurEdit);
    
    dimensionsEdit = new QLineEdit();
    dimensionsEdit->setPlaceholderText("30x25x10 cm");
    leftForm->addRow("Dimensions :", dimensionsEdit);
    
    // RIGHT COLUMN
    QFormLayout *rightForm = new QFormLayout();
    rightForm->setLabelAlignment(Qt::AlignRight);
    rightForm->setHorizontalSpacing(14);
    rightForm->setVerticalSpacing(12);
    
    prixSpin = new QDoubleSpinBox();
    prixSpin->setMaximum(999999.99);
    prixSpin->setSuffix(QString::fromUtf8(" TND"));
    rightForm->addRow("Prix Unitaire * :", prixSpin);
    
    coutSpin = new QDoubleSpinBox();
    coutSpin->setMaximum(999999.99);
    coutSpin->setSuffix(QString::fromUtf8(" TND"));
    rightForm->addRow(QString::fromUtf8("Coût Fabrication * :"), coutSpin);
    
    stockSpin = new QSpinBox();
    stockSpin->setMaximum(999999);
    rightForm->addRow("Stock * :", stockSpin);
    
    statutCombo = new QComboBox();
    statutCombo->addItems({"Disponible", "En Production", "Obsolète"});
    rightForm->addRow("Statut * :", statutCombo);
    
    descriptionEdit = new QTextEdit();
    descriptionEdit->setMaximumHeight(100);
    descriptionEdit->setPlaceholderText("Description du produit...");
    rightForm->addRow("Description :", descriptionEdit);
    
    columnsLayout->addLayout(leftForm);
    columnsLayout->addLayout(rightForm);
    
    contentLayout->addLayout(columnsLayout);
    
    // Required fields note
    QLabel *noteLabel = new QLabel("* Champs obligatoires", contentFrame);
    noteLabel->setObjectName("noteLabel");
    noteLabel->setStyleSheet("color: #8D6E63; font-family: Arial, sans-serif; font-size: 10px; font-style: italic;");
    contentLayout->addWidget(noteLabel);
    
    buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    cancelButton = new QPushButton("Annuler", contentFrame);
    confirmButton = new QPushButton("Enregistrer", contentFrame);
    
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(confirmButton);
    contentLayout->addLayout(buttonLayout);
    
    connect(confirmButton, &QPushButton::clicked, this, &ProductDialog::onConfirmClicked);
    connect(cancelButton, &QPushButton::clicked, this, &ProductDialog::onCancelClicked);
}

void ProductDialog::setupDeleteMode()
{
    titleLabel->setText("Supprimer le produit");
    titleLabel->setObjectName("dialogTitle");
    
    warningLabel = new QLabel("⚠️ ATTENTION : Cette action est irréversible !", contentFrame);
    warningLabel->setAlignment(Qt::AlignCenter);
    contentLayout->addWidget(warningLabel);
    
    QGroupBox *detailsGroup = new QGroupBox("Détails du Produit à Supprimer", contentFrame);
    QFormLayout *detailsLayout = new QFormLayout(detailsGroup);
    detailsLayout->setSpacing(15);
    
    idValue = new QLabel("-", detailsGroup);
    detailsLayout->addRow("ID :", idValue);
    
    refValue = new QLabel("-", detailsGroup);
    detailsLayout->addRow("Référence :", refValue);
    
    nomValue = new QLabel("-", detailsGroup);
    detailsLayout->addRow("Nom :", nomValue);
    
    prixValue = new QLabel("-", detailsGroup);
    detailsLayout->addRow("Prix :", prixValue);
    
    contentLayout->addWidget(detailsGroup);
    
    confirmLabel = new QLabel("Êtes-vous absolument certain de vouloir supprimer ce produit ?", contentFrame);
    confirmLabel->setAlignment(Qt::AlignCenter);
    confirmLabel->setWordWrap(true);
    contentLayout->addWidget(confirmLabel);
    
    contentLayout->addStretch();
    
    buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    cancelButton = new QPushButton("Non, Annuler", contentFrame);
    confirmButton = new QPushButton("Oui, Supprimer", contentFrame);
    
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(confirmButton);
    contentLayout->addLayout(buttonLayout);
    
    connect(confirmButton, &QPushButton::clicked, this, &ProductDialog::onConfirmClicked);
    connect(cancelButton, &QPushButton::clicked, this, &ProductDialog::onCancelClicked);
}

void ProductDialog::setupViewMode()
{
    titleLabel->setText("Détails du produit");
    titleLabel->setObjectName("dialogTitle");
    
    scrollArea = new QScrollArea(contentFrame);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollWidget = new QWidget();
    scrollWidget->setStyleSheet("QWidget { background-color: #FAF5F0; }");
    formLayout = new QFormLayout(scrollWidget);
    formLayout->setSpacing(15);
    formLayout->setHorizontalSpacing(20);
    
    idValue = new QLabel("-", scrollWidget);
    formLayout->addRow("ID :", idValue);
    
    refValue = new QLabel("-", scrollWidget);
    formLayout->addRow("Référence :", refValue);
    
    nomValue = new QLabel("-", scrollWidget);
    formLayout->addRow("Nom :", nomValue);
    
    QLabel *categorieValue = new QLabel("-", scrollWidget);
    categorieValue->setObjectName("categorieValue");
    formLayout->addRow("Catégorie :", categorieValue);
    
    QLabel *typeValue = new QLabel("-", scrollWidget);
    typeValue->setObjectName("typeValue");
    formLayout->addRow("Type :", typeValue);
    
    QLabel *couleurValue = new QLabel("-", scrollWidget);
    couleurValue->setObjectName("couleurValue");
    formLayout->addRow("Couleur :", couleurValue);
    
    QLabel *dimensionsValue = new QLabel("-", scrollWidget);
    dimensionsValue->setObjectName("dimensionsValue");
    formLayout->addRow("Dimensions :", dimensionsValue);
    
    prixValue = new QLabel("-", scrollWidget);
    formLayout->addRow("Prix Unitaire :", prixValue);
    
    QLabel *coutValue = new QLabel("-", scrollWidget);
    coutValue->setObjectName("coutValue");
    formLayout->addRow("Coût Fabrication :", coutValue);
    
    QLabel *stockValue = new QLabel("-", scrollWidget);
    stockValue->setObjectName("stockValue");
    formLayout->addRow("Stock :", stockValue);
    
    QLabel *statutValue = new QLabel("-", scrollWidget);
    statutValue->setObjectName("statutValue");
    formLayout->addRow("Statut :", statutValue);
    
    QLabel *dateValue = new QLabel("-", scrollWidget);
    dateValue->setObjectName("dateValue");
    formLayout->addRow("Date Création :", dateValue);
    
    descriptionEdit = new QTextEdit(scrollWidget);
    descriptionEdit->setMaximumHeight(100);
    descriptionEdit->setReadOnly(true);
    formLayout->addRow("Description :", descriptionEdit);
    
    scrollArea->setWidget(scrollWidget);
    contentLayout->addWidget(scrollArea);
    
    buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    cancelButton = new QPushButton("Retour", contentFrame);
    buttonLayout->addWidget(cancelButton);
    contentLayout->addLayout(buttonLayout);
    
    connect(cancelButton, &QPushButton::clicked, this, &ProductDialog::onCancelClicked);
}

void ProductDialog::setupStatisticsMode()
{
    titleLabel->setText(QString::fromUtf8("▣ Statistiques Détaillées des Produits"));
    titleLabel->setStyleSheet("color: #291C0E; font-family: 'Times New Roman', serif; font-size: 20px; font-weight: bold;");
    
    // Appliquer un style de base au dialog
    setStyleSheet("QDialog { background-color: #FAF5F0; }");
    setMinimumSize(900, 700);
    
    // Remove contentFrame styling to avoid black border
    contentFrame->setStyleSheet("QFrame { background-color: transparent; border: none; }");
    
    // Create scroll area
    QScrollArea *scrollArea = new QScrollArea(contentFrame);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet(
        "QScrollArea { border: none; background-color: #FAF5F0; }"
        "QScrollBar:vertical { border: none; background-color: #F5F5F5; width: 10px; border-radius: 5px; }"
        "QScrollBar::handle:vertical { background-color: #BCAAA4; border-radius: 5px; min-height: 20px; }"
        "QScrollBar::handle:vertical:hover { background-color: #A78D78; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }"
    );
    
    // Create main widget
    QWidget *statsWidget = new QWidget();
    statsWidget->setStyleSheet("QWidget { background-color: #FAF5F0; }");
    QVBoxLayout *mainStatsLayout = new QVBoxLayout(statsWidget);
    mainStatsLayout->setSpacing(20);
    mainStatsLayout->setContentsMargins(10, 10, 10, 10);
    
    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setSpacing(15);
    gridLayout->setContentsMargins(0, 0, 0, 0);
    
    // Card 1: Total
    QFrame *totalFrame = new QFrame(statsWidget);
    totalFrame->setMinimumSize(250, 160);
    totalFrame->setStyleSheet("QFrame { background-color: #8D6E63; border-radius: 12px; padding: 20px; }");
    QVBoxLayout *totalLayout = new QVBoxLayout(totalFrame);
    totalLayout->setSpacing(10);
    QLabel *totalTitle = new QLabel(QString::fromUtf8("📦 Total Produits"), totalFrame);
    totalTitle->setStyleSheet("color: #FFFFFF; font-size: 15px; font-weight: bold;");
    totalTitle->setAlignment(Qt::AlignCenter);
    statTotalValue = new QLabel("0", totalFrame);
    statTotalValue->setStyleSheet("color: #FFFFFF; font-size: 40px; font-weight: bold;");
    statTotalValue->setAlignment(Qt::AlignCenter);
    QLabel *totalDesc = new QLabel("Nombre total de produits", totalFrame);
    totalDesc->setStyleSheet("color: #E1D4C2; font-size: 11px;");
    totalDesc->setAlignment(Qt::AlignCenter);
    totalDesc->setWordWrap(true);
    totalLayout->addWidget(totalTitle);
    totalLayout->addWidget(statTotalValue);
    totalLayout->addWidget(totalDesc);
    gridLayout->addWidget(totalFrame, 0, 0);
    
    // Card 2: Disponibles
    QFrame *dispFrame = new QFrame(statsWidget);
    dispFrame->setMinimumSize(250, 160);
    dispFrame->setStyleSheet("QFrame { background-color: #A0826D; border-radius: 12px; padding: 20px; }");
    QVBoxLayout *dispLayout = new QVBoxLayout(dispFrame);
    dispLayout->setSpacing(10);
    QLabel *dispTitle = new QLabel(QString::fromUtf8("✓ Disponibles"), dispFrame);
    dispTitle->setStyleSheet("color: #FFFFFF; font-size: 15px; font-weight: bold;");
    dispTitle->setAlignment(Qt::AlignCenter);
    statDisponibleValue = new QLabel("0", dispFrame);
    statDisponibleValue->setStyleSheet("color: #FFFFFF; font-size: 40px; font-weight: bold;");
    statDisponibleValue->setAlignment(Qt::AlignCenter);
    QLabel *dispDesc = new QLabel(QString::fromUtf8("Prêts à la vente"), dispFrame);
    dispDesc->setStyleSheet("color: #E8F5E8; font-size: 11px;");
    dispDesc->setAlignment(Qt::AlignCenter);
    dispDesc->setWordWrap(true);
    dispLayout->addWidget(dispTitle);
    dispLayout->addWidget(statDisponibleValue);
    dispLayout->addWidget(dispDesc);
    gridLayout->addWidget(dispFrame, 0, 1);
    
    // Card 3: En Production
    QFrame *prodFrame = new QFrame(statsWidget);
    prodFrame->setMinimumSize(250, 160);
    prodFrame->setStyleSheet("QFrame { background-color: #BCAAA4; border-radius: 12px; padding: 20px; }");
    QVBoxLayout *prodLayout = new QVBoxLayout(prodFrame);
    prodLayout->setSpacing(10);
    QLabel *prodTitle = new QLabel(QString::fromUtf8("⟳ En Production"), prodFrame);
    prodTitle->setStyleSheet("color: #FFFFFF; font-size: 15px; font-weight: bold;");
    prodTitle->setAlignment(Qt::AlignCenter);
    statProductionValue = new QLabel("0", prodFrame);
    statProductionValue->setStyleSheet("color: #FFFFFF; font-size: 40px; font-weight: bold;");
    statProductionValue->setAlignment(Qt::AlignCenter);
    QLabel *prodDesc = new QLabel("En fabrication", prodFrame);
    prodDesc->setStyleSheet("color: #FFF3E0; font-size: 11px;");
    prodDesc->setAlignment(Qt::AlignCenter);
    prodDesc->setWordWrap(true);
    prodLayout->addWidget(prodTitle);
    prodLayout->addWidget(statProductionValue);
    prodLayout->addWidget(prodDesc);
    gridLayout->addWidget(prodFrame, 0, 2);
    
    // Card 4: Obsolètes
    QFrame *obsFrame = new QFrame(statsWidget);
    obsFrame->setMinimumSize(250, 160);
    obsFrame->setStyleSheet("QFrame { background-color: #6E473B; border-radius: 12px; padding: 20px; }");
    QVBoxLayout *obsLayout = new QVBoxLayout(obsFrame);
    obsLayout->setSpacing(10);
    QLabel *obsTitle = new QLabel(QString::fromUtf8("⊗ Obsolètes"), obsFrame);
    obsTitle->setStyleSheet("color: #FFFFFF; font-size: 15px; font-weight: bold;");
    obsTitle->setAlignment(Qt::AlignCenter);
    statObsoleteValue = new QLabel("0", obsFrame);
    statObsoleteValue->setStyleSheet("color: #FFFFFF; font-size: 40px; font-weight: bold;");
    statObsoleteValue->setAlignment(Qt::AlignCenter);
    QLabel *obsDesc = new QLabel("Fin de vie", obsFrame);
    obsDesc->setStyleSheet("color: #FFEBEE; font-size: 11px;");
    obsDesc->setAlignment(Qt::AlignCenter);
    obsDesc->setWordWrap(true);
    obsLayout->addWidget(obsTitle);
    obsLayout->addWidget(statObsoleteValue);
    obsLayout->addWidget(obsDesc);
    gridLayout->addWidget(obsFrame, 1, 0);
    
    // Card 5: Valeur Stock
    QFrame *valFrame = new QFrame(statsWidget);
    valFrame->setMinimumSize(250, 160);
    valFrame->setStyleSheet("QFrame { background-color: #8D6E63; border-radius: 12px; padding: 20px; }");
    QVBoxLayout *valLayout = new QVBoxLayout(valFrame);
    valLayout->setSpacing(10);
    QLabel *valTitle = new QLabel(QString::fromUtf8("▣ Valeur Stock"), valFrame);
    valTitle->setStyleSheet("color: #FFFFFF; font-size: 15px; font-weight: bold;");
    valTitle->setAlignment(Qt::AlignCenter);
    statValeurValue = new QLabel("0 TND", valFrame);
    statValeurValue->setStyleSheet("color: #FFFFFF; font-size: 32px; font-weight: bold;");
    statValeurValue->setAlignment(Qt::AlignCenter);
    QLabel *valDesc = new QLabel("Valeur totale", valFrame);
    valDesc->setStyleSheet("color: #E1D4C2; font-size: 11px;");
    valDesc->setAlignment(Qt::AlignCenter);
    valDesc->setWordWrap(true);
    valLayout->addWidget(valTitle);
    valLayout->addWidget(statValeurValue);
    valLayout->addWidget(valDesc);
    gridLayout->addWidget(valFrame, 1, 1);
    
    // Card 6: Plus Demandé
    QFrame *demFrame = new QFrame(statsWidget);
    demFrame->setMinimumSize(250, 160);
    demFrame->setStyleSheet("QFrame { background-color: #A0826D; border-radius: 12px; padding: 20px; }");
    QVBoxLayout *demLayout = new QVBoxLayout(demFrame);
    demLayout->setSpacing(10);
    QLabel *demTitle = new QLabel(QString::fromUtf8("★ Plus Demandé"), demFrame);
    demTitle->setStyleSheet("color: #FFFFFF; font-size: 15px; font-weight: bold;");
    demTitle->setAlignment(Qt::AlignCenter);
    statDemandeValue = new QLabel("N/A", demFrame);
    statDemandeValue->setStyleSheet("color: #FFFFFF; font-size: 18px; font-weight: bold;");
    statDemandeValue->setAlignment(Qt::AlignCenter);
    statDemandeValue->setWordWrap(true);
    QLabel *demDesc = new QLabel(QString::fromUtf8("Stock élevé"), demFrame);
    demDesc->setStyleSheet("color: #E8F5E8; font-size: 11px;");
    demDesc->setAlignment(Qt::AlignCenter);
    demDesc->setWordWrap(true);
    demLayout->addWidget(demTitle);
    demLayout->addWidget(statDemandeValue);
    demLayout->addWidget(demDesc);
    gridLayout->addWidget(demFrame, 1, 2);
    
    mainStatsLayout->addLayout(gridLayout);
    
    // Add detailed analysis section below the cards
    QGroupBox *analysisGroup = new QGroupBox(QString::fromUtf8("▣ Analyse Détaillée par Catégorie"), statsWidget);
    analysisGroup->setStyleSheet(
        "QGroupBox { "
        "background-color: #FFFFFF; "
        "border: 2px solid #BCAAA4; "
        "border-radius: 10px; "
        "padding: 25px 20px 20px 20px; "
        "margin-top: 20px; "
        "font-weight: bold; "
        "font-size: 14px; "
        "color: #291C0E; "
        "} "
        "QGroupBox::title { "
        "subcontrol-origin: margin; "
        "subcontrol-position: top left; "
        "left: 20px; "
        "top: 10px; "
        "padding: 5px 15px; "
        "background-color: #8D6E63; "
        "color: #FFFFFF; "
        "font-weight: bold; "
        "font-size: 13px; "
        "border-radius: 5px; "
        "}"
    );
    
    QVBoxLayout *analysisLayout = new QVBoxLayout(analysisGroup);
    analysisLayout->setSpacing(12);
    
    // Répartition par Catégorie
    QLabel *repartitionTitle = new QLabel(QString::fromUtf8("▣ Répartition par Catégorie :"), analysisGroup);
    repartitionTitle->setStyleSheet("color: #8D6E63; font-size: 12pt; font-weight: 600; margin-top: 10px;");
    analysisLayout->addWidget(repartitionTitle);
    
    // Frame pour Sacs
    QFrame *sacsFrame = new QFrame(analysisGroup);
    sacsFrame->setStyleSheet("QFrame { background-color: #FFF8F0; border-left: 4px solid #A0826D; border-radius: 6px; padding: 10px; margin: 5px 0px; }");
    QHBoxLayout *sacsLayout = new QHBoxLayout(sacsFrame);
    sacsLayout->setContentsMargins(10, 8, 10, 8);
    QLabel *sacs = new QLabel(QString::fromUtf8("• Sacs : Catégorie la plus représentée avec une forte demande"), sacsFrame);
    sacs->setStyleSheet("color: #A0826D; font-size: 11pt; font-weight: 600; border: none;");
    sacsLayout->addWidget(sacs);
    analysisLayout->addWidget(sacsFrame);
    
    // Frame pour Chaussures
    QFrame *chaussuresFrame = new QFrame(analysisGroup);
    chaussuresFrame->setStyleSheet("QFrame { background-color: #FFF8F0; border-left: 4px solid #BCAAA4; border-radius: 6px; padding: 10px; margin: 5px 0px; }");
    QHBoxLayout *chaussuresLayout = new QHBoxLayout(chaussuresFrame);
    chaussuresLayout->setContentsMargins(10, 8, 10, 8);
    QLabel *chaussures = new QLabel(QString::fromUtf8("• Chaussures : Produits premium avec marge élevée"), chaussuresFrame);
    chaussures->setStyleSheet("color: #BCAAA4; font-size: 11pt; font-weight: 600; border: none;");
    chaussuresLayout->addWidget(chaussures);
    analysisLayout->addWidget(chaussuresFrame);
    
    // Frame pour Accessoires
    QFrame *accessoiresFrame = new QFrame(analysisGroup);
    accessoiresFrame->setStyleSheet("QFrame { background-color: #FFF8F0; border-left: 4px solid #A78D78; border-radius: 6px; padding: 10px; margin: 5px 0px; }");
    QHBoxLayout *accessoiresLayout = new QHBoxLayout(accessoiresFrame);
    accessoiresLayout->setContentsMargins(10, 8, 10, 8);
    QLabel *accessoires = new QLabel(QString::fromUtf8("• Accessoires : Volume élevé, rotation rapide"), accessoiresFrame);
    accessoires->setStyleSheet("color: #A78D78; font-size: 11pt; font-weight: 600; border: none;");
    accessoiresLayout->addWidget(accessoires);
    analysisLayout->addWidget(accessoiresFrame);
    
    analysisLayout->addSpacing(15);
    
    // Recommandations Stratégiques
    QLabel *recommTitle = new QLabel(QString::fromUtf8("◆ Recommandations Stratégiques :"), analysisGroup);
    recommTitle->setStyleSheet("color: #8D6E63; font-size: 12pt; font-weight: 600;");
    analysisLayout->addWidget(recommTitle);
    
    // Frame pour Innovation
    QFrame *innovFrame = new QFrame(analysisGroup);
    innovFrame->setStyleSheet("QFrame { background-color: #FFF8F0; border-left: 4px solid #8D6E63; border-radius: 6px; padding: 12px; margin: 5px 0px; }");
    QVBoxLayout *innovLayout = new QVBoxLayout(innovFrame);
    innovLayout->setContentsMargins(10, 8, 10, 8);
    innovLayout->setSpacing(8);
    QLabel *innovTitle = new QLabel(QString::fromUtf8("Innovation :"), innovFrame);
    innovTitle->setStyleSheet("color: #8D6E63; font-size: 11pt; font-weight: 600; border: none;");
    innovLayout->addWidget(innovTitle);
    QLabel *innov1 = new QLabel(QString::fromUtf8("  - Développer une ligne de sacs écologiques en cuir recyclé"), innovFrame);
    innov1->setStyleSheet("color: #291C0E; font-size: 10pt; border: none;");
    innovLayout->addWidget(innov1);
    QLabel *innov2 = new QLabel(QString::fromUtf8("  - Lancer une collection de chaussures personnalisables"), innovFrame);
    innov2->setStyleSheet("color: #291C0E; font-size: 10pt; border: none;");
    innovLayout->addWidget(innov2);
    analysisLayout->addWidget(innovFrame);
    
    analysisLayout->addSpacing(10);
    
    // Frame pour Investissement
    QFrame *investFrame = new QFrame(analysisGroup);
    investFrame->setStyleSheet("QFrame { background-color: #FFF8F0; border-left: 4px solid #8D6E63; border-radius: 6px; padding: 12px; margin: 5px 0px; }");
    QVBoxLayout *investLayout = new QVBoxLayout(investFrame);
    investLayout->setContentsMargins(10, 8, 10, 8);
    investLayout->setSpacing(8);
    QLabel *investTitle = new QLabel(QString::fromUtf8("Investissement :"), investFrame);
    investTitle->setStyleSheet("color: #8D6E63; font-size: 11pt; font-weight: 600; border: none;");
    investLayout->addWidget(investTitle);
    QLabel *invest1 = new QLabel(QString::fromUtf8("  - Augmenter la production des accessoires (forte rotation)"), investFrame);
    invest1->setStyleSheet("color: #291C0E; font-size: 10pt; border: none;");
    investLayout->addWidget(invest1);
    QLabel *invest2 = new QLabel(QString::fromUtf8("  - Investir dans la catégorie Vestes (marge premium)"), investFrame);
    invest2->setStyleSheet("color: #291C0E; font-size: 10pt; border: none;");
    investLayout->addWidget(invest2);
    analysisLayout->addWidget(investFrame);
    
    mainStatsLayout->addWidget(analysisGroup);
    mainStatsLayout->addStretch();
    
    scrollArea->setWidget(statsWidget);
    contentLayout->addWidget(scrollArea);
    
    buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    cancelButton = new QPushButton(QString::fromUtf8("Retour"), contentFrame);
    buttonLayout->addWidget(cancelButton);
    contentLayout->addLayout(buttonLayout);
    
    connect(cancelButton, &QPushButton::clicked, this, &ProductDialog::onCancelClicked);
}

void ProductDialog::setProductData(const QString &id, const QString &ref, const QString &nom,
                                   const QString &categorie, const QString &type, const QString &couleur,
                                   const QString &dimensions, double prix, double cout, int stock,
                                   const QString &statut, const QString &description, const QString &dateCreation)
{
    if (currentMode == EditMode) {
        idValue->setText(id);
        referenceEdit->setText(ref);
        nomEdit->setText(nom);
        categorieCombo->setCurrentText(categorie);
        typeEdit->setText(type);
        couleurEdit->setText(couleur);
        dimensionsEdit->setText(dimensions);
        prixSpin->setValue(prix);
        coutSpin->setValue(cout);
        stockSpin->setValue(stock);
        statutCombo->setCurrentText(statut);
        descriptionEdit->setText(description);
    } else if (currentMode == DeleteMode) {
        idValue->setText(id);
        refValue->setText(ref);
        nomValue->setText(nom);
        prixValue->setText(QString::number(prix, 'f', 2) + QString::fromUtf8(" TND"));
    } else if (currentMode == ViewMode) {
        idValue->setText(id);
        refValue->setText(ref);
        nomValue->setText(nom);
        
        QLabel *categorieValue = scrollWidget->findChild<QLabel*>("categorieValue");
        if (categorieValue) categorieValue->setText(categorie);
        
        QLabel *typeValue = scrollWidget->findChild<QLabel*>("typeValue");
        if (typeValue) typeValue->setText(type);
        
        QLabel *couleurValue = scrollWidget->findChild<QLabel*>("couleurValue");
        if (couleurValue) couleurValue->setText(couleur);
        
        QLabel *dimensionsValue = scrollWidget->findChild<QLabel*>("dimensionsValue");
        if (dimensionsValue) dimensionsValue->setText(dimensions);
        
        prixValue->setText(QString::number(prix, 'f', 2) + QString::fromUtf8(" TND"));
        
        QLabel *coutValue = scrollWidget->findChild<QLabel*>("coutValue");
        if (coutValue) coutValue->setText(QString::number(cout, 'f', 2) + QString::fromUtf8(" TND"));
        
        QLabel *stockValue = scrollWidget->findChild<QLabel*>("stockValue");
        if (stockValue) stockValue->setText(QString::number(stock));
        
        QLabel *statutValue = scrollWidget->findChild<QLabel*>("statutValue");
        if (statutValue) statutValue->setText(statut);
        
        QLabel *dateValue = scrollWidget->findChild<QLabel*>("dateValue");
        if (dateValue) dateValue->setText(dateCreation);
        
        descriptionEdit->setText(description);
    }
}

void ProductDialog::applyStyles()
{
    qDebug() << "applyStyles() called for mode:" << currentMode;
    
    // Pour le mode statistiques, appliquer un style spécifique
    if (currentMode == StatisticsMode) {
        qDebug() << "Applying styles for StatisticsMode";
        setStyleSheet(
            "QDialog { background-color: #FAF5F0; }"
            "QPushButton { background-color: #8D6E63; color: white; border: none; "
            "border-radius: 10px; padding: 10px 30px; font-family: Arial, sans-serif; "
            "font-size: 12px; font-weight: bold; }"
            "QPushButton:hover { background-color: #A0826D; }"
            "QPushButton:pressed { background-color: #6E473B; }"
        );
        if (contentFrame) {
            contentFrame->setStyleSheet("QFrame { background-color: transparent; border: none; }");
        }
        return;
    }
    
    // Style unifié pour tous les autres modes (Add, Edit, Delete, View)
    setStyleSheet(
        "QDialog { background-color: #FAF5F0; }"
        "QLabel { color: #291C0E; font-family: Arial, sans-serif; font-size: 12px; font-weight: bold; }"
        "QLineEdit, QComboBox, QSpinBox, QDoubleSpinBox, QTextEdit, QDateEdit { "
        "background-color: white; color: #291C0E; border: 2px solid #BCAAA4; border-radius: 6px; "
        "padding: 8px; font-family: Arial, sans-serif; font-size: 11px; min-height: 25px; }"
        "QLineEdit:focus, QComboBox:focus, QSpinBox:focus, QDoubleSpinBox:focus, QTextEdit:focus, QDateEdit:focus { "
        "border: 2px solid #8D6E63; }"
        "QLineEdit::placeholder { color: #BCAAA4; }"
        "QPushButton { background-color: #8D6E63; color: white; border: none; border-radius: 10px; "
        "padding: 10px 20px; font-family: Arial, sans-serif; font-size: 12px; font-weight: bold; }"
        "QPushButton:hover { background-color: #A0826D; }"
        "QPushButton:pressed { background-color: #6E473B; }"
        "QComboBox::drop-down { border: none; width: 30px; }"
        "QComboBox::down-arrow { image: none; border-left: 5px solid transparent; border-right: 5px solid transparent; "
        "border-top: 5px solid #8D6E63; margin-right: 10px; }"
        "QSpinBox::up-button, QSpinBox::down-button, QDoubleSpinBox::up-button, QDoubleSpinBox::down-button { "
        "background-color: #E7DDD1; border: none; width: 20px; }"
        "QSpinBox::up-button:hover, QSpinBox::down-button:hover, QDoubleSpinBox::up-button:hover, QDoubleSpinBox::down-button:hover { "
        "background-color: #BCAAA4; }"
        "QGroupBox { color: #291C0E; font-family: Arial, sans-serif; font-size: 13px; font-weight: bold; "
        "border: 2px solid #BCAAA4; border-radius: 8px; margin-top: 10px; padding-top: 15px; background-color: #FAF5F0; }"
        "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; padding: 5px 10px; "
        "background-color: #FAF5F0; border-radius: 4px; }"
        "QScrollArea { border: none; background-color: #FAF5F0; }"
        "QWidget { background-color: #FAF5F0; }"
    );
    
    // Style spécifique pour le titre
    if (currentMode == DeleteMode) {
        titleLabel->setStyleSheet("color: #C62828; font-family: 'Times New Roman', serif; "
                                 "font-size: 20px; font-weight: bold; padding: 10px;");
        if (warningLabel) {
            warningLabel->setStyleSheet("color: #C62828; font-family: Arial, sans-serif; "
                                       "font-size: 12px; font-weight: bold;");
        }
        if (confirmButton) {
            confirmButton->setStyleSheet("background-color: #A0522D; color: white; border: none; "
                                        "border-radius: 10px; padding: 10px 20px; font-family: Arial, sans-serif; "
                                        "font-size: 12px; font-weight: bold;");
        }
    } else {
        titleLabel->setStyleSheet("color: #291C0E; font-family: 'Times New Roman', serif; "
                                 "font-size: 20px; font-weight: bold; padding: 10px;");
    }
    
    // Style pour contentFrame et scrollArea
    if (contentFrame) {
        contentFrame->setStyleSheet("QFrame { background-color: #FAF5F0; border: none; }");
    }
    
    if (scrollArea) {
        scrollArea->setStyleSheet("QScrollArea { border: none; background-color: #FAF5F0; }");
    }
    
    if (scrollWidget) {
        scrollWidget->setStyleSheet("QWidget { background-color: #FAF5F0; }");
    }
    
    qDebug() << "applyStyles() complete";
}

void ProductDialog::generateReference()
{
    QString ref = "PRD-" + QString::number(QDateTime::currentMSecsSinceEpoch() % 10000);
    if (referenceEdit) {
        referenceEdit->setText(ref);
    }
}

void ProductDialog::onConfirmClicked()
{
    if (currentMode == AddMode || currentMode == EditMode) {
        if (nomEdit && nomEdit->text().isEmpty()) {
            QMessageBox::warning(this, "Champ requis", "Le nom du produit est obligatoire.");
            return;
        }
        
        QMessageBox::information(this, "Simulation",
                               QString::fromUtf8("✅ Le produit serait ") + 
                               (currentMode == AddMode ? QString::fromUtf8("créé") : QString::fromUtf8("modifié")) + 
                               QString::fromUtf8(" ici (mode statique).\n\n") +
                               QString::fromUtf8("Référence : ") + (referenceEdit ? referenceEdit->text() : QString()) + QString::fromUtf8("\n") +
                               QString::fromUtf8("Nom : ") + (nomEdit ? nomEdit->text() : QString()));
    } else if (currentMode == DeleteMode) {
        QMessageBox::information(this, "Simulation",
                               "✅ Le produit serait supprimé ici (mode statique).\n\n"
                               "Cette action ne modifie pas réellement les données.");
    }
    
    accept();
}

void ProductDialog::onCancelClicked()
{
    reject();
}


void ProductDialog::setStatisticsData(int total, int disponible, int enProduction, int obsolete, 
                                     double valeurStock, const QString &plusDemande)
{
    if (currentMode != StatisticsMode) {
        return;
    }
    
    // Vérifier que tous les pointeurs existent avant de les utiliser
    if (statTotalValue) {
        statTotalValue->setText(QString::number(total));
    }
    
    if (statDisponibleValue) {
        statDisponibleValue->setText(QString::number(disponible));
    }
    
    if (statProductionValue) {
        statProductionValue->setText(QString::number(enProduction));
    }
    
    if (statObsoleteValue) {
        statObsoleteValue->setText(QString::number(obsolete));
    }
    
    if (statValeurValue) {
        statValeurValue->setText(QString::number(valeurStock, 'f', 2) + QString::fromUtf8(" TND"));
    }
    
    if (statDemandeValue) {
        statDemandeValue->setText(plusDemande);
    }
}
