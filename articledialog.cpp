#include "articledialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QGroupBox>

ArticleDialog::ArticleDialog(QWidget *parent, Mode mode)
    : QDialog(parent)
    , dialogMode(mode)
{
    setupUI();
    applyStyles();
    
    switch (dialogMode) {
    case AddMode:
        setWindowTitle("Ajouter un Article");
        break;
    case EditMode:
        setWindowTitle("Modifier un Article");
        break;
    case DeleteMode:
        setWindowTitle("Supprimer un Article");
        lineEditReference->setReadOnly(true);
        lineEditNom->setReadOnly(true);
        comboBoxCategorie->setEnabled(false);
        lineEditType->setReadOnly(true);
        lineEditCouleur->setReadOnly(true);
        lineEditDimensions->setReadOnly(true);
        spinBoxPrix->setReadOnly(true);
        spinBoxCout->setReadOnly(true);
        comboBoxStatut->setEnabled(false);
        break;
    case ViewMode:
        setWindowTitle("Consulter un Article");
        lineEditReference->setReadOnly(true);
        lineEditNom->setReadOnly(true);
        comboBoxCategorie->setEnabled(false);
        lineEditType->setReadOnly(true);
        lineEditCouleur->setReadOnly(true);
        lineEditDimensions->setReadOnly(true);
        spinBoxPrix->setReadOnly(true);
        spinBoxCout->setReadOnly(true);
        comboBoxStatut->setEnabled(false);
        break;
    }
}

ArticleDialog::~ArticleDialog()
{
}

void ArticleDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // Form group
    QGroupBox *formGroup = new QGroupBox("Informations de l'Article");
    QFormLayout *formLayout = new QFormLayout();
    
    lineEditReference = new QLineEdit();
    lineEditNom = new QLineEdit();
    comboBoxCategorie = new QComboBox();
    comboBoxCategorie->addItems({"Sacs", "Portefeuilles", "Ceintures", "Accessoires", "Chaussures"});
    lineEditType = new QLineEdit();
    lineEditCouleur = new QLineEdit();
    lineEditDimensions = new QLineEdit();
    lineEditDimensions->setPlaceholderText("ex: 30x20x10cm");
    
    spinBoxPrix = new QDoubleSpinBox();
    spinBoxPrix->setRange(0, 999999);
    spinBoxPrix->setDecimals(2);
    spinBoxPrix->setSuffix(" €");
    
    spinBoxCout = new QDoubleSpinBox();
    spinBoxCout->setRange(0, 999999);
    spinBoxCout->setDecimals(2);
    spinBoxCout->setSuffix(" €");
    
    comboBoxStatut = new QComboBox();
    comboBoxStatut->addItems({"disponible", "en_production", "obsolete"});
    
    formLayout->addRow("Référence:", lineEditReference);
    formLayout->addRow("Nom *:", lineEditNom);
    formLayout->addRow("Catégorie:", comboBoxCategorie);
    formLayout->addRow("Type:", lineEditType);
    formLayout->addRow("Couleur:", lineEditCouleur);
    formLayout->addRow("Dimensions:", lineEditDimensions);
    formLayout->addRow("Prix Unitaire:", spinBoxPrix);
    formLayout->addRow("Coût Fabrication:", spinBoxCout);
    formLayout->addRow("Statut:", comboBoxStatut);
    
    formGroup->setLayout(formLayout);
    mainLayout->addWidget(formGroup);
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    QPushButton *btnSave = new QPushButton(dialogMode == DeleteMode ? "Confirmer" : "Enregistrer");
    QPushButton *btnCancel = new QPushButton("Annuler");
    
    if (dialogMode == ViewMode) {
        btnSave->setVisible(false);
        btnCancel->setText("Fermer");
    }
    
    connect(btnSave, &QPushButton::clicked, this, &ArticleDialog::onSave);
    connect(btnCancel, &QPushButton::clicked, this, &ArticleDialog::onCancel);
    
    buttonLayout->addWidget(btnSave);
    buttonLayout->addWidget(btnCancel);
    
    mainLayout->addLayout(buttonLayout);
    
    setMinimumWidth(500);
}

void ArticleDialog::applyStyles()
{
    setStyleSheet(
        "QDialog { background-color: #FAF5F0; }"
        "QGroupBox { font-weight: bold; color: #291C0E; border: 2px solid #BCAAA4; "
        "border-radius: 8px; margin-top: 10px; padding-top: 10px; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px; }"
        "QLabel { color: #291C0E; font-size: 11px; }"
        "QLineEdit, QComboBox, QDoubleSpinBox { background-color: white; border: 2px solid #BCAAA4; "
        "border-radius: 6px; padding: 6px; color: #291C0E; font-size: 11px; }"
        "QPushButton { background-color: #8D6E63; color: white; border: none; border-radius: 8px; "
        "padding: 10px 20px; font-size: 12px; font-weight: bold; min-width: 100px; }"
        "QPushButton:hover { background-color: #A0826D; }"
        "QPushButton:pressed { background-color: #6E473B; }"
    );
}

void ArticleDialog::setArticleData(const QString &ref, const QString &nom, const QString &cat,
                                   const QString &type, const QString &couleur, const QString &dim,
                                   double prix, double cout, const QString &statut)
{
    lineEditReference->setText(ref);
    lineEditNom->setText(nom);
    comboBoxCategorie->setCurrentText(cat);
    lineEditType->setText(type);
    lineEditCouleur->setText(couleur);
    lineEditDimensions->setText(dim);
    spinBoxPrix->setValue(prix);
    spinBoxCout->setValue(cout);
    comboBoxStatut->setCurrentText(statut);
}

QString ArticleDialog::getReference() const { return lineEditReference->text(); }
QString ArticleDialog::getNom() const { return lineEditNom->text(); }
QString ArticleDialog::getCategorie() const { return comboBoxCategorie->currentText(); }
QString ArticleDialog::getType() const { return lineEditType->text(); }
QString ArticleDialog::getCouleur() const { return lineEditCouleur->text(); }
QString ArticleDialog::getDimensions() const { return lineEditDimensions->text(); }
double ArticleDialog::getPrixUnitaire() const { return spinBoxPrix->value(); }
double ArticleDialog::getCoutFabrication() const { return spinBoxCout->value(); }
QString ArticleDialog::getStatut() const { return comboBoxStatut->currentText(); }

void ArticleDialog::onSave()
{
    if (lineEditNom->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Validation", "Le nom de l'article est obligatoire !");
        return;
    }
    
    if (lineEditReference->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Validation", "La référence est obligatoire !");
        return;
    }
    
    if (dialogMode != DeleteMode && spinBoxPrix->value() <= 0) {
        QMessageBox::warning(this, "Validation", "Le prix unitaire doit être supérieur à 0 !");
        spinBoxPrix->setFocus();
        return;
    }
    
    if (dialogMode != DeleteMode && spinBoxCout->value() > spinBoxPrix->value()) {
        QMessageBox::warning(this, "Validation", "Le coût de fabrication ne peut pas dépasser le prix de vente !");
        return;
    }
    
    if (dialogMode == DeleteMode) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "Confirmation",
            "Êtes-vous sûr de vouloir supprimer cet article ?",
            QMessageBox::Yes | QMessageBox::No
        );
        if (reply == QMessageBox::Yes) accept();
    } else {
        accept();
    }
}

void ArticleDialog::onCancel()
{
    reject();
}
