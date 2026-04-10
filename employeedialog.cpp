#include "employeedialog.h"
#include "connection.h"
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
#include <QRegularExpression>
#include <QFileDialog>
#include <QPixmap>
#include <QSqlQuery>
#include <QSqlError>

EmployeeDialog::EmployeeDialog(QWidget *parent, DialogMode mode)
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
        setWindowTitle("Créer Employé - CUIREA");
    } else if (mode == EditMode) {
        setWindowTitle("Modifier Employé - CUIREA");
    } else if (mode == ExportMode) {
        setWindowTitle("Exporter les Employés - CUIREA");
    } else {
        setWindowTitle("Supprimer Employé - CUIREA");
    }

    if (mode == ExportMode) {
        setMinimumSize(700, 600);
    } else {
        setMinimumSize(850, 520);
        setMaximumSize(900, 600);
    }
}

EmployeeDialog::~EmployeeDialog()
{
}

void EmployeeDialog::setupAddEditUI()
{
    QVBoxLayout *mainLay = new QVBoxLayout(this);
    mainLay->setContentsMargins(20, 18, 20, 18);
    mainLay->setSpacing(12);

    QLabel *title = new QLabel(mode == AddMode ? "Nouveau employé" : "Modifier l'employé");
    title->setObjectName("dialogTitle");
    mainLay->addWidget(title);

    // Three column layout
    QHBoxLayout *columnsLayout = new QHBoxLayout();
    columnsLayout->setSpacing(20);

    // LEFT COLUMN
    QFormLayout *leftForm = new QFormLayout();
    leftForm->setLabelAlignment(Qt::AlignRight);
    leftForm->setHorizontalSpacing(10);
    leftForm->setVerticalSpacing(10);

    matriculeEdit = new QLineEdit();
    matriculeEdit->setPlaceholderText("EMP-2024-XXX");
    matriculeEdit->setMaximumWidth(180);

    nomEdit = new QLineEdit();
    nomEdit->setPlaceholderText("Obligatoire");
    nomEdit->setMaximumWidth(180);

    prenomEdit = new QLineEdit();
    prenomEdit->setPlaceholderText("Obligatoire");
    prenomEdit->setMaximumWidth(180);

    cinEdit = new QLineEdit();
    cinEdit->setPlaceholderText("AB123456");
    cinEdit->setMaximumWidth(180);

    dateNaissanceEdit = new QDateEdit(QDate::currentDate().addYears(-25));
    dateNaissanceEdit->setCalendarPopup(true);
    dateNaissanceEdit->setDisplayFormat("dd/MM/yyyy");
    dateNaissanceEdit->setMaximumWidth(180);

    sexeCombo = new QComboBox();
    sexeCombo->addItems({"Homme", "Femme"});
    sexeCombo->setMaximumWidth(180);

    adresseEdit = new QLineEdit();
    adresseEdit->setPlaceholderText("Adresse");
    adresseEdit->setMaximumWidth(180);

    telephoneEdit = new QLineEdit();
    telephoneEdit->setPlaceholderText("0612345678");
    telephoneEdit->setMaximumWidth(180);

    leftForm->addRow("Matricule *:", matriculeEdit);
    leftForm->addRow("Nom *:", nomEdit);
    leftForm->addRow("Prénom *:", prenomEdit);
    leftForm->addRow("CIN *:", cinEdit);
    leftForm->addRow("Date Naissance *:", dateNaissanceEdit);
    leftForm->addRow("Sexe *:", sexeCombo);
    leftForm->addRow("Adresse:", adresseEdit);
    leftForm->addRow("Téléphone *:", telephoneEdit);

    // PHOTO SECTION (Center)
    QVBoxLayout *photoLayout = new QVBoxLayout();
    photoLayout->setSpacing(8);

    QLabel *photoLabel = new QLabel("Photo", this);
    photoLabel->setObjectName("sectionLabel");
    photoLabel->setAlignment(Qt::AlignCenter);
    photoLayout->addWidget(photoLabel);

    photoPreview = new QLabel(this);
    photoPreview->setObjectName("photoPreview");
    photoPreview->setFixedSize(120, 120);
    photoPreview->setAlignment(Qt::AlignCenter);
    photoPreview->setText("👤");
    photoPreview->setStyleSheet("font-size: 60px; color: #BCAAA4;");
    photoPreview->setScaledContents(true);
    photoLayout->addWidget(photoPreview, 0, Qt::AlignCenter);

    browsePhotoButton = new QPushButton("📁 Parcourir", this);
    browsePhotoButton->setObjectName("browseButton");
    browsePhotoButton->setMinimumHeight(30);
    browsePhotoButton->setMaximumWidth(120);
    connect(browsePhotoButton, &QPushButton::clicked, this, &EmployeeDialog::onBrowsePhoto);
    photoLayout->addWidget(browsePhotoButton, 0, Qt::AlignCenter);

    photoUrlEdit = new QLineEdit(this);
    photoUrlEdit->setPlaceholderText("Chemin");
    photoUrlEdit->setReadOnly(true);
    photoUrlEdit->setMaximumWidth(120);
    photoLayout->addWidget(photoUrlEdit, 0, Qt::AlignCenter);

    photoLayout->addStretch();

    // RIGHT COLUMN
    QFormLayout *rightForm = new QFormLayout();
    rightForm->setLabelAlignment(Qt::AlignRight);
    rightForm->setHorizontalSpacing(10);
    rightForm->setVerticalSpacing(10);

    emailEdit = new QLineEdit();
    emailEdit->setPlaceholderText("email@cuirea.com");
    emailEdit->setMaximumWidth(220);

    posteCombo = new QComboBox();
    posteCombo->addItems({"Supervisor", "Inspector", "Operator", "Team Leader", "Technician",
                         "Designer", "Manager", "HR Officer"});
    posteCombo->setMaximumWidth(220);

    departementCombo = new QComboBox();
    departementCombo->addItems({"Production", "Quality Control", "Cutting", "Stitching",
                               "Finishing", "Design", "Warehouse", "Administration",
                               "Maintenance", "Packaging"});
    departementCombo->setMaximumWidth(220);

    dateEmbaucheEdit = new QDateEdit(QDate::currentDate());
    dateEmbaucheEdit->setCalendarPopup(true);
    dateEmbaucheEdit->setDisplayFormat("dd/MM/yyyy");
    dateEmbaucheEdit->setMaximumWidth(220);

    rightForm->addRow("Email *:", emailEdit);
    rightForm->addRow("Poste *:", posteCombo);
    rightForm->addRow("Département *:", departementCombo);
    rightForm->addRow("Date Embauche *:", dateEmbaucheEdit);
    
    // === SECTION ACCÈS SYSTÈME ===
    QLabel *accessTitle = new QLabel("🔐 Accès Système", this);
    accessTitle->setStyleSheet(
        "font-size: 12px; font-weight: bold; color: #8D6E63; "
        "margin-top: 8px; padding: 5px; background-color: #FFF8F0; "
        "border-left: 3px solid #8D6E63; border-radius: 3px;"
    );
    rightForm->addRow(accessTitle);
    
    // Rôle système
    roleSystemeCombo = new QComboBox();
    roleSystemeCombo->addItems({
        "Consultation uniquement",
        "Gestion fournisseurs",
        "Service client",
        "Service vente",
        "Responsable stock",
        "Responsable vente",
        "Ressources Humaines",
        "Administrateur"
    });
    roleSystemeCombo->setCurrentIndex(0); // Par défaut: EMPLOYE
    roleSystemeCombo->setMaximumWidth(220);
    roleSystemeCombo->setStyleSheet(
        "QComboBox { background-color: white; color: #291C0E; border: 2px solid #8D6E63; "
        "border-radius: 6px; padding: 6px; font-family: Arial, sans-serif; font-size: 10px; "
        "min-height: 22px; font-weight: bold; }"
        "QComboBox:focus { border: 2px solid #6E473B; }"
        "QComboBox::drop-down { border: none; width: 25px; }"
        "QComboBox::down-arrow { image: none; border-left: 4px solid transparent; "
        "border-right: 4px solid transparent; border-top: 4px solid #8D6E63; margin-right: 8px; }"
    );
    rightForm->addRow("Rôle Système *:", roleSystemeCombo);
    
    // Mot de passe
    motDePasseEdit = new QLineEdit();
    motDePasseEdit->setEchoMode(QLineEdit::Password);
    motDePasseEdit->setPlaceholderText("Min. 6 caractères");
    motDePasseEdit->setMaximumWidth(220);
    motDePasseEdit->setStyleSheet(
        "QLineEdit { background-color: white; color: #291C0E; border: 2px solid #BCAAA4; "
        "border-radius: 6px; padding: 6px; font-family: Arial, sans-serif; font-size: 10px; "
        "min-height: 22px; }"
        "QLineEdit:focus { border: 2px solid #8D6E63; }"
    );
    rightForm->addRow("Mot de passe *:", motDePasseEdit);
    
    // Confirmation mot de passe
    confirmMotDePasseEdit = new QLineEdit();
    confirmMotDePasseEdit->setEchoMode(QLineEdit::Password);
    confirmMotDePasseEdit->setPlaceholderText("Retaper");
    confirmMotDePasseEdit->setMaximumWidth(220);
    confirmMotDePasseEdit->setStyleSheet(motDePasseEdit->styleSheet());
    rightForm->addRow("Confirmation *:", confirmMotDePasseEdit);
    
    // Compte actif
    actifCheckBox = new QCheckBox("Compte actif");
    actifCheckBox->setChecked(true);
    actifCheckBox->setStyleSheet(
        "QCheckBox { color: #291C0E; font-family: Arial, sans-serif; font-size: 10px; "
        "font-weight: bold; spacing: 5px; }"
        "QCheckBox::indicator { width: 14px; height: 14px; border: 2px solid #8D6E63; "
        "border-radius: 3px; background-color: white; }"
        "QCheckBox::indicator:checked { background-color: #8D6E63; }"
    );
    rightForm->addRow("", actifCheckBox);

    columnsLayout->addLayout(leftForm);
    columnsLayout->addLayout(photoLayout);
    columnsLayout->addLayout(rightForm);

    mainLay->addLayout(columnsLayout);

    QDialogButtonBox *btnBox = new QDialogButtonBox(
        QDialogButtonBox::Save | QDialogButtonBox::Cancel);
    QPushButton *saveBtn = btnBox->button(QDialogButtonBox::Save);
    saveBtn->setText("Enregistrer");
    saveBtn->setObjectName("saveButton");

    mainLay->addWidget(btnBox);

    connect(btnBox, &QDialogButtonBox::accepted, this, &EmployeeDialog::validateAndAccept);
    connect(btnBox, &QDialogButtonBox::rejected, this, &EmployeeDialog::reject);
}

