#include "articledialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QGroupBox>
#include <QColorDialog>

ArticleDialog::ArticleDialog(QWidget *parent, Mode mode)
    : QDialog(parent), dialogMode(mode)
{
    setupUI();
    applyStyles();

    switch (dialogMode) {
    case AddMode:  setWindowTitle("Ajouter un Article"); break;
    case EditMode: setWindowTitle("Modifier un Article"); break;
    case DeleteMode:
        setWindowTitle("Supprimer un Article");
        lineEditReference->setReadOnly(true); lineEditNom->setReadOnly(true);
        comboBoxCategorie->setEnabled(false); comboBoxType->setEnabled(false);
        lineEditModele3D->setReadOnly(true);
        sliderR->setEnabled(false); sliderG->setEnabled(false); sliderB->setEnabled(false);
        spinLargeur->setReadOnly(true); spinHauteur->setReadOnly(true); spinProfondeur->setReadOnly(true);
        spinBoxPrix->setReadOnly(true); spinBoxCout->setReadOnly(true);
        comboBoxStatut->setEnabled(false);
        break;
    case ViewMode:
        setWindowTitle("Consulter un Article");
        lineEditReference->setReadOnly(true); lineEditNom->setReadOnly(true);
        comboBoxCategorie->setEnabled(false); comboBoxType->setEnabled(false);
        lineEditModele3D->setReadOnly(true);
        sliderR->setEnabled(false); sliderG->setEnabled(false); sliderB->setEnabled(false);
        spinLargeur->setReadOnly(true); spinHauteur->setReadOnly(true); spinProfondeur->setReadOnly(true);
        spinBoxPrix->setReadOnly(true); spinBoxCout->setReadOnly(true);
        comboBoxStatut->setEnabled(false);
        break;
    }
}

ArticleDialog::~ArticleDialog() {}

