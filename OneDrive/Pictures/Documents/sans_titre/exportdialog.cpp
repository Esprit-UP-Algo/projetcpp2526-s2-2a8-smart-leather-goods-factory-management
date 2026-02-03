#include "exportdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>

ExportDialog::ExportDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    applyStyles();
    
    setWindowTitle("Exporter les Données - CUIREA (Mode Statique)");
    setMinimumSize(700, 500);
    
    // Buttons do nothing in static mode
    connect(exportButton, &QPushButton::clicked, [this]() {
        QMessageBox::information(this, "Mode Statique", 
            "Cette interface est en mode statique.\n"
            "Les données ne seront pas exportées.");
        reject();
    });
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(browseButton, &QPushButton::clicked, [this]() {
        QMessageBox::information(this, "Mode Statique", 
            "Fonctionnalité de parcours désactivée en mode statique.");
    });
}

ExportDialog::~ExportDialog()
{
}

void ExportDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    
    // Title
    QLabel *titleLabel = new QLabel("Exporter les Données des Employés", this);
    titleLabel->setObjectName("dialogTitle");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    
    // Format selection group
    QGroupBox *formatGroup = new QGroupBox("Format d'Export", this);
    formatGroup->setObjectName("groupBox");
    QVBoxLayout *formatLayout = new QVBoxLayout(formatGroup);
    formatLayout->setSpacing(15);
    
    csvRadio = new QRadioButton("CSV (Excel) - Fichier compatible avec Excel et tableurs", this);
    csvRadio->setObjectName("radioButton");
    csvRadio->setChecked(true);
    
    htmlRadio = new QRadioButton("HTML - Page web avec style CUIREA", this);
    htmlRadio->setObjectName("radioButton");
    
    txtRadio = new QRadioButton("TXT - Fichier texte formaté", this);
    txtRadio->setObjectName("radioButton");
    
    formatLayout->addWidget(csvRadio);
    formatLayout->addWidget(htmlRadio);
    formatLayout->addWidget(txtRadio);
    
    mainLayout->addWidget(formatGroup);
    
    // File settings
    QGroupBox *fileGroup = new QGroupBox("Paramètres du Fichier", this);
    fileGroup->setObjectName("groupBox");
    QFormLayout *fileLayout = new QFormLayout(fileGroup);
    fileLayout->setSpacing(15);
    fileLayout->setLabelAlignment(Qt::AlignRight);
    
    fileNameEdit = new QLineEdit(this);
    fileNameEdit->setText("employes_export");
    fileNameEdit->setPlaceholderText("Nom du fichier");
    
    destinationEdit = new QLineEdit(this);
    destinationEdit->setText("C:/Users/Documents/");
    destinationEdit->setPlaceholderText("Chemin de destination");
    destinationEdit->setReadOnly(true);
    
    browseButton = new QPushButton("Parcourir...", this);
    browseButton->setObjectName("browseButton");
    browseButton->setMinimumHeight(35);
    browseButton->setEnabled(false); // Disabled in static mode
    
    QHBoxLayout *destLayout = new QHBoxLayout();
    destLayout->addWidget(destinationEdit);
    destLayout->addWidget(browseButton);
    
    fileLayout->addRow("Nom du fichier:", fileNameEdit);
    fileLayout->addRow("Destination:", destLayout);
    
    mainLayout->addWidget(fileGroup);
    
    // Note
    QLabel *noteLabel = new QLabel("Mode Statique - Aucune donnée ne sera exportée", this);
    noteLabel->setObjectName("noteLabel");
    mainLayout->addWidget(noteLabel);
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    exportButton = new QPushButton("Exporter", this);
    exportButton->setObjectName("exportButton");
    exportButton->setMinimumSize(120, 40);
    
    cancelButton = new QPushButton("Annuler", this);
    cancelButton->setObjectName("cancelButton");
    cancelButton->setMinimumSize(120, 40);
    
    buttonLayout->addWidget(exportButton);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
}

void ExportDialog::applyStyles()
{
    setStyleSheet(
        "QDialog { background-color: #FAF5F0; }"
        "QLabel#dialogTitle { color: #291C0E; font-family: 'Times New Roman', serif; font-size: 20px; font-weight: bold; padding: 10px; }"
        "QLabel#noteLabel { color: #8D6E63; font-family: Arial, sans-serif; font-size: 10px; font-style: italic; }"
        "QLabel { color: #291C0E; font-family: Arial, sans-serif; font-size: 12px; font-weight: bold; }"
        "QGroupBox { font-family: 'Times New Roman', serif; font-size: 14px; font-weight: bold; color: #8D6E63; border: 2px solid #BCAAA4; border-radius: 8px; margin-top: 10px; padding-top: 10px; }"
        "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; padding: 5px 10px; background-color: #FAF5F0; }"
        "QRadioButton { color: #291C0E; font-family: Arial, sans-serif; font-size: 12px; font-weight: normal; spacing: 8px; }"
        "QRadioButton::indicator { width: 18px; height: 18px; }"
        "QRadioButton::indicator::unchecked { border: 2px solid #BCAAA4; border-radius: 9px; background-color: white; }"
        "QRadioButton::indicator::checked { border: 2px solid #8D6E63; border-radius: 9px; background-color: #8D6E63; }"
        "QLineEdit { background-color: white; color: #291C0E; border: 2px solid #BCAAA4; border-radius: 6px; padding: 8px; font-family: Arial, sans-serif; font-size: 11px; min-height: 25px; }"
        "QLineEdit:focus { border: 2px solid #8D6E63; }"
        "QLineEdit:read-only { background-color: #F5F5F5; color: #757575; }"
        "QLineEdit::placeholder { color: #BCAAA4; }"
        "QPushButton#browseButton { background-color: #A0826D; color: white; border: none; border-radius: 8px; padding: 8px 15px; font-family: Arial, sans-serif; font-size: 11px; font-weight: bold; }"
        "QPushButton#browseButton:hover { background-color: #B89580; }"
        "QPushButton#browseButton:disabled { background-color: #BCAAA4; }"
        "QPushButton#exportButton { background-color: #8D6E63; color: white; border: none; border-radius: 10px; padding: 10px 20px; font-family: Arial, sans-serif; font-size: 12px; font-weight: bold; }"
        "QPushButton#exportButton:hover { background-color: #A0826D; }"
        "QPushButton#cancelButton { background-color: #E7DDD1; color: #291C0E; border: 1px solid #BCAAA4; border-radius: 10px; padding: 10px 20px; font-family: Arial, sans-serif; font-size: 12px; font-weight: bold; }"
        "QPushButton#cancelButton:hover { background-color: #F0E6DA; }"
    );
}
