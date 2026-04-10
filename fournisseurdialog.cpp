#include "fournisseurdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include <QRegularExpressionValidator>

FournisseurDialog::FournisseurDialog(QWidget *parent, DialogMode mode)
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
    setMinimumHeight(500);
}

FournisseurDialog::~FournisseurDialog()
{
}

void FournisseurDialog::setupUI()
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
    
    // ID (hidden)
    lblId = new QLabel("ID Fournisseur :", this);
    txtId = new QLineEdit(this);
    txtId->setReadOnly(true);
    lblId->setVisible(false);
    txtId->setVisible(false);
    formLayout->addWidget(lblId, 0, 0);
    formLayout->addWidget(txtId, 0, 1);

    // Nom Entreprise
    lblNomEntreprise = new QLabel("Nom Entreprise * :", this);
    txtNomEntreprise = new QLineEdit(this);
    txtNomEntreprise->setPlaceholderText("Entrez le nom de l'entreprise");
    formLayout->addWidget(lblNomEntreprise, 1, 0);
    formLayout->addWidget(txtNomEntreprise, 1, 1);

    // Email
    lblEmail = new QLabel("Email * :", this);
    txtEmail = new QLineEdit(this);
    txtEmail->setPlaceholderText("exemple@email.com");
    formLayout->addWidget(lblEmail, 2, 0);
    formLayout->addWidget(txtEmail, 2, 1);

    // Telephone — chiffres uniquement, 8 max
    lblTelephone = new QLabel("Téléphone * :", this);
    txtTelephone = new QLineEdit(this);
    txtTelephone->setPlaceholderText("Ex: 58405688");
    txtTelephone->setValidator(new QRegularExpressionValidator(
        QRegularExpression("[0-9]{0,8}"), this));
    formLayout->addWidget(lblTelephone, 3, 0);
    formLayout->addWidget(txtTelephone, 3, 1);

    // Type Produit
    lblTypeProduit = new QLabel("Type Produit :", this);
    cmbTypeProduit = new QComboBox(this);
    cmbTypeProduit->addItems({"Vêtements", "Sacs", "Accessoires", "Tissus", "Cuir", "Autres"});
    formLayout->addWidget(lblTypeProduit, 4, 0);
    formLayout->addWidget(cmbTypeProduit, 4, 1);

    // Condition Paiement
    lblConditionPaiement = new QLabel("Condition Paiement :", this);
    cmbConditionPaiement = new QComboBox(this);
    cmbConditionPaiement->addItems({"Chèque", "Carte Bancaire", "Virement", "Espèces", "Crédit 30 jours", "Crédit 60 jours"});
    formLayout->addWidget(lblConditionPaiement, 5, 0);
    formLayout->addWidget(cmbConditionPaiement, 5, 1);

    // Matricule Fiscal — obligatoire, format 1234567/A/M/000
    lblMatriculeFiscal = new QLabel("Matricule Fiscal * :", this);
    txtMatriculeFiscal = new QLineEdit(this);
    txtMatriculeFiscal->setPlaceholderText("Ex: 1234567/A/M/000");
    txtMatriculeFiscal->setMaxLength(15);
    txtMatriculeFiscal->setValidator(new QRegularExpressionValidator(
        QRegularExpression("[0-9]{0,7}(/[A-Za-z](/[A-Za-z](/[0-9]{0,3})?)?)?"), this));
    formLayout->addWidget(lblMatriculeFiscal, 6, 0);
    formLayout->addWidget(txtMatriculeFiscal, 6, 1);
    
    // Statut
    lblStatut = new QLabel("Statut :", this);
    cmbStatut = new QComboBox(this);
    cmbStatut->addItems({"Actif", "Suspendu"});
    formLayout->addWidget(lblStatut, 7, 0);
    formLayout->addWidget(cmbStatut, 7, 1);

    // Adresse
    QLabel *lblAdresse = new QLabel("Adresse :", this);
    txtAdresse = new QLineEdit(this);
    txtAdresse->setPlaceholderText("Ex: 12 Rue de la Paix, Tunis");
    formLayout->addWidget(lblAdresse, 8, 0);
    formLayout->addWidget(txtAdresse, 8, 1);
    
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
    connect(btnSave, &QPushButton::clicked, this, &FournisseurDialog::onSaveClicked);
    
    btnDelete = new QPushButton("Confirmer Suppression", this);
    btnDelete->setStyleSheet(
        "QPushButton { background-color: #D32F2F; color: white; border: none; border-radius: 8px; "
        "padding: 10px 25px; font-size: 13px; font-weight: bold; }"
        "QPushButton:hover { background-color: #F44336; }"
        "QPushButton:pressed { background-color: #B71C1C; }"
    );
    connect(btnDelete, &QPushButton::clicked, this, &FournisseurDialog::onDeleteConfirmed);
    btnDelete->setVisible(false);
    
    btnCancel = new QPushButton("Annuler", this);
    btnCancel->setStyleSheet(
        "QPushButton { background-color: #BCAAA4; color: white; border: none; border-radius: 8px; "
        "padding: 10px 25px; font-size: 13px; font-weight: bold; }"
        "QPushButton:hover { background-color: #A78D78; }"
        "QPushButton:pressed { background-color: #8D7A6D; }"
    );
    connect(btnCancel, &QPushButton::clicked, this, &FournisseurDialog::onCancelClicked);
    
    buttonLayout->addWidget(btnSave);
    buttonLayout->addWidget(btnDelete);
    buttonLayout->addWidget(btnCancel);
    
    mainLayout->addLayout(buttonLayout);
    
    // Apply general styling
    setStyleSheet(
        "QDialog { background-color: #FAF5F0; }"
        "QLabel { color: #291C0E; font-family: Arial, sans-serif; font-size: 12px; font-weight: bold; }"
        "QLineEdit, QComboBox { background-color: white; border: 2px solid #BCAAA4; "
        "border-radius: 6px; padding: 8px; font-size: 12px; color: #291C0E; }"
        "QLineEdit:focus, QComboBox:focus { border-color: #8D6E63; }"
    );
}

