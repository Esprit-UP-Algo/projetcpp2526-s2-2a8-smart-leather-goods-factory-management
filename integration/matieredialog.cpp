#include "matieredialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include <QPixmap>
#include <QFile>

MatiereDialog::MatiereDialog(QWidget *parent, DialogMode mode)
    : QDialog(parent), m_mode(mode)
{
    setupUI();
    setMinimumSize(600, 480);

    bool readOnly = (mode == DeleteMode);
    for (auto *w : {(QWidget*)txtModule,(QWidget*)txtReference,(QWidget*)txtQuantite,(QWidget*)txtSeuil})
        w->setEnabled(!readOnly);
    cmbType->setEnabled(!readOnly);
    dateExpiration->setEnabled(!readOnly);
    btnSelectPhoto->setEnabled(!readOnly);

    switch (mode) {
    case AddMode:
        setWindowTitle("Ajouter une Matière Première");
        lblTitle->setText("+ Ajouter une Nouvelle Matière");
        break;
    case EditMode:
        setWindowTitle("Modifier une Matière Première");
        lblTitle->setText("✎ Modifier la Matière");
        btnSave->setText("Mettre à Jour");
        break;
    case DeleteMode:
        setWindowTitle("Supprimer une Matière Première");
        lblTitle->setText("⚠ Confirmer la Suppression");
        btnSave->setVisible(false);
        btnDelete->setVisible(true);
        lblDeleteWarning->setText(
            "⚠ ATTENTION : Vous êtes sur le point de supprimer cette matière première.\n"
            "Cette action est irréversible.");
        lblDeleteWarning->setVisible(true);
        break;
    }
}

MatiereDialog::~MatiereDialog() {}

void MatiereDialog::setupUI()
{
    setStyleSheet(
        "QDialog{background:#FAF5F0;}"
        "QLabel{color:#291C0E;font-size:12px;font-weight:bold;}"
        "QLineEdit,QDateEdit,QComboBox{background:white;border:2px solid #BCAAA4;"
        "border-radius:6px;padding:8px;font-size:12px;color:#291C0E;}"
        "QLineEdit:focus,QDateEdit:focus,QComboBox:focus{border-color:#8D6E63;}");

    auto *lay = new QVBoxLayout(this);
    lay->setSpacing(15); lay->setContentsMargins(25,25,25,25);

    lblTitle = new QLabel(this);
    lblTitle->setStyleSheet("font-size:17px;font-weight:bold;color:#8D6E63;");
    lblTitle->setAlignment(Qt::AlignCenter);
    lay->addWidget(lblTitle);

    auto *form = new QGridLayout(); form->setSpacing(12); form->setColumnStretch(1,1);
    auto addRow = [&](int r, const QString &lbl, QWidget *w){
        form->addWidget(new QLabel(lbl,this), r, 0);
        form->addWidget(w, r, 1);
    };
    txtModule      = new QLineEdit(this); txtModule->setPlaceholderText("Ex: Cuir Pleine Fleur");
    txtReference   = new QLineEdit(this); txtReference->setPlaceholderText("Ex: Peau de Veau");
    cmbType        = new QComboBox(this);
    cmbType->addItems({"Cuir","Peau de Veau","Peau de Vachette","Ficelinée","Quincaillerie","Autre"});
    txtQuantite    = new QLineEdit(this); txtQuantite->setPlaceholderText("Ex: 2.5 m²/jour");
    txtSeuil       = new QLineEdit(this); txtSeuil->setPlaceholderText("Ex: 80");
    dateExpiration = new QDateEdit(this);
    dateExpiration->setCalendarPopup(true);
    dateExpiration->setDate(QDate::currentDate().addYears(2));

    addRow(0,"Module :",      txtModule);
    addRow(1,"Référence :",   txtReference);
    addRow(2,"Type :",        cmbType);
    addRow(3,"Consommation :",txtQuantite);
    addRow(4,"Seuil :",       txtSeuil);
    addRow(5,"Expiration :",  dateExpiration);
    
    // Photo section
    btnSelectPhoto = new QPushButton("📷 Sélectionner une Photo", this);
    btnSelectPhoto->setStyleSheet(
        "QPushButton{background:#6D4C41;color:white;border:none;border-radius:6px;padding:8px 16px;font-size:12px;}"
        "QPushButton:hover{background:#8D6E63;}");
    connect(btnSelectPhoto, &QPushButton::clicked, this, &MatiereDialog::onSelectPhoto);
    form->addWidget(new QLabel("Photo :", this), 6, 0);
    form->addWidget(btnSelectPhoto, 6, 1);
    
    lblPhotoPreview = new QLabel(this);
    lblPhotoPreview->setFixedSize(120, 120);
    lblPhotoPreview->setStyleSheet("background:#E0E0E0;border:2px dashed #BCAAA4;border-radius:8px;");
    lblPhotoPreview->setAlignment(Qt::AlignCenter);
    lblPhotoPreview->setText("Aucune\nphoto");
    form->addWidget(lblPhotoPreview, 7, 1);
    
    lay->addLayout(form);

    lblDeleteWarning = new QLabel(this);
    lblDeleteWarning->setStyleSheet("background:#f7d9d9;padding:12px;border-radius:6px;color:#8B0000;font-weight:bold;");
    lblDeleteWarning->setWordWrap(true);
    lblDeleteWarning->setVisible(false);
    lay->addWidget(lblDeleteWarning);
    lay->addStretch();

    // Buttons
    static const QString BTN_PRIMARY =
        "QPushButton{background:#8D6E63;color:white;border:none;border-radius:7px;padding:9px 22px;font-size:13px;font-weight:bold;}"
        "QPushButton:hover{background:#A0826D;}";
    static const QString BTN_DANGER  =
        "QPushButton{background:#D32F2F;color:white;border:none;border-radius:7px;padding:9px 22px;font-size:13px;font-weight:bold;}"
        "QPushButton:hover{background:#F44336;}";
    static const QString BTN_CANCEL  =
        "QPushButton{background:#BCAAA4;color:white;border:none;border-radius:7px;padding:9px 22px;font-size:13px;font-weight:bold;}"
        "QPushButton:hover{background:#A78D78;}";

    btnSave   = new QPushButton("Enregistrer", this); btnSave->setStyleSheet(BTN_PRIMARY);
    btnDelete = new QPushButton("Confirmer Suppression", this); btnDelete->setStyleSheet(BTN_DANGER); btnDelete->setVisible(false);
    btnCancel = new QPushButton("Annuler", this); btnCancel->setStyleSheet(BTN_CANCEL);

    connect(btnSave,   &QPushButton::clicked, this, &MatiereDialog::onSaveClicked);
    connect(btnDelete, &QPushButton::clicked, this, &MatiereDialog::onDeleteConfirmed);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);

    auto *btnRow = new QHBoxLayout(); btnRow->addStretch();
    btnRow->addWidget(btnSave); btnRow->addWidget(btnDelete); btnRow->addWidget(btnCancel);
    lay->addLayout(btnRow);
}