void ArticleDialog::setupUI()
{
    auto *mainLayout = new QVBoxLayout(this);

    // ── Infos de base ─────────────────────────────────────────────────────
    auto *infoBox = new QGroupBox("Informations de l'Article");
    auto *infoForm = new QFormLayout(infoBox);

    lineEditReference = new QLineEdit();
    lineEditNom = new QLineEdit();
    comboBoxCategorie = new QComboBox();
    comboBoxCategorie->addItems({"Sacs","Portefeuilles","Ceintures","Accessoires","Chaussures"});
    comboBoxType = new QComboBox();
    comboBoxType->addItems({"Sac à main","Sac bandoulière","Tote bag","Sac à dos",
                            "Portefeuille long","Portefeuille compact","Porte-cartes",
                            "Ceinture classique","Ceinture fashion","Ceinture luxe",
                            "Porte-monnaie","Pochette","Bandoulière","Porte-clés"});
    lineEditModele3D = new QLineEdit();
    lineEditModele3D->setReadOnly(true);
    lineEditModele3D->setPlaceholderText("Généré automatiquement selon le type");

    connect(comboBoxType, &QComboBox::currentTextChanged, this, &ArticleDialog::onTypeChanged);
    onTypeChanged(comboBoxType->currentText());

    infoForm->addRow("Référence *:", lineEditReference);
    infoForm->addRow("Nom *:", lineEditNom);
    infoForm->addRow("Catégorie:", comboBoxCategorie);
    infoForm->addRow("Type:", comboBoxType);
    infoForm->addRow("Modèle 3D:", lineEditModele3D);
    mainLayout->addWidget(infoBox);

    // ── Couleur RGB ───────────────────────────────────────────────────────
    auto *colorBox = new QGroupBox("Couleur (RGB)");
    auto *colorLay = new QVBoxLayout(colorBox);

    lblColorPreview = new QLabel();
    lblColorPreview->setFixedHeight(25);
    lblColorPreview->setStyleSheet("background:rgb(141,110,99);border-radius:4px;");
    colorLay->addWidget(lblColorPreview);

    auto makeSlider = [&](const QString &label, QSlider *&slider, QLabel *&valLbl, int defVal) {
        auto *row = new QHBoxLayout();
        auto *lbl = new QLabel(label); lbl->setFixedWidth(15);
        slider = new QSlider(Qt::Horizontal); slider->setRange(0,255); slider->setValue(defVal);
        valLbl = new QLabel(QString::number(defVal)); valLbl->setFixedWidth(30);
        connect(slider, &QSlider::valueChanged, [this, valLbl](int v) {
            valLbl->setText(QString::number(v));
            updateColorPreview();
        });
        row->addWidget(lbl); row->addWidget(slider); row->addWidget(valLbl);
        colorLay->addLayout(row);
    };
    makeSlider("R", sliderR, lblRVal, 141);
    makeSlider("G", sliderG, lblGVal, 110);
    makeSlider("B", sliderB, lblBVal, 99);

    auto *btnPicker = new QPushButton("🎨 Choisir Couleur");
    connect(btnPicker, &QPushButton::clicked, [this]() {
        QColor c = QColorDialog::getColor(QColor(sliderR->value(),sliderG->value(),sliderB->value()), this);
        if (c.isValid()) { sliderR->setValue(c.red()); sliderG->setValue(c.green()); sliderB->setValue(c.blue()); }
    });
    colorLay->addWidget(btnPicker);
    mainLayout->addWidget(colorBox);

    // ── Dimensions ────────────────────────────────────────────────────────
    auto *dimBox = new QGroupBox("Dimensions");
    auto *dimForm = new QFormLayout(dimBox);

    auto makeDimSpin = [](double defVal) {
        auto *s = new QDoubleSpinBox(); s->setRange(0.1,999); s->setDecimals(1);
        s->setValue(defVal); s->setSuffix(" cm"); return s;
    };
    spinLargeur    = makeDimSpin(30.0);
    spinHauteur    = makeDimSpin(25.0);
    spinProfondeur = makeDimSpin(10.0);
    dimForm->addRow("Largeur:", spinLargeur);
    dimForm->addRow("Hauteur:", spinHauteur);
    dimForm->addRow("Profondeur:", spinProfondeur);
    mainLayout->addWidget(dimBox);

    // ── Prix & Statut ─────────────────────────────────────────────────────
    auto *prixBox = new QGroupBox("Prix & Statut");
    auto *prixForm = new QFormLayout(prixBox);

    spinBoxPrix = new QDoubleSpinBox(); spinBoxPrix->setRange(0,999999);
    spinBoxPrix->setDecimals(2); spinBoxPrix->setSuffix(" DT");
    spinBoxCout = new QDoubleSpinBox(); spinBoxCout->setRange(0,999999);
    spinBoxCout->setDecimals(2); spinBoxCout->setSuffix(" DT");
    comboBoxStatut = new QComboBox();
    comboBoxStatut->addItems({"disponible","en_production","obsolete"});

    prixForm->addRow("Prix Unitaire:", spinBoxPrix);
    prixForm->addRow("Coût Fabrication:", spinBoxCout);
    prixForm->addRow("Statut:", comboBoxStatut);
    mainLayout->addWidget(prixBox);

    // ── Boutons ───────────────────────────────────────────────────────────
    auto *btnLayout = new QHBoxLayout(); btnLayout->addStretch();
    auto *btnSave = new QPushButton(dialogMode == DeleteMode ? "Confirmer Suppression" : "Enregistrer");
    auto *btnCancel = new QPushButton("Annuler");
    if (dialogMode == ViewMode) { btnSave->setVisible(false); btnCancel->setText("Fermer"); }
    connect(btnSave, &QPushButton::clicked, this, &ArticleDialog::onSave);
    connect(btnCancel, &QPushButton::clicked, this, &ArticleDialog::onCancel);
    btnLayout->addWidget(btnSave); btnLayout->addWidget(btnCancel);
    mainLayout->addLayout(btnLayout);

    setMinimumWidth(550);
}