void FournisseurDialog::setupAddMode()
{
    setWindowTitle("Ajouter un Fournisseur");
    lblTitle->setText("+ Ajouter un Nouveau Fournisseur");
    btnSave->setVisible(true);
    btnDelete->setVisible(false);
    
    // Generate new ID
    txtId->setText("F016");
    
    // Enable all fields
    txtNomEntreprise->setEnabled(true);
    txtEmail->setEnabled(true);
    txtTelephone->setEnabled(true);
    cmbTypeProduit->setEnabled(true);
    cmbConditionPaiement->setEnabled(true);
    txtMatriculeFiscal->setEnabled(true);
    cmbStatut->setEnabled(true);
}

void FournisseurDialog::setupEditMode()
{
    setWindowTitle("Modifier un Fournisseur");
    lblTitle->setText("✎ Modifier le Fournisseur");
    btnSave->setText("Mettre à Jour");
    btnSave->setVisible(true);
    btnDelete->setVisible(false);
    
    // Enable all fields
    txtNomEntreprise->setEnabled(true);
    txtEmail->setEnabled(true);
    txtTelephone->setEnabled(true);
    cmbTypeProduit->setEnabled(true);
    cmbConditionPaiement->setEnabled(true);
    txtMatriculeFiscal->setEnabled(true);
    cmbStatut->setEnabled(true);
}

void FournisseurDialog::setupDeleteMode()
{
    setWindowTitle("Supprimer un Fournisseur");
    lblTitle->setText("⚠ Confirmer la Suppression");
    btnSave->setVisible(false);
    btnDelete->setVisible(true);
    
    // Disable all fields
    txtNomEntreprise->setEnabled(false);
    txtEmail->setEnabled(false);
    txtTelephone->setEnabled(false);
    cmbTypeProduit->setEnabled(false);
    cmbConditionPaiement->setEnabled(false);
    txtMatriculeFiscal->setEnabled(false);
    cmbStatut->setEnabled(false);
    
    // Show warning
    lblDeleteWarning->setText(
        "⚠ ATTENTION : Vous êtes sur le point de supprimer ce fournisseur.\n"
        "Cette action est irréversible et supprimera toutes les données associées."
    );
    lblDeleteWarning->setVisible(true);
}

void FournisseurDialog::setFournisseurData(const QString &id, const QString &nomEntreprise, const QString &email,
                                           const QString &telephone, const QString &typeProduit,
                                           const QString &conditionPaiement, const QString &matriculeFiscal,
                                           const QString &statut, const QString &adresse)
{
    txtId->setText(id);
    txtNomEntreprise->setText(nomEntreprise);
    txtEmail->setText(email);
    txtTelephone->setText(telephone);
    int typeIndex = cmbTypeProduit->findText(typeProduit);
    if (typeIndex >= 0) cmbTypeProduit->setCurrentIndex(typeIndex);
    int conditionIndex = cmbConditionPaiement->findText(conditionPaiement);
    if (conditionIndex >= 0) cmbConditionPaiement->setCurrentIndex(conditionIndex);
    txtMatriculeFiscal->setText(matriculeFiscal);
    int statutIndex = cmbStatut->findText(statut);
    if (statutIndex >= 0) cmbStatut->setCurrentIndex(statutIndex);
    txtAdresse->setText(adresse);
}

