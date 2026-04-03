#include "clientmanagerdialog.h"
#include "qsqlquery.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QDate>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

// ============================================================
// Constructeur
// ============================================================

ClientManagerDialog::ClientManagerDialog(QWidget *parent, DialogMode mode)
    : QDialog(parent),
    mode(mode),
    nomEdit(nullptr), prenomEdit(nullptr), sexeCombo(nullptr),
    cinEdit(nullptr), paysCombo(nullptr), villeEdit(nullptr),
    adresseEdit(nullptr), emailEdit(nullptr), dateInscrit(nullptr),
    formatCombo(nullptr), fileNameEdit(nullptr), locationEdit(nullptr),
    chkNom(nullptr), chkPrenom(nullptr), chkSexe(nullptr), chkCIN(nullptr),
    chkPays(nullptr), chkVille(nullptr), chkAdresse(nullptr), chkEmail(nullptr),
    radioAll(nullptr), radioSelected(nullptr), radioFiltered(nullptr),
    deleteId(-1), editingId(-1)
{
    switch (mode) {
    case AddMode:
        setWindowTitle("Ajouter un client");
        setupAddEditUI();

        cinEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("^\\d{0,8}$"), this));
        nomEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("^[a-zA-Z\\s]+$"), this));
        prenomEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("^[a-zA-Z\\s]+$"), this));
        break;

    case EditMode:
        setWindowTitle("Modifier un client");
        setupAddEditUI();

        cinEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("^\\d{0,8}$"), this));
        nomEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("^[a-zA-Z\\s]+$"), this));
        prenomEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("^[a-zA-Z\\s]+$"), this));
        break;

    case DeleteMode:
        setWindowTitle("Supprimer un client");
        setupDeleteUI();
        break;

    case ExportMode:
        setWindowTitle("Exporter les clients");
        setupExportUI();
        break;
    }

    applyStyles();
}

ClientManagerDialog::~ClientManagerDialog() {}

// ============================================================
// UI — Add / Edit
// ============================================================

void ClientManagerDialog::setupAddEditUI()
{
    QFormLayout *form = new QFormLayout();

    nomEdit     = new QLineEdit(this);
    prenomEdit  = new QLineEdit(this);
    sexeCombo   = new QComboBox(this);
    cinEdit     = new QLineEdit(this);
    paysCombo   = new QComboBox(this);
    villeEdit   = new QLineEdit(this);
    adresseEdit = new QLineEdit(this);
    emailEdit   = new QLineEdit(this);
    dateInscrit = new QDateEdit(QDate::currentDate(), this);

    dateInscrit->setCalendarPopup(true);
    dateInscrit->setDisplayFormat("yyyy-MM-dd");

    sexeCombo->addItems({"Homme", "Femme", "Autre"});

    // ✅ Pays list
    paysCombo->addItems({
        "Tunisie",
        "France",
        "Algérie",
        "Maroc",
        "Libye",
        "Italie",
        "Espagne"
    });

    nomEdit->setPlaceholderText("Nom");
    prenomEdit->setPlaceholderText("Prénom");
    cinEdit->setPlaceholderText("ex: 12345678");
    cinEdit->setMaxLength(8);
    villeEdit->setPlaceholderText("Ville");
    adresseEdit->setPlaceholderText("Adresse complète");
    emailEdit->setPlaceholderText("exemple@mail.com");

    form->addRow("Nom *", nomEdit);
    form->addRow("Prénom", prenomEdit);
    form->addRow("Sexe", sexeCombo);
    form->addRow("CIN", cinEdit);
    form->addRow("Pays", paysCombo);
    form->addRow("Ville", villeEdit);
    form->addRow("Adresse", adresseEdit);
    form->addRow("Email", emailEdit);
    form->addRow("Date d'inscription", dateInscrit);

    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    connect(buttons, &QDialogButtonBox::accepted, this, &ClientManagerDialog::onAccepted);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QVBoxLayout *main = new QVBoxLayout(this);
    main->addLayout(form);
    main->addWidget(buttons);
    setLayout(main);
    setMinimumWidth(400);
}

// ============================================================
// Delete UI
// ============================================================

