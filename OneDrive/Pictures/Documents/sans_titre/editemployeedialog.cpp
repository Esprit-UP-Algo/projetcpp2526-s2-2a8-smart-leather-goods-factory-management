#include "editemployeedialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QDate>
#include <QMessageBox>

EditEmployeeDialog::EditEmployeeDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    applyStyles();
    
    setWindowTitle("Modifier un Employé - CUIREA (Mode Statique)");
    setMinimumSize(950, 550);
    
    // Buttons do nothing in static mode
    connect(saveButton, &QPushButton::clicked, [this]() {
        QMessageBox::information(this, "Mode Statique", 
            "Cette interface est en mode statique.\n"
            "Les données ne seront pas enregistrées.");
        reject();
    });
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

EditEmployeeDialog::~EditEmployeeDialog()
{
}

void EditEmployeeDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    
    // Title
    QLabel *titleLabel = new QLabel("Modifier les Informations de l'Employé", this);
    titleLabel->setObjectName("dialogTitle");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    
    // Three column layout
    QHBoxLayout *columnsLayout = new QHBoxLayout();
    columnsLayout->setSpacing(30);
    
    // LEFT COLUMN
    QFormLayout *leftFormLayout = new QFormLayout();
    leftFormLayout->setSpacing(15);
    leftFormLayout->setLabelAlignment(Qt::AlignRight);
    
    matriculeEdit = new QLineEdit(this);
    matriculeEdit->setText("EMP-2024-001");
    
    nomEdit = new QLineEdit(this);
    nomEdit->setText("Benali");
    
    prenomEdit = new QLineEdit(this);
    prenomEdit->setText("Ahmed");
    
    cinEdit = new QLineEdit(this);
    cinEdit->setText("AB123456");
    
    dateNaissanceEdit = new QDateEdit(this);
    dateNaissanceEdit->setDate(QDate(1985, 3, 15));
    dateNaissanceEdit->setCalendarPopup(true);
    dateNaissanceEdit->setDisplayFormat("dd/MM/yyyy");
    
    sexeCombo = new QComboBox(this);
    sexeCombo->addItems({"M", "F"});
    sexeCombo->setCurrentText("M");
    
    leftFormLayout->addRow("Matricule *:", matriculeEdit);
    leftFormLayout->addRow("Nom *:", nomEdit);
    leftFormLayout->addRow("Prénom *:", prenomEdit);
    leftFormLayout->addRow("CIN *:", cinEdit);
    leftFormLayout->addRow("Date Naissance *:", dateNaissanceEdit);
    leftFormLayout->addRow("Sexe *:", sexeCombo);
    
    // PHOTO SECTION (Center)
    QVBoxLayout *photoLayout = new QVBoxLayout();
    photoLayout->setSpacing(10);
    
    QLabel *photoLabel = new QLabel("Photo", this);
    photoLabel->setObjectName("sectionLabel");
    photoLabel->setAlignment(Qt::AlignCenter);
    photoLayout->addWidget(photoLabel);
    
    photoPreview = new QLabel(this);
    photoPreview->setObjectName("photoPreview");
    photoPreview->setFixedSize(150, 150);
    photoPreview->setAlignment(Qt::AlignCenter);
    photoPreview->setText("●");
    photoLayout->addWidget(photoPreview, 0, Qt::AlignCenter);
    
    browsePhotoButton = new QPushButton("Parcourir...", this);
    browsePhotoButton->setObjectName("browseButton");
    browsePhotoButton->setMinimumHeight(35);
    browsePhotoButton->setEnabled(false); // Disabled in static mode
    photoLayout->addWidget(browsePhotoButton);
    
    photoUrlEdit = new QLineEdit(this);
    photoUrlEdit->setPlaceholderText("Chemin de la photo");
    photoUrlEdit->setReadOnly(true);
    photoLayout->addWidget(photoUrlEdit);
    
    photoLayout->addStretch();
    
    // RIGHT COLUMN
    QFormLayout *rightFormLayout = new QFormLayout();
    rightFormLayout->setSpacing(15);
    rightFormLayout->setLabelAlignment(Qt::AlignRight);
    
    adresseEdit = new QLineEdit(this);
    adresseEdit->setText("Casablanca");
    
    telephoneEdit = new QLineEdit(this);
    telephoneEdit->setText("0612345678");
    
    emailEdit = new QLineEdit(this);
    emailEdit->setText("a.benali@cuirea.com");
    
    posteEdit = new QLineEdit(this);
    posteEdit->setText("Supervisor");
    
    specialiteEdit = new QLineEdit(this);
    specialiteEdit->setText("Production");
    
    departementCombo = new QComboBox(this);
    departementCombo->addItems({
        "Production", "Quality Control", "Cutting", "Stitching",
        "Finishing", "Design", "Warehouse", "Administration",
        "Maintenance", "Packaging"
    });
    departementCombo->setCurrentText("Production");
    
    dateEmbaucheEdit = new QDateEdit(this);
    dateEmbaucheEdit->setDate(QDate(2024, 1, 1));
    dateEmbaucheEdit->setCalendarPopup(true);
    dateEmbaucheEdit->setDisplayFormat("dd/MM/yyyy");
    
    rightFormLayout->addRow("Adresse:", adresseEdit);
    rightFormLayout->addRow("Téléphone *:", telephoneEdit);
    rightFormLayout->addRow("Email *:", emailEdit);
    rightFormLayout->addRow("Poste *:", posteEdit);
    rightFormLayout->addRow("Spécialité:", specialiteEdit);
    rightFormLayout->addRow("Département *:", departementCombo);
    rightFormLayout->addRow("Date Embauche *:", dateEmbaucheEdit);
    
    columnsLayout->addLayout(leftFormLayout);
    columnsLayout->addLayout(photoLayout);
    columnsLayout->addLayout(rightFormLayout);
    
    mainLayout->addLayout(columnsLayout);
    
    // Required fields note
    QLabel *noteLabel = new QLabel("* Champs obligatoires (Mode Statique - Aucune donnée ne sera enregistrée)", this);
    noteLabel->setObjectName("noteLabel");
    mainLayout->addWidget(noteLabel);
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    saveButton = new QPushButton("Enregistrer", this);
    saveButton->setObjectName("saveButton");
    saveButton->setMinimumSize(120, 40);
    
    cancelButton = new QPushButton("Annuler", this);
    cancelButton->setObjectName("cancelButton");
    cancelButton->setMinimumSize(120, 40);
    
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
}