QString FournisseurDialog::getId() const
{
    return txtId->text();
}

QString FournisseurDialog::getNomEntreprise() const
{
    return txtNomEntreprise->text();
}

QString FournisseurDialog::getEmail() const
{
    return txtEmail->text();
}

QString FournisseurDialog::getTelephone() const
{
    return txtTelephone->text();
}

QString FournisseurDialog::getTypeProduit() const
{
    return cmbTypeProduit->currentText();
}

QString FournisseurDialog::getConditionPaiement() const
{
    return cmbConditionPaiement->currentText();
}

QString FournisseurDialog::getMatriculeFiscal() const
{
    return txtMatriculeFiscal->text();
}

QString FournisseurDialog::getStatut() const
{
    return cmbStatut->currentText();
}

QString FournisseurDialog::getAdresse() const
{
    return txtAdresse ? txtAdresse->text().trimmed() : QString();
}

void FournisseurDialog::onSaveClicked()
{
    QStringList errors;

    // Nom Entreprise
    if (txtNomEntreprise->text().trimmed().isEmpty()) {
        errors << "- Le nom de l'entreprise est obligatoire";
        txtNomEntreprise->setStyleSheet("border: 2px solid red;");
    } else {
        txtNomEntreprise->setStyleSheet("");
    }

    // Email
    if (txtEmail->text().trimmed().isEmpty()) {
        errors << "- L'email est obligatoire";
        txtEmail->setStyleSheet("border: 2px solid red;");
    } else if (!txtEmail->text().contains("@") || !txtEmail->text().contains(".")) {
        errors << "- L'email doit être valide (contenir @ et .)";
        txtEmail->setStyleSheet("border: 2px solid red;");
    } else {
        txtEmail->setStyleSheet("");
    }

    // Téléphone
    if (txtTelephone->text().trimmed().isEmpty()) {
        errors << "- Le téléphone est obligatoire";
        txtTelephone->setStyleSheet("border: 2px solid red;");
    } else if (txtTelephone->text().trimmed().length() != 8) {
        errors << "- Le téléphone doit contenir exactement 8 chiffres";
        txtTelephone->setStyleSheet("border: 2px solid red;");
    } else {
        txtTelephone->setStyleSheet("");
    }

    // Matricule Fiscal obligatoire, format exact
    if (txtMatriculeFiscal->text().trimmed().isEmpty()) {
        errors << "- Le matricule fiscal est obligatoire";
        txtMatriculeFiscal->setStyleSheet("border: 2px solid red;");
    } else {
        QRegularExpression re("^[0-9]{7}/[A-Za-z]/[A-Za-z]/[0-9]{3}$");
        if (!re.match(txtMatriculeFiscal->text().trimmed()).hasMatch()) {
            errors << "- Le matricule fiscal doit être au format : 1234567/A/M/000";
            txtMatriculeFiscal->setStyleSheet("border: 2px solid red;");
        } else {
            txtMatriculeFiscal->setStyleSheet("");
        }
    }

    if (!errors.isEmpty()) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Validation");
        msgBox.setText("Veuillez corriger les erreurs suivantes :");
        msgBox.setInformativeText(errors.join("\n"));
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStyleSheet(
            "QMessageBox { background-color: #FAF5F0; }"
            "QMessageBox QLabel { color: #291C0E; font-size: 12px; }"
            "QPushButton { background-color: #8D6E63; color: white; border: none; "
            "border-radius: 6px; padding: 8px 20px; font-size: 11px; font-weight: bold; }"
            "QPushButton:hover { background-color: #A0826D; }"
        );
        msgBox.exec();
        return;
    }

    accept();
}

void FournisseurDialog::onCancelClicked()
{
    reject();
}

void FournisseurDialog::onDeleteConfirmed()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Confirmation finale",
        "Êtes-vous absolument sûr de vouloir supprimer ce fournisseur ?",
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        QMessageBox::information(this, "Supprimé", "Le fournisseur a été supprimé avec succès.");
        accept();
    }
}
