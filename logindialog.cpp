#include "logindialog.h"
#include "usersession.h"
#include "connection.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QPixmap>
#include <QFrame>
#include <QTimer>
#include <QSqlQuery>
#include <QSqlError>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    setWindowTitle("CUIREA - Connexion Employé");
    setFixedSize(700, 700);
    setModal(true);
    
    // Style moderne et professionnel
    setStyleSheet(
        "QDialog {"
        "   background-color: #FFFFFF;"
        "}"
        "QLineEdit {"
        "   background-color: #FFFFFF;"
        "   border: 2px solid #D7CCC8;"
        "   border-radius: 8px;"
        "   padding: 14px 16px;"
        "   font-size: 14px;"
        "   color: #3E2723;"
        "   font-family: 'Segoe UI', Arial, sans-serif;"
        "}"
        "QLineEdit:focus {"
        "   border: 2px solid #8D6E63;"
        "   background-color: #FAFAFA;"
        "}"
        "QLineEdit::placeholder {"
        "   color: #BCAAA4;"
        "}"
        "QPushButton#btnLogin {"
        "   background-color: #8D6E63;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 8px;"
        "   padding: 14px 24px;"
        "   font-size: 15px;"
        "   font-weight: 600;"
        "   font-family: 'Segoe UI', Arial, sans-serif;"
        "}"
        "QPushButton#btnLogin:hover {"
        "   background-color: #A0826D;"
        "}"
        "QPushButton#btnLogin:pressed {"
        "   background-color: #6D4C41;"
        "}"
        "QCheckBox {"
        "   color: #5D4037;"
        "   font-size: 13px;"
        "   font-family: 'Segoe UI', Arial, sans-serif;"
        "   spacing: 8px;"
        "}"
        "QCheckBox::indicator {"
        "   width: 18px;"
        "   height: 18px;"
        "   border: 2px solid #BCAAA4;"
        "   border-radius: 4px;"
        "   background-color: white;"
        "}"
        "QCheckBox::indicator:checked {"
        "   background-color: #8D6E63;"
        "   border-color: #8D6E63;"
        "   image: url(:/icons/check.png);"
        "}"
        "QLabel {"
        "   color: #3E2723;"
        "   font-family: 'Segoe UI', Arial, sans-serif;"
        "}"
    );
}

LoginDialog::~LoginDialog()
{
}

void LoginDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // Container principal avec padding
    QWidget *container = new QWidget(this);
    QVBoxLayout *containerLayout = new QVBoxLayout(container);
    containerLayout->setSpacing(12);
    containerLayout->setContentsMargins(80, 25, 80, 25);
    
    // Logo
    lblLogo = new QLabel(container);
    QPixmap logoPixmap(":/logo.png");
    if (!logoPixmap.isNull()) {
        lblLogo->setPixmap(logoPixmap.scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    lblLogo->setAlignment(Qt::AlignCenter);
    lblLogo->setMinimumHeight(120);
    lblLogo->setScaledContents(false);
    containerLayout->addWidget(lblLogo);
    
    containerLayout->addSpacing(8);
    
    // Titre
    lblTitle = new QLabel("🔐 Connexion Employé", container);
    lblTitle->setStyleSheet(
        "font-size: 26px;"
        "font-weight: 600;"
        "color: #8D6E63;"
        "font-family: 'Segoe UI', Arial, sans-serif;"
    );
    lblTitle->setAlignment(Qt::AlignCenter);
    containerLayout->addWidget(lblTitle);
    
    // Sous-titre
    QLabel *lblSubtitle = new QLabel("Utilisez votre matricule pour vous connecter", container);
    lblSubtitle->setStyleSheet(
        "font-size: 14px;"
        "color: #8D6E63;"
        "font-family: 'Segoe UI', Arial, sans-serif;"
    );
    lblSubtitle->setAlignment(Qt::AlignCenter);
    containerLayout->addWidget(lblSubtitle);
    
    containerLayout->addSpacing(12);
    
    // Séparateur élégant
    QFrame *line = new QFrame(container);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Plain);
    line->setStyleSheet("background-color: #E0E0E0;");
    line->setFixedHeight(1);
    containerLayout->addWidget(line);
    
    containerLayout->addSpacing(12);
    
    // === NOM D'UTILISATEUR ===
    QLabel *lblUsername = new QLabel("👤 Matricule:", container);
    lblUsername->setStyleSheet(
        "font-size: 14px;"
        "font-weight: 600;"
        "color: #5D4037;"
        "font-family: 'Segoe UI', Arial, sans-serif;"
    );
    containerLayout->addWidget(lblUsername);
    
    containerLayout->addSpacing(6);
    
    txtUsername = new QLineEdit(container);
    txtUsername->setPlaceholderText("Ex: EMP001, ADMIN001");
    txtUsername->setMinimumHeight(50);
    containerLayout->addWidget(txtUsername);
    
    containerLayout->addSpacing(12);
    
    // === MOT DE PASSE ===
    QLabel *lblPassword = new QLabel(" Mot de passe:", container);
    lblPassword->setStyleSheet(
        "font-size: 14px;"
        "font-weight: 600;"
        "color: #5D4037;"
        "font-family: 'Segoe UI', Arial, sans-serif;"
    );
    containerLayout->addWidget(lblPassword);
    
    containerLayout->addSpacing(6);
    
    txtPassword = new QLineEdit(container);
    txtPassword->setPlaceholderText("Entrez votre mot de passe");
    txtPassword->setEchoMode(QLineEdit::Password);
    txtPassword->setMinimumHeight(50);
    containerLayout->addWidget(txtPassword);
    
    containerLayout->addSpacing(12);
    
    // Message d'erreur
    lblError = new QLabel(container);
    lblError->setStyleSheet(
        "color: #C62828;"
        "background-color: #FFEBEE;"
        "border-left: 4px solid #E53935;"
        "border-radius: 4px;"
        "padding: 12px 16px;"
        "font-size: 13px;"
        "font-weight: 500;"
        "font-family: 'Segoe UI', Arial, sans-serif;"
    );
    lblError->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    lblError->setWordWrap(true);
    lblError->setVisible(false);
    containerLayout->addWidget(lblError);
    
    // Bouton de connexion
    btnLogin = new QPushButton("✓ Se Connecter", container);
    btnLogin->setObjectName("btnLogin");
    btnLogin->setMinimumHeight(52);
    btnLogin->setCursor(Qt::PointingHandCursor);
    containerLayout->addWidget(btnLogin);
    
    containerLayout->addSpacing(8);
    
    // Bouton de connexion par reconnaissance faciale
    btnFaceLogin = new QPushButton("📸 Se connecter par reconnaissance faciale", container);
    btnFaceLogin->setStyleSheet(
        "QPushButton {"
        "   background-color: #4CAF50;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 8px;"
        "   padding: 14px 24px;"
        "   font-size: 15px;"
        "   font-weight: 600;"
        "   font-family: 'Segoe UI', Arial, sans-serif;"
        "}"
        "QPushButton:hover {"
        "   background-color: #45a049;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #388E3C;"
        "}"
    );
    btnFaceLogin->setMinimumHeight(52);
    btnFaceLogin->setCursor(Qt::PointingHandCursor);
    btnFaceLogin->setToolTip("Utilisez votre visage pour vous connecter");
    containerLayout->addWidget(btnFaceLogin);
    
    containerLayout->addStretch();
    
    // Footer
    QLabel *lblFooter = new QLabel("© 2026 CUIREA Management System", container);
    lblFooter->setStyleSheet(
        "font-size: 11px;"
        "color: #BCAAA4;"
        "padding: 8px;"
        "font-family: 'Segoe UI', Arial, sans-serif;"
    );
    lblFooter->setAlignment(Qt::AlignCenter);
    containerLayout->addWidget(lblFooter);
    
    mainLayout->addWidget(container);
    
    // Connexions
    connect(btnLogin, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(btnFaceLogin, &QPushButton::clicked, this, &LoginDialog::onFaceLoginClicked);
    connect(txtPassword, &QLineEdit::returnPressed, this, &LoginDialog::onLoginClicked);
    connect(txtUsername, &QLineEdit::returnPressed, txtPassword, QOverload<>::of(&QLineEdit::setFocus));
}

void LoginDialog::onLoginClicked()
{
    QString username = txtUsername->text().trimmed().toUpper();
    QString password = txtPassword->text();
    
    // Validation
    if (username.isEmpty() || password.isEmpty()) {
        lblError->setText("⚠ Veuillez remplir tous les champs");
        lblError->setVisible(true);
        return;
    }
    
    // Vérification des identifiants via UserSession
    if (validateCredentials(username, password)) {
        m_username = username;
        m_password = password;
        
        // Afficher un message de bienvenue
        QString welcomeMsg = QString("✓ Bienvenue %1 (%2)")
                            .arg(UserSession::instance().getNomComplet())
                            .arg(UserSession::instance().getRoleString());
        
        lblError->setStyleSheet(
            "color: #2E7D32;"
            "background-color: #E8F5E9;"
            "border-left: 4px solid #4CAF50;"
            "border-radius: 4px;"
            "padding: 12px 16px;"
            "font-size: 13px;"
            "font-weight: 500;"
            "font-family: 'Segoe UI', Arial, sans-serif;"
        );
        lblError->setText(welcomeMsg);
        lblError->setVisible(true);
        
        // Fermer après un court délai
        QTimer::singleShot(800, this, &QDialog::accept);
    } else {
        lblError->setStyleSheet(
            "color: #C62828;"
            "background-color: #FFEBEE;"
            "border-left: 4px solid #E53935;"
            "border-radius: 4px;"
            "padding: 12px 16px;"
            "font-size: 13px;"
            "font-weight: 500;"
            "font-family: 'Segoe UI', Arial, sans-serif;"
        );
        lblError->setText("⚠ Matricule ou mot de passe incorrect");
        lblError->setVisible(true);
        txtPassword->clear();
        txtPassword->setFocus();
        
        // Animation de secousse (shake)
        txtPassword->setStyleSheet("border: 2px solid #E53935;");
        QTimer::singleShot(500, this, [this]() {
            txtPassword->setStyleSheet("");
        });
    }
}

bool LoginDialog::validateCredentials(const QString &username, const QString &password)
{
    // Utiliser UserSession pour l'authentification
    return UserSession::instance().login(username, password);
}

QString LoginDialog::getUsername() const
{
    return m_username;
}

QString LoginDialog::getPassword() const
{
    return m_password;
}

bool LoginDialog::isRememberMe() const
{
    return chkRememberMe->isChecked();
}

void LoginDialog::onFaceLoginClicked()
{
    // Vérifier si Python est disponible
    PythonFaceRecognition pythonFace;
    if (!pythonFace.isAvailable()) {
        QMessageBox::critical(this, "Erreur",
            "Python n'est pas installé ou le module face_recognition est manquant.\n\n"
            "Installez Python depuis python.org puis exécutez:\n"
            "pip install face_recognition");
        return;
    }
    
    // Créer le dialogue de reconnaissance
    QDialog *dlg = new QDialog(this);
    dlg->setWindowTitle("Connexion par reconnaissance faciale");
    dlg->setMinimumSize(700, 800);
    dlg->setStyleSheet("QDialog { background-color: #FFFFFF; }");
    
    QVBoxLayout *layout = new QVBoxLayout(dlg);
    layout->setSpacing(12);
    layout->setContentsMargins(20, 20, 20, 20);
    
    // Titre
    QLabel *title = new QLabel("📸 Reconnaissance faciale", dlg);
    title->setStyleSheet(
        "font-size: 20px; font-weight: bold; color: #8D6E63; "
        "padding: 10px; font-family: 'Segoe UI', Arial, sans-serif;");
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);
    
    
    // Widget de capture
    SimpleFaceCapture *capture = new SimpleFaceCapture(dlg);
    layout->addWidget(capture);
    
    // Label de progression
    QLabel *progress = new QLabel("Prêt à capturer", dlg);
    progress->setStyleSheet(
        "padding: 10px; background-color: #E7DDD1; border-radius: 6px; "
        "color: #5D4037; font-size: 14px; font-family: 'Segoe UI', Arial, sans-serif;");
    progress->setAlignment(Qt::AlignCenter);
    layout->addWidget(progress);
    
    // Boutons
    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *captureBtn = new QPushButton("📸 Capturer et identifier", dlg);
    QPushButton *cancelBtn = new QPushButton("Annuler", dlg);
    
    captureBtn->setStyleSheet(
        "QPushButton {"
        "   background-color: #4CAF50; color: white; border: none; "
        "   border-radius: 6px; padding: 12px 24px; font-size: 14px; "
        "   font-weight: 600; font-family: 'Segoe UI', Arial, sans-serif;"
        "}"
        "QPushButton:hover { background-color: #45a049; }"
    );
    
    cancelBtn->setStyleSheet(
        "QPushButton {"
        "   background-color: #E0E0E0; color: #5D4037; border: none; "
        "   border-radius: 6px; padding: 12px 24px; font-size: 14px; "
        "   font-weight: 600; font-family: 'Segoe UI', Arial, sans-serif;"
        "}"
        "QPushButton:hover { background-color: #D0D0D0; }"
    );
    
    btnLayout->addStretch();
    btnLayout->addWidget(captureBtn);
    btnLayout->addWidget(cancelBtn);
    btnLayout->addStretch();
    
    layout->addLayout(btnLayout);
    
    // Créer l'objet de reconnaissance faciale
    PythonFaceRecognition *faceRec = new PythonFaceRecognition(dlg);
    
    // Connexions
    connect(faceRec, &PythonFaceRecognition::recognitionProgress,
            progress, &QLabel::setText);
    
    connect(captureBtn, &QPushButton::clicked, [=]() {
        if (capture->m_imageCapture) {
            capture->m_imageCapture->capture();
        }
    });
    
    connect(capture, &SimpleFaceCapture::frameCaptured, [=](const QImage &img) {
        int employeeId = faceRec->recognizeFace(img);
        
        if (employeeId > 0) {
            // Visage reconnu ! Charger les informations de l'employé
            QSqlQuery query(Connection::instance()->getDatabase());
            query.prepare("SELECT MATRICULE, NOM, PRENOM, MOT_DE_PASSE FROM CUIREA.EMPLOYES WHERE ID_EMPLOYE = :id");
            query.bindValue(":id", employeeId);
            
            if (query.exec() && query.next()) {
                QString matricule = query.value(0).toString();
                QString nom = query.value(1).toString();
                QString prenom = query.value(2).toString();
                QString motDePasse = query.value(3).toString();
                
                // Authentifier avec UserSession
                if (UserSession::instance().login(matricule, motDePasse)) {
                    m_username = matricule;
                    m_password = motDePasse;
                    
                    QMessageBox::information(dlg, "Succès",
                        QString("✅ Bienvenue %1 %2 !\n\nConnexion réussie par reconnaissance faciale.")
                        .arg(prenom).arg(nom));
                    
                    dlg->accept();
                    this->accept();
                } else {
                    QMessageBox::warning(dlg, "Erreur",
                        "Visage reconnu mais authentification échouée.\n"
                        "Veuillez contacter l'administrateur.");
                }
            } else {
                QMessageBox::warning(dlg, "Erreur",
                    "Visage reconnu mais employé non trouvé dans la base de données.");
            }
        }
    });
    
    connect(cancelBtn, &QPushButton::clicked, dlg, &QDialog::reject);
    
    // Démarrer la capture
    capture->startCapture();
    
    dlg->exec();
    
    // Arrêter la capture
    capture->stopCapture();
    
    delete dlg;
}