void ClientManagerDialog::setupDeleteUI()
{
    QVBoxLayout *main = new QVBoxLayout(this);

    QLabel *lbl = new QLabel(
        QString("Voulez-vous vraiment supprimer le client :\n\n"
                "  Nom    : %1 %2\n"
                "  Sexe   : %3\n"
                "  CIN    : %4\n"
                "  Pays   : %5  —  Ville : %6")
            .arg(deleteNom, deletePrenom, deleteSexe, deleteCIN, deletePays, deleteVille),
        this);

    lbl->setWordWrap(true);

    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Yes | QDialogButtonBox::No, this);

    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    main->addWidget(lbl);
    main->addWidget(buttons);
    setLayout(main);
    setMinimumWidth(380);
}

// ============================================================
// Export UI
// ============================================================

void ClientManagerDialog::setupExportUI()
{
    QVBoxLayout *main = new QVBoxLayout(this);

    QGroupBox *colGroup = new QGroupBox("Colonnes à exporter", this);
    QHBoxLayout *colLayout = new QHBoxLayout(colGroup);

    chkNom = new QCheckBox("Nom", colGroup); chkNom->setChecked(true);
    chkPrenom = new QCheckBox("Prénom", colGroup); chkPrenom->setChecked(true);
    chkSexe = new QCheckBox("Sexe", colGroup);
    chkCIN = new QCheckBox("CIN", colGroup);
    chkPays = new QCheckBox("Pays", colGroup);
    chkVille = new QCheckBox("Ville", colGroup);
    chkAdresse = new QCheckBox("Adresse", colGroup);
    chkEmail = new QCheckBox("Email", colGroup);

    colLayout->addWidget(chkNom);
    colLayout->addWidget(chkPrenom);
    colLayout->addWidget(chkSexe);
    colLayout->addWidget(chkCIN);
    colLayout->addWidget(chkPays);
    colLayout->addWidget(chkVille);
    colLayout->addWidget(chkAdresse);
    colLayout->addWidget(chkEmail);

    QGroupBox *scopeGroup = new QGroupBox("Périmètre", this);
    QHBoxLayout *scopeLayout = new QHBoxLayout(scopeGroup);

    radioAll = new QRadioButton("Tous", scopeGroup);
    radioAll->setChecked(true);
    radioSelected = new QRadioButton("Sélectionnés", scopeGroup);
    radioFiltered = new QRadioButton("Filtrés", scopeGroup);

    scopeLayout->addWidget(radioAll);
    scopeLayout->addWidget(radioSelected);
    scopeLayout->addWidget(radioFiltered);

    QFormLayout *destForm = new QFormLayout();

    formatCombo = new QComboBox(this);
    formatCombo->addItems({"CSV", "Excel (.xlsx)", "PDF"});

    fileNameEdit = new QLineEdit(this);
    fileNameEdit->setPlaceholderText("clients_export");

    locationEdit = new QLineEdit(this);
    locationEdit->setPlaceholderText("/chemin/vers/dossier");

    destForm->addRow("Format", formatCombo);
    destForm->addRow("Nom du fichier", fileNameEdit);
    destForm->addRow("Emplacement", locationEdit);

    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    main->addWidget(colGroup);
    main->addWidget(scopeGroup);
    main->addLayout(destForm);
    main->addWidget(buttons);

    setLayout(main);
    setMinimumWidth(520);
}

// ============================================================
// setClientData
// ============================================================

void ClientManagerDialog::setClientData(const QString &nom, const QString &prenom,
                                        const QString &sexe, const QString &cin,
                                        const QString &pays, const QString &ville,
                                        const QString &adresse, const QString &email)
{
    if (mode == AddMode || mode == EditMode) {
        if (nomEdit) nomEdit->setText(nom);
        if (prenomEdit) prenomEdit->setText(prenom);

        if (sexeCombo) {
            int idx = sexeCombo->findText(sexe);
            if (idx >= 0) sexeCombo->setCurrentIndex(idx);
        }

        if (cinEdit) cinEdit->setText(cin);

        if (paysCombo) {
            int idx = paysCombo->findText(pays);
            if (idx >= 0) paysCombo->setCurrentIndex(idx);
        }

        if (villeEdit) villeEdit->setText(ville);
        if (adresseEdit) adresseEdit->setText(adresse);
        if (emailEdit) emailEdit->setText(email);
    }

    deleteNom = nom;
    deletePrenom = prenom;
    deleteSexe = sexe;
    deleteCIN = cin;
    deletePays = pays;
    deleteVille = ville;
}

