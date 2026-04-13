#include "articledialog.h"
#include "tripo3dgenerator.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QGroupBox>
#include <QColorDialog>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QPixmap>
#include <QScrollArea>

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
    auto *outerLayout = new QVBoxLayout(this);
    auto *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea{border:none;}");
    auto *scrollWidget = new QWidget();
    scrollWidget->setStyleSheet("background-color:#FAF5F0;");
    auto *mainLayout = new QVBoxLayout(scrollWidget);

    // ÔöÇÔöÇ Infos de base ÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇ
    auto *infoBox = new QGroupBox("Informations de l'Article");
    auto *infoForm = new QFormLayout(infoBox);

    lineEditReference = new QLineEdit();
    lineEditNom = new QLineEdit();

    // Nom : uniquement lettres, espaces, tirets, apostrophes
    lineEditNom->setValidator(new QRegularExpressionValidator(
        QRegularExpression("[A-Za-z├Ç-├┐\\s\\-']+"), this));
    lineEditNom->setPlaceholderText("Lettres uniquement");

    // Validation temps r├®el : bordure rouge si vide
    auto validateField = [](QLineEdit *field) {
        if (field->text().trimmed().isEmpty())
            field->setStyleSheet("background:white;border:2px solid #E53935;border-radius:6px;padding:6px;color:#291C0E;font-size:11px;");
        else
            field->setStyleSheet("background:white;border:2px solid #BCAAA4;border-radius:6px;padding:6px;color:#291C0E;font-size:11px;");
    };
    connect(lineEditReference, &QLineEdit::textChanged, [=](){ validateField(lineEditReference); });
    connect(lineEditNom, &QLineEdit::textChanged, [=](){ validateField(lineEditNom); });
    comboBoxCategorie = new QComboBox();
    comboBoxCategorie->addItems({"Sacs","Portefeuilles","Ceintures","Accessoires","Chaussures"});
    comboBoxType = new QComboBox();
    comboBoxType->addItems({"Sac ├á main","Sac bandouli├¿re","Tote bag","Sac ├á dos",
                            "Portefeuille long","Portefeuille compact","Porte-cartes",
                            "Ceinture classique","Ceinture fashion","Ceinture luxe",
                            "Porte-monnaie","Pochette","Bandouli├¿re","Porte-cl├®s"});
    lineEditModele3D = new QLineEdit();
    lineEditModele3D->setReadOnly(true);
    lineEditModele3D->setPlaceholderText("G├®n├®r├® automatiquement selon le type");

    connect(comboBoxType, &QComboBox::currentTextChanged, this, &ArticleDialog::onTypeChanged);
    onTypeChanged(comboBoxType->currentText());

    infoForm->addRow("R├®f├®rence *:", lineEditReference);
    infoForm->addRow("Nom *:", lineEditNom);
    infoForm->addRow("Cat├®gorie:", comboBoxCategorie);
    infoForm->addRow("Type:", comboBoxType);
    infoForm->addRow("Mod├¿le 3D:", lineEditModele3D);
    mainLayout->addWidget(infoBox);

    // ÔöÇÔöÇ Couleur RGB ÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇ
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

    auto *btnPicker = new QPushButton("Choisir Couleur");
    btnPicker->setStyleSheet("QPushButton{background:#8D6E63;color:white;border:none;border-radius:8px;padding:8px 16px;font-weight:bold;}"
                             "QPushButton:hover{background:#A0826D;}");
    connect(btnPicker, &QPushButton::clicked, [this]() {
        QColor c = QColorDialog::getColor(QColor(sliderR->value(),sliderG->value(),sliderB->value()), this);
        if (c.isValid()) { sliderR->setValue(c.red()); sliderG->setValue(c.green()); sliderB->setValue(c.blue()); }
    });
    colorLay->addWidget(btnPicker);
    mainLayout->addWidget(colorBox);

    // ÔöÇÔöÇ Dimensions ÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇ
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

    // ÔöÇÔöÇ Prix & Statut ÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇ
    auto *prixBox = new QGroupBox("Prix & Statut");
    auto *prixForm = new QFormLayout(prixBox);

    spinBoxPrix = new QDoubleSpinBox(); spinBoxPrix->setRange(0,999999);
    spinBoxPrix->setDecimals(2); spinBoxPrix->setSuffix(" DT");
    spinBoxCout = new QDoubleSpinBox(); spinBoxCout->setRange(0,999999);
    spinBoxCout->setDecimals(2); spinBoxCout->setSuffix(" DT");
    comboBoxStatut = new QComboBox();
    comboBoxStatut->addItems({"disponible","en_production","obsolete"});

    prixForm->addRow("Prix Unitaire:", spinBoxPrix);
    prixForm->addRow("Co├╗t Fabrication:", spinBoxCout);
    prixForm->addRow("Statut:", comboBoxStatut);
    mainLayout->addWidget(prixBox);

    // ÔöÇÔöÇ Generation 3D (Tripo3D) ÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇ
    if (dialogMode == AddMode || dialogMode == EditMode) {
        auto *tripoBox = new QGroupBox("Generation 3D IA");
        auto *tripoLay = new QVBoxLayout(tripoBox);

        QHBoxLayout *tripoRow = new QHBoxLayout();
        btnGenerate3D = new QPushButton("Generer image 3D");
        btnGenerate3D->setStyleSheet("QPushButton{background:#4A148C;color:white;}QPushButton:hover{background:#6A1B9A;}");
        progress3D = new QProgressBar(); progress3D->setRange(0, 100); progress3D->setValue(0);
        progress3D->setFixedHeight(18);
        progress3D->setStyleSheet("QProgressBar{border:1px solid #BCAAA4;border-radius:4px;background:#FAF5F0;}"
                                  "QProgressBar::chunk{background:#8D6E63;border-radius:3px;}");
        tripoRow->addWidget(btnGenerate3D);
        tripoRow->addWidget(progress3D);
        tripoLay->addLayout(tripoRow);

        lblStatus3D = new QLabel("");
        lblStatus3D->setStyleSheet("color:#888;font-size:10px;");

        lblPreview3D = new QLabel();
        lblPreview3D->setFixedSize(100, 100);
        lblPreview3D->setAlignment(Qt::AlignCenter);
        lblPreview3D->setStyleSheet("border:1px solid #BCAAA4;border-radius:6px;background:#FFF8F0;font-size:9px;color:#888;");
        lblPreview3D->setText("Apercu");

        QHBoxLayout *previewRow = new QHBoxLayout();
        previewRow->addWidget(lblPreview3D);
        previewRow->addWidget(lblStatus3D, 1);
        tripoLay->addLayout(previewRow);

        connect(btnGenerate3D, &QPushButton::clicked, this, &ArticleDialog::onGenerate3D);

        m_tripoGen = new Tripo3DGenerator(this);
        connect(m_tripoGen, &Tripo3DGenerator::progress, this, [this](int pct, const QString &msg) {
            progress3D->setValue(pct);
            lblStatus3D->setText(msg);
        });
        connect(m_tripoGen, &Tripo3DGenerator::finished, this, [this](const QString &imgPath, const QString &) {
            m_photoPath = imgPath;
            if (!imgPath.isEmpty()) {
                QPixmap px(imgPath);
                if (!px.isNull())
                    lblPreview3D->setPixmap(px.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            }
            lblStatus3D->setText("Generation terminee !");
            btnGenerate3D->setEnabled(true);
        });
        connect(m_tripoGen, &Tripo3DGenerator::error, this, [this](const QString &msg) {
            lblStatus3D->setText("Erreur: " + msg);
            btnGenerate3D->setEnabled(true);
        });

        mainLayout->addWidget(tripoBox);
    } else {
        btnGenerate3D = nullptr;
        progress3D = nullptr;
        lblPreview3D = nullptr;
        lblStatus3D = nullptr;
        m_tripoGen = nullptr;
    }

    // ÔöÇÔöÇ Boutons ÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇ
    auto *btnLayout = new QHBoxLayout(); btnLayout->addStretch();
    auto *btnSave = new QPushButton(dialogMode == DeleteMode ? "Confirmer Suppression" : "Enregistrer");
    auto *btnCancel = new QPushButton("Annuler");
    if (dialogMode == ViewMode) { btnSave->setVisible(false); btnCancel->setText("Fermer"); }
    connect(btnSave, &QPushButton::clicked, this, &ArticleDialog::onSave);
    connect(btnCancel, &QPushButton::clicked, this, &ArticleDialog::onCancel);
    btnLayout->addWidget(btnSave); btnLayout->addWidget(btnCancel);
    mainLayout->addLayout(btnLayout);

    scroll->setWidget(scrollWidget);
    outerLayout->addWidget(scroll);

    // Boutons toujours visibles en bas (hors scroll)
    auto *bottomBtnLayout = new QHBoxLayout();
    bottomBtnLayout->addStretch();
    auto *btnSave2 = new QPushButton(dialogMode == DeleteMode ? "Confirmer Suppression" : "Enregistrer");
    auto *btnCancel2 = new QPushButton("Annuler");
    btnSave2->setStyleSheet("QPushButton{background:#8D6E63;color:white;border:none;border-radius:8px;padding:10px 20px;font-size:12px;font-weight:bold;min-width:100px;}QPushButton:hover{background:#A0826D;}");
    btnCancel2->setStyleSheet("QPushButton{background:#BCAAA4;color:white;border:none;border-radius:8px;padding:10px 20px;font-size:12px;font-weight:bold;min-width:100px;}QPushButton:hover{background:#A0826D;}");
    if (dialogMode == ViewMode) { btnSave2->setVisible(false); btnCancel2->setText("Fermer"); }
    connect(btnSave2, &QPushButton::clicked, this, &ArticleDialog::onSave);
    connect(btnCancel2, &QPushButton::clicked, this, &ArticleDialog::onCancel);
    bottomBtnLayout->addWidget(btnSave2);
    bottomBtnLayout->addWidget(btnCancel2);
    outerLayout->addLayout(bottomBtnLayout);

    setMinimumWidth(550);
    setMinimumHeight(500);
}

