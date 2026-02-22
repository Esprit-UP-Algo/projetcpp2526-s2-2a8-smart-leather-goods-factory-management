#include "clientmanagerdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QDate>
#include <QGroupBox>
#include <QCheckBox>
#include <QRadioButton>

ClientManagerDialog::ClientManagerDialog(QWidget *parent, DialogMode mode)
    : QDialog(parent), mode(mode)
{
    if (mode == DeleteMode) {
        setupDeleteUI();
    } else if (mode == ExportMode) {
        setupExportUI();
    } else {
        setupAddEditUI();
    }
    applyStyles();

    if (mode == AddMode) {
        setWindowTitle("Créer Client - CUIREA");
    } else if (mode == EditMode) {
        setWindowTitle("Modifier Client - CUIREA");
    } else if (mode == ExportMode) {
        setWindowTitle("Exporter les Clients - CUIREA");
    } else {
        setWindowTitle("Supprimer Client - CUIREA");
    }

    if (mode == ExportMode) {
        setMinimumSize(700, 550);
    } else {
        setMinimumSize(700, 450);
    }
}

ClientManagerDialog::~ClientManagerDialog() {}

// ------------------- Add/Edit UI -------------------
void ClientManagerDialog::setupAddEditUI()
{
    QVBoxLayout *mainLay = new QVBoxLayout(this);
    mainLay->setContentsMargins(28, 24, 28, 24);
    mainLay->setSpacing(16);

    QLabel *title = new QLabel(mode == AddMode ? "Nouveau client" : "Modifier le client");
    title->setObjectName("dialogTitle");
    mainLay->addWidget(title);

    QHBoxLayout *columnsLayout = new QHBoxLayout();
    columnsLayout->setSpacing(40);

    QFormLayout *leftForm = new QFormLayout();
    leftForm->setLabelAlignment(Qt::AlignRight);
    leftForm->setHorizontalSpacing(14);
    leftForm->setVerticalSpacing(12);

    nomEdit = new QLineEdit();
    nomEdit->setPlaceholderText("Obligatoire");

    prenomEdit = new QLineEdit();
    prenomEdit->setPlaceholderText("Obligatoire");

    sexeCombo = new QComboBox();
    sexeCombo->addItems({"Homme", "Femme"});

    cinEdit = new QLineEdit();
    cinEdit->setPlaceholderText("AB123456");

    leftForm->addRow("Nom *:", nomEdit);
    leftForm->addRow("Prénom *:", prenomEdit);
    leftForm->addRow("Sexe *:", sexeCombo);
    leftForm->addRow("CIN *:", cinEdit);

    QFormLayout *rightForm = new QFormLayout();
    rightForm->setLabelAlignment(Qt::AlignRight);
    rightForm->setHorizontalSpacing(14);
    rightForm->setVerticalSpacing(12);

    paysEdit = new QLineEdit();
    paysEdit->setPlaceholderText("Maroc");

    villeEdit = new QLineEdit();
    villeEdit->setPlaceholderText("Casablanca");

    adresseEdit = new QLineEdit();
    adresseEdit->setPlaceholderText("Adresse complète");

    emailEdit = new QLineEdit();
    emailEdit->setPlaceholderText("email@example.com");

    dateInscrit = new QDateEdit(QDate::currentDate());
    dateInscrit->setCalendarPopup(true);
    dateInscrit->setDisplayFormat("dd/MM/yyyy");

    rightForm->addRow("Pays *:", paysEdit);
    rightForm->addRow("Ville *:", villeEdit);
    rightForm->addRow("Adresse:", adresseEdit);
    rightForm->addRow("Email *:", emailEdit);
    rightForm->addRow("Date inscription *:", dateInscrit);

    columnsLayout->addLayout(leftForm);
    columnsLayout->addLayout(rightForm);
    mainLay->addLayout(columnsLayout);

    QLabel *noteLabel = new QLabel("* Champs obligatoires", this);
    noteLabel->setObjectName("noteLabel");
    mainLay->addWidget(noteLabel);

    QDialogButtonBox *btnBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
    QPushButton *saveBtn = btnBox->button(QDialogButtonBox::Save);
    saveBtn->setText("Enregistrer");
    saveBtn->setObjectName("saveButton");
    mainLay->addWidget(btnBox);

    connect(btnBox, &QDialogButtonBox::accepted, this, [this]() {
        Client c = getClient();

        if (mode == AddMode) {
            if (c.ajouter()) {
                accept();
            } else {
                QMessageBox::critical(this, "Erreur", "Échec d'ajout du client !");
            }
        } else if (mode == EditMode) {
            if (c.modifier ()) {
                accept();
            } else {
                QMessageBox::critical(this, "Erreur", "Échec de modification du client !");
            }
        }
    });
    connect(btnBox, &QDialogButtonBox::rejected, this, &ClientManagerDialog::reject);
}

