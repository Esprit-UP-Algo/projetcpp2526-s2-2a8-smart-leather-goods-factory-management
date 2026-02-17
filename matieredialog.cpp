#include "matieredialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include <QDate>

MatiereDialog::MatiereDialog(QWidget *parent, DialogMode mode)
    : QDialog(parent), m_mode(mode)
{
    setupUI();
    
    switch (m_mode) {
    case AddMode:
        setupAddMode();
        break;
    case EditMode:
        setupEditMode();
        break;
    case DeleteMode:
        setupDeleteMode();
        break;
    }
    
    setMinimumWidth(600);
    setMinimumHeight(400);
}

MatiereDialog::~MatiereDialog()
{
}

void MatiereDialog::setupUI()
{
    // Create main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    
    // Title
    lblTitle = new QLabel(this);
    lblTitle->setStyleSheet("font-size: 18px; font-weight: bold; color: #8D6E63;");
    lblTitle->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(lblTitle);
    
    // Form layout
    QGridLayout *formLayout = new QGridLayout();
    formLayout->setSpacing(15);
    formLayout->setColumnStretch(1, 1);
    
    // Module
    lblModule = new QLabel("Module :", this);
    txtModule = new QLineEdit(this);
    txtModule->setPlaceholderText("Ex: Cuir Pleine Fleur");
    formLayout->addWidget(lblModule, 0, 0);
    formLayout->addWidget(txtModule, 0, 1);
    
    // Reference
    lblReference = new QLabel("Référence :", this);
    txtReference = new QLineEdit(this);
    txtReference->setPlaceholderText("Ex: Peau de Veau");
    formLayout->addWidget(lblReference, 1, 0);
    formLayout->addWidget(txtReference, 1, 1);
    
    // Type
    lblType = new QLabel("Type :", this);
    cmbType = new QComboBox(this);
    cmbType->addItems({"Cuir", "Peau de Veau", "Peau de Vachette", "Ficelinée", "Quincaillerie", "Autre"});
    formLayout->addWidget(lblType, 2, 0);
    formLayout->addWidget(cmbType, 2, 1);
    
    // Quantite
    lblQuantite = new QLabel("Consommation Moy. :", this);
    txtQuantite = new QLineEdit(this);
    txtQuantite->setPlaceholderText("Ex: 2.5 m²/jour");
    formLayout->addWidget(lblQuantite, 3, 0);
    formLayout->addWidget(txtQuantite, 3, 1);
    
    // Seuil
    lblSeuil = new QLabel("Seuil :", this);
    txtSeuil = new QLineEdit(this);
    txtSeuil->setPlaceholderText("Ex: 80");
    formLayout->addWidget(lblSeuil, 4, 0);
    formLayout->addWidget(txtSeuil, 4, 1);
    
    // Date Expiration
    lblDateExp = new QLabel("Date Expiration :", this);
    dateExpiration = new QDateEdit(this);
    dateExpiration->setCalendarPopup(true);
    dateExpiration->setDate(QDate::currentDate().addYears(2));
    formLayout->addWidget(lblDateExp, 5, 0);
    formLayout->addWidget(dateExpiration, 5, 1);
    
    mainLayout->addLayout(formLayout);
    
    // Delete warning label (hidden by default)
    lblDeleteWarning = new QLabel(this);
    lblDeleteWarning->setStyleSheet("background-color: #f7d9d9; padding: 15px; border-radius: 8px; color: #8B0000; font-weight: bold;");
    lblDeleteWarning->setWordWrap(true);
    lblDeleteWarning->setVisible(false);
    mainLayout->addWidget(lblDeleteWarning);
    
    mainLayout->addStretch();
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    btnSave = new QPushButton("Enregistrer", this);
    btnSave->setStyleSheet(
        "QPushButton { background-color: #8D6E63; color: white; border: none; border-radius: 8px; "
        "padding: 10px 25px; font-size: 13px; font-weight: bold; }"
        "QPushButton:hover { background-color: #A0826D; }"
        "QPushButton:pressed { background-color: #6E473B; }"
    );
    connect(btnSave, &QPushButton::clicked, this, &MatiereDialog::onSaveClicked);
    
    btnDelete = new QPushButton("Confirmer Suppression", this);
    btnDelete->setStyleSheet(
        "QPushButton { background-color: #D32F2F; color: white; border: none; border-radius: 8px; "
        "padding: 10px 25px; font-size: 13px; font-weight: bold; }"
        "QPushButton:hover { background-color: #F44336; }"
        "QPushButton:pressed { background-color: #B71C1C; }"
    );
    connect(btnDelete, &QPushButton::clicked, this, &MatiereDialog::onDeleteConfirmed);
    btnDelete->setVisible(false);
    
    btnCancel = new QPushButton("Annuler", this);
    btnCancel->setStyleSheet(
        "QPushButton { background-color: #BCAAA4; color: white; border: none; border-radius: 8px; "
        "padding: 10px 25px; font-size: 13px; font-weight: bold; }"
        "QPushButton:hover { background-color: #A78D78; }"
        "QPushButton:pressed { background-color: #8D7A6D; }"
    );
    connect(btnCancel, &QPushButton::clicked, this, &MatiereDialog::onCancelClicked);
    
    buttonLayout->addWidget(btnSave);
    buttonLayout->addWidget(btnDelete);
    buttonLayout->addWidget(btnCancel);
    
    mainLayout->addLayout(buttonLayout);
    
    // Apply general styling
    setStyleSheet(
        "QDialog { background-color: #FAF5F0; }"
        "QLabel { color: #291C0E; font-family: Arial, sans-serif; font-size: 12px; font-weight: bold; }"
        "QLineEdit, QDateEdit, QComboBox { background-color: white; border: 2px solid #BCAAA4; "
        "border-radius: 6px; padding: 8px; font-size: 12px; color: #291C0E; }"
        "QLineEdit:focus, QDateEdit:focus, QComboBox:focus { border-color: #8D6E63; }"
    );
}