void ArticleDialog::applyStyles()
{
    setStyleSheet(
        "QDialog{background-color:#FAF5F0;}"
        "QGroupBox{font-weight:bold;color:#291C0E;border:2px solid #BCAAA4;"
        "border-radius:8px;margin-top:10px;padding-top:10px;}"
        "QGroupBox::title{subcontrol-origin:margin;left:10px;padding:0 5px;}"
        "QLabel{color:#291C0E;font-size:11px;}"
        "QLineEdit,QDoubleSpinBox,QSpinBox{background:white;border:2px solid #BCAAA4;"
        "border-radius:6px;padding:6px;color:#291C0E;font-size:11px;}"
        "QComboBox{background:white;border:2px solid #BCAAA4;"
        "border-radius:6px;padding:6px;color:#291C0E;font-size:11px;}"
        "QComboBox::drop-down{border:none;}"
        "QComboBox QAbstractItemView{background:white;color:#291C0E;selection-background-color:#8D6E63;selection-color:white;}"
        "QSlider::groove:horizontal{background:#D7CCC8;height:6px;border-radius:3px;}"
        "QSlider::handle:horizontal{background:#8D6E63;width:14px;height:14px;border-radius:7px;margin:-4px 0;}"
        "QSlider::sub-page:horizontal{background:#8D6E63;border-radius:3px;}"
        "QPushButton{background:#8D6E63;color:white;border:none;border-radius:8px;"
        "padding:10px 20px;font-size:12px;font-weight:bold;min-width:100px;}"
        "QPushButton:hover{background:#A0826D;}"
        "QPushButton:pressed{background:#6E473B;}"
        "QScrollArea{border:none;background:#FAF5F0;}"
        "QProgressBar{border:1px solid #BCAAA4;border-radius:4px;background:#FAF5F0;}"
        "QProgressBar::chunk{background:#8D6E63;border-radius:3px;}"
    );
}

