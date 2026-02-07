#include "productiondialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include <QDate>

ProductionDialog::ProductionDialog(QWidget *parent, DialogMode mode)
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
    
    setMinimumWidth(700);
    setMinimumHeight(600);
}

ProductionDialog::~ProductionDialog()
{
}

void ProductionDialog::setupUI()
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
    
    // ID
    lblId = new QLabel("ID Production :", this);
    txtId = new QLineEdit(this);
    txtId->setReadOnly(true);
    formLayout->addWidget(lblId, 0, 0);
    formLayout->addWidget(txtId, 0, 1);
    
    // Reference
    lblReference = new QLabel("Référence * :", this);
    txtReference = new QLineEdit(this);
    txtReference->setPlaceholderText("Ex: PROD-2024-001");
    formLayout->addWidget(lblReference, 1, 0);
    formLayout->addWidget(txtReference, 1, 1);
    
    // Produit
    lblProduit = new QLabel("Produit * :", this);
    cmbProduit = new QComboBox(this);
    cmbProduit->addItems({"Sac à Main Cuir", "Portefeuille", "Ceinture", "Sacoche", "Porte-documents", "Sac à Dos"});
    formLayout->addWidget(lblProduit, 2, 0);
    formLayout->addWidget(cmbProduit, 2, 1);
    
    // Quantite
    lblQuantite = new QLabel("Quantité * :", this);
    txtQuantite = new QLineEdit(this);
    txtQuantite->setPlaceholderText("Ex: 100");
    formLayout->addWidget(lblQuantite, 3, 0);
    formLayout->addWidget(txtQuantite, 3, 1);
    
    // Statut
    lblStatut = new QLabel("Statut :", this);
    cmbStatut = new QComboBox(this);
    cmbStatut->addItems({"En Attente", "En Cours", "Terminé", "Suspendu", "Annulé"});
    formLayout->addWidget(lblStatut, 4, 0);
    formLayout->addWidget(cmbStatut, 4, 1);
    
    // Date Debut
    lblDateDebut = new QLabel("Date Début * :", this);
    dateDebut = new QDateEdit(this);
    dateDebut->setCalendarPopup(true);
    dateDebut->setDate(QDate::currentDate());
    dateDebut->setDisplayFormat("dd/MM/yyyy");
    formLayout->addWidget(lblDateDebut, 5, 0);
    formLayout->addWidget(dateDebut, 5, 1);
    
    // Date Fin
    lblDateFin = new QLabel("Date Fin Prévue * :", this);
    dateFin = new QDateEdit(this);
    dateFin->setCalendarPopup(true);
    dateFin->setDate(QDate::currentDate().addDays(7));
    dateFin->setDisplayFormat("dd/MM/yyyy");
    formLayout->addWidget(lblDateFin, 6, 0);
    formLayout->addWidget(dateFin, 6, 1);
    
    // Responsable
    lblResponsable = new QLabel("Responsable * :", this);
    cmbResponsable = new QComboBox(this);
    cmbResponsable->addItems({"Ahmed Benali", "Fatima Zahra", "Mohammed Alami", "Khadija Mansouri", "Youssef Idrissi"});
    formLayout->addWidget(lblResponsable, 7, 0);
    formLayout->addWidget(cmbResponsable, 7, 1);
    
    // Priorite
    lblPriorite = new QLabel("Priorité :", this);
    cmbPriorite = new QComboBox(this);
    cmbPriorite->addItems({"Basse", "Normale", "Haute", "Urgente"});
    cmbPriorite->setCurrentIndex(1); // Default to "Normale"
    formLayout->addWidget(lblPriorite, 8, 0);
    formLayout->addWidget(cmbPriorite, 8, 1);
    
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
    connect(btnSave, &QPushButton::clicked, this, &ProductionDialog::onSaveClicked);
    
    btnDelete = new QPushButton("Confirmer Suppression", this);
    btnDelete->setStyleSheet(
        "QPushButton { background-color: #D32F2F; color: white; border: none; border-radius: 8px; "
        "padding: 10px 25px; font-size: 13px; font-weight: bold; }"
        "QPushButton:hover { background-color: #F44336; }"
        "QPushButton:pressed { background-color: #B71C1C; }"
    );
    connect(btnDelete, &QPushButton::clicked, this, &ProductionDialog::onDeleteConfirmed);
    btnDelete->setVisible(false);
    
    btnCancel = new QPushButton("Annuler", this);
    btnCancel->setStyleSheet(
        "QPushButton { background-color: #BCAAA4; color: white; border: none; border-radius: 8px; "
        "padding: 10px 25px; font-size: 13px; font-weight: bold; }"
        "QPushButton:hover { background-color: #A78D78; }"
        "QPushButton:pressed { background-color: #8D7A6D; }"
    );
    connect(btnCancel, &QPushButton::clicked, this, &ProductionDialog::onCancelClicked);
    
    buttonLayout->addWidget(btnSave);
    buttonLayout->addWidget(btnDelete);
    buttonLayout->addWidget(btnCancel);
    
    mainLayout->addLayout(buttonLayout);
    
    // Apply general styling
    setStyleSheet(
        "QDialog { background-color: #FAF5F0; }"
        "QLabel { color: #291C0E; font-family: Arial, sans-serif; font-size: 12px; font-weight: bold; }"
        "QLineEdit, QComboBox, QDateEdit { background-color: white; border: 2px solid #BCAAA4; "
        "border-radius: 6px; padding: 8px; font-size: 12px; color: #291C0E; }"
        "QLineEdit:focus, QComboBox:focus, QDateEdit:focus { border-color: #8D6E63; }"
    );
}

