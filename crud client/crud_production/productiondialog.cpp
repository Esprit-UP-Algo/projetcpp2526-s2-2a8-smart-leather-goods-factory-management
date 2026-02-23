#include "productiondialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include <QDate>

// ═══════════════════════════════════════════════════════════════════════════
// IMPLÉMENTATION: ProductionCommande
// ═══════════════════════════════════════════════════════════════════════════

ProductionCommande::ProductionCommande()
    : idCommande(0)
    , ordrePassage(0)
    , avancement(0)
    , retard(false)
{
}

int ProductionCommande::getJoursRetard() const
{
    if (!retard || !dateFinPrevue.isValid()) {
        return 0;
    }
    return dateFinPrevue.daysTo(QDate::currentDate());
}

// ═══════════════════════════════════════════════════════════════════════════
// IMPLÉMENTATION: ProductionDialog
// ═══════════════════════════════════════════════════════════════════════════

ProductionDialog::ProductionDialog(QWidget *parent, DialogMode mode)
    : QDialog(parent), m_mode(mode)
{
    setupUI();
    setMinimumSize(620, 520);

    bool readOnly = (mode == DeleteMode);
    for (auto *w : {(QWidget*)txtReference,(QWidget*)txtQuantite})
        w->setEnabled(!readOnly);
    for (auto *w : {cmbProduit, cmbStatut, cmbResponsable, cmbPriorite})
        w->setEnabled(!readOnly);
    dateDebut->setEnabled(!readOnly);
    dateFin->setEnabled(!readOnly);

    switch (mode) {
    case AddMode:
        setWindowTitle("Créer une Production");
        lblTitle->setText("+ Créer une Nouvelle Production");
        txtId->setText("P016");
        break;
    case EditMode:
        setWindowTitle("Modifier une Production");
        lblTitle->setText("✎ Modifier la Production");
        btnSave->setText("Mettre à Jour");
        break;
    case DeleteMode:
        setWindowTitle("Supprimer une Production");
        lblTitle->setText("⚠ Confirmer la Suppression");
        btnSave->setVisible(false);
        btnDelete->setVisible(true);
        lblDeleteWarning->setText(
            "⚠ ATTENTION : Vous êtes sur le point de supprimer cette production.\n"
            "Cette action est irréversible.");
        lblDeleteWarning->setVisible(true);
        break;
    }
}

ProductionDialog::~ProductionDialog() {}

void ProductionDialog::setupUI()
{
    setStyleSheet(
        "QDialog{background:#FAF5F0;}"
        "QLabel{color:#291C0E;font-size:12px;font-weight:bold;}"
        "QLineEdit,QComboBox,QDateEdit{background:white;border:2px solid #BCAAA4;"
        "border-radius:6px;padding:8px;font-size:12px;color:#291C0E;}"
        "QLineEdit:focus,QComboBox:focus,QDateEdit:focus{border-color:#8D6E63;}");

    auto *lay = new QVBoxLayout(this);
    lay->setSpacing(15); lay->setContentsMargins(25,25,25,25);

    lblTitle = new QLabel(this);
    lblTitle->setStyleSheet("font-size:17px;font-weight:bold;color:#8D6E63;");
    lblTitle->setAlignment(Qt::AlignCenter);
    lay->addWidget(lblTitle);

    auto *form = new QGridLayout(); form->setSpacing(12); form->setColumnStretch(1,1);
    int r = 0;
    auto addRow = [&](const QString &lbl, QWidget *w){
        form->addWidget(new QLabel(lbl,this), r, 0);
        form->addWidget(w, r++, 1);
    };

    txtId          = new QLineEdit(this); txtId->setReadOnly(true);
    txtReference   = new QLineEdit(this); txtReference->setPlaceholderText("Ex: PROD-2024-001");
    cmbProduit     = new QComboBox(this);
    cmbProduit->addItems({"Sac à Main Cuir","Portefeuille","Ceinture","Sacoche","Porte-documents","Sac à Dos"});
    txtQuantite    = new QLineEdit(this); txtQuantite->setPlaceholderText("Ex: 100");
    cmbStatut      = new QComboBox(this);
    cmbStatut->addItems({"En Attente","En Cours","Terminé","Suspendu","Annulé"});
    dateDebut      = new QDateEdit(this);
    dateDebut->setCalendarPopup(true); dateDebut->setDate(QDate::currentDate());
    dateDebut->setDisplayFormat("dd/MM/yyyy");
    dateFin        = new QDateEdit(this);
    dateFin->setCalendarPopup(true); dateFin->setDate(QDate::currentDate().addDays(7));
    dateFin->setDisplayFormat("dd/MM/yyyy");
    cmbResponsable = new QComboBox(this);
    cmbResponsable->addItems({"Ahmed Benali","Fatima Zahra","Mohammed Alami","Khadija Mansouri","Youssef Idrissi"});
    cmbPriorite    = new QComboBox(this);
    cmbPriorite->addItems({"Basse","Normale","Haute","Urgente"});
    cmbPriorite->setCurrentIndex(1);

    addRow("ID Production :",    txtId);
    addRow("Référence * :",      txtReference);
    addRow("Produit * :",        cmbProduit);
    addRow("Quantité * :",       txtQuantite);
    addRow("Statut :",           cmbStatut);
    addRow("Date Début * :",     dateDebut);
    addRow("Date Fin Prévue * :",dateFin);
    addRow("Responsable * :",    cmbResponsable);
    addRow("Priorité :",         cmbPriorite);
    lay->addLayout(form);

    lblDeleteWarning = new QLabel(this);
    lblDeleteWarning->setStyleSheet("background:#f7d9d9;padding:12px;border-radius:6px;color:#8B0000;font-weight:bold;");
    lblDeleteWarning->setWordWrap(true); lblDeleteWarning->setVisible(false);
    lay->addWidget(lblDeleteWarning);
    lay->addStretch();

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

    connect(btnSave,   &QPushButton::clicked, this, &ProductionDialog::onSaveClicked);
    connect(btnDelete, &QPushButton::clicked, this, &ProductionDialog::onDeleteConfirmed);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);

    auto *btnRow = new QHBoxLayout(); btnRow->addStretch();
    btnRow->addWidget(btnSave); btnRow->addWidget(btnDelete); btnRow->addWidget(btnCancel);
    lay->addLayout(btnRow);
}