void EditEmployeeDialog::applyStyles()
{
    setStyleSheet(
        "QDialog { background-color: #FAF5F0; }"
        "QLabel#dialogTitle { color: #291C0E; font-family: 'Times New Roman', serif; font-size: 20px; font-weight: bold; padding: 10px; }"
        "QLabel#sectionLabel { color: #8D6E63; font-family: 'Times New Roman', serif; font-size: 14px; font-weight: bold; }"
        "QLabel#photoPreview { background-color: #FFF8F0; border: 3px solid #BCAAA4; border-radius: 8px; color: #BCAAA4; font-size: 60px; }"
        "QLabel#noteLabel { color: #8D6E63; font-family: Arial, sans-serif; font-size: 10px; font-style: italic; }"
        "QLabel { color: #291C0E; font-family: Arial, sans-serif; font-size: 12px; font-weight: bold; }"
        "QLineEdit, QComboBox, QDateEdit { background-color: white; color: #291C0E; border: 2px solid #BCAAA4; border-radius: 6px; padding: 8px; font-family: Arial, sans-serif; font-size: 11px; min-height: 25px; }"
        "QLineEdit:focus, QComboBox:focus, QDateEdit:focus { border: 2px solid #8D6E63; }"
        "QLineEdit::placeholder { color: #BCAAA4; }"
        "QPushButton#browseButton { background-color: #A0826D; color: white; border: none; border-radius: 8px; padding: 8px 15px; font-family: Arial, sans-serif; font-size: 11px; font-weight: bold; }"
        "QPushButton#browseButton:hover { background-color: #B89580; }"
        "QPushButton#browseButton:disabled { background-color: #BCAAA4; }"
        "QPushButton#saveButton { background-color: #8D6E63; color: white; border: none; border-radius: 10px; padding: 10px 20px; font-family: Arial, sans-serif; font-size: 12px; font-weight: bold; }"
        "QPushButton#saveButton:hover { background-color: #A0826D; }"
        "QPushButton#cancelButton { background-color: #E7DDD1; color: #291C0E; border: 1px solid #BCAAA4; border-radius: 10px; padding: 10px 20px; font-family: Arial, sans-serif; font-size: 12px; font-weight: bold; }"
        "QPushButton#cancelButton:hover { background-color: #F0E6DA; }"
    );
}
