#include "deleteemployeedialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QDate>
#include <QMessageBox>

DeleteEmployeeDialog::DeleteEmployeeDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    applyStyles();
    
    setWindowTitle("Supprimer un Employé - CUIREA (Mode Statique)");
    setMinimumSize(950, 550);
    
    // Buttons do nothing in static mode
    connect(deleteButton, &QPushButton::clicked, [this]() {
        QMessageBox::information(this, "Mode Statique", 
            "Cette interface est en mode statique.\n"
            "Les données ne seront pas supprimées.");
        reject();
    });
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

DeleteEmployeeDialog::~DeleteEmployeeDialog()
{
}

void DeleteEmployeeDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    
    // Title
    QLabel *titleLabel = new QLabel("Supprimer l'Employé", this);
    titleLabel->setObjectName("dialogTitle");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    
    // Warning message
    QLabel *warningLabel = new QLabel("⚠ Êtes-vous sûr de vouloir supprimer cet employé ?", this);
    warningLabel->setObjectName("warningLabel");
    warningLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(warningLabel);
    
    // Three column layout
    QHBoxLayout *columnsLayout = new QHBoxLayout();
    columnsLayout->setSpacing(30);
    
    // LEFT COLUMN
    QFormLayout *leftFormLayout = new QFormLayout();
    leftFormLayout->setSpacing(15);
    leftFormLayout->setLabelAlignment(Qt::AlignRight);
    
    matriculeEdit = new QLineEdit(this);
    matriculeEdit->setText("EMP-2024-001");
    matriculeEdit->setReadOnly(true);
    
    nomEdit = new QLineEdit(this);
    nomEdit->setText("Benali");
    nomEdit->setReadOnly(true);
    
    prenomEdit = new QLineEdit(this);
    prenomEdit->setText("Ahmed");
    prenomEdit->setReadOnly(true);
    
    cinEdit = new QLineEdit(this);
    cinEdit->setText("AB123456");
    cinEdit->setReadOnly(true);
    
    dateNaissanceEdit = new QDateEdit(this);
    dateNaissanceEdit->setDate(QDate(1985, 3, 15));
    dateNaissanceEdit->setCalendarPopup(false);
    dateNaissanceEdit->setDisplayFormat("dd/MM/yyyy");
    dateNaissanceEdit->setReadOnly(true);
    
    sexeCombo = new QComboBox(this);
    sexeCombo->addItems({"M", "F"});
    sexeCombo->setCurrentText("M");
    sexeCombo->setEnabled(false);
    
    leftFormLayout->addRow("Matricule:", matriculeEdit);
    leftFormLayout->addRow("Nom:", nomEdit);
    leftFormLayout->addRow("Prénom:", prenomEdit);
    leftFormLayout->addRow("CIN:", cinEdit);
    leftFormLayout->addRow("Date Naissance:", dateNaissanceEdit);
    leftFormLayout->addRow("Sexe:", sexeCombo);
    
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
    
    photoUrlEdit = new QLineEdit(this);
    photoUrlEdit->setPlaceholderText("Aucune photo");
    photoUrlEdit->setReadOnly(true);
    photoLayout->addWidget(photoUrlEdit);
    
    photoLayout->addStretch();
    
    // RIGHT COLUMN
    QFormLayout *rightFormLayout = new QFormLayout();
    rightFormLayout->setSpacing(15);
    rightFormLayout->setLabelAlignment(Qt::AlignRight);
    
    adresseEdit = new QLineEdit(this);
    adresseEdit->setText("Casablanca");
    adresseEdit->setReadOnly(true);
    
    telephoneEdit = new QLineEdit(this);
    telephoneEdit->setText("0612345678");
    telephoneEdit->setReadOnly(true);
    
    emailEdit = new QLineEdit(this);
    emailEdit->setText("a.benali@cuirea.com");
    emailEdit->setReadOnly(true);
    
    posteEdit = new QLineEdit(this);
    posteEdit->setText("Supervisor");
    posteEdit->setReadOnly(true);
    
    specialiteEdit = new QLineEdit(this);
    specialiteEdit->setText("Production");
    specialiteEdit->setReadOnly(true);
    
    departementCombo = new QComboBox(this);
    departementCombo->addItems({
        "Production", "Quality Control", "Cutting", "Stitching",
        "Finishing", "Design", "Warehouse", "Administration",
        "Maintenance", "Packaging"
    });
    departementCombo->setCurrentText("Production");
    departementCombo->setEnabled(false);
    
    dateEmbaucheEdit = new QDateEdit(this);
    dateEmbaucheEdit->setDate(QDate(2024, 1, 1));
    dateEmbaucheEdit->setCalendarPopup(false);
    dateEmbaucheEdit->setDisplayFormat("dd/MM/yyyy");
    dateEmbaucheEdit->setReadOnly(true);
    
    rightFormLayout->addRow("Adresse:", adresseEdit);
    rightFormLayout->addRow("Téléphone:", telephoneEdit);
    rightFormLayout->addRow("Email:", emailEdit);
    rightFormLayout->addRow("Poste:", posteEdit);
    rightFormLayout->addRow("Spécialité:", specialiteEdit);
    rightFormLayout->addRow("Département:", departementCombo);
    rightFormLayout->addRow("Date Embauche:", dateEmbaucheEdit);
    
    columnsLayout->addLayout(leftFormLayout);
    columnsLayout->addLayout(photoLayout);
    columnsLayout->addLayout(rightFormLayout);
    
    mainLayout->addLayout(columnsLayout);
    
    // Note
    QLabel *noteLabel = new QLabel("Mode Statique - Aucune donnée ne sera supprimée", this);
    noteLabel->setObjectName("noteLabel");
    mainLayout->addWidget(noteLabel);
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    deleteButton = new QPushButton("Supprimer", this);
    deleteButton->setObjectName("deleteButton");
    deleteButton->setMinimumSize(120, 40);
    
    cancelButton = new QPushButton("Annuler", this);
    cancelButton->setObjectName("cancelButton");
    cancelButton->setMinimumSize(120, 40);
    
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
}