void ArticleDialog::onTypeChanged(const QString &type)
{
    // G├®n├®rer automatiquement le nom du mod├¿le 3D
    QString modele = type.toLower()
        .replace("├á","a").replace("├®","e").replace(" ","_")
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
    bool valid = true;
    QString errStyle = "background:white;border:2px solid #E53935;border-radius:6px;padding:6px;color:#291C0E;font-size:11px;";
    QString okStyle  = "background:white;border:2px solid #BCAAA4;border-radius:6px;padding:6px;color:#291C0E;font-size:11px;";

    // R├®f├®rence obligatoire
    if (lineEditReference->text().trimmed().isEmpty()) {
        lineEditReference->setStyleSheet(errStyle);
        valid = false;
    } else {
        lineEditReference->setStyleSheet(okStyle);
    }

    // Nom obligatoire + lettres uniquement
    QString nom = lineEditNom->text().trimmed();
    QRegularExpression rxLettres("^[A-Za-z├Ç-├┐\\s\\-']+$");
    if (nom.isEmpty() || !rxLettres.match(nom).hasMatch()) {
        lineEditNom->setStyleSheet(errStyle);
        valid = false;
    } else {
        lineEditNom->setStyleSheet(okStyle);
    }

    if (!valid) {
        QMessageBox::warning(this, "Validation",
            "Veuillez remplir tous les champs obligatoires.\n"
            "Le nom doit contenir uniquement des lettres.");
        return;
    }

    if (dialogMode != DeleteMode && spinBoxPrix->value() <= 0) {
        QMessageBox::warning(this, "Validation", "Le prix unitaire doit ├¬tre > 0 !"); return;
    }
    if (dialogMode != DeleteMode && spinBoxCout->value() > spinBoxPrix->value()) {
        QMessageBox::warning(this, "Validation", "Le co├╗t ne peut pas d├®passer le prix !"); return;
    }
    if (dialogMode == DeleteMode) {
        auto reply = QMessageBox::question(this, "Confirmation",
            "├ètes-vous s├╗r de vouloir supprimer cet article ?",
            QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) accept();
    } else {
        accept();
    }
}

void ArticleDialog::onCancel() { reject(); }

void ArticleDialog::onGenerate3D()
{
    QString type = comboBoxType->currentText();
    QString cat = comboBoxCategorie->currentText();
    int r = sliderR->value(), g = sliderG->value(), b = sliderB->value();
    QString couleur;
    if (r > 150 && g < 80 && b < 80) couleur = "red";
    else if (r < 60 && g < 60 && b < 60) couleur = "black";
    else if (r > 180 && g > 150 && b > 100) couleur = "beige";
    else if (r > 120 && g > 80 && b < 70) couleur = "brown";
    else couleur = "leather colored";

    QString prompt = QString("A realistic %1, %2 %3 leather, luxury maroquinerie, studio lighting, product photography")
        .arg(type, couleur, cat.toLower());

    qDebug() << "[Tripo3D] Prompt:" << prompt;
    btnGenerate3D->setEnabled(false);
    lblStatus3D->setText("Envoi a Tripo3D...");
    progress3D->setValue(0);
    m_tripoGen->generate(prompt);
}

QString ArticleDialog::getPhotoPath() const { return m_photoPath; }