void MatiereDialog::setupAddMode()
{
    setWindowTitle("Ajouter une Matière Première");
    lblTitle->setText("+ Ajouter une Nouvelle Matière");
    btnSave->setVisible(true);
    btnDelete->setVisible(false);
    
    // Enable all fields
    txtModule->setEnabled(true);
    txtReference->setEnabled(true);
    cmbType->setEnabled(true);
    txtQuantite->setEnabled(true);
    txtSeuil->setEnabled(true);
    dateExpiration->setEnabled(true);
}

void MatiereDialog::setupEditMode()
{
    setWindowTitle("Modifier une Matière Première");
    lblTitle->setText("✎ Modifier la Matière");
    btnSave->setText("Mettre à Jour");
    btnSave->setVisible(true);
    btnDelete->setVisible(false);
    
    // Enable all fields
    txtModule->setEnabled(true);
    txtReference->setEnabled(true);
    cmbType->setEnabled(true);
    txtQuantite->setEnabled(true);
    txtSeuil->setEnabled(true);
    dateExpiration->setEnabled(true);
}

void MatiereDialog::setupDeleteMode()
{
    setWindowTitle("Supprimer une Matière Première");
    lblTitle->setText("⚠ Confirmer la Suppression");
    btnSave->setVisible(false);
    btnDelete->setVisible(true);
    
    // Disable all fields
    txtModule->setEnabled(false);
    txtReference->setEnabled(false);
    cmbType->setEnabled(false);
    txtQuantite->setEnabled(false);
    txtSeuil->setEnabled(false);
    dateExpiration->setEnabled(false);
    
    // Show warning
    lblDeleteWarning->setText(
        "⚠ ATTENTION : Vous êtes sur le point de supprimer cette matière première.\n"
        "Cette action est irréversible et supprimera toutes les données associées."
    );
    lblDeleteWarning->setVisible(true);
}

void MatiereDialog::setMatiereData(const QString &module, const QString &reference, const QString &type,
                                   const QString &quantite, const QString &seuil, const QString &dateExp)
{
    txtModule->setText(module);
    txtReference->setText(reference);
    
    int typeIndex = cmbType->findText(type);
    if (typeIndex >= 0) {
        cmbType->setCurrentIndex(typeIndex);
    }
    
    txtQuantite->setText(quantite);
    txtSeuil->setText(seuil);
    dateExpiration->setDate(QDate::fromString(dateExp, "yyyy-MM-dd"));
}

QString MatiereDialog::getModule() const
{
    return txtModule->text();
}

QString MatiereDialog::getReference() const
{
    return txtReference->text();
}

QString MatiereDialog::getType() const
{
    return cmbType->currentText();
}

QString MatiereDialog::getQuantite() const
{
    return txtQuantite->text();
}

QString MatiereDialog::getSeuil() const
{
    return txtSeuil->text();
}

QString MatiereDialog::getDateExpiration() const
{
    return dateExpiration->date().toString("yyyy-MM-dd");
}

void MatiereDialog::onSaveClicked()
{
    // Validation
    if (txtModule->text().isEmpty()) {
        QMessageBox::warning(this, "Champ requis", "Le module est obligatoire.");
        return;
    }
    
    if (txtReference->text().isEmpty()) {
        QMessageBox::warning(this, "Champ requis", "La référence est obligatoire.");
        return;
    }
    
    // Success message
    QString message = (m_mode == AddMode) 
        ? "La matière première a été ajoutée avec succès !" 
        : "La matière première a été mise à jour avec succès !";
    
    QMessageBox::information(this, "Succès", message);
    accept();
}

void MatiereDialog::onCancelClicked()
{
    reject();
}

void MatiereDialog::onDeleteConfirmed()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Confirmation finale",
        "Êtes-vous absolument sûr de vouloir supprimer cette matière première ?",
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        QMessageBox::information(this, "Supprimé", "La matière première a été supprimée avec succès.");
        accept();
    }
}