// ------------------- Delete UI -------------------
void ClientManagerDialog::setupDeleteUI()
{
    QVBoxLayout *mainLay = new QVBoxLayout(this);
    mainLay->setContentsMargins(40, 30, 40, 30);
    mainLay->setSpacing(20);

    setMinimumSize(500, 350);

    QLabel *title = new QLabel("Confirmation de Suppression");
    title->setObjectName("dialogTitle");
    title->setAlignment(Qt::AlignCenter);
    mainLay->addWidget(title);

    QLabel *question = new QLabel("Êtes-vous sûr de vouloir supprimer ce client ?");
    question->setObjectName("questionLabel");
    question->setAlignment(Qt::AlignCenter);
    question->setWordWrap(true);
    mainLay->addWidget(question);

    mainLay->addSpacing(10);

    QLabel *infoLabel = new QLabel();
    infoLabel->setObjectName("infoLabel");
    infoLabel->setAlignment(Qt::AlignLeft);
    infoLabel->setWordWrap(true);
    mainLay->addWidget(infoLabel);

    mainLay->addSpacing(10);

    QLabel *warningLabel = new QLabel("Cette action est irréversible !");
    warningLabel->setObjectName("warningLabel");
    warningLabel->setAlignment(Qt::AlignCenter);
    mainLay->addWidget(warningLabel);

    mainLay->addStretch();

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(15);

    QPushButton *cancelBtn = new QPushButton("Annuler");
    cancelBtn->setObjectName("cancelButton");
    cancelBtn->setMinimumSize(120, 40);

    QPushButton *deleteBtn = new QPushButton("Supprimer");
    deleteBtn->setObjectName("deleteButton");
    deleteBtn->setMinimumSize(120, 40);

    btnLayout->addStretch();
    btnLayout->addWidget(cancelBtn);
    btnLayout->addWidget(deleteBtn);
    btnLayout->addStretch();

    mainLay->addLayout(btnLayout);

    connect(cancelBtn, &QPushButton::clicked, this, &ClientManagerDialog::reject);
    connect(deleteBtn, &QPushButton::clicked, this, [this]() {
        if (Client().supprimer(deleteId)) {
            accept();
        } else {
            QMessageBox::critical(this, "Erreur", "Échec de suppression du client !");
        }
    });
}

// ------------------- Set client data -------------------
void ClientManagerDialog::setClientData(const QString &nom, const QString &prenom, const QString &sexe,
                                        const QString &cin, const QString &pays, const QString &ville,
                                        const QString &adresse, const QString &email)
{
    if (mode == DeleteMode) {
        deleteNom = nom;
        deletePrenom = prenom;
        deleteSexe = sexe;
        deleteCIN = cin;
        deletePays = pays;
        deleteVille = ville;
    } else if (mode == EditMode) {
        nomEdit->setText(nom);
        prenomEdit->setText(prenom);
        sexeCombo->setCurrentText(sexe);
        cinEdit->setText(cin);
        paysEdit->setText(pays);
        villeEdit->setText(ville);
        adresseEdit->setText(adresse);
        emailEdit->setText(email);
    }
}

// ------------------- Get client object -------------------
Client ClientManagerDialog::getClient() const
{
    Client c;
    if (mode != DeleteMode) {
        c.setNom(nomEdit->text().trimmed());
        c.setPrenom(prenomEdit->text().trimmed());
        c.setSexe(sexeCombo->currentText());
        c.setCin(cinEdit->text().trimmed());
        c.setPays(paysEdit->text().trimmed());
        c.setVille(villeEdit->text().trimmed());
        c.setAdresse(adresseEdit->text().trimmed());
        c.setEmail(emailEdit->text().trimmed());
        c.setDateInscrit(dateInscrit->date());
    }
    return c;
}

// ------------------- Export UI -------------------
void ClientManagerDialog::setupExportUI()
{
    QVBoxLayout *mainLay = new QVBoxLayout(this);
    mainLay->setContentsMargins(28, 24, 28, 24);
    mainLay->setSpacing(20);

    QLabel *title = new QLabel("Exporter les données des clients");
    title->setObjectName("dialogTitle");
    mainLay->addWidget(title);

    // ... same export UI as before ...
}