void EmployeeDialog::setupDeleteUI()
{
    QVBoxLayout *mainLay = new QVBoxLayout(this);
    mainLay->setContentsMargins(40, 30, 40, 30);
    mainLay->setSpacing(20);

    setMinimumSize(500, 350);

    QLabel *title = new QLabel("Confirmation de Suppression");
    title->setObjectName("dialogTitle");
    title->setAlignment(Qt::AlignCenter);
    mainLay->addWidget(title);

    QLabel *question = new QLabel("Êtes-vous sûr de vouloir supprimer cet employé ?");
    question->setObjectName("questionLabel");
    question->setAlignment(Qt::AlignCenter);
    question->setWordWrap(true);
    mainLay->addWidget(question);

    mainLay->addSpacing(10);

    // Employee info will be set later
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

    connect(cancelBtn, &QPushButton::clicked, this, &EmployeeDialog::reject);
    connect(deleteBtn, &QPushButton::clicked, this, &EmployeeDialog::accept);
}

void EmployeeDialog::setEmployeeData(const QString &id, const QString &matricule, const QString &nom,
                                    const QString &prenom, const QString &cin, const QString &dateNaissance,
                                    const QString &sexe, const QString &adresse, const QString &telephone,
                                    const QString &email, const QString &poste, const QString &role,
                                    const QString &departement, const QString &dateEmbauche, const QString &photoPath)
{
    if (mode == DeleteMode) {
        deleteId = id;
        deleteMatricule = matricule;
        deleteNom = nom;
        deletePrenom = prenom;
        deleteDepartement = departement;
        deletePoste = poste;

        QLabel *infoLabel = findChild<QLabel*>("infoLabel");
        if (infoLabel) {
            infoLabel->setText(
                QString("<b>ID:</b> %1<br>"
                       "<b>Matricule:</b> %2<br>"
                       "<b>Nom:</b> %3 %4<br>"
                       "<b>Département:</b> %5<br>"
                       "<b>Poste:</b> %6")
                    .arg(id, matricule, nom, prenom, departement, poste)
            );
        }
    } else if (mode == EditMode) {
        matriculeEdit->setText(matricule);
        nomEdit->setText(nom);
        prenomEdit->setText(prenom);
        cinEdit->setText(cin);
        dateNaissanceEdit->setDate(QDate::fromString(dateNaissance, "dd/MM/yyyy"));
        sexeCombo->setCurrentText(sexe);
        adresseEdit->setText(adresse);
        telephoneEdit->setText(telephone);
        emailEdit->setText(email);
        posteCombo->setCurrentText(poste);
        departementCombo->setCurrentText(departement);
        dateEmbaucheEdit->setDate(QDate::fromString(dateEmbauche, "dd/MM/yyyy"));
        
        // Charger la photo si disponible
        if (!photoPath.isEmpty() && QFile::exists(photoPath)) {
            photoUrlEdit->setText(photoPath);
            QPixmap pixmap(photoPath);
            if (!pixmap.isNull()) {
                photoPreview->setPixmap(pixmap.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));
                photoPreview->setStyleSheet("border: 2px solid #8D6E63; border-radius: 8px;");
            }
        }
        
        // Charger les données d'authentification depuis la base de données
        QSqlQuery query(Connection::instance()->getDatabase());
        query.prepare("SELECT MOT_DE_PASSE, ROLE_SYSTEME, ACTIF FROM CUIREA.EMPLOYES WHERE ID_EMPLOYE = :id");
        query.bindValue(":id", id.toInt());
        
        if (query.exec() && query.next()) {
            QString motDePasse = query.value(0).toString();
            QString roleSysteme = query.value(1).toString();
            int actif = query.value(2).toInt();
            
            // Remplir les champs d'authentification
            motDePasseEdit->setText(motDePasse);
            confirmMotDePasseEdit->setText(motDePasse);
            actifCheckBox->setChecked(actif == 1);
            
            // Mapper le rôle système vers l'index du combo
            int roleIndex = 0;
            if (roleSysteme == "EMPLOYE") roleIndex = 0;
            else if (roleSysteme == "SERVICE_FOURNISSEUR") roleIndex = 1;
            else if (roleSysteme == "SERVICE_CLIENT") roleIndex = 2;
            else if (roleSysteme == "SERVICE_VENTE") roleIndex = 3;
            else if (roleSysteme == "RESPONSABLE_STOCK") roleIndex = 4;
            else if (roleSysteme == "RESPONSABLE_VENTE") roleIndex = 5;
            else if (roleSysteme == "RH") roleIndex = 6;
            else if (roleSysteme == "ADMIN") roleIndex = 7;
            
            roleSystemeCombo->setCurrentIndex(roleIndex);
        }
    }
}