void ArticleDialog::applyStyles()
{
    setStyleSheet(
        "QDialog{background-color:#FAF5F0;}"
        "QGroupBox{font-weight:bold;color:#291C0E;border:2px solid #BCAAA4;"
        "border-radius:8px;margin-top:10px;padding-top:10px;}"
        "QGroupBox::title{subcontrol-origin:margin;left:10px;padding:0 5px;}"
        "QLabel{color:#291C0E;font-size:11px;}"
        "QLineEdit,QComboBox,QDoubleSpinBox,QSpinBox{background:white;border:2px solid #BCAAA4;"
        "border-radius:6px;padding:6px;color:#291C0E;font-size:11px;}"
        "QSlider::groove:horizontal{background:#D7CCC8;height:6px;border-radius:3px;}"
        "QSlider::handle:horizontal{background:#8D6E63;width:14px;height:14px;border-radius:7px;margin:-4px 0;}"
        "QSlider::sub-page:horizontal{background:#8D6E63;border-radius:3px;}"
        "QPushButton{background:#8D6E63;color:white;border:none;border-radius:8px;"
        "padding:10px 20px;font-size:12px;font-weight:bold;min-width:100px;}"
        "QPushButton:hover{background:#A0826D;}"
        "QPushButton:pressed{background:#6E473B;}"
    );
}

void ArticleDialog::onTypeChanged(const QString &type)
{
    // Générer automatiquement le nom du modèle 3D
    QString modele = type.toLower()
        .replace("à","a").replace("é","e").replace(" ","_")
        .replace("-","_") + ".obj";
    lineEditModele3D->setText(modele);
}

void ArticleDialog::updateColorPreview()
{
    lblColorPreview->setStyleSheet(
        QString("background:rgb(%1,%2,%3);border-radius:4px;")
        .arg(sliderR->value()).arg(sliderG->value()).arg(sliderB->value()));
}

void ArticleDialog::setArticleData(const QString &ref, const QString &nom, const QString &cat,
                                    const QString &type, const QString &modele3d,
                                    int cr, int cg, int cb,
                                    double largeur, double hauteur, double profondeur,
                                    double prix, double cout, const QString &statut)
{
    lineEditReference->setText(ref);
    lineEditNom->setText(nom);
    comboBoxCategorie->setCurrentText(cat);
    comboBoxType->setCurrentText(type);
    if (!modele3d.isEmpty()) lineEditModele3D->setText(modele3d);
    sliderR->setValue(cr); sliderG->setValue(cg); sliderB->setValue(cb);
    spinLargeur->setValue(largeur); spinHauteur->setValue(hauteur); spinProfondeur->setValue(profondeur);
    spinBoxPrix->setValue(prix); spinBoxCout->setValue(cout);
    comboBoxStatut->setCurrentText(statut);
    updateColorPreview();
}

// Getters
QString ArticleDialog::getReference() const { return lineEditReference->text(); }
QString ArticleDialog::getNom() const { return lineEditNom->text(); }
QString ArticleDialog::getCategorie() const { return comboBoxCategorie->currentText(); }
QString ArticleDialog::getType() const { return comboBoxType->currentText(); }
QString ArticleDialog::getModele3D() const { return lineEditModele3D->text(); }
int ArticleDialog::getCouleurR() const { return sliderR->value(); }
int ArticleDialog::getCouleurG() const { return sliderG->value(); }
int ArticleDialog::getCouleurB() const { return sliderB->value(); }
double ArticleDialog::getLargeur() const { return spinLargeur->value(); }
double ArticleDialog::getHauteur() const { return spinHauteur->value(); }
double ArticleDialog::getProfondeur() const { return spinProfondeur->value(); }
double ArticleDialog::getPrixUnitaire() const { return spinBoxPrix->value(); }
double ArticleDialog::getCoutFabrication() const { return spinBoxCout->value(); }
QString ArticleDialog::getStatut() const { return comboBoxStatut->currentText(); }

void ArticleDialog::onSave()
{
    if (lineEditReference->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Validation", "La référence est obligatoire !"); return;
    }
    if (lineEditNom->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Validation", "Le nom est obligatoire !"); return;
    }
    if (dialogMode != DeleteMode && spinBoxPrix->value() <= 0) {
        QMessageBox::warning(this, "Validation", "Le prix unitaire doit être > 0 !"); return;
    }
    if (dialogMode != DeleteMode && spinBoxCout->value() > spinBoxPrix->value()) {
        QMessageBox::warning(this, "Validation", "Le coût ne peut pas dépasser le prix !"); return;
    }
    if (dialogMode == DeleteMode) {
        auto reply = QMessageBox::question(this, "Confirmation",
            "Êtes-vous sûr de vouloir supprimer cet article ?",
            QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) accept();
    } else {
        accept();
    }
}

void ArticleDialog::onCancel() { reject(); }