void ProductionDialog::setupAddMode()
{
    setWindowTitle("Créer une Production");
    lblTitle->setText("➕ Créer une Nouvelle Production");
    btnSave->setVisible(true);
    btnDelete->setVisible(false);
    
    // Generate new ID
    txtId->setText("P016");
    
    // Enable all fields
    txtReference->setEnabled(true);
    cmbProduit->setEnabled(true);
    txtQuantite->setEnabled(true);
    cmbStatut->setEnabled(true);
    dateDebut->setEnabled(true);
    dateFin->setEnabled(true);
    cmbResponsable->setEnabled(true);
    cmbPriorite->setEnabled(true);
}

void ProductionDialog::setupEditMode()
{
    setWindowTitle("Modifier une Production");
    lblTitle->setText("✎ Modifier la Production");
    btnSave->setText("Mettre à Jour");
    btnSave->setVisible(true);
    btnDelete->setVisible(false);
    
    // Enable all fields
    txtReference->setEnabled(true);
    cmbProduit->setEnabled(true);
    txtQuantite->setEnabled(true);
    cmbStatut->setEnabled(true);
    dateDebut->setEnabled(true);
    dateFin->setEnabled(true);
    cmbResponsable->setEnabled(true);
    cmbPriorite->setEnabled(true);
}

void ProductionDialog::setupDeleteMode()
{
    setWindowTitle("Supprimer une Production");
    lblTitle->setText("⚠ Confirmer la Suppression");
    btnSave->setVisible(false);
    btnDelete->setVisible(true);
    
    // Disable all fields
    txtReference->setEnabled(false);
    cmbProduit->setEnabled(false);
    txtQuantite->setEnabled(false);
    cmbStatut->setEnabled(false);
    dateDebut->setEnabled(false);
    dateFin->setEnabled(false);
    cmbResponsable->setEnabled(false);
    cmbPriorite->setEnabled(false);
    
    // Show warning
    lblDeleteWarning->setText(
        "⚠ ATTENTION : Vous êtes sur le point de supprimer cette production.\n"
        "Cette action est irréversible et supprimera toutes les données associées."
    );
    lblDeleteWarning->setVisible(true);
}

void ProductionDialog::setProductionData(const QString &id, const QString &reference, const QString &produit,
                                         const QString &quantite, const QString &statut, const QString &dateDebut,
                                         const QString &dateFin, const QString &responsable, const QString &priorite)
{
    txtId->setText(id);
    txtReference->setText(reference);
    
    int produitIndex = cmbProduit->findText(produit);
    if (produitIndex >= 0) {
        cmbProduit->setCurrentIndex(produitIndex);
    }
    
    txtQuantite->setText(quantite);
    
    int statutIndex = cmbStatut->findText(statut);
    if (statutIndex >= 0) {
        cmbStatut->setCurrentIndex(statutIndex);
    }
    
    this->dateDebut->setDate(QDate::fromString(dateDebut, "dd/MM/yyyy"));
    this->dateFin->setDate(QDate::fromString(dateFin, "dd/MM/yyyy"));
    
    int responsableIndex = cmbResponsable->findText(responsable);
    if (responsableIndex >= 0) {
        cmbResponsable->setCurrentIndex(responsableIndex);
    }
    
    int prioriteIndex = cmbPriorite->findText(priorite);
    if (prioriteIndex >= 0) {
        cmbPriorite->setCurrentIndex(prioriteIndex);
    }
}

QString ProductionDialog::getId() const
{
    return txtId->text();
}

QString ProductionDialog::getReference() const
{
    return txtReference->text();
}

QString ProductionDialog::getProduit() const
{
    return cmbProduit->currentText();
}

QString ProductionDialog::getQuantite() const
{
    return txtQuantite->text();
}

QString ProductionDialog::getStatut() const
{
    return cmbStatut->currentText();
}

QString ProductionDialog::getDateDebut() const
{
    return dateDebut->date().toString("dd/MM/yyyy");
}

QString ProductionDialog::getDateFin() const
{
    return dateFin->date().toString("dd/MM/yyyy");
}

QString ProductionDialog::getResponsable() const
{
    return cmbResponsable->currentText();
}

QString ProductionDialog::getPriorite() const
{
    return cmbPriorite->currentText();
}

void ProductionDialog::onSaveClicked()
{
    // Validation
    if (txtReference->text().isEmpty()) {
        QMessageBox::warning(this, "Champ requis", "La référence est obligatoire.");
        return;
    }
    
    if (txtQuantite->text().isEmpty()) {
        QMessageBox::warning(this, "Champ requis", "La quantité est obligatoire.");
        return;
    }
    
    bool ok;
    int quantite = txtQuantite->text().toInt(&ok);
    if (!ok || quantite <= 0) {
        QMessageBox::warning(this, "Valeur invalide", "La quantité doit être un nombre positif.");
        return;
    }
    
    if (dateDebut->date() > dateFin->date()) {
        QMessageBox::warning(this, "Dates invalides", "La date de début doit être antérieure à la date de fin.");
        return;
    }
    
    // Success message
    QString message = (m_mode == AddMode) 
        ? "La production a été créée avec succès !" 
        : "La production a été mise à jour avec succès !";
    
    QMessageBox::information(this, "Succès", message);
    accept();
}

void ProductionDialog::onCancelClicked()
{
    reject();
}

void ProductionDialog::onDeleteConfirmed()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Confirmation finale",
        "Êtes-vous absolument sûr de vouloir supprimer cette production ?",
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        QMessageBox::information(this, "Supprimé", "La production a été supprimée avec succès.");
        accept();
    }
}