void EmployeeDialog::setupExportUI()
{
    QVBoxLayout *mainLay = new QVBoxLayout(this);
    mainLay->setContentsMargins(28, 24, 28, 24);
    mainLay->setSpacing(20);

    QLabel *title = new QLabel("Exporter les données des employés");
    title->setObjectName("dialogTitle");
    mainLay->addWidget(title);

    // Format section
    QGroupBox *formatGroup = new QGroupBox("Format d'exportation");
    formatGroup->setObjectName("groupBox");
    QFormLayout *formatLayout = new QFormLayout(formatGroup);
    formatLayout->setLabelAlignment(Qt::AlignRight);
    formatLayout->setHorizontalSpacing(14);
    formatLayout->setVerticalSpacing(12);

    formatCombo = new QComboBox();
    formatCombo->addItems({"Excel (.xlsx)", "CSV (.csv)", "PDF (.pdf)", "JSON (.json)"});
    formatLayout->addRow("Format *:", formatCombo);

    fileNameEdit = new QLineEdit();
    fileNameEdit->setPlaceholderText("employees_export");
    formatLayout->addRow("Nom du fichier *:", fileNameEdit);

    locationEdit = new QLineEdit();
    locationEdit->setPlaceholderText("C:\\Users\\Documents\\");
    locationEdit->setReadOnly(true);
    QPushButton *browseBtn = new QPushButton("Parcourir...");
    browseBtn->setObjectName("browseButton");
    browseBtn->setEnabled(false);
    QHBoxLayout *locLayout = new QHBoxLayout();
    locLayout->addWidget(locationEdit);
    locLayout->addWidget(browseBtn);
    formatLayout->addRow("Emplacement *:", locLayout);

    mainLay->addWidget(formatGroup);

    // Data selection section
    QGroupBox *dataGroup = new QGroupBox("Données à exporter");
    dataGroup->setObjectName("groupBox");
    QVBoxLayout *dataLayout = new QVBoxLayout(dataGroup);

    // Radio buttons for scope
    QLabel *scopeLabel = new QLabel("Portée de l'export :");
    scopeLabel->setObjectName("sectionLabel");
    dataLayout->addWidget(scopeLabel);

    radioAll = new QRadioButton("Tous les employés");
    radioSelected = new QRadioButton("Employés sélectionnés uniquement");
    radioFiltered = new QRadioButton("Résultats de recherche actuels");
    radioAll->setChecked(true);

    dataLayout->addWidget(radioAll);
    dataLayout->addWidget(radioSelected);
    dataLayout->addWidget(radioFiltered);

    dataLayout->addSpacing(10);

    // Checkboxes for fields
    QLabel *fieldsLabel = new QLabel("Champs à inclure :");
    fieldsLabel->setObjectName("sectionLabel");
    dataLayout->addWidget(fieldsLabel);

    QWidget *fieldsWidget = new QWidget();
    QGridLayout *fieldsGrid = new QGridLayout(fieldsWidget);
    fieldsGrid->setSpacing(10);
    fieldsGrid->setContentsMargins(10, 10, 10, 10);

    chkID = new QCheckBox("ID");
    chkMatricule = new QCheckBox("Matricule");
    chkNom = new QCheckBox("Nom");
    chkPrenom = new QCheckBox("Prénom");
    chkCIN = new QCheckBox("CIN");
    chkDateNaissance = new QCheckBox("Date de naissance");
    chkDepartement = new QCheckBox("Département");
    chkPoste = new QCheckBox("Poste");
    chkTelephone = new QCheckBox("Téléphone");
    chkEmail = new QCheckBox("Email");

    // Check all by default
    chkID->setChecked(true);
    chkMatricule->setChecked(true);
    chkNom->setChecked(true);
    chkPrenom->setChecked(true);
    chkCIN->setChecked(true);
    chkDateNaissance->setChecked(true);
    chkDepartement->setChecked(true);
    chkPoste->setChecked(true);
    chkTelephone->setChecked(true);
    chkEmail->setChecked(true);

    fieldsGrid->addWidget(chkID, 0, 0);
    fieldsGrid->addWidget(chkMatricule, 0, 1);
    fieldsGrid->addWidget(chkNom, 1, 0);
    fieldsGrid->addWidget(chkPrenom, 1, 1);
    fieldsGrid->addWidget(chkCIN, 2, 0);
    fieldsGrid->addWidget(chkDateNaissance, 2, 1);
    fieldsGrid->addWidget(chkDepartement, 3, 0);
    fieldsGrid->addWidget(chkPoste, 3, 1);
    fieldsGrid->addWidget(chkTelephone, 4, 0);
    fieldsGrid->addWidget(chkEmail, 4, 1);

    dataLayout->addWidget(fieldsWidget);

    mainLay->addWidget(dataGroup);

    // Note
    QLabel *noteLabel = new QLabel("* Champs obligatoires");
    noteLabel->setObjectName("noteLabel");
    mainLay->addWidget(noteLabel);

    // Buttons
    QDialogButtonBox *btnBox = new QDialogButtonBox(
        QDialogButtonBox::Save | QDialogButtonBox::Cancel);
    QPushButton *exportBtn = btnBox->button(QDialogButtonBox::Save);
    exportBtn->setText("Exporter");
    exportBtn->setObjectName("exportButton");

    mainLay->addWidget(btnBox);

    connect(btnBox, &QDialogButtonBox::accepted, this, &EmployeeDialog::accept);
    connect(btnBox, &QDialogButtonBox::rejected, this, &EmployeeDialog::reject);
}