void ClientManagerDialog::applyStyles()
{
    setStyleSheet(
        "QDialog { background-color: #FAF5F0; }"
        "QLabel#dialogTitle { color: #291C0E; font-family: 'Times New Roman', serif; font-size: 20px; font-weight: bold; padding: 10px; }"
        "QLabel#questionLabel { color: #291C0E; font-family: Arial, sans-serif; font-size: 14px; }"
        "QLabel#infoLabel { color: #291C0E; font-family: Arial, sans-serif; font-size: 12px; background-color: #FFF8F0; "
        "border: 2px solid #BCAAA4; border-radius: 8px; padding: 15px; }"
        "QLabel#warningLabel { color: #A0522D; font-family: Arial, sans-serif; font-size: 12px; font-weight: bold; }"
        "QLabel#sectionLabel { color: #8D6E63; font-family: 'Times New Roman', serif; font-size: 14px; font-weight: bold; }"
        "QLabel#photoPreview { background-color: #FFF8F0; border: 3px solid #BCAAA4; border-radius: 8px; color: #BCAAA4; font-size: 60px; }"
        "QLabel#noteLabel { color: #8D6E63; font-family: Arial, sans-serif; font-size: 10px; font-style: italic; }"
        "QLabel { color: #291C0E; font-family: Arial, sans-serif; font-size: 12px; font-weight: bold; }"
        "QLineEdit, QComboBox, QDateEdit { background-color: white; color: #291C0E; border: 2px solid #BCAAA4; border-radius: 6px; "
        "padding: 8px; font-family: Arial, sans-serif; font-size: 11px; min-height: 25px; }"
        "QLineEdit:focus, QComboBox:focus, QDateEdit:focus { border: 2px solid #8D6E63; }"
        "QLineEdit::placeholder { color: #BCAAA4; }"
        "QPushButton#browseButton { background-color: #A0826D; color: white; border: none; border-radius: 8px; "
        "padding: 8px 15px; font-family: Arial, sans-serif; font-size: 11px; font-weight: bold; }"
        "QPushButton#browseButton:hover { background-color: #B89580; }"
        "QPushButton#browseButton:disabled { background-color: #BCAAA4; }"
        "QPushButton#saveButton { background-color: #8D6E63; color: white; border: none; border-radius: 10px; "
        "padding: 10px 20px; font-family: Arial, sans-serif; font-size: 12px; font-weight: bold; }"
        "QPushButton#saveButton:hover { background-color: #A0826D; }"
        "QPushButton#deleteButton { background-color: #A0522D; color: white; border: none; border-radius: 8px; "
        "padding: 10px 20px; font-family: Arial, sans-serif; font-size: 11px; font-weight: bold; }"
        "QPushButton#deleteButton:hover { background-color: #B8633D; }"
        "QPushButton#cancelButton { background-color: #E7DDD1; color: #291C0E; border: 1px solid #BCAAA4; border-radius: 8px; "
        "padding: 10px 20px; font-family: Arial, sans-serif; font-size: 11px; font-weight: bold; }"
        "QPushButton#cancelButton:hover { background-color: #F0E6DA; }"
        "QPushButton#exportButton { background-color: #8D6E63; color: white; border: none; border-radius: 10px; "
        "padding: 10px 20px; font-family: Arial, sans-serif; font-size: 12px; font-weight: bold; }"
        "QPushButton#exportButton:hover { background-color: #A0826D; }"
        "QPushButton { background-color: #E7DDD1; color: #291C0E; border: 1px solid #BCAAA4; border-radius: 10px; "
        "padding: 10px 20px; font-family: Arial, sans-serif; font-size: 12px; font-weight: bold; }"
        "QPushButton:hover { background-color: #F0E6DA; }"
        "QGroupBox { color: #291C0E; font-family: Arial, sans-serif; font-size: 13px; font-weight: bold; "
        "border: 2px solid #BCAAA4; border-radius: 8px; margin-top: 10px; padding-top: 15px; }"
        "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; padding: 5px 10px; "
        "background-color: #FAF5F0; border-radius: 4px; }"
        "QCheckBox, QRadioButton { color: #291C0E; font-family: Arial, sans-serif; font-size: 11px; spacing: 5px; }"
        "QCheckBox::indicator, QRadioButton::indicator { width: 12px; height: 12px; }"
        "QCheckBox::indicator { border: 1px solid #BCAAA4; border-radius: 2px; background-color: white; }"
        "QCheckBox::indicator:checked { background-color: #8D6E63; border-color: #8D6E63; }"
        "QRadioButton::indicator { border: 1px solid #BCAAA4; border-radius: 6px; background-color: white; }"
        "QRadioButton::indicator:checked { background-color: #8D6E63; border-color: #8D6E63; }"
        "QComboBox::drop-down { border: none; width: 30px; }"
        "QComboBox::down-arrow { image: none; border-left: 5px solid transparent; border-right: 5px solid transparent; "
        "border-top: 5px solid #8D6E63; margin-right: 10px; }"
        );
}
