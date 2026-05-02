#include "verificationlivraison.h"
#include "arduino.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QTimer>
#include <QLineEdit>

VerificationLivraison::VerificationLivraison(const QString &fournisseurNom, Arduino *arduino, QWidget *parent)
    : QDialog(parent)
    , m_fournisseurNom(fournisseurNom)
    , m_arduino(arduino)
{
    setupUI();
    setModal(true);
    
    // Connecter le signal Arduino pour recevoir le poids
    if (m_arduino) {
        connect(m_arduino, &Arduino::weightStable, this, &VerificationLivraison::onWeightReceived);
    }
    
    // Vérifier la connexion Arduino
    if (!m_arduino || !m_arduino->isConnected()) {
        chkArduino->setChecked(false);
        chkArduino->setText("✗ Arduino non connecté");
        chkArduino->setStyleSheet("QCheckBox { font-size: 12px; color: #F44336; font-weight: bold; margin-top: 10px; }");
    }
}

VerificationLivraison::~VerificationLivraison()
{
}

void VerificationLivraison::setupUI()
{
    setWindowTitle("Vérification Livraison");
    setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // Header sombre avec titre
    QWidget *headerWidget = new QWidget(this);
    headerWidget->setStyleSheet("background-color: #3E2723; padding: 15px;");
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(20, 15, 20, 15);
    
    QLabel *headerTitle = new QLabel(QString("⚖ Vérification Livraison — %1").arg(m_fournisseurNom), headerWidget);
    headerTitle->setStyleSheet("color: white; font-size: 14px; font-weight: bold;");
    headerLayout->addWidget(headerTitle);
    
    QPushButton *btnClose = new QPushButton("✕", headerWidget);
    btnClose->setFixedSize(30, 30);
    btnClose->setStyleSheet(
        "QPushButton { background-color: transparent; color: white; border: none; "
        "font-size: 18px; font-weight: bold; }"
        "QPushButton:hover { background-color: rgba(255,255,255,0.1); border-radius: 15px; }"
    );
    connect(btnClose, &QPushButton::clicked, this, &VerificationLivraison::onAnnuler);
    headerLayout->addWidget(btnClose);
    
    mainLayout->addWidget(headerWidget);
    
    // Corps du dialog
    QWidget *bodyWidget = new QWidget(this);
    bodyWidget->setStyleSheet("background-color: #FAF5F0;");
    QVBoxLayout *bodyLayout = new QVBoxLayout(bodyWidget);
    bodyLayout->setSpacing(15);
    bodyLayout->setContentsMargins(30, 30, 30, 30);
    
    // Fournisseur
    lblTitle = new QLabel(QString("Fournisseur : %1").arg(m_fournisseurNom), bodyWidget);
    lblTitle->setStyleSheet("font-size: 14px; font-weight: bold; color: #3E2723;");
    bodyLayout->addWidget(lblTitle);
    
    // Matière livrée
    QLabel *lblMatiereTitle = new QLabel("Matière livrée :", bodyWidget);
    lblMatiereTitle->setStyleSheet("font-size: 11px; color: #5D4037; margin-top: 5px;");
    bodyLayout->addWidget(lblMatiereTitle);
    
    // Input pour matière (non éditable, juste affichage)
    QLineEdit *inputMatiere = new QLineEdit("Cuir", bodyWidget);
    inputMatiere->setReadOnly(true);
    inputMatiere->setStyleSheet(
        "QLineEdit { background-color: white; border: 1px solid #D0D0D0; border-radius: 4px; "
        "padding: 8px 12px; font-size: 12px; color: #3E2723; }"
    );
    bodyLayout->addWidget(inputMatiere);
    lblMatiere = new QLabel(); // Garder pour compatibilité
    lblMatiere->setVisible(false);
    
    // Quantité commandée
    QLabel *lblQuantiteTitle = new QLabel("Quantité commandée (kg) :", bodyWidget);
    lblQuantiteTitle->setStyleSheet("font-size: 11px; color: #5D4037; margin-top: 8px;");
    bodyLayout->addWidget(lblQuantiteTitle);
    
    // Container pour le spinbox avec les boutons +/-
    QWidget *spinContainer = new QWidget(bodyWidget);
    QHBoxLayout *spinLayout = new QHBoxLayout(spinContainer);
    spinLayout->setContentsMargins(0, 0, 0, 0);
    spinLayout->setSpacing(0);
    
    spinQuantite = new QDoubleSpinBox(spinContainer);
    spinQuantite->setRange(0.01, 10000.0);
    spinQuantite->setDecimals(2);
    spinQuantite->setValue(50.0);
    spinQuantite->setSuffix(" kg");
    spinQuantite->setButtonSymbols(QAbstractSpinBox::UpDownArrows);
    spinQuantite->setAlignment(Qt::AlignLeft);
    spinQuantite->setStyleSheet(
        "QDoubleSpinBox { background-color: white; border: 1px solid #D0D0D0; "
        "border-radius: 4px; padding: 8px 12px; font-size: 12px; color: #3E2723; min-width: 200px; }"
        "QDoubleSpinBox::up-button { background-color: #F5F5F5; border-left: 1px solid #D0D0D0; "
        "width: 25px; subcontrol-origin: border; subcontrol-position: right; }"
        "QDoubleSpinBox::down-button { background-color: #F5F5F5; border-left: 1px solid #D0D0D0; "
        "width: 25px; subcontrol-origin: border; subcontrol-position: right; }"
        "QDoubleSpinBox::up-button:hover, QDoubleSpinBox::down-button:hover { background-color: #E8E8E8; }"
        "QDoubleSpinBox::up-arrow { image: none; width: 0; height: 0; "
        "border-left: 4px solid transparent; border-right: 4px solid transparent; border-bottom: 6px solid #666; }"
        "QDoubleSpinBox::down-arrow { image: none; width: 0; height: 0; "
        "border-left: 4px solid transparent; border-right: 4px solid transparent; border-top: 6px solid #666; }"
    );
    spinLayout->addWidget(spinQuantite);
    spinLayout->addStretch();
    
    bodyLayout->addWidget(spinContainer);
    
    // Checkbox Arduino avec icône verte
    chkArduino = new QCheckBox("✓ Arduino connecté — balance prête", bodyWidget);
    chkArduino->setChecked(true);
    chkArduino->setEnabled(false); // Non modifiable par l'utilisateur
    chkArduino->setStyleSheet(
        "QCheckBox { font-size: 11px; color: #4CAF50; font-weight: bold; margin-top: 8px; spacing: 8px; }"
        "QCheckBox::indicator { width: 16px; height: 16px; background-color: #4CAF50; "
        "border: none; border-radius: 3px; }"
        "QCheckBox::indicator::checked { background-color: #4CAF50; }"
    );
    bodyLayout->addWidget(chkArduino);
    
    // Status label
    lblStatus = new QLabel("", bodyWidget);
    lblStatus->setStyleSheet(
        "font-size: 12px; color: #666; padding: 12px; margin-top: 10px; "
        "background-color: #FFF3E0; border-radius: 6px;"
    );
    lblStatus->setWordWrap(true);
    lblStatus->setVisible(false);
    bodyLayout->addWidget(lblStatus);
    
    bodyLayout->addStretch();
    
    // Boutons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(12);
    buttonLayout->addStretch();
    
    btnDemarrer = new QPushButton("⚖ Démarrer mesure", bodyWidget);
    btnDemarrer->setMinimumWidth(160);
    btnDemarrer->setMinimumHeight(40);
    btnDemarrer->setCursor(Qt::PointingHandCursor);
    btnDemarrer->setStyleSheet(
        "QPushButton { background-color: #8D6E63; color: white; border: none; border-radius: 6px; "
        "padding: 10px 24px; font-size: 12px; font-weight: bold; }"
        "QPushButton:hover { background-color: #A0826D; }"
        "QPushButton:pressed { background-color: #6E473B; }"
        "QPushButton:disabled { background-color: #BCAAA4; }"
    );
    connect(btnDemarrer, &QPushButton::clicked, this, &VerificationLivraison::onDemarrerMesure);
    
    btnAnnuler = new QPushButton("Annuler", bodyWidget);
    btnAnnuler->setMinimumWidth(120);
    btnAnnuler->setMinimumHeight(40);
    btnAnnuler->setCursor(Qt::PointingHandCursor);
    btnAnnuler->setStyleSheet(
        "QPushButton { background-color: #D0D0D0; color: #666; border: none; border-radius: 6px; "
        "padding: 10px 24px; font-size: 12px; font-weight: bold; }"
        "QPushButton:hover { background-color: #BDBDBD; }"
    );
    connect(btnAnnuler, &QPushButton::clicked, this, &VerificationLivraison::onAnnuler);
    
    buttonLayout->addWidget(btnDemarrer);
    buttonLayout->addWidget(btnAnnuler);
    
    bodyLayout->addLayout(buttonLayout);
    
    mainLayout->addWidget(bodyWidget);
    
    // Taille du dialog
    setMinimumSize(520, 480);
    resize(560, 520);
}