void EmployeeDialog::applyStyles()
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

// Getters pour récupérer les données du dialog
QString EmployeeDialog::getMatricule() const
{
    return matriculeEdit ? matriculeEdit->text().trimmed() : QString();
}

QString EmployeeDialog::getNom() const
{
    return nomEdit ? nomEdit->text().trimmed() : QString();
}

QString EmployeeDialog::getPrenom() const
{
    return prenomEdit ? prenomEdit->text().trimmed() : QString();
}

QString EmployeeDialog::getCin() const
{
    return cinEdit ? cinEdit->text().trimmed() : QString();
}

QDate EmployeeDialog::getDateNaissance() const
{
    return dateNaissanceEdit ? dateNaissanceEdit->date() : QDate();
}

QString EmployeeDialog::getSexe() const
{
    if (!sexeCombo) return QString();
    // Convertir "Homme" -> "M" et "Femme" -> "F" pour la base de données
    QString sexe = sexeCombo->currentText();
    if (sexe == "Homme") return "M";
    if (sexe == "Femme") return "F";
    return "M"; // Par défaut
}

QString EmployeeDialog::getAdresse() const
{
    return adresseEdit ? adresseEdit->text().trimmed() : QString();
}

QString EmployeeDialog::getTelephone() const
{
    return telephoneEdit ? telephoneEdit->text().trimmed() : QString();
}