void MatiereDialog::setMatiereData(const QString &module, const QString &reference, const QString &type,
                                   const QString &quantite, const QString &seuil, const QString &dateExp,
                                   const QString &photoUrl)
{
    txtModule->setText(module);
    txtReference->setText(reference);
    int idx = cmbType->findText(type);
    if (idx >= 0) cmbType->setCurrentIndex(idx);
    txtQuantite->setText(quantite);
    txtSeuil->setText(seuil);
    dateExpiration->setDate(QDate::fromString(dateExp,"yyyy-MM-dd"));
    m_photoUrl = photoUrl;
    updatePhotoPreview();
}

QString MatiereDialog::getModule()         const { return txtModule->text(); }
QString MatiereDialog::getReference()      const { return txtReference->text(); }
QString MatiereDialog::getType()           const { return cmbType->currentText(); }
QString MatiereDialog::getQuantite()       const { return txtQuantite->text(); }
QString MatiereDialog::getSeuil()          const { return txtSeuil->text(); }
QString MatiereDialog::getDateExpiration() const { return dateExpiration->date().toString("yyyy-MM-dd"); }
QString MatiereDialog::getPhotoUrl()       const { return m_photoUrl; }

void MatiereDialog::onSelectPhoto()
{
    QString filePath = QFileDialog::getOpenFileName(this, 
        "Sélectionner une image", 
        QString(), 
        "Images (*.png *.jpg *.jpeg *.bmp *.gif)");
    
    if (!filePath.isEmpty()) {
        m_photoUrl = filePath;
        updatePhotoPreview();
    }
}

void MatiereDialog::updatePhotoPreview()
{
    if (!m_photoUrl.isEmpty() && QFile::exists(m_photoUrl)) {
        QPixmap pixmap(m_photoUrl);
        lblPhotoPreview->setPixmap(pixmap.scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        lblPhotoPreview->setText("Aucune\nphoto");
    }
}

void MatiereDialog::onSaveClicked()
{
    if (txtModule->text().isEmpty())    { QMessageBox::warning(this,"","Le module est obligatoire.");    return; }
    if (txtReference->text().isEmpty()) { QMessageBox::warning(this,"","La référence est obligatoire."); return; }
    QMessageBox::information(this,"Succès",
        m_mode==AddMode ? "Matière ajoutée avec succès !" : "Matière mise à jour avec succès !");
    accept();
}

void MatiereDialog::onDeleteConfirmed()
{
    if (QMessageBox::question(this,"Confirmation","Êtes-vous sûr de vouloir supprimer cette matière ?",
            QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes) {
        QMessageBox::information(this,"Supprimé","Matière supprimée avec succès.");
        accept();
    }
}