void VerificationLivraison::onDemarrerMesure()
{
    if (!chkArduino->isChecked() || !m_arduino || !m_arduino->isConnected()) {
        QMessageBox::warning(this, "Erreur", "Arduino non connecté. Impossible de mesurer le poids.");
        return;
    }
    
    double qteCommandee = spinQuantite->value();
    
    lblStatus->setText("⏳ Mesure en cours... Placez la livraison sur la balance");
    lblStatus->setStyleSheet(
        "font-size: 12px; color: #F57C00; padding: 12px; margin-top: 10px; "
        "background-color: #FFF3E0; border-radius: 6px; font-weight: bold;"
    );
    lblStatus->setVisible(true);
    btnDemarrer->setEnabled(false);
    btnDemarrer->setText("⏳ Mesure en cours...");
    
    // Demander le poids à l'Arduino
    m_arduino->requestWeight();
}

void VerificationLivraison::onAnnuler()
{
    reject();
}

void VerificationLivraison::onWeightReceived(double kg)
{
    double qteCommandee = spinQuantite->value();
    double diff = qAbs(kg - qteCommandee);
    double diffPct = (diff / qteCommandee) * 100.0;
    
    // Stocker les valeurs
    m_measuredWeight = kg;
    m_isValid = (diffPct <= 5.0);
    
    QString msg = QString("✓ Poids mesuré : %1 kg\n").arg(kg, 0, 'f', 2);
    msg += QString("Commandé : %1 kg\n").arg(qteCommandee, 0, 'f', 2);
    
    if (m_isValid) {
        msg += QString("✓ Livraison conforme (écart : %1%)").arg(diffPct, 0, 'f', 1);
        lblStatus->setStyleSheet(
            "font-size: 12px; color: #2E7D32; padding: 12px; margin-top: 10px; "
            "background-color: #E8F5E9; border-radius: 6px; font-weight: bold;"
        );
        btnDemarrer->setText("✓ Valider et Enregistrer");
    } else {
        msg += QString("✗ Livraison non conforme (écart : %1%)").arg(diffPct, 0, 'f', 1);
        lblStatus->setStyleSheet(
            "font-size: 12px; color: #C62828; padding: 12px; margin-top: 10px; "
            "background-color: #FFEBEE; border-radius: 6px; font-weight: bold;"
        );
        btnDemarrer->setText("✗ Refuser la livraison");
    }
    
    lblStatus->setText(msg);
    lblStatus->setVisible(true);
    btnDemarrer->setEnabled(true);
    
    // Déconnecter l'ancien signal et connecter le nouveau pour valider
    disconnect(btnDemarrer, &QPushButton::clicked, this, &VerificationLivraison::onDemarrerMesure);
    connect(btnDemarrer, &QPushButton::clicked, this, &VerificationLivraison::accept);
}

double VerificationLivraison::getQuantiteCommandee() const
{
    return spinQuantite->value();
}

bool VerificationLivraison::isArduinoConnected() const
{
    return chkArduino->isChecked();
}