QString EmployeeDialog::getEmail() const
{
    return emailEdit ? emailEdit->text().trimmed() : QString();
}

QString EmployeeDialog::getPoste() const
{
    return posteCombo ? posteCombo->currentText() : QString();
}

QString EmployeeDialog::getRole() const
{
    return QString(); // Champ supprimé
}

QString EmployeeDialog::getRoleSysteme() const
{
    if (!roleSystemeCombo) return "EMPLOYE";
    
    // Mapper l'affichage vers le code du rôle
    int index = roleSystemeCombo->currentIndex();
    switch (index) {
        case 0: return "EMPLOYE";
        case 1: return "SERVICE_FOURNISSEUR";
        case 2: return "SERVICE_CLIENT";
        case 3: return "SERVICE_VENTE";
        case 4: return "RESPONSABLE_STOCK";
        case 5: return "RESPONSABLE_VENTE";
        case 6: return "RH";
        case 7: return "ADMIN";
        default: return "EMPLOYE";
    }
}

QString EmployeeDialog::getMotDePasse() const
{
    return motDePasseEdit ? motDePasseEdit->text() : QString();
}

bool EmployeeDialog::isActif() const
{
    return actifCheckBox ? actifCheckBox->isChecked() : true;
}

QString EmployeeDialog::getDepartement() const
{
    return departementCombo ? departementCombo->currentText() : QString();
}