// ============================================================
// onAccepted
// ============================================================

void ClientManagerDialog::onAccepted()
{
    if (mode == AddMode || mode == EditMode)
    {
        QString nom = nomEdit->text().trimmed();
        QString prenom = prenomEdit->text().trimmed();
        QString sexe = sexeCombo->currentText();
        QString cin = cinEdit->text().trimmed();
        QString pays = paysCombo->currentText();
        QString ville = villeEdit->text().trimmed();
        QString adresse = adresseEdit->text().trimmed();
        QString email = emailEdit->text().trimmed();

        if (nom.isEmpty() || prenom.isEmpty() || cin.isEmpty())
        {
            QMessageBox::warning(this, "Erreur", "Champs obligatoires manquants");
            return;
        }

        if (cin.length() != 8)
        {
            QMessageBox::warning(this, "Erreur", "CIN doit contenir 8 chiffres");
            return;
        }
    }

    accept();
}

// ============================================================
// applyStyles
// ============================================================

void ClientManagerDialog::applyStyles()
{
    setStyleSheet(
        "QDialog { background-color: #FAF5F0; }"

        "QLabel#dialogTitle { color: #291C0E; font-family: 'Times New Roman', serif; font-size: 20px; font-weight: bold; padding: 10px; }"

        "QLabel#questionLabel { color: #291C0E; font-family: Arial, sans-serif; font-size: 14px; }"

        "QLabel#infoLabel { color: #291C0E; font-family: Arial, sans-serif; font-size: 12px; background-color: #FFF8F0; border: 2px solid #BCAAA4; border-radius: 8px; padding: 15px; }"

        "QLabel#warningLabel { color: #A0522D; font-family: Arial, sans-serif; font-size: 12px; font-weight: bold; }"

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

        "QPushButton#deleteButton { background-color: #A0522D; color: white; border: none; border-radius: 8px; padding: 10px 20px; font-family: Arial, sans-serif; font-size: 11px; font-weight: bold; }"
        "QPushButton#deleteButton:hover { background-color: #B8633D; }"

        "QPushButton#cancelButton { background-color: #E7DDD1; color: #291C0E; border: 1px solid #BCAAA4; border-radius: 8px; padding: 10px 20px; font-family: Arial, sans-serif; font-size: 11px; font-weight: bold; }"
        "QPushButton#cancelButton:hover { background-color: #F0E6DA; }"

        "QPushButton#exportButton { background-color: #8D6E63; color: white; border: none; border-radius: 10px; padding: 10px 20px; font-family: Arial, sans-serif; font-size: 12px; font-weight: bold; }"
        "QPushButton#exportButton:hover { background-color: #A0826D; }"

        "QPushButton { background-color: #E7DDD1; color: #291C0E; border: 1px solid #BCAAA4; border-radius: 10px; padding: 10px 20px; font-family: Arial, sans-serif; font-size: 12px; font-weight: bold; }"
        "QPushButton:hover { background-color: #F0E6DA; }"

        "QGroupBox { color: #291C0E; font-family: Arial, sans-serif; font-size: 13px; font-weight: bold; border: 2px solid #BCAAA4; border-radius: 8px; margin-top: 10px; padding-top: 15px; }"

        "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; padding: 5px 10px; background-color: #FAF5F0; border-radius: 4px; }"

        "QCheckBox, QRadioButton { color: #291C0E; font-family: Arial, sans-serif; font-size: 11px; spacing: 5px; }"

        "QCheckBox::indicator, QRadioButton::indicator { width: 12px; height: 12px; }"

        "QCheckBox::indicator { border: 1px solid #BCAAA4; border-radius: 2px; background-color: white; }"
        "QCheckBox::indicator:checked { background-color: #8D6E63; border-color: #8D6E63; }"

        "QRadioButton::indicator { border: 1px solid #BCAAA4; border-radius: 6px; background-color: white; }"
        "QRadioButton::indicator:checked { background-color: #8D6E63; border-color: #8D6E63; }"

        "QComboBox::drop-down { border: none; width: 30px; }"

        "QComboBox::down-arrow { image: none; border-left: 5px solid transparent; border-right: 5px solid transparent; border-top: 5px solid #8D6E63; margin-right: 10px; }"
        );
}