void DeleteEmployeeDialog::applyStyles()
{
    setStyleSheet(
        "QDialog { background-color: #FAF5F0; }"
        "QLabel#dialogTitle { color: #291C0E; font-family: 'Times New Roman', serif; font-size: 20px; font-weight: bold; padding: 10px; }"
        "QLabel#warningLabel { color: #D32F2F; font-family: Arial, sans-serif; font-size: 14px; font-weight: bold; padding: 10px; background-color: #FFEBEE; border: 2px solid #EF5350; border-radius: 8px; }"
        "QLabel#sectionLabel { color: #8D6E63; font-family: 'Times New Roman', serif; font-size: 14px; font-weight: bold; }"
        "QLabel#photoPreview { background-color: #FFF8F0; border: 3px solid #BCAAA4; border-radius: 8px; color: #BCAAA4; font-size: 60px; }"
        "QLabel#noteLabel { color: #8D6E63; font-family: Arial, sans-serif; font-size: 10px; font-style: italic; }"
        "QLabel { color: #291C0E; font-family: Arial, sans-serif; font-size: 12px; font-weight: bold; }"
        "QLineEdit, QComboBox, QDateEdit { background-color: #F5F5F5; color: #757575; border: 2px solid #BCAAA4; border-radius: 6px; padding: 8px; font-family: Arial, sans-serif; font-size: 11px; min-height: 25px; }"
        "QLineEdit:read-only, QDateEdit:read-only { background-color: #F5F5F5; }"
        "QComboBox:disabled, QDateEdit:read-only { background-color: #F5F5F5; color: #757575; }"
        "QPushButton#deleteButton { background-color: #D32F2F; color: white; border: none; border-radius: 10px; padding: 10px 20px; font-family: Arial, sans-serif; font-size: 12px; font-weight: bold; }"
        "QPushButton#deleteButton:hover { background-color: #E53935; }"
        "QPushButton#cancelButton { background-color: #E7DDD1; color: #291C0E; border: 1px solid #BCAAA4; border-radius: 10px; padding: 10px 20px; font-family: Arial, sans-serif; font-size: 12px; font-weight: bold; }"
        "QPushButton#cancelButton:hover { background-color: #F0E6DA; }"
    );
}