void ProductionDialog::setProductionData(const QString &id, const QString &reference, const QString &produit,
                                         const QString &quantite, const QString &statut, const QString &dDebut,
                                         const QString &dFin, const QString &responsable, const QString &priorite)
{
    txtId->setText(id);
    txtReference->setText(reference);
    txtQuantite->setText(quantite);
    auto setCombo = [](QComboBox *c, const QString &v){ int i=c->findText(v); if(i>=0) c->setCurrentIndex(i); };
    setCombo(cmbProduit, produit); setCombo(cmbStatut, statut);
    setCombo(cmbResponsable, responsable); setCombo(cmbPriorite, priorite);
    dateDebut->setDate(QDate::fromString(dDebut,"dd/MM/yyyy"));
    dateFin->setDate(QDate::fromString(dFin,"dd/MM/yyyy"));
}

QString ProductionDialog::getId()         const { return txtId->text(); }
QString ProductionDialog::getReference()  const { return txtReference->text(); }
QString ProductionDialog::getProduit()    const { return cmbProduit->currentText(); }
QString ProductionDialog::getQuantite()   const { return txtQuantite->text(); }
QString ProductionDialog::getStatut()     const { return cmbStatut->currentText(); }
QString ProductionDialog::getDateDebut()  const { return dateDebut->date().toString("dd/MM/yyyy"); }
QString ProductionDialog::getDateFin()    const { return dateFin->date().toString("dd/MM/yyyy"); }
QString ProductionDialog::getResponsable()const { return cmbResponsable->currentText(); }
QString ProductionDialog::getPriorite()   const { return cmbPriorite->currentText(); }

void ProductionDialog::onSaveClicked()
{
    if (txtReference->text().isEmpty()) { QMessageBox::warning(this,"","La référence est obligatoire."); return; }
    if (txtQuantite->text().isEmpty())  { QMessageBox::warning(this,"","La quantité est obligatoire."); return; }
    bool ok; int q = txtQuantite->text().toInt(&ok);
    if (!ok || q <= 0) { QMessageBox::warning(this,"","La quantité doit être un nombre positif."); return; }
    if (dateDebut->date() > dateFin->date()) { QMessageBox::warning(this,"","Date début > date fin."); return; }
    QMessageBox::information(this,"Succès",
        m_mode==AddMode ? "Production créée avec succès !" : "Production mise à jour avec succès !");
    accept();
}

void ProductionDialog::onDeleteConfirmed()
{
    if (QMessageBox::question(this,"Confirmation","Êtes-vous sûr de vouloir supprimer cette production ?",
            QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes) {
        QMessageBox::information(this,"Supprimé","Production supprimée avec succès.");
        accept();
    }
}