QDate EmployeeDialog::getDateEmbauche() const
{
    return dateEmbaucheEdit ? dateEmbaucheEdit->date() : QDate();
}

QString EmployeeDialog::getPhotoPath() const
{
    return photoUrlEdit ? photoUrlEdit->text() : QString();
}

void EmployeeDialog::onBrowsePhoto()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Sélectionner une photo", "",
        "Images (*.png *.jpg *.jpeg *.bmp)");
    
    if (!fileName.isEmpty()) {
        photoUrlEdit->setText(fileName);
        
        // Afficher l'aperçu de la photo
        QPixmap pixmap(fileName);
        if (!pixmap.isNull()) {
            photoPreview->setPixmap(pixmap.scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            photoPreview->setStyleSheet("border: 2px solid #8D6E63; border-radius: 8px;");
        }
    }
}

// Validation des champs obligatoires
void EmployeeDialog::validateAndAccept()
{
    if (mode != AddMode && mode != EditMode) {
        accept();
        return;
    }

    // Validation des champs obligatoires
    QStringList errors;

    if (matriculeEdit->text().trimmed().isEmpty()) {
        errors << "- Le matricule est obligatoire";
        matriculeEdit->setStyleSheet("border: 2px solid red;");
    } else {
        matriculeEdit->setStyleSheet("");
    }

    if (nomEdit->text().trimmed().isEmpty()) {
        errors << "- Le nom est obligatoire";
        nomEdit->setStyleSheet("border: 2px solid red;");
    } else {
        nomEdit->setStyleSheet("");
    }

    if (prenomEdit->text().trimmed().isEmpty()) {
        errors << "- Le prénom est obligatoire";
        prenomEdit->setStyleSheet("border: 2px solid red;");
    } else {
        prenomEdit->setStyleSheet("");
    }

    if (cinEdit->text().trimmed().isEmpty()) {
        errors << "- Le CIN est obligatoire";
        cinEdit->setStyleSheet("border: 2px solid red;");
    } else if (cinEdit->text().trimmed().length() != 8) {
        errors << "- Le CIN doit contenir 8 caractères";
        cinEdit->setStyleSheet("border: 2px solid red;");
    } else {
        cinEdit->setStyleSheet("");
    }

    if (telephoneEdit->text().trimmed().isEmpty()) {
        errors << "- Le téléphone est obligatoire";
        telephoneEdit->setStyleSheet("border: 2px solid red;");
    } else if (!telephoneEdit->text().trimmed().startsWith("0") ||
               telephoneEdit->text().trimmed().length() != 10) {
        errors << "- Le téléphone doit contenir 10 chiffres et commencer par 0";
        telephoneEdit->setStyleSheet("border: 2px solid red;");
    } else {
        telephoneEdit->setStyleSheet("");
    }

    if (emailEdit->text().trimmed().isEmpty()) {
        errors << "- L'email est obligatoire";
        emailEdit->setStyleSheet("border: 2px solid red;");
    } else if (!emailEdit->text().contains("@") || !emailEdit->text().contains(".")) {
        errors << "- L'email doit être valide (contenir @ et .)";
        emailEdit->setStyleSheet("border: 2px solid red;");
    } else {
        emailEdit->setStyleSheet("");
    }

    // Validation du mot de passe
    QString password = motDePasseEdit->text();
    QString confirmPassword = confirmMotDePasseEdit->text();
    
    if (password.isEmpty()) {
        errors << "- Le mot de passe est obligatoire";
        motDePasseEdit->setStyleSheet("border: 2px solid red;");
    } else if (password.length() < 6) {
        errors << "- Le mot de passe doit contenir au moins 6 caractères";
        motDePasseEdit->setStyleSheet("border: 2px solid red;");
    } else {
        motDePasseEdit->setStyleSheet("");
    }
    
    if (confirmPassword.isEmpty()) {
        errors << "- La confirmation du mot de passe est obligatoire";
        confirmMotDePasseEdit->setStyleSheet("border: 2px solid red;");
    } else if (password != confirmPassword) {
        errors << "- Les mots de passe ne correspondent pas";
        confirmMotDePasseEdit->setStyleSheet("border: 2px solid red;");
    } else {
        confirmMotDePasseEdit->setStyleSheet("");
    }

    // Si des erreurs existent, afficher un message
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

    // Validation réussie
    accept();
}
