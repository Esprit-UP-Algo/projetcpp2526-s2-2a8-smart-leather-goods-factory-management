#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "notification.h"
#include "envloader.h"
#include "bilandialog.h"
#include "statscharts.h"
#include "usersession.h"
#include "logindialog.h"
#include <QStatusBar>
#include <QRegularExpression>
#include <QMouseEvent>
#include <QEvent>
#include "employeedialog.h"
#include "clientmanagerdialog.h"
#include "matieredialog.h"
#include "fournisseurdialog.h"
#include "verificationlivraison.h"
#include "smsfournisseurdialog.h"
#include "qrfournisseurdialog.h"
#include "productionview.h"
#include "articledialog.h"
#include "employe.h"
#include "production.h"
#include "connection.h"
#include "email.h"
#include "fichepaie.h"
#include "statsrh.h"
#include <QTableWidgetItem>
#include <QDebug>
#include <QMessageBox>
#include <QInputDialog>
#include <QDate>
#include <QDateTime>
#include <QSqlQuery>
#include <QSqlError>
#include <QMenu>
#include <QDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDateEdit>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QFile>
#include <QIODevice>
#include <QBuffer>
#include <QByteArray>
#include <QPrinter>
#include <QPageLayout>
#include <QPainter>
#include <QPixmap>
#include <QIcon>
#include <QFileDialog>
#include <QTextDocument>
#include <QTextEdit>
#include <QTextBrowser>
#include <QFrame>
#include <QFile>
#include <QTextStream>
#include <QStringConverter>
#include <QScrollArea>
#include <QTimer>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <algorithm>
#include <QChart>
#include <QChartView>
#include <QPieSeries>
#include <QPieSlice>
#include <QBarSet>
#include <QBarSeries>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QtCharts>
#include <QRandomGenerator>
#include <QDesktopServices>
#include <QUrl>
#include <QResizeEvent>
#include <cmath>
// -- Shared style constants ----------------------------------------------------
static const QString DIALOG_STYLE =
    "QDialog { background-color: #FAF5F0; }"
    "QLabel { color: #291C0E; font-size: 12px; font-weight: bold; }"
    "QLineEdit, QComboBox, QDateEdit { background-color: white; border: 2px solid #BCAAA4; "
    "border-radius: 6px; padding: 8px; font-size: 12px; color: #291C0E; }"
    "QPushButton { background-color: #8D6E63; color: white; border: none; border-radius: 6px; "
    "padding: 8px 20px; font-size: 12px; font-weight: bold; min-width: 80px; }"
    "QPushButton:hover { background-color: #A0826D; }";

static const QString MSGBOX_STYLE =
    "QMessageBox { background-color: #FAF5F0; }"
    "QMessageBox QLabel { color: #291C0E; font-family: 'Courier New', monospace; font-size: 11px; }"
    "QPushButton { background-color: #8D6E63; color: white; border: none; border-radius: 6px; "
    "padding: 8px 20px; font-size: 11px; font-weight: bold; min-width: 80px; }"
    "QPushButton:hover { background-color: #A0826D; }";

static const QString NAV_ACTIVE_STYLE =
    "QPushButton { background-color: #6E473B; color: #FFFFFF; border-left: 3px solid #FFFFFF; }";

// -- Helper: show a styled info message box ------------------------------------
static void showInfo(QWidget *parent, const QString &title, const QString &text)
{
    QMessageBox mb(parent);
    mb.setWindowTitle(title);
    mb.setText(text);
    mb.setIcon(QMessageBox::Information);
    mb.setStyleSheet(MSGBOX_STYLE);
    mb.exec();
}

// -- Helper: get text from a table cell safely --------------------------------
static QString cellText(QTableWidget *t, int row, int col)
{
    auto *item = t->item(row, col);
    return item ? item->text() : QString();
}

// -- Helper: filter table rows by search text ---------------------------------
static void filterTable(QTableWidget *table, const QString &text)
{
    QString lower = text.trimmed().toLower();
    for (int row = 0; row < table->rowCount(); ++row) {
        bool match = lower.isEmpty();
        if (!match) {
            for (int col = 0; col < table->columnCount() && !match; ++col) {
                auto *item = table->item(row, col);
                if (item && item->text().toLower().contains(lower))
                    match = true;
            }
        }
        table->setRowHidden(row, !match);
    }
}

// -----------------------------------------------------------------------------
// FloatingAIButton  cercle animé style Meta AI
// -----------------------------------------------------------------------------
FloatingAIButton::FloatingAIButton(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(64, 64);
    // Pas de WA_TranslucentBackground  on peint le fond nous-mêmes
    setCursor(Qt::PointingHandCursor);
    setToolTip("Assistant IA");

    m_animTimer = new QTimer(this);
    connect(m_animTimer, &QTimer::timeout, this, [this]() {
        m_angle     = std::fmod(m_angle     + 2.0f, 360.0f); // anneau : rapide
        m_logoAngle = std::fmod(m_logoAngle + 0.4f, 360.0f); // logo   : lent
        update();
    });
    m_animTimer->start(16);
}

void FloatingAIButton::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);

    // Fond : couleur du parent
    p.fillRect(rect(), parentWidget() ? parentWidget()->palette().window() : Qt::transparent);

    // Ombre portée
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(0, 0, 0, 50));
    p.drawEllipse(5, 7, 58, 58);

    // Anneau tournant aux couleurs CUIREA
    QConicalGradient g(32, 32, m_angle);
    g.setColorAt(0.0,  QColor("#C4795A"));
    g.setColorAt(0.35, QColor("#E8A87C"));
    g.setColorAt(0.65, QColor("#7A3E1F"));
    g.setColorAt(1.0,  QColor("#C4795A"));
    p.setPen(QPen(QBrush(g), 4));
    p.setBrush(Qt::NoBrush);
    p.drawEllipse(3, 3, 58, 58);

    // Cercle intérieur beige CUIREA
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#FAF5F0"));
    p.drawEllipse(7, 7, 50, 50);

    // Logo CUIREA qui tourne lentement au centre
    p.save();
    p.translate(32, 32);
    p.rotate(m_logoAngle);

    // Clip circulaire pour que le logo reste dans le cercle
    QPainterPath clip;
    clip.addEllipse(-22, -22, 44, 44);
    p.setClipPath(clip);

    static QPixmap logo;
    if (logo.isNull())
        logo = QPixmap(":/logo.png");

    if (!logo.isNull()) {
        QPixmap scaled = logo.scaled(44, 44, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        p.drawPixmap(-scaled.width() / 2, -scaled.height() / 2, scaled);
    }

    p.restore();
}

void FloatingAIButton::mousePressEvent(QMouseEvent *)
{
    emit clicked();
}

void FloatingAIButton::enterEvent(QEnterEvent *)
{
    m_hovered = true;
    update();
}

void FloatingAIButton::leaveEvent(QEvent *)
{
    m_hovered = false;
    update();
}

// -----------------------------------------------------------------------------

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , isEditMode(false)
    , editingRow(-1)
{
    ui->setupUi(this);
    setWindowTitle("CUIREA - Management System");
    
    // Initialiser la détection de défauts (Mode API Cloud - Hugging Face Spaces)
    networkManager = new QNetworkAccessManager(this);
    apiUrl = "https://ahmedomar10-detection-cuir.hf.space";
    detectionResultLabel = nullptr;
    detectionProgress = nullptr;
    
    // Initialiser les classes extraites pour matières premiéres
    matiereDetection = new MatiereDetection(this, ui->matiereTable, networkManager, apiUrl);
    voiceMatieres = new VoiceMatieres(this, ui->matiereTable);
    //=========================MAPFOURNISSEUR
    mapService = new Map(this);

        connect(mapService, &Map::coordinatesReady, this, [=](double lat, double lon){
            openMap(lat, lon);
        });
    // -- Employee table ------------------------------------------------------
    ui->employeeTable->verticalHeader()->setVisible(false);
    // Colonnes: 0=Matricule, 1=Nom, 2=Prénom, 3=CIN, 4=DateNaissance, 5=Sexe,
    //           6=Adresse, 7=Téléphone, 8=Email, 9=Poste, 10=Département,
    //           11=DateEmbauche, 12=Photo, 13=ID
    ui->employeeTable->setColumnCount(14);
    ui->employeeTable->setHorizontalHeaderLabels({"Matricule", "Nom", "Prénom", "CIN", 
                                                   "Date Naissance", "Sexe", "Adresse", 
                                                   "Téléphone", "Email", "Poste", 
                                                   "Département", "Date Embauche", "Photo", "ID"});
    // Cacher: Adresse, DateEmbauche, Photo, ID
    ui->employeeTable->setColumnHidden(6, true);   // Adresse
    ui->employeeTable->setColumnHidden(11, true);  // Date Embauche
    ui->employeeTable->setColumnHidden(12, true);  // Photo
    ui->employeeTable->setColumnHidden(13, true);  // ID
    
    int empWidths[] = {110,100,100,100,120,60,0,110,180,120,130,0,0,0};
    for (int i = 0; i < 14; ++i) ui->employeeTable->setColumnWidth(i, empWidths[i]);
    populateEmployeeTable();
    connect(ui->employeeTable, &QTableWidget::currentCellChanged,
            this, &MainWindow::onEmployeeSelected);
    
    // Permettre de désélectionner en cliquant sur une zone vide du tableau
    ui->employeeTable->viewport()->installEventFilter(this);
    
    // Masquer le panneau de profil au démarrage
    ui->employeeProfilePanel->setVisible(false);
    
    // Recherche en temps réel avec critéres
    connect(ui->searchBox, &QLineEdit::textChanged, this, [this](const QString &text) {
        QString searchTerm = text.trimmed();
        
        if (searchTerm.isEmpty()) {
            // Afficher tous les employés
            populateEmployeeTable();
            return;
        }
        
        // Recherche intelligente
        Employe e;
        QSqlQueryModel* model = nullptr;
        
        // Détecter le type de recherche
        if (searchTerm.startsWith("id:")) {
            // Recherche par ID
            QString id = searchTerm.mid(3).trimmed();
            model = e.rechercherParCritere("id", id);
            ui->searchBox->setPlaceholderText("? Recherche par ID...");
        } else if (searchTerm.startsWith("nom:")) {
            // Recherche par nom
            QString nom = searchTerm.mid(4).trimmed();
            model = e.rechercherParCritere("nom", nom);
            ui->searchBox->setPlaceholderText("? Recherche par nom...");
        } else if (searchTerm.startsWith("prenom:")) {
            // Recherche par prénom
            QString prenom = searchTerm.mid(7).trimmed();
            model = e.rechercherParCritere("prenom", prenom);
            ui->searchBox->setPlaceholderText("? Recherche par prénom...");
        } else if (searchTerm.startsWith("dept:")) {
            // Recherche par département
            QString dept = searchTerm.mid(5).trimmed();
            model = e.rechercherParCritere("departement", dept);
            ui->searchBox->setPlaceholderText("? Recherche par département...");
        } else if (searchTerm.startsWith("poste:")) {
            // Recherche par poste
            QString poste = searchTerm.mid(6).trimmed();
            model = e.rechercherParCritere("poste", poste);
            ui->searchBox->setPlaceholderText("? Recherche par poste...");
        } else {
            // Recherche globale
            model = e.rechercher(searchTerm);
            ui->searchBox->setPlaceholderText("? Rechercher (nom:, prenom:, id:, dept:, poste:)...");
        }
        
        if (!model) {
            ui->employeeTable->setRowCount(0);
            return;
        }
        
        // Afficher les résultats - IMPORTANT: afficher toutes les 15 colonnes
        int n = model->rowCount();
        ui->employeeTable->setRowCount(n);
        
        // Le modèle retourne maintenant 12 colonnes dans cet ordre:
        // 0=ID, 1=Matricule, 2=Nom, 3=Prénom, 4=CIN, 5=DateNaissance, 6=Sexe,
        // 7=Département, 8=Poste, 9=Téléphone, 10=Email, 11=Photo
        
        // Le tableau a 14 colonnes dans cet ordre:
        // 0=Matricule, 1=Nom, 2=Prénom, 3=CIN, 4=DateNaissance, 5=Sexe,
        // 6=Adresse, 7=Téléphone, 8=Email, 9=Poste,
        // 10=Département, 11=DateEmbauche, 12=Photo, 13=ID
        
        for (int i = 0; i < n; ++i) {
            // Mapper les colonnes du modèle vers le tableau
            ui->employeeTable->setItem(i, 0, new QTableWidgetItem(model->data(model->index(i, 1)).toString())); // Matricule
            ui->employeeTable->setItem(i, 1, new QTableWidgetItem(model->data(model->index(i, 2)).toString())); // Nom
            ui->employeeTable->setItem(i, 2, new QTableWidgetItem(model->data(model->index(i, 3)).toString())); // Prénom
            ui->employeeTable->setItem(i, 3, new QTableWidgetItem(model->data(model->index(i, 4)).toString())); // CIN
            ui->employeeTable->setItem(i, 4, new QTableWidgetItem(model->data(model->index(i, 5)).toString())); // Date Naissance
            ui->employeeTable->setItem(i, 5, new QTableWidgetItem(model->data(model->index(i, 6)).toString())); // Sexe
            ui->employeeTable->setItem(i, 6, new QTableWidgetItem("")); // Adresse (cachée)
            ui->employeeTable->setItem(i, 7, new QTableWidgetItem(model->data(model->index(i, 9)).toString())); // Téléphone
            ui->employeeTable->setItem(i, 8, new QTableWidgetItem(model->data(model->index(i, 10)).toString())); // Email
            ui->employeeTable->setItem(i, 9, new QTableWidgetItem(model->data(model->index(i, 8)).toString())); // Poste
            ui->employeeTable->setItem(i, 10, new QTableWidgetItem(model->data(model->index(i, 7)).toString())); // Département
            ui->employeeTable->setItem(i, 11, new QTableWidgetItem("")); // Date Embauche (cachée)
            ui->employeeTable->setItem(i, 12, new QTableWidgetItem(model->data(model->index(i, 11)).toString())); // Photo
            ui->employeeTable->setItem(i, 13, new QTableWidgetItem(model->data(model->index(i, 0)).toString())); // ID
        }
        
        delete model;
    });
    
    // === BOUTON POINTAGE - Création dynamique ===
    QPushButton *btnPointage = new QPushButton("📅 Pointage", this);
    btnPointage->setObjectName("btnPointage");
    btnPointage->setMinimumSize(120, 35);
    btnPointage->setStyleSheet(
        "QPushButton {"
        "  background-color: #8D6E63;"
        "  color: white;"
        "  border: none;"
        "  border-radius: 10px;"
        "  padding: 10px 20px;"
        "  font-family: Arial, sans-serif;"
        "  font-size: 12px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "  background-color: #A0826D;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #6E473B;"
        "}"
    );
    
    // Trouver le layout des boutons employés et ajouter le bouton
    QWidget *employeePage = ui->stackedWidget->widget(0); // Page Employés (index 0)
    if (employeePage) {
        // Chercher tous les layouts horizontaux dans la page
        QList<QHBoxLayout*> layouts = employeePage->findChildren<QHBoxLayout*>();
        for (QHBoxLayout *layout : layouts) {
            // Vérifier si ce layout contient les boutons d'action (btnAdd, btnEdit, etc.)
            for (int i = 0; i < layout->count(); ++i) {
                QWidget *widget = layout->itemAt(i)->widget();
                if (widget && (widget->objectName() == "btnAdd" || 
                              widget->objectName() == "btnStatistics" ||
                              widget->objectName() == "btnExport")) {
                    // On a trouvé le bon layout, ajouter le bouton Pointage
                    layout->addWidget(btnPointage);
                    qDebug() << "Bouton Pointage ajoute";
                    goto button_added;
                }
            }
        }
    }
    button_added:
    
    // Connecter le signal
    connect(btnPointage, &QPushButton::clicked, 
            this, &MainWindow::on_btnPointage_clicked);



    // -- Raw materials -------------------------------------------------------
    ui->matiereTable->verticalHeader()->setVisible(false);
    setupMatiereTable();
    connect(ui->btnAddMatiere,  &QPushButton::clicked, this, &MainWindow::onAddMatiere);
    connect(ui->btnEditMatiere, &QPushButton::clicked, this, &MainWindow::onEditMatiere);
    connect(ui->btnDeleteMatiere, &QPushButton::clicked, this, &MainWindow::onDeleteMatiere);
    connect(ui->btnRecherche,   &QPushButton::clicked, this, &MainWindow::onRechercheTriMatiere);
    connect(ui->searchBoxMatiere, &QLineEdit::textChanged, this, [this](const QString &text) {
        QString lower = text.trimmed().toLower();
        for (int r = 0; r < ui->matiereTable->rowCount(); ++r) {
            bool match = lower.isEmpty();
            if (!match) {
                for (int c = 0; c < 3; ++c) { // Module, Ref, Type
                    if (cellText(ui->matiereTable, r, c).toLower().contains(lower)) {
                        match = true;
                        break;
                    }
                }
            }
            ui->matiereTable->setRowHidden(r, !match);
        }
    });
    connect(ui->btnStatistiques,&QPushButton::clicked, this, &MainWindow::onStatistiquesMatiere);
    connect(ui->btnTri,         &QPushButton::clicked, this, &MainWindow::onTriMatiere);
    connect(ui->btnDetection,   &QPushButton::clicked, this, &MainWindow::onDetectionDefauts);
    // Voice recognition init
    voiceListening = false;
    voiceFeedbackLabel = nullptr;
#ifdef Q_OS_WIN
    spRecognizer  = nullptr;
    spRecoContext = nullptr;
    spGrammar     = nullptr;
    CoInitialize(nullptr);
#endif
    connect(ui->btnVoice, &QPushButton::clicked, [this]() { voiceMatieres->openVoiceDialog(); });
    connect(ui->btnExportMatiere,&QPushButton::clicked, this, &MainWindow::onExportMatiere);

    // -- Client extra buttons ------------------------------------------------


    // -- Fournisseurs --------------------------------------------------------
    ui->fournisseurTable->verticalHeader()->setVisible(false);
    setupFournisseurTable();

    // -- Production ----------------------------------------------------------
    ui->productionTable->verticalHeader()->setVisible(false);
    setupProductionTable();
    loadProductionData();
    connect(ui->btnCreerProduction,       &QPushButton::clicked, this, &MainWindow::onCreerProduction);
    connect(ui->btnModifierProduction,    &QPushButton::clicked, this, &MainWindow::onModifierProduction);
    connect(ui->btnSupprimerProduction,   &QPushButton::clicked, this, &MainWindow::onSupprimerProduction);
    connect(ui->btnFactureProduction,     &QPushButton::clicked, this, &MainWindow::onFactureProduction);
    connect(ui->btnExcelProduction,       &QPushButton::clicked, this, &MainWindow::onExcelProduction);
    connect(ui->btnTrierProduction,       &QPushButton::clicked, this, &MainWindow::onTrierProduction);
    connect(ui->btnStatistiquesProduction,&QPushButton::clicked, this, &MainWindow::onStatistiquesProduction);
    connect(ui->btnExpedier,              &QPushButton::clicked, this, &MainWindow::expedierActionArduino);
    ui->btnExpedier->setStyleSheet(
        "QPushButton { background-color:#8D6E63; color:white; border:none; border-radius:10px;"
        " padding:10px 20px; font-family:Arial,sans-serif; font-size:12px; font-weight:bold; }"
        "QPushButton:hover { background-color:#A0826D; }"
        "QPushButton:pressed { background-color:#6E473B; padding:11px 20px 9px 20px; }");
    connect(ui->searchBoxProduction, &QLineEdit::textChanged, this, &MainWindow::onRechercherProduction);

    // -- Timer retard notifications -------------------------------------------
    m_retardTimer = new QTimer(this);
    connect(m_retardTimer, &QTimer::timeout, this, &MainWindow::checkRetards);
    m_retardTimer->start(20000);
    QTimer::singleShot(3000, this, &MainWindow::checkRetards);

    // -- Notification AI (Groq) + Bell + Watcher ------------------------------
    const QString groqKey = EnvLoader::get("GROQ_API_KEY");
    if (!groqKey.isEmpty()) {
        m_ai = new NotificationAI(groqKey, "llama-3.1-8b-instant", this);
        NotificationAI::setGlobalInstance(m_ai);
    }

    // Cloche positionnee en overlay coin superieur droit
    m_bell = new NotificationBell(this);
    m_bell->raise();
    
    // Indicateur de température à côté de la cloche
    m_tempIndicator = new QLabel(this);
    m_tempIndicator->setFixedSize(80, 32);
    m_tempIndicator->setAlignment(Qt::AlignCenter);
    m_tempIndicator->setText("🌡 --°C");
    m_tempIndicator->setStyleSheet(
        "QLabel { background-color: #5D4037; color: white; border-radius: 16px; "
        "font-size: 11px; font-weight: bold; padding: 4px 8px; }"
    );
    m_tempIndicator->setToolTip("Température matière première");
    m_tempIndicator->setVisible(false); // masqué sur Production au démarrage
    m_tempIndicator->raise();

    m_watcher = new NotificationWatcher(QSqlDatabase::database(), this);
    if (m_ai) m_watcher->setAI(m_ai);
    m_watcher->start(120000);

    // -- Notifications systeme natives (tray icon OS) ---------------------------
    SystemNotification::instance().initialize(this);

    // -- Articles ------------------------------------------------------------
    setupArticleTable();
    loadArticlesFromDB();
    updateArticleStatistics();

    // -- Initial page --------------------------------------------------------
    ui->stackedWidget->setCurrentIndex(0);
    ui->btnEmployees->setStyleSheet(NAV_ACTIVE_STYLE);
    
    // -- AI floating button ---------------------------------------------------
    m_aiWidget = new AIChatWidget(this);
    m_aiWidget->setContext("Gestion des Employés");

    m_floatingBtn = new FloatingAIButton(this);
    connect(m_floatingBtn, &FloatingAIButton::clicked, this, [this]() {
        m_aiWidget->toggleChat();
    });

    // Forcer le plein écran au démarrage
    showMaximized();

    // Positionner et afficher le bouton flottant aprés que la fenétre soit visible
    QTimer::singleShot(100, this, [this]() {
        m_floatingBtn->move(190, height() - 84);
        m_floatingBtn->show();
        m_floatingBtn->raise();

        if (m_bell) {
            m_bell->move(width() - m_bell->width() - 12, 6);
            m_bell->setVisible(false); // masquée par défaut, visible seulement sur Production
            m_bell->raise();
        }
        
        if (m_tempIndicator) {
            // Positionner à gauche de la cloche
            int bellX = m_bell ? (width() - m_bell->width() - 12) : width();
            m_tempIndicator->move(bellX - m_tempIndicator->width() - 8, 6);
            // Visibilité gérée par switchPage — ne pas forcer ici
            m_tempIndicator->raise();
        }
    });
    
    // === ARDUINO MULTI-CARTES - Découverte automatique ===
    setupArduinoMultiCartes();
}

MainWindow::~MainWindow() 
{
    // Arrêter la surveillance Arduino
    if (m_arduinoMonitor) {
        m_arduinoMonitor->stopTemperatureMonitoring();
        delete m_arduinoMonitor;
        m_arduinoMonitor = nullptr;
    }
    // ArduinoManager supprime les instances Arduino qu'il a créées
    delete m_arduinoManager;
    m_arduinoManager = nullptr;
    
    // Arrêter l'API Python via taskkill (processus détaché)
    QProcess::execute("cmd.exe", QStringList() << "/c" << "taskkill /f /im python.exe >nul 2>&1");
    delete ui; 
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    // Gérer le clic sur une zone vide du tableau employés
    if (obj == ui->employeeTable->viewport() && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        QModelIndex index = ui->employeeTable->indexAt(mouseEvent->pos());
        
        // Si on clique sur une zone vide (pas sur une cellule)
        if (!index.isValid()) {
            ui->employeeTable->clearSelection();
            ui->employeeProfilePanel->setVisible(false);
            return true; // événement traité
        }
    }
    
    // Passer l'événement au gestionnaire parent
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    if (m_floatingBtn) {
        m_floatingBtn->move(190, height() - 84);
        m_floatingBtn->show();
        m_floatingBtn->raise();
    }
    if (m_bell) {
        m_bell->move(width() - m_bell->width() - 12, 6);
        m_bell->raise();
    }
    if (m_tempIndicator) {
        int bellX = m_bell ? (width() - m_bell->width() - 12) : width();
        m_tempIndicator->move(bellX - m_tempIndicator->width() - 8, 6);
        m_tempIndicator->raise();
    }
    if (m_arduinoIndicator) {
        m_arduinoIndicator->move(width() - m_arduinoIndicator->width() - 12,
                                  m_bell ? m_bell->height() + 10 : 6);
        m_arduinoIndicator->raise();
    }
    if (m_lcdLigne1) {
        // LCD positionné juste sous l'indicateur Arduino
        QWidget *lcdFrame = m_lcdLigne1->parentWidget();
        if (lcdFrame) {
            int indY = m_arduinoIndicator
                ? (m_bell ? m_bell->height() + 10 : 6) + m_arduinoIndicator->height() + 6
                : 50;
            lcdFrame->move(width() - lcdFrame->width() - 12, indY);
            lcdFrame->raise();
        }
    }
}

// -- Navigation helpers --------------------------------------------------------
void MainWindow::switchPage(int index, QPushButton *activeBtn, const QString &title)
{
    const QList<QPushButton*> navBtns = {
        ui->btnEmployees, ui->btnClients, ui->btnProducts,
        ui->btnRawMaterials, ui->btnSuppliers, ui->btnProduction
    };
    for (auto *b : navBtns) b->setStyleSheet("");
    if (activeBtn) activeBtn->setStyleSheet(NAV_ACTIVE_STYLE);

    ui->stackedWidget->setCurrentIndex(index);

    if (!title.isEmpty()) setWindowTitle(title);

    // Garder le bouton flottant toujours au-dessus
    if (m_floatingBtn) m_floatingBtn->raise();

    // Restaurer la position fixe du chat IA si visible (équivalent position:fixed)
    if (m_aiWidget && m_aiWidget->isVisible()) {
        m_aiWidget->raise();
    }

    // Cloche et indicateur Arduino visibles uniquement sur la page Production (index 4)
    if (m_bell) m_bell->setVisible(index == 4);
    if (m_arduinoIndicator) m_arduinoIndicator->setVisible(index == 4);
    // Indicateur température visible sur toutes les pages SAUF Production
    if (m_tempIndicator) m_tempIndicator->setVisible(index != 4);
    // LCD miroir visible uniquement sur la page Production
    if (m_lcdLigne1) {
        QWidget *lcdFrame = m_lcdLigne1->parentWidget();
        if (lcdFrame) lcdFrame->setVisible(index == 4);
    }
}

void MainWindow::on_btnEmployees_clicked()  
{ 
    switchPage(0, ui->btnEmployees,  "CUIREA - Gestion des Employés");  
    m_aiWidget->setContext("Gestion des Employés"); 
    // Masquer le panneau de profil et désélectionner
    ui->employeeProfilePanel->setVisible(false);
    ui->employeeTable->clearSelection();
}

void MainWindow::on_btnClients_clicked()    
{ 
    switchPage(1, ui->btnClients,    "CUIREA - Gestion des Clients"); 
    m_aiWidget->setContext("Gestion des Clients");
    // Masquer le panneau de profil employé quand on change de page
    ui->employeeProfilePanel->setVisible(false);
    afficherClients();
}

void MainWindow::on_btnProducts_clicked()   
{ 
    switchPage(5, ui->btnProducts,   "CUIREA - Gestion des Articles"); 
    m_aiWidget->setContext("Gestion des Articles"); 
    // Masquer le panneau de profil employé quand on change de page
    ui->employeeProfilePanel->setVisible(false);
}

void MainWindow::on_btnRawMaterials_clicked()
{ 
    switchPage(2, ui->btnRawMaterials,"CUIREA - Matières Premières"); 
    m_aiWidget->setContext("Gestion des Matières Premières"); 
    // Masquer le panneau de profil employé quand on change de page
    ui->employeeProfilePanel->setVisible(false);
}

void MainWindow::on_btnSuppliers_clicked()  
{ 
    switchPage(3, ui->btnSuppliers,  ""); 
    m_aiWidget->setContext("Gestion des Fournisseurs"); 
    // Masquer le panneau de profil employé quand on change de page
    ui->employeeProfilePanel->setVisible(false);
}
void MainWindow::on_btnProduction_clicked() 
{ 
    switchPage(4, ui->btnProduction, "CUIREA - Gestion de la Production");
    updateProductionStatsCards();
    m_aiWidget->setContext("Gestion de la Production");
    // Masquer le panneau de profil employe quand on change de page
    ui->employeeProfilePanel->setVisible(false);
    m_notifiedIds.clear();
    QTimer::singleShot(3000, this, &MainWindow::checkRetards);
    // setupKeypadSimulator(); // Désactivé - simulateur keypad non nécessaire
}

// -- Employee CRUD -------------------------------------------------------------
void MainWindow::on_btnAdd_clicked()
{
    EmployeeDialog dlg(this, EmployeeDialog::AddMode);
    if (dlg.exec() == QDialog::Accepted) {
        Employe e;
        
        // Mode BD - toujours essayer d'ajouter dans la base de données
        e.setMatricule(dlg.getMatricule());
        e.setNom(dlg.getNom());
        e.setPrenom(dlg.getPrenom());
        e.setCin(dlg.getCin());
        e.setDateNaissance(dlg.getDateNaissance());
        e.setSexe(dlg.getSexe());
        e.setAdresse(dlg.getAdresse());
        e.setTelephone(dlg.getTelephone());
        e.setRfidUid(dlg.getRfidUid());
        e.setEmail(dlg.getEmail());
        e.setPoste(dlg.getPoste());
        e.setDepartement(dlg.getDepartement());
        e.setDateEmbauche(dlg.getDateEmbauche());
        e.setPhotoPath(dlg.getPhotoPath());
        
        // Nouveaux champs d'authentification
        e.setMotDePasse(dlg.getMotDePasse());
        e.setRoleSysteme(dlg.getRoleSysteme());
        e.setActif(dlg.isActif());
        
        if (e.ajouter()) {
            populateEmployeeTable();
            NotificationWidget::show("Employe ajoute",
                dlg.getNom() + " " + dlg.getPrenom() + " - " + dlg.getRoleSysteme(),
                NotificationWidget::Success);
            QMessageBox::information(this, "Succes", 
                QString("Employe ajoute avec succes !\n\n"
                       "Identifiants de connexion:\n"
                       "Matricule: %1\n"
                       "Role: %2\n"
                       "Compte actif: %3")
                .arg(dlg.getMatricule())
                .arg(dlg.getRoleSysteme())
                .arg(dlg.isActif() ? "Oui" : "Non"));
        } else {
            NotificationWidget::show("Erreur ajout employe",
                "Impossible d'ajouter l'employe dans la base.",
                NotificationWidget::Critical);
            QMessageBox::critical(this, "Erreur", 
                "Impossible d'ajouter l'employe.\n"
                "Verifiez que la table EMPLOYES existe dans la base de donnees.");
        }
    }
}

void MainWindow::on_btnExport_clicked()
{
    int row = ui->employeeTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Attention", 
            "Veuillez sélectionner un employé pour générer sa fiche de paie.");
        return;
    }
    
    // Recuperer les donnees de l'employe selectionne
    int idEmploye = cellText(ui->employeeTable, row, 13).toInt();  // Colonne 13 = ID_EMPLOYE
    QString matricule = cellText(ui->employeeTable, row, 0);
    QString nom = cellText(ui->employeeTable, row, 1);
    QString prenom = cellText(ui->employeeTable, row, 2);
    QString cin = cellText(ui->employeeTable, row, 3);
    QString poste = cellText(ui->employeeTable, row, 9);
    QString departement = cellText(ui->employeeTable, row, 10);
    QString dateEmbaucheStr = cellText(ui->employeeTable, row, 11);
    QDate dateEmbauche = QDate::fromString(dateEmbaucheStr, "dd/MM/yyyy");
    
    // ✅ Utiliser le nouveau constructeur avec ID employé pour calculer les absences
    FichePaieDialog dlg(idEmploye, matricule, nom, prenom, cin, poste, departement, dateEmbauche, this);
    dlg.exec();
}

void MainWindow::on_btnEdit_clicked()
{
    int row = ui->employeeTable->currentRow();
    if (row < 0) { QMessageBox::warning(this,"","Veuillez sélectionner un employé à modifier."); return; }
    
    EmployeeDialog dlg(this, EmployeeDialog::EditMode);
    // Colonnes: 0=Matricule, 1=Nom, 2=Prénom, 3=CIN, 4=DateNaissance, 5=Sexe,
    //           6=Adresse, 7=Téléphone, 8=Email, 9=Poste, 10=Département,
    //           11=DateEmbauche, 12=Photo, 13=ID
    QTableWidgetItem *idItem = ui->employeeTable->item(row, 13);
    QString employeeId = idItem ? idItem->text() : "";
    dlg.setEmployeeData(employeeId, cellText(ui->employeeTable,row,0),
                        cellText(ui->employeeTable,row,1), cellText(ui->employeeTable,row,2),
                        cellText(ui->employeeTable,row,3), cellText(ui->employeeTable,row,4),
                        cellText(ui->employeeTable,row,5), cellText(ui->employeeTable,row,6),
                        cellText(ui->employeeTable,row,7), cellText(ui->employeeTable,row,8),
                        cellText(ui->employeeTable,row,9), "",
                        cellText(ui->employeeTable,row,10), cellText(ui->employeeTable,row,11),
                        cellText(ui->employeeTable,row,12));
    
    if (dlg.exec() == QDialog::Accepted) {
        Employe e;
        
        // Mode BD - toujours modifier dans la base de données
        e.setId(employeeId.toInt());
        e.setMatricule(dlg.getMatricule());
        e.setNom(dlg.getNom());
        e.setPrenom(dlg.getPrenom());
        e.setCin(dlg.getCin());
        e.setDateNaissance(dlg.getDateNaissance());
        e.setSexe(dlg.getSexe());
        e.setAdresse(dlg.getAdresse());
        e.setTelephone(dlg.getTelephone());
        e.setRfidUid(dlg.getRfidUid());
        e.setEmail(dlg.getEmail());
        e.setPoste(dlg.getPoste());
        e.setDepartement(dlg.getDepartement());
        e.setDateEmbauche(dlg.getDateEmbauche());
        e.setPhotoPath(dlg.getPhotoPath());
        
        // Nouveaux champs d'authentification
        e.setMotDePasse(dlg.getMotDePasse());
        e.setRoleSysteme(dlg.getRoleSysteme());
        e.setActif(dlg.isActif());
        
        if (e.modifier()) {
            populateEmployeeTable();
            NotificationWidget::show("Employe modifie", "Modifications enregistrees.", NotificationWidget::Success);
            QMessageBox::information(this, "Succes", "Employe modifie avec succes !");
        } else {
            QMessageBox::critical(this, "Erreur", 
                "Impossible de modifier l'employé.\n"
                "Vérifiez que la table EMPLOYES existe dans la base de données.");
        }
    }
}

void MainWindow::on_btnDelete_clicked()
{
    int row = ui->employeeTable->currentRow();
    if (row < 0) { QMessageBox::warning(this,"","Veuillez sélectionner un employé à supprimer."); return; }
    
    EmployeeDialog dlg(this, EmployeeDialog::DeleteMode);
    // Colonnes: 0=Matricule, 1=Nom, 2=Prénom, 3=CIN, 4=DateNaissance, 5=Sexe,
    //           6=Adresse, 7=Téléphone, 8=Email, 9=Poste, 10=Département,
    //           11=DateEmbauche, 12=Photo, 13=ID
    QTableWidgetItem *idItem = ui->employeeTable->item(row, 13);
    QString employeeId = idItem ? idItem->text() : "";
    dlg.setEmployeeData(employeeId, cellText(ui->employeeTable,row,0),
                        cellText(ui->employeeTable,row,1), cellText(ui->employeeTable,row,2),
                        "","","","","","",
                        cellText(ui->employeeTable,row,9),"",
                        cellText(ui->employeeTable,row,10),"");
    
    if (dlg.exec() == QDialog::Accepted) {
        int id = employeeId.toInt();
        
        // Mode BD - toujours supprimer de la base de données
        Employe e;
        if (e.supprimer(id)) {
            populateEmployeeTable();
            NotificationWidget::show("Employe supprime", "L'employe a ete supprime.", NotificationWidget::Warning);
            QMessageBox::information(this, "Succes", "Employe supprime avec succes !");
        } else {
            QMessageBox::critical(this, "Erreur", 
                QString("Impossible de supprimer l'employé.\n"
                       "ID: %1\n"
                       "Vérifiez que la table EMPLOYES existe dans la base de données.").arg(id));
        }
    }
}

void MainWindow::on_btnSort_clicked()
{
    QMenu menu(this);
    menu.setStyleSheet(
        "QMenu{background:#FAF5F0;border:2px solid #8D6E63;border-radius:6px;padding:8px;}"
        "QMenu::item{padding:8px 25px;color:#291C0E;border-radius:4px;}"
        "QMenu::item:selected{background:#8D6E63;color:white;}"
        "QMenu::separator{height:2px;background:#BCAAA4;margin:5px 10px;}"
    );

    // Sous-menu pour chaque colonne
    auto addSortOptions = [&](const QString &label, int col) {
        QMenu *sub = menu.addMenu("?? " + label);
        sub->setStyleSheet(menu.styleSheet());
        auto *asc = sub->addAction("? Croissant (A ? Z)");
        auto *desc = sub->addAction("? Décroissant (Z ? A)");
        connect(asc, &QAction::triggered, [=]{ ui->employeeTable->sortItems(col, Qt::AscendingOrder); });
        connect(desc, &QAction::triggered, [=]{ ui->employeeTable->sortItems(col, Qt::DescendingOrder); });
    };

    addSortOptions("Matricule", 1);
    addSortOptions("Nom", 2);
    addSortOptions("Prénom", 3);

    // Afficher le menu sous le bouton
    QPoint pos = ui->btnSort->mapToGlobal(QPoint(0, ui->btnSort->height()));
    menu.exec(pos);
}

void MainWindow::on_btnStatistics_clicked()
{
    // Ouvrir le dialog de statistiques RH professionnel
    StatsRHDialog dlg(this);
    dlg.exec();
}

void MainWindow::populateEmployeeTable()
{
    Employe e;
    QSqlQueryModel* model = e.afficher();
    
    if (!model || model->rowCount() == 0) {
        ui->employeeTable->setRowCount(0);
        ui->statsValue->setText("0");
        ui->statsValue_2->setText("0");
        ui->statsValue_3->setText("0");
        ui->employeeProfilePanel->setVisible(false);
        if (model) delete model;
        return;
    }
    
    int n = model->rowCount();
    ui->employeeTable->setRowCount(n);
    QSet<QString> depts;
    
    for (int i = 0; i < n; ++i) {
        for (int col = 0; col < 14; ++col) {
            QString value = model->data(model->index(i, col)).toString();
            ui->employeeTable->setItem(i, col, new QTableWidgetItem(value));
        }
        depts.insert(model->data(model->index(i, 11)).toString());
    }
    
    ui->statsValue->setText(QString::number(n));
    ui->statsValue_2->setText(QString::number(n));
    ui->statsValue_3->setText(QString::number(depts.size()));
    
    delete model;
}

void MainWindow::updateEmployeeStatistics()
{
    int total = ui->employeeTable->rowCount();
    QSet<QString> depts;
    
    for (int r = 0; r < total; ++r) {
        QString dept = cellText(ui->employeeTable, r, 6);
        if (!dept.isEmpty()) {
            depts.insert(dept);
        }
    }
    
    ui->statsValue->setText(QString::number(total));
    ui->statsValue_2->setText(QString::number(total));
    ui->statsValue_3->setText(QString::number(depts.size()));
}

void MainWindow::onEmployeeSelected()
{
    int row = ui->employeeTable->currentRow();
    
    // Si aucune ligne sélectionnée, masquer le panneau
    if (row < 0) {
        ui->employeeProfilePanel->setVisible(false);
        return;
    }
    
    // Vérifier que la ligne a des données valides
    if (ui->employeeTable->item(row, 1) == nullptr) {
        ui->employeeProfilePanel->setVisible(false);
        return;
    }
    
    // Afficher le panneau et remplir les informations
    ui->employeeProfilePanel->setVisible(true);
    
    // Remplir les labels avec les données de l'employé sélectionné
    // Colonnes: 0=Matricule, 1=Nom, 2=Prénom, 3=CIN, 4=DateNaissance, 5=Sexe,
    //           6=Adresse, 7=Téléphone, 8=Email, 9=Poste, 10=Département,
    //           11=DateEmbauche, 12=Photo, 13=ID
    ui->valMatricule->setText(cellText(ui->employeeTable, row, 0));
    ui->valNom->setText(cellText(ui->employeeTable, row, 1));
    ui->valPrenom->setText(cellText(ui->employeeTable, row, 2));
    ui->valCIN->setText(cellText(ui->employeeTable, row, 3));
    ui->valSexe->setText(cellText(ui->employeeTable, row, 5));
    ui->valAdresse->setText(cellText(ui->employeeTable, row, 6));
    ui->valTelephone->setText(cellText(ui->employeeTable, row, 7));
    ui->valPoste->setText(cellText(ui->employeeTable, row, 9));
    ui->valRole->setText(cellText(ui->employeeTable, row, 10));
    ui->valDepartement->setText(cellText(ui->employeeTable, row, 10));
    ui->valDateEmbauche->setText(cellText(ui->employeeTable, row, 11));
    
    // Récupérer la photo depuis la base de données
    QTableWidgetItem *idItem = ui->employeeTable->item(row, 13);
    int employeId = idItem ? idItem->text().toInt() : 0;
    
    QSqlQuery query(Connection::instance()->getDatabase());
    query.prepare("SELECT PHOTO_URL FROM CUIREA.EMPLOYES WHERE ID_EMPLOYE = :id");
    query.bindValue(":id", employeId);
    
    QString photoPath;
    if (query.exec() && query.next()) {
        photoPath = query.value(0).toString();
    }
    
    if (!photoPath.isEmpty() && QFile::exists(photoPath)) {
        QPixmap pixmap(photoPath);
        if (!pixmap.isNull()) {
            ui->employeePhotoPlaceholder->setPixmap(pixmap.scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            ui->employeePhotoPlaceholder->setStyleSheet("background-color: #FFF8F0; border: 2px solid #8D6E63; border-radius: 8px;");
        } else {
            ui->employeePhotoPlaceholder->setPixmap(QPixmap());
            ui->employeePhotoPlaceholder->setText("??");
            ui->employeePhotoPlaceholder->setStyleSheet("background-color: #FFF8F0; border: 2px solid #BCAAA4; border-radius: 8px; font-size: 48px; color: #8D6E63;");
        }
    } else {
        ui->employeePhotoPlaceholder->setPixmap(QPixmap());
        ui->employeePhotoPlaceholder->setText("??");
        ui->employeePhotoPlaceholder->setStyleSheet("background-color: #FFF8F0; border: 2px solid #BCAAA4; border-radius: 8px; font-size: 48px; color: #8D6E63;");
    }
}

// -- Client CRUD ---------------------------------------------------------------
// ============================================================
// Afficher
// ============================================================
void MainWindow::afficherClients()
{
    Client c;
    QSqlQueryModel* model = c.afficherClients();
    ui->clientTable->clear();
    ui->clientTable->setRowCount(model->rowCount());
    ui->clientTable->setColumnCount(model->columnCount());
    QStringList headers = { "ID","Nom", "Prénom", "Sexe", "CIN", "Pays", "Ville", "Adresse", "Email", "Date Inscription"};
    ui->clientTable->setHorizontalHeaderLabels(headers);
    for(int row = 0; row < model->rowCount(); ++row)
        for(int col = 0; col < model->columnCount(); ++col)
            ui->clientTable->setItem(row, col,
                                     new QTableWidgetItem(model->index(row,col).data().toString()));
    ui->clientTable->resizeColumnsToContents();
    ui->clientTable->horizontalHeader()->setStretchLastSection(true);
    ui->clientTable->setColumnHidden(0, true);
    delete model;
}

// ============================================================
// Ajouter
// ============================================================
void MainWindow::on_btnAddClient_clicked()
{
    ClientManagerDialog dlg(this, ClientManagerDialog::AddMode);
    if(dlg.exec() == QDialog::Accepted) {
        Client c;
        c.setNom(dlg.getNom());
        c.setPrenom(dlg.getPrenom());
        c.setSexe(dlg.getSexe());
        c.setCin(dlg.getCin());
        c.setPays(dlg.getPays());
        c.setVille(dlg.getVille());
        c.setAdresse(dlg.getAdresse());
        c.setEmail(dlg.getEmail());
        c.setDate_inscription(dlg.getDateInscription());
        // id_employe: laisser 0 si non requis, sinon : c.setId_employe(...);

        if(c.ajouter()) {
            afficherClients();
            QMessageBox::information(this, "Succès", "Client ajouté avec succès !");
        } else {
            QMessageBox::critical(this, "Erreur",
                                  "Impossible d'ajouter le client.\n"
                                  "Vérifiez que la table CLIENTS existe dans la base de données.");
        }
    }
}

// ============================================================
// Modifier
// ============================================================
void MainWindow::on_btnEditClient_clicked()
{
    int row = ui->clientTable->currentRow();
    if(row < 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un client à modifier.");
        return;
    }

    int id = ui->clientTable->item(row, 0)->text().toInt();

    ClientManagerDialog dlg(this, ClientManagerDialog::EditMode);
    dlg.setEditingId(id);
    dlg.setClientData(
        ui->clientTable->item(row, 1)->text(),  // Nom
        ui->clientTable->item(row, 2)->text(),  // Prénom
        ui->clientTable->item(row, 3)->text(),  // Sexe
        ui->clientTable->item(row, 4)->text(),  // CIN
        ui->clientTable->item(row, 5)->text(),  // Pays
        ui->clientTable->item(row, 6)->text(),  // Ville
        ui->clientTable->item(row, 7)->text(),  // Adresse
        ui->clientTable->item(row, 8)->text()   // Email
        );

    if(dlg.exec() == QDialog::Accepted) {
        Client c;
        c.setId_client(id);
        c.setNom(dlg.getNom());
        c.setPrenom(dlg.getPrenom());
        c.setSexe(dlg.getSexe());
        c.setCin(dlg.getCin());
        c.setPays(dlg.getPays());
        c.setVille(dlg.getVille());
        c.setAdresse(dlg.getAdresse());
        c.setEmail(dlg.getEmail());
        c.setDate_inscription(dlg.getDateInscription());

        if(c.modifier()) {
            afficherClients();
            QMessageBox::information(this, "Succès", "Client modifié avec succès !");
        } else {
            QMessageBox::critical(this, "Erreur", "échec de modification du client !");
        }
    }
}

// ============================================================
// Supprimer
// ============================================================
void MainWindow::on_btnDeleteClient_clicked()
{
    int row = ui->clientTable->currentRow();
    if(row < 0) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un client à supprimer.");
        return;
    }

    int id = ui->clientTable->item(row, 0)->text().toInt();

    ClientManagerDialog dlg(this, ClientManagerDialog::DeleteMode);
    dlg.setDeleteId(id);
    dlg.setClientData(
        ui->clientTable->item(row, 1)->text(),  // Nom
        ui->clientTable->item(row, 2)->text(),  // Prénom
        ui->clientTable->item(row, 3)->text(),  // Sexe
        ui->clientTable->item(row, 4)->text(),  // CIN
        ui->clientTable->item(row, 5)->text(),  // Pays
        ui->clientTable->item(row, 6)->text(),  // Ville
        "", ""
        );

    if(dlg.exec() == QDialog::Accepted) {
        if(Client().supprimer(id)) {
            afficherClients();
            QMessageBox::information(this, "Succès", "Client supprimé avec succès !");
        } else {
            QMessageBox::critical(this, "Erreur", "échec de suppression du client !");
        }
    }
}

// ============================================================
// Refresh
// ============================================================
void MainWindow::on_btnRefreshClient_clicked()
{
    afficherClients();
}


// ============================================================
// Recherche
// ============================================================


void MainWindow::on_searchBoxClient_textChanged(const QString &text)
{
    QString search = text.trimmed();

    ui->clientTable->setSortingEnabled(false);
    ui->clientTable->clearContents();
    ui->clientTable->setRowCount(0);

    Client temp;
    QList<Client> liste = temp.getAll();

    int row = 0;
    for (const Client &c : liste) {
        bool match = search.isEmpty()
        || c.getNom().contains(search,    Qt::CaseInsensitive)
            || c.getPrenom().contains(search, Qt::CaseInsensitive)
            || c.getCin().contains(search,    Qt::CaseInsensitive);

        if (match) {
            ui->clientTable->insertRow(row);
            ui->clientTable->setItem(row, 0, new QTableWidgetItem(QString::number(c.getId_client())));
            ui->clientTable->setItem(row, 1, new QTableWidgetItem(c.getNom()));
            ui->clientTable->setItem(row, 2, new QTableWidgetItem(c.getPrenom()));
            ui->clientTable->setItem(row, 3, new QTableWidgetItem(c.getSexe()));
            ui->clientTable->setItem(row, 4, new QTableWidgetItem(c.getCin()));
            ui->clientTable->setItem(row, 5, new QTableWidgetItem(c.getPays()));
            ui->clientTable->setItem(row, 6, new QTableWidgetItem(c.getVille()));
            ui->clientTable->setItem(row, 7, new QTableWidgetItem(c.getAdresse()));
            ui->clientTable->setItem(row, 8, new QTableWidgetItem(c.getEmail()));
            ui->clientTable->setItem(row, 9, new QTableWidgetItem(c.getDate_inscription()));
            ++row;
        }
    }

    ui->clientTable->setSortingEnabled(true);
}
//-------------------------------------------------
//btn tri
//-------------------------------------------------


void MainWindow::on_btntrie_clicked()
{
    QMenu *menu = new QMenu(this);
    // ?? STYLE
    menu->setStyleSheet(
        "QMenu{background:#FAF5F0;border:2px solid #8D6E63;border-radius:6px;padding:8px;}"
        "QMenu::item{padding:8px 25px;color:#291C0E;border-radius:4px;}"
        "QMenu::item:selected{background:#8D6E63;color:white;}"
        );
    // ?? DATE SUBMENU
    QMenu *dateMenu = menu->addMenu("?? Date inscription");
    QAction *dateAsc  = dateMenu->addAction("Croissant");
    QAction *dateDesc = dateMenu->addAction("Décroissant");
    // ?? ALPHA SUBMENU
    QMenu *alphaMenu = menu->addMenu("?? Alphabétique");
    QAction *alphaAsc  = alphaMenu->addAction("Croissant");
    QAction *alphaDesc = alphaMenu->addAction("Décroissant");
    // =========================
    // ?? ACTION HANDLING
    // =========================
    connect(menu, &QMenu::triggered, this, [=](QAction *action) {
        QString orderBy;
        QString label;
        if (action == dateAsc) {
            orderBy = "date_inscription ASC";
            label = "Date ?";
        }
        else if (action == dateDesc) {
            orderBy = "date_inscription DESC";
            label = "Date ?";
        }
        else if (action == alphaAsc) {
            orderBy = "nom ASC, prenom ASC";
            label = "A ? Z";
        }
        else if (action == alphaDesc) {
            orderBy = "nom DESC, prenom DESC";
            label = "Z ? A";
        }
        // ===== REFRESH TABLE =====
        ui->clientTable->setSortingEnabled(false);
        ui->clientTable->clearContents();
        ui->clientTable->setRowCount(0);
        QSqlQuery q;
        q.exec("SELECT id_client, nom, prenom, sexe, cin, pays, ville, adresse, email, "
               "TO_CHAR(date_inscription, 'YYYY-MM-DD'), id_employe "
               "FROM CLIENTS ORDER BY " + orderBy);
        int row = 0;
        while (q.next()) {
            ui->clientTable->insertRow(row);
            for (int i = 0; i < 10; ++i)
                ui->clientTable->setItem(row, i, new QTableWidgetItem(q.value(i).toString()));
            row++;
        }
        ui->clientTable->setSortingEnabled(true);
        ui->btntrie->setText("Trier : " + label);
    });
    // ?? Show menu under button
    menu->exec(ui->btntrie->mapToGlobal(QPoint(0, ui->btntrie->height())));
}
//-------------------------------------------------------------------------------------------------
//btn statclient
//-------------------------------------------------------------------------------------------------
void MainWindow::on_btnStatsByRegion_clicked()
{
    Client c;

    QMap<QString, int> stats = c.statistiquesParVille();
    QMap<QString, int> monthlyStats = c.statistiquesParMois();

    if(stats.isEmpty())
        return;

    QWidget *window = StatsCharts::createStatsWindow(stats, monthlyStats);
    window->show();
}
void MainWindow::on_btnFidelityClassification_clicked() {}
void MainWindow::on_btnTriClient_clicked() {}

//--exportclient


void MainWindow::on_btnExportClient_clicked()
{
    int row = ui->productionTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Erreur", "Sélectionnez une commande !");
        return;
    }

    // 🔹 Find mail column
    int mailColumn = -1;
    for (int c = 0; c < ui->productionTable->columnCount(); ++c) {
        if (ui->productionTable->horizontalHeaderItem(c)->text().toLower().contains("mail client")) {
            mailColumn = c;
            break;
        }
    }

    if (mailColumn == -1) {
        QMessageBox::critical(this, "Erreur", "Colonne mail_client introuvable !");
        return;
    }

    QString mailClient = ui->productionTable->item(row, mailColumn)->text();

    QString filePath = QFileDialog::getSaveFileName(
        this,
        "Exporter les commandes du client",
        "",
        "CSV (*.csv);;PDF (*.pdf);"
    );

    if (filePath.isEmpty())
        return;

    Client c;
    if (c.exporterCommandesParClient(mailClient, filePath)) {
        QMessageBox::information(this, "Succes", "Export reussi !");
    } else {
        QMessageBox::critical(this, "Erreur", "Echec de l'export !");
    }
}
//historique client

void MainWindow::on_btnhistorique_clicked()
{
    QWidget *window = new QWidget();
    window->resize(700, 450);

    QVBoxLayout *layout = new QVBoxLayout(window);

    // Title
    QLabel *title = new QLabel("Historique des clients");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(
        "font-size: 20px;"
        "font-weight: bold;"
        "color: white;"   // dark brown (CUIREA theme)
        "padding: 8px;"
    );

    // Table
    QTableView *table = new QTableView();
    Client c;
    table->setModel(c.afficherHistorique());
    // Styling (CUIREA palette)
    table->setStyleSheet(
            "QTableView {"
            "background-color: #FAF5F0;"
            "alternate-background-color: #F3E9DD;"
            "gridline-color: #D8C3A5;"
            "color: #291C0E;"
            "selection-background-color: #C19A6B;"
            "selection-color: white;"
            "border: 1px solid #D8C3A5;"
            "}"

            "QHeaderView::section {"
            "background-color: #6B4F3B;"
            "color: white;"
            "padding: 6px;"
            "border: none;"
            "font-weight: bold;"
        "}"
    );

    table->setAlternatingRowColors(true);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->verticalHeader()->setVisible(false);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(title);
    layout->addWidget(table);
    window->setLayout(layout);
    window->show();
}

// -- Raw Materials -------------------------------------------------------------
void MainWindow::setupMatiereTable()
{
    Matiere matiereTmp;
    QSqlQueryModel* model = matiereTmp.afficher();
    if (!model) {
        return;
    }
    ui->matiereTable->setRowCount(0);
    ui->matiereTable->setColumnCount(7);
    ui->matiereTable->setHorizontalHeaderLabels({"MODULE", "RÉFÉRENCE", "TYPE", "QUANTITÉ ACTUELLE", "SEUIL", "DATE D'EXPIRATION", "PHOTO"});
    const int rowCount = model->rowCount();
    for (int row = 0; row < rowCount; ++row) {
        ui->matiereTable->insertRow(row);
        // Créer les items de base
        ui->matiereTable->setItem(row, 0, new QTableWidgetItem(model->data(model->index(row, 1)).toString()));
        ui->matiereTable->setItem(row, 1, new QTableWidgetItem(model->data(model->index(row, 2)).toString()));
        ui->matiereTable->setItem(row, 2, new QTableWidgetItem(model->data(model->index(row, 3)).toString()));
        ui->matiereTable->setItem(row, 3, new QTableWidgetItem(model->data(model->index(row, 4)).toString() + " m²"));
        ui->matiereTable->setItem(row, 4, new QTableWidgetItem(model->data(model->index(row, 5)).toString()));
        ui->matiereTable->setItem(row, 5, new QTableWidgetItem(model->data(model->index(row, 6)).toDate().toString("yyyy-MM-dd")));
        QString photoPath = model->data(model->index(row, 7)).toString();
        QTableWidgetItem* photoItem = new QTableWidgetItem();
        if (!photoPath.isEmpty() && QFile::exists(photoPath)) {
            // Utiliser setCellWidget avec QLabel pour afficher la photo en grand
            QLabel *photoLabel = new QLabel();
            QPixmap pix(photoPath);
            photoLabel->setPixmap(pix.scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            photoLabel->setAlignment(Qt::AlignCenter);
            photoLabel->setToolTip("Double-cliquez pour agrandir");
            ui->matiereTable->setCellWidget(row, 6, photoLabel);
        } else {
            photoItem->setText("é");
            ui->matiereTable->setItem(row, 6, photoItem);
        }
        
        // Stocker l'ID et le chemin photo
        ui->matiereTable->item(row, 0)->setData(Qt::UserRole, model->data(model->index(row, 0)).toInt());
        ui->matiereTable->item(row, 0)->setData(Qt::UserRole + 1, photoPath);
        
        // Hauteur de ligne pour la photo
        ui->matiereTable->setRowHeight(row, 130);
    }
    delete model;
    ui->matiereTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // Fixer la largeur de la colonne PHOTO
    ui->matiereTable->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Fixed);
    ui->matiereTable->setColumnWidth(6, 140);
    ui->matiereTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    updateMatiereStatistics();
}

void MainWindow::onAddMatiere()
{
    MatiereDialog dlg(this, MatiereDialog::AddMode);
    if (dlg.exec() != QDialog::Accepted) return;
    
    Matiere matiere;
    matiere.setNom(dlg.getModule());
    matiere.setReference(dlg.getReference());
    matiere.setType(dlg.getType());
    matiere.setQuantite(dlg.getQuantite().split(" ").first().toDouble());
    matiere.setSeuil(dlg.getSeuil().toInt());
    matiere.setDateExpiration(QDate::fromString(dlg.getDateExpiration(), "yyyy-MM-dd"));
    matiere.setIdFournisseur(1);
    matiere.setPhotoUrl(dlg.getPhotoUrl());
    
    if (matiere.ajouter()) {
        QMessageBox::information(this, "Succès", "Matière ajoutée avec succès!");
        setupMatiereTable();
    } else {
        QMessageBox::critical(this, "Erreur", "Erreur lors de l'ajout de la matière.");
    }
}
void MainWindow::onEditMatiere()
{
    int row = ui->matiereTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "", "Veuillez sélectionner une matière a modifier.");
        return;
    }
    int matiereId = ui->matiereTable->item(row, 0)->data(Qt::UserRole).toInt();
    QString photoPath = ui->matiereTable->item(row, 0)->data(Qt::UserRole + 1).toString();
    MatiereDialog dlg(this, MatiereDialog::EditMode);
    dlg.setMatiereData(cellText(ui->matiereTable, row, 0), cellText(ui->matiereTable, row, 1),
                       cellText(ui->matiereTable, row, 2), cellText(ui->matiereTable, row, 3),
                       cellText(ui->matiereTable, row, 4), cellText(ui->matiereTable, row, 5),
                       photoPath);
    if (dlg.exec() != QDialog::Accepted) return;
    Matiere matiere;
    matiere.setId(matiereId);
    matiere.setNom(dlg.getModule());
    matiere.setReference(dlg.getReference());
    matiere.setType(dlg.getType());
    matiere.setQuantite(dlg.getQuantite().split(" ").first().toDouble());
    matiere.setSeuil(dlg.getSeuil().toInt());
    matiere.setDateExpiration(QDate::fromString(dlg.getDateExpiration(), "yyyy-MM-dd"));
    matiere.setIdFournisseur(1);
    matiere.setPhotoUrl(dlg.getPhotoUrl());
    
    if (matiere.modifier()) {
        QMessageBox::information(this, "Succès", "Matière mise à jour avec succès!");
        setupMatiereTable();
    } else {
        QMessageBox::critical(this, "Erreur", "Erreur lors de la modification de la matière.");
    }
}
void MainWindow::onDeleteMatiere()
{
    int row = ui->matiereTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "", "Veuillez sélectionner une matière à supprimer.");
        return;
    }
    int matiereId = ui->matiereTable->item(row, 0)->data(Qt::UserRole).toInt();
    QString photoPath = ui->matiereTable->item(row, 0)->data(Qt::UserRole + 1).toString();
    MatiereDialog dlg(this, MatiereDialog::DeleteMode);
    dlg.setMatiereData(cellText(ui->matiereTable, row, 0), cellText(ui->matiereTable, row, 1),
                       cellText(ui->matiereTable, row, 2), cellText(ui->matiereTable, row, 3),
                       cellText(ui->matiereTable, row, 4), cellText(ui->matiereTable, row, 5),
                       photoPath);
    
    if (dlg.exec() != QDialog::Accepted) return;
    Matiere matiereTmp;
    if (matiereTmp.supprimer(matiereId)) {
        QMessageBox::information(this, "Succès", "Matière supprimée avec succès!");
        setupMatiereTable();
    } else {
        QMessageBox::critical(this, "Erreur", "Erreur lors de la suppression de la matière.");
    }
}
void MainWindow::onExportMatiere()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        "Exporter en PDF",
        QDir::homePath() + "/Rapport_Matieres_" + QDate::currentDate().toString("yyyy-MM-dd") + ".pdf",
        "PDF Files (*.pdf)");
    if (fileName.isEmpty()) return;
    //Calcul des statistiques
    int total = ui->matiereTable->rowCount();
    int critique = 0, normal = 0, eleve = 0, expires = 0, proche30j = 0;
    QMap<QString, int> parType;
    for (int r = 0; r < total; ++r) {
        // Extraire la quantité proprement
        QString qtyText = cellText(ui->matiereTable, r, 3);
        qtyText = qtyText.remove(" m²").remove(" mé").remove("m²").remove("mé").trimmed();
        double qty = qtyText.toDouble();
        
        int seuil = cellText(ui->matiereTable, r, 4).toInt();
        QString type = cellText(ui->matiereTable, r, 2);
        if      (qty < seuil * 0.5) critique++;
        else if (qty < seuil)       normal++;
        else                        eleve++;
        parType[type]++;
        QDate expDate = QDate::fromString(cellText(ui->matiereTable, r, 5), "yyyy-MM-dd");
        int daysLeft  = QDate::currentDate().daysTo(expDate);
        if      (daysLeft < 0)  expires++;
        else if (daysLeft < 30) proche30j++;
    }
    int stockOK = normal + eleve;
    // Construction HTML identique à l'image
    QString html;
    html += R"(
    <html><head><meta charset="UTF-8">
    <style>
      * { margin: 0; padding: 0; box-sizing: border-box; }
      body { font-family: Arial, sans-serif; background: #F5F0EB;
             color: #2C2416; padding: 0; }
      .header { background: #4A3428; color: white; padding: 18px 30px;
                display: flex; align-items: center; gap: 15px; }
      .logo { width: 60px; height: 60px; background: rgba(255,255,255,0.15);
              border-radius: 6px; display: flex; align-items: center;
              justify-content: center; font-size: 10px; font-weight: 700;
              text-align: center; line-height: 1.1; padding: 6px; }
      .header-title { flex: 1; font-size: 28px; font-weight: 700;
                      letter-spacing: 3px; text-transform: uppercase; }
      .subheader { background: #E8DED3; text-align: center; padding: 10px;
                   font-size: 12px; color: #5D4037; font-weight: 500; }
      .content { padding: 20px 25px; }
      .section-title { font-size: 15px; font-weight: 700; color: #2C2416;
                       margin: 20px 0 12px 0; text-transform: uppercase;
                       letter-spacing: 0.5px; }
      .cards { display: grid; grid-template-columns: repeat(5, 1fr);
               gap: 10px; margin-bottom: 20px; }
      .card { background: white; border-radius: 8px; padding: 14px;
              text-align: center; box-shadow: 0 2px 5px rgba(0,0,0,0.08);
              border: 2px solid #E0D5CC; }
      .card-icon { font-size: 24px; margin-bottom: 4px; }
      .card-title { font-size: 9px; font-weight: 700; text-transform: uppercase;
                    letter-spacing: 0.3px; margin-bottom: 4px; line-height: 1.2; }
      .card-value { font-size: 32px; font-weight: 700; margin: 4px 0; }
      .card-label { font-size: 10px; color: #666; font-weight: 500; }
      .card-total .card-title    { color: #2C2416; }
      .card-total .card-value    { color: #2C2416; }
      .card-critique .card-title { color: #D84315; }
      .card-critique .card-value { color: #D84315; }
      .card-expires .card-title  { color: #8B4513; }
      .card-expires .card-value  { color: #8B4513; }
      .card-proche .card-title   { color: #D4A574; }
      .card-proche .card-value   { color: #D4A574; }
      .card-ok .card-title       { color: #7B8A5C; }
      .card-ok .card-value       { color: #7B8A5C; }
      .charts-row { display: grid; grid-template-columns: 1fr 0.7fr;
                    gap: 15px; margin-bottom: 20px; }
      .chart-section { background: white; border-radius: 8px; padding: 16px;
                       box-shadow: 0 2px 5px rgba(0,0,0,0.08); }
      .bar-chart { margin-top: 10px; }
      .bar-row { display: flex; align-items: center; margin-bottom: 12px; }
      .bar-label { width: 100px; font-size: 11px; font-weight: 500;
                   color: #2C2416; }
      .bar-container { flex: 1; background: #F0E6DA; height: 28px;
                       border-radius: 4px; position: relative; overflow: hidden; }
      .bar-fill { background: #5D4037; height: 100%; border-radius: 4px;
                  display: flex; align-items: center; justify-content: flex-end;
                  padding-right: 8px; color: white; font-size: 12px;
                  font-weight: 700; }
      .summary-table { width: 100%; border-collapse: collapse; font-size: 11px; }
      .summary-table th { background: #5D4037; color: white; padding: 8px;
                          text-align: left; font-weight: 600; font-size: 10px; }
      .summary-table td { padding: 8px; border-bottom: 1px solid #E8DED3; }
      .summary-table tr:nth-child(even) td { background: #FAF7F4; }
      .detail-table { width: 100%; border-collapse: collapse; font-size: 10px;
                      background: white; border-radius: 8px; overflow: hidden;
                      box-shadow: 0 2px 5px rgba(0,0,0,0.08); }
      .detail-table th { background: #5D4037; color: white; padding: 10px 8px;
                         text-align: left; font-weight: 600; text-transform: uppercase;
                         font-size: 10px; }
      .detail-table td { padding: 8px; border-bottom: 1px solid #E8DED3; }
      .detail-table tr:nth-child(even) td { background: #FAF7F4; }
      .detail-table tr:last-child td { border-bottom: none; }
      .icon-leather { color: #8D6E63; font-size: 14px; margin-right: 4px; }
      .badge { display: inline-block; padding: 3px 10px; border-radius: 10px;
               font-size: 9px; font-weight: 700; }
      .badge-critique { background: #FDECEA; color: #C0392B; }
      .badge-normal   { background: #FEF5E7; color: #E67E22; }
      .badge-ok       { background: #E8F5E9; color: #2E7D32; }
      .footer { font-size: 10px; color: #666; margin-top: 25px;
                padding: 12px 0; border-top: 2px solid #E8DED3;
                display: flex; justify-content: space-between; }
      .footer-right { font-weight: 600; }
    </style></head><body>
    )";
    // En-téte avec logo
    html += R"(
    <div class="header">
      <div class="logo">CUIREA</div>
      <div class="header-title">RAPPORT DES MATIERES PREMIERES</div>
    </div>
    )";
    html += QString(R"(
    <div class="subheader">Genere le %1</div>
    )").arg(QDateTime::currentDateTime().toString("dd/MM/yyyy 'pm' HH:mm"));
    html += R"(<div class="content">)";
    //Statistiques générales
    html += R"(<div class="section-title">STATISTIQUES GENERALES</div>)";
    html += QString(R"(
    <div class="cards">
      <div class="card card-total">
        <div class="card-icon">📦</div>
        <div class="card-title">Total<br>Matières</div>
        <div class="card-value">%1</div>
        <div class="card-label">Actives</div>
      </div>
      <div class="card card-critique">
        <div class="card-icon">⚠️</div>
        <div class="card-title">Stock<br>Critique</div>
        <div class="card-value">%2</div>
        <div class="card-label">Items à réapprovisionner</div>
      </div>
      <div class="card card-expires">
        <div class="card-icon">❌</div>
        <div class="card-title">Expirés</div>
        <div class="card-value">%3</div>
        <div class="card-label">Expirés</div>
      </div>
      <div class="card card-proche">
        <div class="card-icon">⏰</div>
        <div class="card-title">< 30 Jours</div>
        <div class="card-value">%4</div>
        <div class="card-label">A surveiller</div>
      </div>
      <div class="card card-ok">
        <div class="card-icon">✅</div>
        <div class="card-title">Stock<br>OK</div>
        <div class="card-value">%5</div>
        <div class="card-label">Stock OK</div>
      </div>
    </div>
    )").arg(total).arg(critique).arg(expires).arg(proche30j).arg(stockOK);
    // Répartition par type (graphique + tableau)
    html += R"(<div class="section-title">REPARTITION PAR TYPE</div>)";
    html += R"(<div class="charts-row">)";
    // Graphique à barres
    html += R"(<div class="chart-section"><div class="bar-chart">)";
    int maxType = 0;
    for (auto it = parType.begin(); it != parType.end(); ++it)
        if (it.value() > maxType) maxType = it.value();
    for (auto it = parType.begin(); it != parType.end(); ++it) {
        int width = maxType > 0 ? (it.value() * 100 / maxType) : 0;
        html += QString(R"(
          <div class="bar-row">
            <div class="bar-label">%1</div>
            <div class="bar-container">
              <div class="bar-fill" style="width: %2%%;">%3</div>
            </div>
          </div>
        )").arg(it.key()).arg(width).arg(it.value());
    }
    html += R"(</div></div>)";
    // Tableau récapitulatif
    html += R"(
      <div class="chart-section">
        <table class="summary-table">
          <tr><th>Type</th><th>Nombre de matieres</th></tr>
    )";
    for (auto it = parType.begin(); it != parType.end(); ++it) {
        html += QString("<tr><td>%1</td><td>%2</td></tr>")
            .arg(it.key()).arg(it.value());
    }
    html += R"(
        </table>
      </div>
    </div>
    )";
    //Liste détaillée
    html += R"(<div class="section-title">LISTE DETAILLEE DES MATIERES</div>)";
    html += R"(
    <table class="detail-table">
      <tr>
        <th>Module</th><th>Référence</th><th>Type</th>
        <th>Quantité Stock</th><th>Qté Demandée</th><th>Seuil</th><th>Expiration</th><th>Statut</th>
      </tr>
    )";
    
    for (int r = 0; r < total; ++r) {
        // Extraire la quantité stock proprement
        QString qtyText = cellText(ui->matiereTable, r, 3);
        qtyText = qtyText.remove(" m²").remove(" mé").remove("m²").remove("mé").trimmed();
        double qty = qtyText.toDouble();
        
        int seuil = cellText(ui->matiereTable, r, 4).toInt();
        
        // Calculer la quantité demandée automatiquement
        // Si stock < seuil, il faut commander la différence
        double qteDemandee = 0.0;
        if (qty < seuil) {
            qteDemandee = seuil - qty;
        }
        
        QString badge, label;
        if      (qty < seuil * 0.5) { badge = "badge-critique"; label = "Critique"; }
        else if (qty < seuil)       { badge = "badge-normal";   label = "Normal";   }
        else                        { badge = "badge-ok";       label = "OK";       }
        
        // Couleur selon l'urgence
        QString couleurDemande = "#999999"; // Gris par défaut
        if (qteDemandee > 0) {
            if (qty < seuil * 0.5) {
                couleurDemande = "#D32F2F"; // Rouge - Urgent
            } else {
                couleurDemande = "#E67E22"; // Orange - À commander
            }
        }
        
        html += QString(R"(
        <tr>
          <td><span class="icon-leather">🔶</span> %1</td>
          <td><strong>%2</strong></td>
          <td>%3</td>
          <td><strong>%4</strong> m²</td>
          <td style="color: %5; font-weight: bold;">%6 m²</td>
          <td>%7</td>
          <td>%8</td>
          <td><span class="badge %9">%10</span></td>
        </tr>
        )").arg(cellText(ui->matiereTable, r, 0))
           .arg(cellText(ui->matiereTable, r, 1))
           .arg(cellText(ui->matiereTable, r, 2))
           .arg(QString::number(qty, 'f', 2))
           .arg(couleurDemande)
           .arg(QString::number(qteDemandee, 'f', 2))
           .arg(cellText(ui->matiereTable, r, 4))
           .arg(cellText(ui->matiereTable, r, 5))
           .arg(badge).arg(label);
    }
    html += R"(</table>)";
    //Footer
    html += R"(
    <div class="footer">
      <div>CUIREA Management System  Rapport genere automatiquement</div>
      <div class="footer-right">1 / 1</div>
    </div>
    )";
    html += R"(</div></body></html>)";
    //Génération PDF
    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setPageOrientation(QPageLayout::Portrait);
    printer.setPageMargins(QMarginsF(0, 0, 0, 0), QPageLayout::Millimeter);
    QTextDocument doc;
    doc.setHtml(html);
    doc.setPageSize(printer.pageRect(QPrinter::DevicePixel).size());
    doc.print(&printer);
    QMessageBox::information(this, "Succes",
        QString("PDF exporte avec succes !\n\n%1").arg(fileName));
}
void MainWindow::onStatistiquesMatiere()
{
    QDialog dlg(this);
    dlg.setWindowTitle("Statistiques - Matières Premières");
    dlg.setMinimumSize(950, 650);
    dlg.setStyleSheet("QDialog { background-color: #F5F0EB; }");
    QVBoxLayout mainLay(&dlg);
    mainLay.setContentsMargins(25, 25, 25, 25);
    mainLay.setSpacing(20);
    // Titre
    auto *title = new QLabel("📊 STATISTIQUES DES MATIÈRES PREMIÈRES");
    title->setStyleSheet("font-size: 18px; font-weight: bold; color: #6D4C41; padding: 8px;");
    title->setAlignment(Qt::AlignCenter);
    mainLay.addWidget(title);
    // Calcul des stats
    int total = ui->matiereTable->rowCount();
    int critique = 0, normal = 0, eleve = 0, expires = 0, proche30j = 0;
    QMap<QString, int> parType;
    for (int r = 0; r < total; ++r) {
        double qty = cellText(ui->matiereTable, r, 3).remove(" m²").toDouble();
        int seuil = cellText(ui->matiereTable, r, 4).toInt();
        QString type = cellText(ui->matiereTable, r, 2);
        if (qty < seuil * 0.5) critique++;
        else if (qty < seuil) normal++;
        else eleve++;
        parType[type]++;
        QString dateStr = cellText(ui->matiereTable, r, 5);
        QDate expDate = QDate::fromString(dateStr, "yyyy-MM-dd");
        int daysLeft = QDate::currentDate().daysTo(expDate);
        if (daysLeft < 0) expires++;
        else if (daysLeft < 30) proche30j++;
    }
    // Cards en haut (4 cartes seulement)
    QHBoxLayout *cardsLay = new QHBoxLayout();
    cardsLay->setSpacing(10);
    auto createCard = [](const QString &label, const QString &value, const QString &color, const QString &borderColor) {
        QFrame *card = new QFrame();
        card->setStyleSheet(QString(
            "QFrame { background: white; border: 2px solid %1; border-radius: 10px; }").arg(borderColor));
        card->setFixedHeight(100);
        card->setMinimumWidth(150);
        QVBoxLayout *lay = new QVBoxLayout(card);
        lay->setSpacing(4);
        lay->setContentsMargins(10, 12, 10, 12);
        auto *lbl = new QLabel(label);
        lbl->setStyleSheet("font-size: 10px; color: #888; font-weight: 600;");
        lbl->setAlignment(Qt::AlignCenter);
        lbl->setWordWrap(false);
        auto *val = new QLabel(value);
        val->setStyleSheet(QString("font-size: 36px; font-weight: bold; color: %1; margin-top: 4px;").arg(color));
        val->setAlignment(Qt::AlignCenter);
        lay->addWidget(lbl);
        lay->addWidget(val);
        lay->addStretch();
        return card;
    };
    cardsLay->addWidget(createCard("Total Matières", QString::number(total), "#2C2416", "#E0D5CC"));
    cardsLay->addWidget(createCard("Stock Critique", QString::number(critique), "#C0392B", "#FDECEA"));
    cardsLay->addWidget(createCard("Expirés", QString::number(expires), "#8B4513", "#FEF5E7"));
    cardsLay->addWidget(createCard("< 30 jours", QString::number(proche30j), "#D4A574", "#FFF8E1"));
    mainLay.addLayout(cardsLay);
    // Graphiques
    QHBoxLayout *chartsLay = new QHBoxLayout();
    chartsLay->setSpacing(20);
    // Donut Chart - Répartition stock (CRITICITÉ) avec pourcentages
    auto *pieSeries = new QPieSeries();
    pieSeries->setHoleSize(0.5);  // Donut chart
    
    // Calculer les pourcentages
    double totalCount = critique + eleve + normal;
    if (totalCount == 0) totalCount = 1; // Éviter division par zéro
    
    auto *sliceCrit = pieSeries->append("Critique", critique);
    sliceCrit->setBrush(QColor("#B33A3A"));
    sliceCrit->setLabelVisible(true);
    sliceCrit->setLabel(QString("Critique\n%1%").arg(QString::number(critique * 100.0 / totalCount, 'f', 1)));
    sliceCrit->setLabelColor(QColor("#B33A3A"));
    sliceCrit->setLabelFont(QFont("Arial", 9, QFont::Bold));
    
    auto *sliceElev = pieSeries->append("Élevé", eleve);
    sliceElev->setBrush(QColor("#E67E22"));
    sliceElev->setLabelVisible(true);
    sliceElev->setLabel(QString("Élevé\n%1%").arg(QString::number(eleve * 100.0 / totalCount, 'f', 1)));
    sliceElev->setLabelColor(QColor("#E67E22"));
    sliceElev->setLabelFont(QFont("Arial", 9, QFont::Bold));
    
    auto *sliceNorm = pieSeries->append("Normal", normal);
    sliceNorm->setBrush(QColor("#95A472"));
    sliceNorm->setLabelVisible(true);
    sliceNorm->setLabel(QString("Normal\n%1%").arg(QString::number(normal * 100.0 / totalCount, 'f', 1)));
    sliceNorm->setLabelColor(QColor("#95A472"));
    sliceNorm->setLabelFont(QFont("Arial", 9, QFont::Bold));

    auto *pieChart = new QChart();
    pieChart->addSeries(pieSeries);
    pieChart->setTitle("Répartition des Stocks (CRITICITÉ)");
    pieChart->setTitleFont(QFont("Arial", 12, QFont::Bold));
    pieChart->setBackgroundBrush(QColor("#FFFFFF"));
    pieChart->setBackgroundRoundness(10);
    
    // Légende personnalisée
    pieChart->legend()->setVisible(true);
    pieChart->legend()->setAlignment(Qt::AlignRight);
    pieChart->legend()->setFont(QFont("Arial", 10));
    
    // Ajouter le nombre total au centre
    auto *centerLabel = new QLabel(QString::number(total));
    centerLabel->setStyleSheet("font-size: 42px; font-weight: bold; color: #6D4C41;");
    centerLabel->setAlignment(Qt::AlignCenter);
    
    auto *pieView = new QChartView(pieChart);
    pieView->setRenderHint(QPainter::Antialiasing);
    pieView->setMinimumHeight(280);
    pieView->setStyleSheet("background: white; border-radius: 10px;");
    chartsLay->addWidget(pieView);

    // -- Bar chart - Par type ----------------------------------
    auto *barSet = new QBarSet("Quantité");
    barSet->setColor(QColor("#6D4C41"));
    QStringList categories;
    for (auto it = parType.begin(); it != parType.end(); ++it) {
        *barSet << it.value();
        categories << it.key();
    }
    
    auto *barSeries = new QBarSeries();
    barSeries->append(barSet);
    
    auto *barChart = new QChart();
    barChart->addSeries(barSeries);
    barChart->setTitle("Répartition par Type");
    barChart->setTitleFont(QFont("Arial", 12, QFont::Bold));
    barChart->legend()->setVisible(false);
    barChart->setBackgroundBrush(QColor("#FFFFFF"));
    barChart->setBackgroundRoundness(10);
    
    auto *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    axisX->setLabelsFont(QFont("Arial", 9));
    barChart->addAxis(axisX, Qt::AlignBottom);
    barSeries->attachAxis(axisX);
    
    auto *axisY = new QValueAxis();
    axisY->setLabelFormat("%d");
    axisY->setLabelsFont(QFont("Arial", 9));
    barChart->addAxis(axisY, Qt::AlignLeft);
    barSeries->attachAxis(axisY);
    
    auto *barView = new QChartView(barChart);
    barView->setRenderHint(QPainter::Antialiasing);
    barView->setMinimumHeight(280);
    barView->setStyleSheet("background: white; border-radius: 10px;");
    chartsLay->addWidget(barView);

    mainLay.addLayout(chartsLay);

    // Boutons
    QHBoxLayout *btnLay = new QHBoxLayout();
    auto *btnExport = new QPushButton("?? Exporter PDF", &dlg);
    btnExport->setStyleSheet("QPushButton { background: #6D4C41; color: white; border: none; "
                             "border-radius: 8px; padding: 12px 24px; font-size: 12px; font-weight: bold; }"
                             "QPushButton:hover { background: #8D6E63; }");
    connect(btnExport, &QPushButton::clicked, [this]() { onExportMatiere(); });
    
    auto *btnClose = new QPushButton("Fermer", &dlg);
    btnClose->setStyleSheet("QPushButton { background: #E7DDD1; color: #291C0E; border: 1px solid #BCAAA4; "
                            "border-radius: 8px; padding: 12px 24px; font-size: 12px; font-weight: bold; }"
                            "QPushButton:hover { background: #F0E6DA; }");
    connect(btnClose, &QPushButton::clicked, &dlg, &QDialog::accept);
    
    btnLay->addStretch();
    btnLay->addWidget(btnExport);
    btnLay->addWidget(btnClose);
    mainLay.addLayout(btnLay);

    dlg.exec();
}

void MainWindow::onTriMatiere()
{
    QStringList options;
    options << "Tri par Module (A-Z)" << "Tri par Module (Z-A)"
            << "Tri par Type (A-Z)" << "Tri par Type (Z-A)"
            << "Tri par Quantité (Croissant)" << "Tri par Quantité (Décroissant)"
            << "Tri par Seuil (Croissant)" << "Tri par Seuil (Décroissant)"
            << "Tri par Date d'expiration (Plus proche)" << "Tri par Date d'expiration (Plus lointaine)";
    
    bool ok;
    QString choice = QInputDialog::getItem(this, "Tri des Matières", 
                                          "Choisissez le critère de tri:", 
                                          options, 0, false, &ok);
    
    if (!ok || choice.isEmpty()) return;
    
    // Structure pour stocker ligne compléte avec données UserRole
    struct RowData {
        QStringList texts;
        QVariant photoUrl;
        int originalId;
    };
    
    // Collecter toutes les données
    QList<RowData> rows;
    for (int r = 0; r < ui->matiereTable->rowCount(); ++r) {
        RowData row;
        for (int c = 0; c < ui->matiereTable->columnCount(); ++c) {
            row.texts << (ui->matiereTable->item(r, c) ? ui->matiereTable->item(r, c)->text() : "");
        }
        row.photoUrl = ui->matiereTable->item(r, 0) ? ui->matiereTable->item(r, 0)->data(Qt::UserRole + 1) : QVariant();
        row.originalId = ui->matiereTable->item(r, 0) ? ui->matiereTable->item(r, 0)->data(Qt::UserRole).toInt() : 0;
        rows.append(row);
    }
    
    // Trier selon le critére
    if (choice.contains("Module")) {
        bool desc = choice.contains("Z-A");
        std::sort(rows.begin(), rows.end(), [desc](const RowData &a, const RowData &b) {
            return desc ? a.texts[0] > b.texts[0] : a.texts[0] < b.texts[0];
        });
    } else if (choice.contains("Type")) {
        bool desc = choice.contains("Z-A");
        std::sort(rows.begin(), rows.end(), [desc](const RowData &a, const RowData &b) {
            return desc ? a.texts[2] > b.texts[2] : a.texts[2] < b.texts[2];
        });
    } else if (choice.contains("Quantité")) {
        bool desc = choice.contains("Décroissant");
        std::sort(rows.begin(), rows.end(), [desc](const RowData &a, const RowData &b) {
            double qtyA = QString(a.texts[3]).remove(" m²").toDouble();
            double qtyB = QString(b.texts[3]).remove(" m²").toDouble();
            return desc ? qtyA > qtyB : qtyA < qtyB;
        });
    } else if (choice.contains("Seuil")) {
        bool desc = choice.contains("Décroissant");
        std::sort(rows.begin(), rows.end(), [desc](const RowData &a, const RowData &b) {
            double seuilA = a.texts[4].toDouble();
            double seuilB = b.texts[4].toDouble();
            return desc ? seuilA > seuilB : seuilA < seuilB;
        });
    } else if (choice.contains("Date")) {
        bool desc = choice.contains("lointaine");
        std::sort(rows.begin(), rows.end(), [desc](const RowData &a, const RowData &b) {
            QDate dateA = QDate::fromString(a.texts[5], "yyyy-MM-dd");
            QDate dateB = QDate::fromString(b.texts[5], "yyyy-MM-dd");
            if (!dateA.isValid()) return !desc;
            if (!dateB.isValid()) return desc;
            return desc ? dateA > dateB : dateA < dateB;
        });
    }
    
    // Réappliquer les données triées avec UserRole
    for (int r = 0; r < rows.size(); ++r) {
        for (int c = 0; c < rows[r].texts.size(); ++c) {
            if (ui->matiereTable->item(r, c)) {
                ui->matiereTable->item(r, c)->setText(rows[r].texts[c]);
                if (c == 0) {
                    ui->matiereTable->item(r, c)->setData(Qt::UserRole, rows[r].originalId);
                    ui->matiereTable->item(r, c)->setData(Qt::UserRole + 1, rows[r].photoUrl);
                }
            }
        }
    }
    
    QMessageBox::information(this, "Tri effectué", 
                           QString("Les matières ont été triées par: %1").arg(choice));
}

void MainWindow::onRechercheTriMatiere()
{
    QDialog dlg(this);
    dlg.setWindowTitle("Recherche Avancée");
    dlg.setMinimumWidth(500);
    dlg.setStyleSheet(DIALOG_STYLE);

    QVBoxLayout lay(&dlg);
    lay.setSpacing(12);
    lay.setContentsMargins(20, 20, 20, 20);

    auto *title = new QLabel("?? Recherche de Matières Premiéres");
    title->setStyleSheet("font-size: 16px; font-weight: bold; color: #8D6E63;");
    title->setAlignment(Qt::AlignCenter);

    QFormLayout form;
    
    auto *txtNom = new QLineEdit(&dlg);
    txtNom->setPlaceholderText("Ex: cuir, peau...");
    
    auto *txtRef = new QLineEdit(&dlg);
    txtRef->setPlaceholderText("Ex: dd, aaaa...");
    
    auto *cmbType = new QComboBox(&dlg);
    cmbType->addItems({"Tous", "Cuir", "Peau de Veau", "Ficelinée", "Quincaillerie"});
    
    auto *cmbStock = new QComboBox(&dlg);
    cmbStock->addItems({"Tous", "Critique", "Normal", "élevé"});
    
    auto *txtSeuil = new QLineEdit(&dlg);
    txtSeuil->setPlaceholderText("Ex: 100, >50, <200");
    
    auto *cmbPeremption = new QComboBox(&dlg);
    cmbPeremption->addItems({"Tous", "Expiré", "< 30 jours", "< 90 jours", "> 90 jours"});
    
    auto *cmbPhoto = new QComboBox(&dlg);
    cmbPhoto->addItems({"Tous", "Avec photo", "Sans photo"});

    form.addRow("Nom / Module :", txtNom);
    form.addRow("Référence :", txtRef);
    form.addRow("Type :", cmbType);
    form.addRow("Niveau Stock :", cmbStock);
    form.addRow("Seuil :", txtSeuil);
    form.addRow("Péremption :", cmbPeremption);
    form.addRow("Photo :", cmbPhoto);

    QDialogButtonBox btns(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    connect(&btns, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(&btns, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    lay.addWidget(title);
    lay.addLayout(&form);
    lay.addWidget(&btns);

    if (dlg.exec() != QDialog::Accepted) return;

    // Appliquer les filtres
    QString nomFilter = txtNom->text().trimmed().toLower();
    QString refFilter = txtRef->text().trimmed().toLower();
    QString typeFilter = cmbType->currentText();
    QString stockFilter = cmbStock->currentText();
    QString seuilFilter = txtSeuil->text().trimmed();
    QString peremptionFilter = cmbPeremption->currentText();
    QString photoFilter = cmbPhoto->currentText();

    for (int r = 0; r < ui->matiereTable->rowCount(); ++r) {
        bool match = true;

        // Filtre nom
        if (!nomFilter.isEmpty()) {
            QString nom = cellText(ui->matiereTable, r, 0).toLower();
            if (!nom.contains(nomFilter)) match = false;
        }

        // Filtre référence
        if (!refFilter.isEmpty() && match) {
            QString ref = cellText(ui->matiereTable, r, 1).toLower();
            if (!ref.contains(refFilter)) match = false;
        }

        // Filtre type
        if (typeFilter != "Tous" && match) {
            QString type = cellText(ui->matiereTable, r, 2);
            if (type != typeFilter) match = false;
        }

        // Filtre stock
        if (stockFilter != "Tous" && match) {
            int qty = cellText(ui->matiereTable, r, 3).remove(" mé").toDouble();
            int seuil = cellText(ui->matiereTable, r, 4).toInt();
            if (stockFilter == "Critique" && qty >= seuil) match = false;
            if (stockFilter == "Normal" && (qty < seuil || qty > seuil * 2)) match = false;
            if (stockFilter == "élevé" && qty <= seuil * 2) match = false;
        }

        // Filtre seuil
        if (!seuilFilter.isEmpty() && match) {
            int seuil = cellText(ui->matiereTable, r, 4).toInt();
            if (seuilFilter.startsWith(">")) {
                int val = seuilFilter.mid(1).toInt();
                if (seuil <= val) match = false;
            } else if (seuilFilter.startsWith("<")) {
                int val = seuilFilter.mid(1).toInt();
                if (seuil >= val) match = false;
            } else {
                if (seuil != seuilFilter.toInt()) match = false;
            }
        }

        // Filtre péremption
        if (peremptionFilter != "Tous" && match) {
            QString dateStr = cellText(ui->matiereTable, r, 5);
            QDate expDate = QDate::fromString(dateStr, "yyyy-MM-dd");
            int daysLeft = QDate::currentDate().daysTo(expDate);
            
            if (peremptionFilter == "Expiré" && daysLeft >= 0) match = false;
            if (peremptionFilter == "< 30 jours" && (daysLeft < 0 || daysLeft >= 30)) match = false;
            if (peremptionFilter == "< 90 jours" && (daysLeft < 0 || daysLeft >= 90)) match = false;
            if (peremptionFilter == "> 90 jours" && daysLeft < 90) match = false;
        }

        // Filtre photo
        if (photoFilter != "Tous" && match) {
            auto *photoItem = ui->matiereTable->item(r, 6);
            bool hasPhoto = photoItem && !photoItem->icon().isNull();
            if (photoFilter == "Avec photo" && !hasPhoto) match = false;
            if (photoFilter == "Sans photo" && hasPhoto) match = false;
        }

        ui->matiereTable->setRowHidden(r, !match);
    }

    int visibleCount = 0;
    for (int r = 0; r < ui->matiereTable->rowCount(); ++r)
        if (!ui->matiereTable->isRowHidden(r)) visibleCount++;

    QMainWindow::statusBar()->showMessage(
        QString("✓ %1 matière(s) trouvée(s)").arg(visibleCount), 5000);
}

void MainWindow::onDetectionDefauts()
{
    matiereDetection->startDetection();
}

void MainWindow::updateMatiereStatistics()
{
    int total = ui->matiereTable->rowCount(), critical = 0;
    for (int r = 0; r < total; ++r) {
        double quantite = cellText(ui->matiereTable, r, 3).split(" ").first().toDouble();
        int seuil = cellText(ui->matiereTable, r, 4).toInt();
        if (quantite < seuil) ++critical; // Stock critique si quantité < seuil
    }
    ui->statsValueMatiere1->setText(QString::number(total));
    ui->statsValueMatiere2->setText(QString::number(critical));
}

// -- Suppliers -----------------------------------------------------------------
void MainWindow::setupFournisseurTable()
{
    // Configurer le tableau avec 11 colonnes
    ui->fournisseurTable->setColumnCount(11);
    ui->fournisseurTable->setHorizontalHeaderLabels({
        "ID", "Nom Entreprise", "Email", "Téléphone", "Matricule Fiscal",
        "Type Produit", "Condition Paiement", "Statut", "Adresse",
        "Qté Commandée (kg)", "Qté Mesurée (kg)"
    });
    
    ui->fournisseurTable->setColumnHidden(0, true);  // Cacher ID
    ui->fournisseurTable->setColumnHidden(8, true);  // Cacher Adresse
    
    // Ajuster les largeurs de colonnes
    ui->fournisseurTable->setColumnWidth(1, 150);  // Nom
    ui->fournisseurTable->setColumnWidth(2, 180);  // Email
    ui->fournisseurTable->setColumnWidth(3, 100);  // Téléphone
    ui->fournisseurTable->setColumnWidth(4, 120);  // Matricule
    ui->fournisseurTable->setColumnWidth(5, 120);  // Type
    ui->fournisseurTable->setColumnWidth(6, 130);  // Condition
    ui->fournisseurTable->setColumnWidth(7, 80);   // Statut
    ui->fournisseurTable->setColumnWidth(9, 130);  // Qté Commandée
    ui->fournisseurTable->setColumnWidth(10, 120); // Qté Mesurée
    
    refreshFournisseurTable();
    updateFournisseurStatistics();
    
    // Créer le bouton Livraison dynamiquement s'il n'existe pas dans l'UI
    if (!findChild<QPushButton*>("btnLivraisonFournisseur")) {
        QPushButton *btnLivraison = new QPushButton("⚖️ Livraison", this);
        btnLivraison->setObjectName("btnLivraisonFournisseur");
        btnLivraison->setStyleSheet(
            "QPushButton { background-color: #2E7D32; color: white; border: none; "
            "border-radius: 8px; padding: 10px 20px; font-size: 12px; font-weight: bold; }"
            "QPushButton:hover { background-color: #388E3C; }"
            "QPushButton:pressed { background-color: #1B5E20; }"
        );
        
        // Trouver le layout des boutons fournisseurs et ajouter le bouton
        QWidget *fournisseurPage = ui->stackedWidget->widget(3); // Page Fournisseurs
        if (fournisseurPage) {
            QList<QHBoxLayout*> layouts = fournisseurPage->findChildren<QHBoxLayout*>();
            for (QHBoxLayout *layout : layouts) {
                // Chercher le layout qui contient les boutons d'action
                for (int i = 0; i < layout->count(); ++i) {
                    QWidget *widget = layout->itemAt(i)->widget();
                    if (widget && widget->objectName().contains("btn") && 
                        widget->objectName().contains("Fournisseur")) {
                        // Insérer après le bouton Supprimer
                        layout->insertWidget(3, btnLivraison);
                        connect(btnLivraison, &QPushButton::clicked, 
                                this, &MainWindow::on_btnLivraisonFournisseur_clicked);
                        qDebug() << "✅ Bouton Livraison ajouté dynamiquement";
                        goto button_added;
                    }
                }
            }
        }
        button_added:;
    }
}

void MainWindow::refreshFournisseurTable()
{
    // Charger depuis la base de données
    FournisseurData f;
    QSqlQueryModel* model = f.afficher();
    
    if (!model) {
        return;
    }
    
    // Charger depuis le modèle BD
    int n = model->rowCount();
    ui->fournisseurTable->setRowCount(n);
    
    for (int i = 0; i < n; ++i) {
        for (int col = 0; col < 11; ++col) {  // 11 colonnes maintenant (ajout qté commandée + mesurée)
            QString value = model->data(model->index(i, col)).toString();
            QTableWidgetItem *item = new QTableWidgetItem(value);
            
            // Coloration pour les quantités
            if (col == 10 && col == 9) {  // Qté Mesurée
                double qteMesuree = value.toDouble();
                double qteCommandee = model->data(model->index(i, 9)).toDouble();
                
                if (qteMesuree > 0 && qteCommandee > 0) {
                    double diff = qAbs(qteMesuree - qteCommandee) / qteCommandee * 100.0;
                    if (diff <= 5.0) {
                        item->setForeground(QColor("#2E7D32"));  // Vert si conforme
                    } else {
                        item->setForeground(QColor("#D32F2F"));  // Rouge si non conforme
                    }
                }
            }
            
            ui->fournisseurTable->setItem(i, col, item);
        }
    }
    
    delete model;
    updateFournisseurStatistics();
}

void MainWindow::updateFournisseurStatistics()
{
    int total = ui->fournisseurTable->rowCount();
    int actifs = 0;
    QSet<QString> types;
    
    for (int i = 0; i < total; ++i) {
        QString statut = ui->fournisseurTable->item(i, 7) ? ui->fournisseurTable->item(i, 7)->text() : "";
        QString type = ui->fournisseurTable->item(i, 5) ? ui->fournisseurTable->item(i, 5)->text() : "";
        
        if (statut == "Actif") ++actifs;
        if (!type.isEmpty()) types.insert(type);
    }
    
    ui->statsValueFournisseur1->setText(QString::number(total));
    ui->statsValueFournisseur2->setText(QString::number(actifs));
    ui->statsValueFournisseur3->setText(QString::number(types.size()));
}

void MainWindow::on_btnAddFournisseur_clicked()
{
    FournisseurDialog dlg(this, FournisseurDialog::AddMode);
    if (dlg.exec() == QDialog::Accepted) {
        FournisseurData f;
        
        // Toujours sauvegarder dans la BD
        f.setNomEntreprise(dlg.getNomEntreprise());
        f.setEmail(dlg.getEmail());
        f.setTelephone(dlg.getTelephone());
        f.setMatriculeFiscal(dlg.getMatriculeFiscal());
        f.setTypeProduit(dlg.getTypeProduit());
        f.setConditionPaiement(dlg.getConditionPaiement());
        f.setStatut(dlg.getStatut());
        f.setAdresse(dlg.getAdresse());
        
        if (f.ajouter()) {
            refreshFournisseurTable();
            NotificationWidget::show("Fournisseur ajoute", "Nouveau fournisseur enregistre.", NotificationWidget::Success);
            QMessageBox::information(this, "Succes", "Fournisseur ajoute avec succes dans la base de donnees!");
        } else {
            QMessageBox::critical(this, "Erreur", 
                "Impossible d'ajouter le fournisseur.\n"
                "Vérifiez que la table FOURNISSEURS existe dans la base de données.");
        }
    }
}

void MainWindow::on_btnEditFournisseur_clicked()
{
    int row = ui->fournisseurTable->currentRow();
    if (row < 0) { QMessageBox::warning(this,"","Veuillez sélectionner un fournisseur à modifier."); return; }
    
    FournisseurDialog dlg(this, FournisseurDialog::EditMode);
    QString id = ui->fournisseurTable->item(row, 0) ? ui->fournisseurTable->item(row, 0)->text() : "";
    QString nomEntreprise = ui->fournisseurTable->item(row, 1) ? ui->fournisseurTable->item(row, 1)->text() : "";
    QString email = ui->fournisseurTable->item(row, 2) ? ui->fournisseurTable->item(row, 2)->text() : "";
    QString telephone = ui->fournisseurTable->item(row, 3) ? ui->fournisseurTable->item(row, 3)->text() : "";
    QString typeProduit = ui->fournisseurTable->item(row, 5) ? ui->fournisseurTable->item(row, 5)->text() : "";
    QString conditionPaiement = ui->fournisseurTable->item(row, 6) ? ui->fournisseurTable->item(row, 6)->text() : "";
    QString matriculeFiscal = ui->fournisseurTable->item(row, 4) ? ui->fournisseurTable->item(row, 4)->text() : "";
    QString statut = ui->fournisseurTable->item(row, 7) ? ui->fournisseurTable->item(row, 7)->text() : "";
    QString adresse = ui->fournisseurTable->item(row, 8) ? ui->fournisseurTable->item(row, 8)->text() : "";
    
    dlg.setFournisseurData(id, nomEntreprise, email, telephone, typeProduit, conditionPaiement, matriculeFiscal, statut, adresse);
    
    if (dlg.exec() == QDialog::Accepted) {
        FournisseurData f;
        
        // Toujours sauvegarder dans la BD
        f.setId(id);
        f.setNomEntreprise(dlg.getNomEntreprise());
        f.setEmail(dlg.getEmail());
        f.setTelephone(dlg.getTelephone());
        f.setMatriculeFiscal(dlg.getMatriculeFiscal());
        f.setTypeProduit(dlg.getTypeProduit());
        f.setConditionPaiement(dlg.getConditionPaiement());
        f.setStatut(dlg.getStatut());
        f.setAdresse(dlg.getAdresse());
        
        if (f.modifier()) {
            refreshFournisseurTable();
            QMessageBox::information(this, "Succès", "Fournisseur modifié avec succès!");
        } else {
            QMessageBox::critical(this, "Erreur", 
                "Impossible de modifier le fournisseur.\n"
                "Vérifiez que la table FOURNISSEURS existe dans la base de données.");
        }
    }
}

void MainWindow::on_btnDeleteFournisseur_clicked()
{
    int row = ui->fournisseurTable->currentRow();
    if (row < 0) { QMessageBox::warning(this,"","Veuillez sélectionner un fournisseur à supprimer."); return; }
    
    FournisseurDialog dlg(this, FournisseurDialog::DeleteMode);
    QString id = ui->fournisseurTable->item(row, 0) ? ui->fournisseurTable->item(row, 0)->text() : "";
    QString nomEntreprise = ui->fournisseurTable->item(row, 1) ? ui->fournisseurTable->item(row, 1)->text() : "";
    QString email = ui->fournisseurTable->item(row, 2) ? ui->fournisseurTable->item(row, 2)->text() : "";
    QString telephone = ui->fournisseurTable->item(row, 3) ? ui->fournisseurTable->item(row, 3)->text() : "";
    QString typeProduit = ui->fournisseurTable->item(row, 5) ? ui->fournisseurTable->item(row, 5)->text() : "";
    QString conditionPaiement = ui->fournisseurTable->item(row, 6) ? ui->fournisseurTable->item(row, 6)->text() : "";
    QString matriculeFiscal = ui->fournisseurTable->item(row, 4) ? ui->fournisseurTable->item(row, 4)->text() : "";
    QString statut = ui->fournisseurTable->item(row, 7) ? ui->fournisseurTable->item(row, 7)->text() : "";
    
    dlg.setFournisseurData(id, nomEntreprise, email, telephone, typeProduit, conditionPaiement, matriculeFiscal, statut);
    
    if (dlg.exec() == QDialog::Accepted) {
        FournisseurData f;
        
        // Toujours supprimer de la BD
        if (f.supprimer(id)) {
            refreshFournisseurTable();
            QMessageBox::information(this, "Succès", "Fournisseur supprimé avec succès!");
        } else {
            QMessageBox::critical(this, "Erreur", 
                QString("Impossible de supprimer le fournisseur.\n"
                       "ID: %1\n"
                       "Vérifiez que la table FOURNISSEURS existe dans la base de données.").arg(id));
        }
    }
}

void MainWindow::on_btnLivraisonFournisseur_clicked()
{
    int row = ui->fournisseurTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Sélection", "Veuillez sélectionner un fournisseur pour valider une livraison.");
        return;
    }
    
    QString idFournisseur = cellText(ui->fournisseurTable, row, 0);
    QString nomEntreprise = cellText(ui->fournisseurTable, row, 1);
    
    // Ouvrir le dialog de vérification avec la carte TEMP_BALANCE (balance HX711)
    Arduino *arduinoBalance = m_arduinoTempBalance ? m_arduinoTempBalance : nullptr;
    VerificationLivraison dialog(nomEntreprise, arduinoBalance, this);
    if (dialog.exec() == QDialog::Accepted) {
        // Récupérer les valeurs mesurées
        double qteCommandee = dialog.getQuantiteCommandee();
        double qteMesuree = dialog.getMeasuredWeight();
        bool isValid = dialog.isDeliveryValid();
        
        // Vérifier si les colonnes existent, sinon les créer
        QSqlQuery checkQuery(Connection::instance()->getDatabase());
        checkQuery.prepare("SELECT column_name FROM user_tab_columns "
                          "WHERE table_name = 'FOURNISSEURS' AND column_name = 'QTE_COMMANDEE'");
        
        if (checkQuery.exec() && !checkQuery.next()) {
            // Les colonnes n'existent pas, les créer
            QSqlQuery alterQuery(Connection::instance()->getDatabase());
            alterQuery.exec("ALTER TABLE FOURNISSEURS ADD QTE_COMMANDEE NUMBER(10,2) DEFAULT 0");
            alterQuery.exec("ALTER TABLE FOURNISSEURS ADD QTE_MESUREE NUMBER(10,2) DEFAULT 0");
            alterQuery.exec("ALTER TABLE FOURNISSEURS ADD DATE_DERNIERE_LIVRAISON DATE");
            qDebug() << "Colonnes de livraison créées dans FOURNISSEURS";
        }
        
        // Mettre à jour la base de données
        QSqlQuery query(Connection::instance()->getDatabase());
        query.prepare("UPDATE FOURNISSEURS SET QTE_COMMANDEE = :cmd, QTE_MESUREE = :mes, "
                     "DATE_DERNIERE_LIVRAISON = SYSDATE WHERE ID_FOURNISSEUR = :id");
        query.bindValue(":cmd", qteCommandee);
        query.bindValue(":mes", qteMesuree);
        query.bindValue(":id", idFournisseur.toInt());
        
        if (query.exec()) {
            // Rafraîchir le tableau
            refreshFournisseurTable();
            
            // Afficher un message de succès
            if (isValid) {
                QMessageBox::information(this, "Livraison validée",
                    QString("Livraison de %1 acceptée\n\n"
                           "Quantité commandée : %2 kg\n"
                           "Quantité mesurée : %3 kg\n"
                           "Statut : ✓ Conforme")
                    .arg(nomEntreprise)
                    .arg(qteCommandee, 0, 'f', 2)
                    .arg(qteMesuree, 0, 'f', 2));
            } else {
                QMessageBox::warning(this, "Livraison refusée",
                    QString("Livraison de %1 refusée\n\n"
                           "Quantité commandée : %2 kg\n"
                           "Quantité mesurée : %3 kg\n"
                           "Statut : ✗ Non conforme")
                    .arg(nomEntreprise)
                    .arg(qteCommandee, 0, 'f', 2)
                    .arg(qteMesuree, 0, 'f', 2));
            }
        } else {
            QMessageBox::critical(this, "Erreur", 
                "Impossible de mettre à jour la base de données:\n" + query.lastError().text());
        }
    }
}

void MainWindow::on_btnSmsFournisseur_clicked()
{
    int row = ui->fournisseurTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "", "Veuillez sélectionner un fournisseur pour envoyer un SMS.");
        return;

    }

    QString nomEntreprise = ui->fournisseurTable->item(row, 1) ? ui->fournisseurTable->item(row, 1)->text() : "";
    QString telephone     = ui->fournisseurTable->item(row, 3) ? ui->fournisseurTable->item(row, 3)->text() : "";

    SmsFournisseurDialog dlg(nomEntreprise, telephone, this);
    dlg.exec();
}

void MainWindow::on_btnQrFournisseur_clicked()
{
    int row = ui->fournisseurTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "", "Veuillez sélectionner un fournisseur pour générer le QR code.");
        return;
    }

    QString nomEntreprise  = ui->fournisseurTable->item(row, 1) ? ui->fournisseurTable->item(row, 1)->text() : "";
    QString email          = ui->fournisseurTable->item(row, 2) ? ui->fournisseurTable->item(row, 2)->text() : "";
    QString telephone      = ui->fournisseurTable->item(row, 3) ? ui->fournisseurTable->item(row, 3)->text() : "";
    QString typeProduit    = ui->fournisseurTable->item(row, 5) ? ui->fournisseurTable->item(row, 5)->text() : "";

    QrFournisseurDialog dlg(nomEntreprise, email, telephone, typeProduit, this);
    dlg.exec();
}

void MainWindow::on_btnExportFournisseur_clicked()
{
    QString fn = QFileDialog::getSaveFileName(this,"Exporter Historique Fournisseurs PDF",
                                               QDir::homePath()+"/Historique_Fournisseurs.pdf","PDF (*.pdf)");
    if (fn.isEmpty()) return;

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fn);
    printer.setPageSize(QPageSize::A4);
    printer.setPageMargins(QMarginsF(15,15,15,15), QPageLayout::Millimeter);

    int actifs=0, suspendus=0;
    for (const FournisseurData &f : fournisseursData) {
        if (f.getStatut()=="Actif") ++actifs;
        if (f.getStatut()=="Suspendu") ++suspendus;
    }

    QString html = QString(
        "<html><head><style>body{font-family:Arial;color:#291C0E;}"
        "h1{text-align:center;color:#8D6E63;border-bottom:3px solid #8D6E63;padding-bottom:8px;}"
        ".hdr{text-align:center;color:#666;font-size:12px;margin-bottom:15px;}"
        ".section{margin-top:25px;padding:15px;background:#FFF8F0;border-left:4px solid #8D6E63;}"
        ".section h2{color:#8D6E63;font-size:14px;margin:0 0 10px 0;}"
        "table{width:100%%;border-collapse:collapse;font-size:10px;margin-top:10px;}"
        "th{background:#8D6E63;color:white;padding:8px;text-align:left;}"
        "td{padding:6px;border-bottom:1px solid #F0E6DA;}"
        "tr:nth-child(even){background:#FFF8F0;}"
        ".summary{background:#E8DED2;padding:10px;border-radius:5px;margin:15px 0;}"
        ".summary p{margin:5px 0;font-size:11px;}"
        "</style></head><body>"
        "<h1>HISTORIQUE DES TRANSACTIONS - FOURNISSEURS</h1>"
        "<div class='hdr'>CUIREA - Export: %1<br>Total Fournisseurs: %2 | Actifs: %3 | Suspendus: %4</div>"
    ).arg(QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm"))
     .arg(fournisseursData.size()).arg(actifs).arg(suspendus);

    // Section 1: Liste des fournisseurs
    html += "<div class='section'><h2>?? Liste des Fournisseurs</h2>"
            "<table><thead><tr><th>ID</th><th>Nom Entreprise</th><th>Email</th><th>Téléphone</th>"
            "<th>Type Produit</th><th>Condition Paiement</th><th>Statut</th></tr></thead><tbody>";

    for (const FournisseurData &f : fournisseursData) {
        html += QString("<tr><td>%1</td><td><b>%2</b></td><td>%3</td><td>%4</td>"
                        "<td>%5</td><td>%6</td><td>%7</td></tr>")
                .arg(f.getId(), f.getNomEntreprise(), f.getEmail(), f.getTelephone(),
                     f.getTypeProduit(), f.getConditionPaiement(), f.getStatut());
    }
    html += "</tbody></table></div>";

    // Section 2: Historique des transactions (données statiques)
    html += "<div class='section'><h2>?? Historique des Transactions</h2>"
            "<table><thead><tr><th>Date</th><th>Fournisseur</th><th>Type</th><th>Montant (TND)</th><th>Référence</th></tr></thead><tbody>"
            "<tr><td>15/01/2025</td><td>Leather Premium Co.</td><td>Paiement</td><td>5,000.00</td><td>PAY-2025-001</td></tr>"
            "<tr><td>10/01/2025</td><td>Textile Supplies Ltd</td><td>Facture</td><td>12,500.00</td><td>INV-2025-003</td></tr>"
            "<tr><td>05/01/2025</td><td>Metal Accessories Inc</td><td>Paiement</td><td>7,500.00</td><td>PAY-2025-002</td></tr>"
            "<tr><td>28/12/2024</td><td>Leather Premium Co.</td><td>Facture</td><td>8,200.00</td><td>INV-2024-125</td></tr>"
            "<tr><td>20/12/2024</td><td>Textile Supplies Ltd</td><td>Paiement</td><td>8,200.00</td><td>PAY-2024-089</td></tr>"
            "<tr><td>15/12/2024</td><td>Zipper World</td><td>Facture</td><td>3,500.00</td><td>INV-2024-118</td></tr>"
            "<tr><td>10/12/2024</td><td>Button Factory</td><td>Paiement</td><td>1,800.00</td><td>PAY-2024-075</td></tr>"
            "</tbody></table></div>";

    // Section 3: Liste des commandes effectuées
    html += "<div class='section'><h2>?? Liste des Commandes Effectuées</h2>"
            "<table><thead><tr><th>Né Commande</th><th>Date</th><th>Fournisseur</th><th>Produit</th><th>Quantité</th><th>Montant (TND)</th></tr></thead><tbody>"
            "<tr><td>CMD-2025-015</td><td>10/01/2025</td><td>Leather Premium Co.</td><td>Cuir Premium</td><td>500 mé</td><td>12,500.00</td></tr>"
            "<tr><td>CMD-2024-098</td><td>28/12/2024</td><td>Textile Supplies Ltd</td><td>Tissu Doublure</td><td>300 m</td><td>8,200.00</td></tr>"
            "<tr><td>CMD-2024-087</td><td>15/12/2024</td><td>Zipper World</td><td>Fermetures éclair</td><td>1000 pcs</td><td>3,500.00</td></tr>"
            "<tr><td>CMD-2024-076</td><td>05/12/2024</td><td>Button Factory</td><td>Boutons Métal</td><td>2000 pcs</td><td>1,800.00</td></tr>"
            "<tr><td>CMD-2024-065</td><td>28/11/2024</td><td>Metal Accessories Inc</td><td>Boucles</td><td>500 pcs</td><td>4,200.00</td></tr>"
            "</tbody></table></div>";

    // Section 4: Résumé financier
    html += "<div class='summary'>"
            "<h2 style='color:#8D6E63;margin-top:0;'>?? Résumé Financier</h2>"
            "<p><b>?? Montant Total Payé:</b> 26,000.00 TND</p>"
            "<p><b>?? Montant Total Facturé:</b> 38,500.00 TND</p>"
            "<p><b>? Solde Restant:</b> 12,500.00 TND</p>"
            "<p><b>?? Nombre de Transactions:</b> 7</p>"
            "<p><b>?? Nombre de Commandes:</b> 5</p>"
            "</div>";

    html += "<div style='text-align:center;margin-top:30px;color:#999;font-size:9px;'>"
            "Document généré automatiquement par CUIREA Management System<br>"
            "Pour usage interne uniquement - Confidentiel</div>"
            "</body></html>";

    QTextDocument doc; doc.setHtml(html); doc.print(&printer);
    QMessageBox::information(this,"Succès","Historique des transactions exporté: "+fn);
}

void MainWindow::on_btnStatsFournisseur_clicked()
{
    QDialog dlg(this); 
    dlg.setWindowTitle("Statistiques de Performance - Fournisseurs"); 
    dlg.setMinimumSize(900,600);
    dlg.setStyleSheet("QDialog{background:#FAF5F0;}");
    
    QVBoxLayout lay(&dlg); 
    lay.setContentsMargins(20,20,20,20);
    lay.setSpacing(15);
    
    // Title
    auto *title = new QLabel("?? STATISTIQUES DE PERFORMANCE DES FOURNISSEURS");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size:18px;font-weight:bold;color:#8D6E63;padding:10px;");
    lay.addWidget(title);
    
    // Table with performance metrics
    auto *table = new QTableWidget(&dlg);
    table->setColumnCount(7);
    table->setHorizontalHeaderLabels({
        "Fournisseur", 
        "?? Délai Moyen", 
        "?? Retards (%)", 
        "? Annulations (%)", 
        "?? Retours (%)", 
        "? Score", 
        "Fiabilité"
    });
    table->horizontalHeader()->setStretchLastSection(true);
    table->setAlternatingRowColors(true);
    table->verticalHeader()->setVisible(false);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setStyleSheet(
        "QTableWidget{background:white;border:2px solid #BCAAA4;border-radius:8px;}"
        "QHeaderView::section{background:#8D6E63;color:white;padding:8px;font-weight:bold;border:none;}"
        "QTableWidget::item{padding:8px;}"
    );
    
    // Static performance data for suppliers
    struct FournisseurPerf {
        QString nom;
        int delaiMoyen;      // en jours
        double tauxRetard;   // %
        double tauxAnnulation; // %
        double tauxRetour;   // %
    };
    
    QList<FournisseurPerf> perfs = {
        {"Leather Premium Co.", 5, 8.5, 2.0, 1.5},
        {"Textile Supplies Ltd", 7, 15.2, 5.0, 3.2},
        {"Metal Accessories Inc", 4, 5.0, 1.0, 0.8},
        {"Zipper World", 6, 12.0, 3.5, 2.0},
        {"Button Factory", 3, 3.0, 0.5, 0.3},
        {"Leather Deluxe", 8, 20.0, 8.0, 5.5},
        {"Fabric Masters", 5, 10.0, 2.5, 1.8},
        {"Hardware Supplies", 4, 6.5, 1.5, 1.0}
    };
    
    int excellents=0, bons=0, moyens=0, faibles=0;
    double scoreMoyen=0;
    
    for (int i=0; i<perfs.size(); ++i) {
        const auto &p = perfs[i];
        
        // Calculate reliability score (0-100)
        double score = 100.0;
        score -= (p.delaiMoyen - 3) * 2;  // Pénalité délai
        score -= p.tauxRetard * 1.5;       // Pénalité retards
        score -= p.tauxAnnulation * 3;     // Pénalité annulations
        score -= p.tauxRetour * 2;         // Pénalité retours
        if (score < 0) score = 0;
        if (score > 100) score = 100;
        
        scoreMoyen += score;
        
        QString fiabilite;
        QString couleur;
        if (score >= 85) {
            fiabilite = "? Excellent";
            couleur = "#2E7D32";
            ++excellents;
        } else if (score >= 70) {
            fiabilite = "? Bon";
            couleur = "#388E3C";
            ++bons;
        } else if (score >= 50) {
            fiabilite = "~ Moyen";
            couleur = "#F57C00";
            ++moyens;
        } else {
            fiabilite = "? Faible";
            couleur = "#C62828";
            ++faibles;
        }
        
        table->insertRow(i);
        table->setItem(i, 0, new QTableWidgetItem(p.nom));
        table->setItem(i, 1, new QTableWidgetItem(QString::number(p.delaiMoyen) + " jours"));
        table->setItem(i, 2, new QTableWidgetItem(QString::number(p.tauxRetard, 'f', 1) + " %"));
        table->setItem(i, 3, new QTableWidgetItem(QString::number(p.tauxAnnulation, 'f', 1) + " %"));
        table->setItem(i, 4, new QTableWidgetItem(QString::number(p.tauxRetour, 'f', 1) + " %"));
        table->setItem(i, 5, new QTableWidgetItem(QString::number(score, 'f', 1)));
        
        auto *fiabItem = new QTableWidgetItem(fiabilite);
        fiabItem->setForeground(QBrush(QColor(couleur)));
        QFont f = fiabItem->font();
        f.setBold(true);
        fiabItem->setFont(f);
        table->setItem(i, 6, fiabItem);
    }
    
    lay.addWidget(table);
    
    scoreMoyen /= perfs.size();
    
    // Summary cards
    QHBoxLayout *cardsLayout = new QHBoxLayout();
    cardsLayout->setSpacing(10);
    
    auto createCard = [](const QString &label, const QString &value, const QString &bgColor) {
        QFrame *card = new QFrame();
        card->setStyleSheet(QString(
            "QFrame{background:%1;border-radius:8px;padding:12px;}"
        ).arg(bgColor));
        QVBoxLayout *vl = new QVBoxLayout(card);
        vl->setSpacing(5);
        auto *lbl = new QLabel(label);
        lbl->setStyleSheet("color:#291C0E;font-size:11px;font-weight:bold;");
        lbl->setAlignment(Qt::AlignCenter);
        auto *val = new QLabel(value);
        val->setStyleSheet("color:#291C0E;font-size:16px;font-weight:bold;");
        val->setAlignment(Qt::AlignCenter);
        vl->addWidget(lbl);
        vl->addWidget(val);
        return card;
    };
    
    cardsLayout->addWidget(createCard("Score Moyen", QString::number(scoreMoyen, 'f', 1), "#C8E6C9"));
    cardsLayout->addWidget(createCard("? Excellents", QString::number(excellents), "#A5D6A7"));
    cardsLayout->addWidget(createCard("? Bons", QString::number(bons), "#81C784"));
    cardsLayout->addWidget(createCard("~ Moyens", QString::number(moyens), "#FFE082"));
    cardsLayout->addWidget(createCard("? Faibles", QString::number(faibles), "#FFAB91"));
    
    lay.addLayout(cardsLayout);
    
    // Summary text
    auto *summary = new QLabel(QString(
        "?? <b>Résumé:</b> Sur %1 fournisseurs analysés, le score moyen de fiabilité est de <b>%2/100</b>. "
        "<span style='color:#2E7D32;'><b>%3</b> excellents</span>, "
        "<span style='color:#388E3C;'><b>%4</b> bons</span>, "
        "<span style='color:#F57C00;'><b>%5</b> moyens</span>, "
        "<span style='color:#C62828;'><b>%6</b> faibles</span>."
    ).arg(perfs.size()).arg(QString::number(scoreMoyen, 'f', 1))
     .arg(excellents).arg(bons).arg(moyens).arg(faibles));
    summary->setWordWrap(true);
    summary->setStyleSheet(
        "background:#FFF8F0;border:2px solid #BCAAA4;border-radius:8px;padding:12px;"
        "color:#291C0E;font-size:12px;"
    );
    lay.addWidget(summary);
    
    // Close button
    QPushButton *close = new QPushButton("Fermer", &dlg);
    close->setStyleSheet(
        "QPushButton{background:#8D6E63;color:white;border:none;border-radius:8px;padding:10px 30px;font-weight:bold;}"
        "QPushButton:hover{background:#A0826D;}"
        "QPushButton:pressed{background:#6E473B;}"
    );
    connect(close, &QPushButton::clicked, &dlg, &QDialog::accept);
    
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(close);
    lay.addLayout(btnLayout);
    
    dlg.exec();
}

void MainWindow::on_searchBoxFournisseur_textChanged(const QString &text)
{
    if (text.trimmed().isEmpty()) {
        refreshFournisseurTable();
        return;
    }
    
    // Essayer d'utiliser la méthode rechercher de la BD
    FournisseurData f;
    QSqlQueryModel* model = f.rechercher(text);
    
    if (!model || model->rowCount() == 0) {
        // Fallback sur le filtrage local
        filterTable(ui->fournisseurTable, text);
        if (model) delete model;
        return;
    }
    
    // Afficher les résultats de la recherche BD
    int n = model->rowCount();
    ui->fournisseurTable->setRowCount(n);
    
    for (int i = 0; i < n; ++i) {
        for (int col = 0; col < 8; ++col) {
            QString value = model->data(model->index(i, col)).toString();
            ui->fournisseurTable->setItem(i, col, new QTableWidgetItem(value));
        }
    }
    
    delete model;
}

void MainWindow::on_btnTriFournisseur_clicked()
{
    QMenu menu(this);
    menu.setStyleSheet(
        "QMenu{background:#FAF5F0;border:2px solid #8D6E63;border-radius:6px;padding:8px;}"
        "QMenu::item{padding:8px 25px;color:#291C0E;border-radius:4px;}"
        "QMenu::item:selected{background:#8D6E63;color:white;}"
        "QMenu::separator{height:2px;background:#BCAAA4;margin:5px 10px;}"
    );

    auto addSortOptions = [&](const QString &label, int col) {
        QMenu *sub = menu.addMenu("?? " + label);
        sub->setStyleSheet(menu.styleSheet());
        auto *asc = sub->addAction("? Croissant (A ? Z)");
        auto *desc = sub->addAction("? Décroissant (Z ? A)");
        connect(asc, &QAction::triggered, [=]{ ui->fournisseurTable->sortItems(col, Qt::AscendingOrder); });
        connect(desc, &QAction::triggered, [=]{ ui->fournisseurTable->sortItems(col, Qt::DescendingOrder); });
    };

    addSortOptions("Nom Entreprise", 1);
    addSortOptions("Email", 2);
    menu.addSeparator();
    addSortOptions("Type Produit", 5);
    addSortOptions("Statut", 7);

    QPoint pos = ui->btnTriFournisseur->mapToGlobal(QPoint(0, ui->btnTriFournisseur->height()));
    menu.exec(pos);
}

// -- Production ----------------------------------------------------------------
void MainWindow::setupProductionTable()
{
    int widths[] = {50,120,150,120,100,110,110,110,100,80,150};
    for (int i = 0; i < 11; ++i) ui->productionTable->setColumnWidth(i, widths[i]);
    ui->productionTable->setColumnHidden(0, true);
    ui->productionTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->productionTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->productionTable->setAlternatingRowColors(true);
    ui->productionTable->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->productionTable->setSortingEnabled(false);

    m_productionSortCol = -1;
    m_productionSortAsc = true;

    // Tri correct via SQL au clic sur les headers
    connect(ui->productionTable->horizontalHeader(), &QHeaderView::sectionClicked,
            this, [this](int logicalIndex) {
        // Mapping colonne visuelle ? nom colonne SQL
        static const QMap<int, QString> colToSql = {
            {3, "PRODUIT"},
            {4, "MONTANT"},
            {6, "DATE_CREATION"},
            {7, "DATE_LIVRAISON"},
            {8, "STATUT"},
            {9, "PRIORITE"}
        };
        if (!colToSql.contains(logicalIndex)) return;

        // Alterner ASC/DESC si même colonne
        if (m_productionSortCol == logicalIndex)
            m_productionSortAsc = !m_productionSortAsc;
        else {
            m_productionSortCol = logicalIndex;
            m_productionSortAsc = true;
        }

        // Indicateur visuel sur le header
        ui->productionTable->horizontalHeader()->setSortIndicatorShown(true);
        ui->productionTable->horizontalHeader()->setSortIndicator(
            logicalIndex, m_productionSortAsc ? Qt::AscendingOrder : Qt::DescendingOrder);

        // Recharger depuis la DB avec ORDER BY correct
        const QString sql =
            "SELECT C.ID_COMMANDE, C.REFERENCE, (E.NOM || ' ' || E.PRENOM) AS EMPLOYE, "
            "C.PRODUIT, C.DATE_CREATION, C.DATE_LIVRAISON, C.STATUT, C.PRIORITE, C.MONTANT, C.MAIL_CLIENT, C.ETAT "
            "FROM COMMANDES C LEFT JOIN EMPLOYES E ON C.ID_EMPLOYE = E.ID_EMPLOYE "
            "ORDER BY C." + colToSql[logicalIndex] + (m_productionSortAsc ? " ASC" : " DESC");
        applyProductionQuery(sql);
    });

    connect(ui->productionTable, &QTableWidget::customContextMenuRequested,
            this, &MainWindow::onProductionTableContextMenu);
}
//----mapfournisseur---------------------------------------------------

void MainWindow::openMap(double lat, double lon)
{
    QString url = QString("https://www.google.com/maps?q=%1,%2")
                      .arg(lat)
                      .arg(lon);

    QDesktopServices::openUrl(QUrl(url));
}

void MainWindow::on_btnmap_clicked()
{
    int row = ui->fournisseurTable->currentRow();

    if (row < 0)
        return;

    const int ADDRESS_COLUMN = 8; // make sure this is correct

    QTableWidgetItem *item = ui->fournisseurTable->item(row, ADDRESS_COLUMN);

    if (!item)
        return;

    QString address = item->text();

    if (address.isEmpty())
        return;

    mapService->geocodeAddress(address);
}



//--------------------------------------------------------------------
// Helper : remplit une ligne du tableau depuis un QAbstractItemModel
// SQL layout: 0=ID, 1=REF, 2=EMPLOYE, 3=PRODUIT, 4=DATE_CREATION,
//             5=DATE_LIVRAISON, 6=STATUT, 7=PRIORITE, 8=MONTANT, 9=MAIL_CLIENT, 10=ETAT
void MainWindow::fillProductionRow(int row, const QAbstractItemModel *m, int i)
{
    auto get = [&](int col) { return m->data(m->index(i, col)); };

    ui->productionTable->setItem(row, 0,  new QTableWidgetItem(get(0).toString()));
    ui->productionTable->setItem(row, 1,  new QTableWidgetItem(get(1).toString()));
    ui->productionTable->setItem(row, 2,  new QTableWidgetItem(get(2).toString()));
    ui->productionTable->setItem(row, 3,  new QTableWidgetItem(get(3).toString()));
    ui->productionTable->setItem(row, 4,  new QTableWidgetItem(
        QString::number(get(8).toDouble(), 'f', 2) + " DT"));

    // Col 5 : état Paiement (coloré)
    QString etat = get(10).toString();
    if (etat.isEmpty()) etat = "Non payée";
    auto *etatItem = new QTableWidgetItem(etat);
    etatItem->setTextAlignment(Qt::AlignCenter);
    const bool paye = etat.toLower() == "payée" || etat.toLower() == "payee";
    etatItem->setBackground(QColor(paye ? "#27AE60" : "#E74C3C"));
    etatItem->setForeground(Qt::white);
    ui->productionTable->setItem(row, 5, etatItem);

    ui->productionTable->setItem(row, 6,  new QTableWidgetItem(
        get(4).toDate().toString("dd/MM/yyyy")));
    const QDate dl = get(5).toDate();
    ui->productionTable->setItem(row, 7,  new QTableWidgetItem(
        dl.isValid() ? dl.toString("dd/MM/yyyy") : "-"));
    ui->productionTable->setItem(row, 8,  new QTableWidgetItem(get(6).toString()));
    ui->productionTable->setItem(row, 9,  new QTableWidgetItem(get(7).toString()));
    ui->productionTable->setItem(row, 10, new QTableWidgetItem(get(9).toString()));
}

// Helper : exécute une requéte et remplit le tableau
void MainWindow::applyProductionQuery(const QString &sql)
{
    QSqlQueryModel model;
    model.setQuery(sql, Connection::instance()->getDatabase());
    if (model.lastError().isValid()) {
        qWarning() << "[Production] Erreur SQL:" << model.lastError().text();
        return;
    }
    ui->productionTable->setRowCount(0);
    for (int i = 0; i < model.rowCount(); ++i) {
        const int row = ui->productionTable->rowCount();
        ui->productionTable->insertRow(row);
        fillProductionRow(row, &model, i);
    }
}

//--------------------------------------------------------------------
void MainWindow::loadProductionData()
{
    static const QString sql =
        "SELECT C.ID_COMMANDE, C.REFERENCE, (E.NOM || ' ' || E.PRENOM) AS EMPLOYE, "
        "C.PRODUIT, C.DATE_CREATION, C.DATE_LIVRAISON, C.STATUT, C.PRIORITE, C.MONTANT, C.MAIL_CLIENT, C.ETAT "
        "FROM COMMANDES C LEFT JOIN EMPLOYES E ON C.ID_EMPLOYE = E.ID_EMPLOYE "
        "ORDER BY C.DATE_CREATION DESC";
    applyProductionQuery(sql);
    updateProductionStatsCards();
}

void MainWindow::ajouterCommandeProduction(const QString &ref, const QString &employe,
    const QString &type, const QString &montant, const QString &dc,
    const QString &dl, const QString &statut, const QString &priorite)
{
    int row = ui->productionTable->rowCount();
    ui->productionTable->insertRow(row);
    ui->productionTable->setItem(row,0,new QTableWidgetItem(QString::number(row+1)));
    ui->productionTable->setItem(row,1,new QTableWidgetItem(ref));
    ui->productionTable->setItem(row,2,new QTableWidgetItem(employe));
    ui->productionTable->setItem(row,3,new QTableWidgetItem(type));
    ui->productionTable->setItem(row,4,new QTableWidgetItem(montant));
    ui->productionTable->setItem(row,5,new QTableWidgetItem(dc));
    ui->productionTable->setItem(row,6,new QTableWidgetItem(dl));
    ui->productionTable->setItem(row,7,new QTableWidgetItem(statut));
    ui->productionTable->setItem(row,8,new QTableWidgetItem(priorite));
}

void MainWindow::updateProductionStatistics()   { updateProductionStatsCards(); }

void MainWindow::checkRetards()
{
    // Seulement sur l'onglet Production (index 4)
    if (ui->stackedWidget->currentIndex() != 4) return;

    QSqlQuery query(Connection::instance()->getDatabase());
    // Statuts qui déclenchent la bulle : En Attente, En Cours, Suspendu
    // On exclut : Expédié, Terminé, Annulé (et variantes sans accent)
    query.prepare(
        "SELECT ID_COMMANDE, REFERENCE, DATE_LIVRAISON, STATUT, PRIORITE "
        "FROM COMMANDES "
        "WHERE DATE_LIVRAISON IS NOT NULL "
        "AND DATE_LIVRAISON < :today "
        "AND UPPER(STATUT) NOT IN ("
        "  'TERMINE','TERMINé',"
        "  'ANNULE','ANNULé'"
        ")"
    );
    query.bindValue(":today", QDate::currentDate());

    if (!query.exec()) {
        qDebug() << "checkRetards SQL error:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        int     id       = query.value(0).toInt();
        QString ref      = query.value(1).toString();
        QDate   dl       = query.value(2).toDate();
        QString statut   = query.value(3).toString();
        QString priorite = query.value(4).toString();

        if (m_notifiedIds.contains(id)) continue;
        m_notifiedIds.insert(id);

        int jours = dl.daysTo(QDate::currentDate());

        NotificationWidget::Type ntype = NotificationWidget::Warning;
        if (priorite.toUpper() == "URGENTE" || jours > 7)
            ntype = NotificationWidget::Critical;

        QString titre = QString("⚠️ Retard — %1").arg(ref);
        QString msg   = QString("Livraison prévue le %1\n%2 jour(s) de retard — Statut : %3")
                            .arg(dl.toString("dd/MM/yyyy"))
                            .arg(jours)
                            .arg(statut);

        NotificationWidget::show(titre, msg, ntype);
    }
}

void MainWindow::onTrierProduction()
{
    // -- Dialogue de tri avancé multi-critéres --------------------------------
    QDialog dlg(this);
    dlg.setWindowTitle("Tri avancé — Production");
    dlg.setFixedSize(380, 240);
    dlg.setStyleSheet(
        "QDialog{background:#FBF5F0;}"
        "QLabel{color:#3E1020;font-size:12px;font-weight:bold;}"
        "QComboBox{background:white;border:1px solid #C4923A;border-radius:4px;"
        "padding:4px 8px;font-size:12px;color:#3E1020;min-height:24px;}"
        "QComboBox::drop-down{border:none;width:18px;}"
        "QComboBox QAbstractItemView{background:white;color:#3E1020;"
        "selection-background-color:#6B2737;selection-color:white;border:1px solid #C4923A;}"
        "QGroupBox{background:white;border:1px solid #E8DDD5;border-radius:6px;"
        "padding:4px;margin-top:8px;}"
        "QGroupBox::title{color:#6B2737;font-size:11px;font-weight:bold;"
        "subcontrol-origin:margin;left:8px;padding:0 3px;}"
    );

    QVBoxLayout *root = new QVBoxLayout(&dlg);
    root->setContentsMargins(14, 10, 14, 10);
    root->setSpacing(6);

    QLabel *title = new QLabel("Trier les commandes par :");
    title->setStyleSheet("font-size:13px;font-weight:bold;color:#6B2737;");
    root->addWidget(title);

    // Colonnes disponibles
    QStringList colLabels = {"Employé","Produit","Montant HT",
                             "Date Création","Date Livraison","Statut","Priorité","Mail Client"};
    QStringList colSql    = {"","PRODUIT","MONTANT",
                             "DATE_CREATION","DATE_LIVRAISON","STATUT","PRIORITE",""};

    // Critère 1
    QGroupBox *g1 = new QGroupBox("Critère principal");
    QHBoxLayout *l1 = new QHBoxLayout(g1);
    l1->setContentsMargins(8, 4, 8, 6);
    l1->setSpacing(6);
    QComboBox *col1 = new QComboBox(); QComboBox *ord1 = new QComboBox();
    for (int i = 0; i < colLabels.size(); ++i)
        if (!colSql[i].isEmpty()) col1->addItem(colLabels[i], colSql[i]);
    col1->setCurrentIndex(2); // Montant par défaut
    ord1->addItem("↓ Décroissant", false);
    ord1->addItem("↑ Croissant",   true);
    ord1->setFixedWidth(120);
    l1->addWidget(col1, 1); l1->addWidget(ord1);
    root->addWidget(g1);

    // Critère 2
    QGroupBox *g2 = new QGroupBox("Critère secondaire (optionnel)");
    QHBoxLayout *l2 = new QHBoxLayout(g2);
    l2->setContentsMargins(8, 4, 8, 6);
    l2->setSpacing(6);
    QComboBox *col2 = new QComboBox(); QComboBox *ord2 = new QComboBox();
    col2->addItem("— Aucun —", "");
    for (int i = 0; i < colLabels.size(); ++i)
        if (!colSql[i].isEmpty()) col2->addItem(colLabels[i], colSql[i]);
    ord2->addItem("↓ Décroissant", false);
    ord2->addItem("↑ Croissant",   true);
    ord2->setFixedWidth(120);
    l2->addWidget(col2, 1); l2->addWidget(ord2);
    root->addWidget(g2);

    root->addStretch();

    // Boutons
    QHBoxLayout *btns = new QHBoxLayout(); btns->setSpacing(8);
    QPushButton *btnReset  = new QPushButton("↺ Réinitialiser");
    QPushButton *btnApply  = new QPushButton("✔ Appliquer");
    QPushButton *btnCancel = new QPushButton("Annuler");
    btnApply->setStyleSheet(
        "QPushButton{background:#6B2737;color:white;border:none;border-radius:5px;"
        "padding:7px 16px;font-size:12px;font-weight:bold;}"
        "QPushButton:hover{background:#4E1A27;}");
    btnReset->setStyleSheet(
        "QPushButton{background:#C4923A;color:white;border:none;border-radius:5px;"
        "padding:7px 12px;font-size:12px;font-weight:bold;}"
        "QPushButton:hover{background:#A87730;}");
    btnCancel->setStyleSheet(
        "QPushButton{background:#E8DDD5;color:#3E1020;border:none;border-radius:5px;"
        "padding:7px 12px;font-size:12px;}"
        "QPushButton:hover{background:#D4C8BC;}");

    connect(btnReset, &QPushButton::clicked, [&]{
        loadProductionData();
        ui->productionTable->horizontalHeader()->setSortIndicatorShown(false);
        dlg.accept();
    });
    connect(btnCancel, &QPushButton::clicked, &dlg, &QDialog::reject);
    connect(btnApply, &QPushButton::clicked, [&]{
        const QString c1 = col1->currentData().toString();
        const bool    a1 = ord1->currentData().toBool();
        const QString c2 = col2->currentData().toString();
        const bool    a2 = ord2->currentData().toBool();

        QString sql =
            "SELECT C.ID_COMMANDE, C.REFERENCE, (E.NOM || ' ' || E.PRENOM) AS EMPLOYE, "
            "C.PRODUIT, C.DATE_CREATION, C.DATE_LIVRAISON, C.STATUT, C.PRIORITE, C.MONTANT, C.MAIL_CLIENT, C.ETAT "
            "FROM COMMANDES C LEFT JOIN EMPLOYES E ON C.ID_EMPLOYE = E.ID_EMPLOYE "
            "ORDER BY C." + c1 + (a1 ? " ASC" : " DESC");
        if (!c2.isEmpty())
            sql += ", C." + c2 + (a2 ? " ASC" : " DESC");

        // Indicateur visuel sur le header
        static const QMap<QString,int> sqlToCol = {
            {"PRODUIT",3},{"MONTANT",4},
            {"DATE_CREATION",6},{"DATE_LIVRAISON",7},{"STATUT",8},{"PRIORITE",9}
        };
        if (sqlToCol.contains(c1)) {
            ui->productionTable->horizontalHeader()->setSortIndicatorShown(true);
            ui->productionTable->horizontalHeader()->setSortIndicator(
                sqlToCol[c1], a1 ? Qt::AscendingOrder : Qt::DescendingOrder);
        }

        applyProductionQuery(sql);
        dlg.accept();
    });

    btns->addWidget(btnReset);
    btns->addStretch();
    btns->addWidget(btnCancel);
    btns->addWidget(btnApply);
    root->addLayout(btns);

    dlg.exec();
}

void MainWindow::updateProductionStatsCards()
{
    // Optimisation: 1 seule requéte au lieu de 4
    QSqlQuery query(Connection::instance()->getDatabase());
    
    QString sql = "SELECT "
                  "COUNT(*) AS total, "
                  "SUM(CASE WHEN STATUT = 'En Production' THEN 1 ELSE 0 END) AS en_prod, "
                  "SUM(CASE WHEN STATUT = 'Terminé' THEN 1 ELSE 0 END) AS terminees, "
                  "NVL(SUM(MONTANT), 0) AS montant_total "
                  "FROM COMMANDES";
    
    int total = 0, enProd = 0, terminees = 0;
    double montantTotal = 0;
    
    if (query.exec(sql) && query.next()) {
        total = query.value(0).toInt();
        enProd = query.value(1).toInt();
        terminees = query.value(2).toInt();
        montantTotal = query.value(3).toDouble();
    }
    
    // Mettre à jour les labels
    ui->statsValueProduction1->setText(QString::number(total));
    ui->statsValueProduction2->setText(QString::number(enProd));
    ui->statsValueProduction3->setText(QString::number(terminees));
    ui->statsValueProduction4->setText(QString::number(montantTotal, 'f', 2) + " DT");
    
    qDebug() << "?? Statistiques mises à jour:";
    qDebug() << "   Total:" << total;
    qDebug() << "   En Production:" << enProd;
    qDebug() << "   Terminées:" << terminees;
    qDebug() << "   Montant Total:" << montantTotal << "DT";
}

void MainWindow::onCreerProduction()
{
    ProductionDialog dlg(this, ProductionDialog::AddMode);

    // Ajouter état paiement directement dans le dialogue
    QComboBox *cmbPaiement = new QComboBox(&dlg);
    cmbPaiement->addItems({"Non payée", "Payée"});
    QFormLayout *form = dlg.findChild<QFormLayout*>();
    if (!form) {
        // Chercher dans les layouts enfants
        for (auto *lay : dlg.findChildren<QGridLayout*>()) {
            lay->addWidget(new QLabel("état Paiement :", &dlg), lay->rowCount(), 0);
            lay->addWidget(cmbPaiement, lay->rowCount()-1, 1);
            break;
        }
    }

    if (dlg.exec() == QDialog::Accepted) {
        qDebug() << "========== AJOUT COMMANDE ==========";
        
        // Créer l'objet Production
        Production prod;
        
        // Générer une référence unique au format ABC + 3 chiffres
        QString ref = dlg.getReference();
        if (ref.isEmpty()) {
            // Fallback : même logique ABC100, ABC101...
            QSqlQuery query(Connection::instance()->getDatabase());
            query.prepare("SELECT REFERENCE FROM COMMANDES WHERE REFERENCE LIKE 'ABC%' ORDER BY REFERENCE DESC");
            int nextNum = 100;
            if (query.exec()) {
                while (query.next()) {
                    QString r = query.value(0).toString();
                    if (r.length() == 6 && r.startsWith("ABC")) {
                        bool ok;
                        int num = r.mid(3).toInt(&ok);
                        if (ok && num >= nextNum) nextNum = num + 1;
                    }
                }
            }
            if (nextNum > 999) nextNum = 100;
            ref = QString("ABC%1").arg(nextNum);
        }
        
        prod.setReference(ref);
        prod.setType(dlg.getProduit()); // Le nom du produit (ex: "Sac à Main Cuir") va dans PRODUIT
        prod.setServiceVente(dlg.getProduit());
        prod.setMontant(dlg.getQuantite().toDouble()); // getQuantite() contient le prix saisi
        prod.setIdEmploye(dlg.getEmployeId());  // Récupérer l'ID de l'employé sélectionné
        prod.setMailClient(dlg.getMailClient());
        
        // Gérer les dates
        QDate dateCreation = QDate::fromString(dlg.getDateDebut(), "dd/MM/yyyy");
        QDate dateLivraison = QDate::fromString(dlg.getDateFin(), "dd/MM/yyyy");
        
        if (!dateCreation.isValid()) {
            dateCreation = QDate::currentDate();
        }
        if (!dateLivraison.isValid()) {
            dateLivraison = QDate::currentDate().addDays(30);
        }
        
        prod.setDateCreation(dateCreation);
        prod.setDateLivraisonPrevue(dateLivraison);
        prod.setStatut(dlg.getStatut());
        prod.setPriorite(dlg.getPriorite());
        prod.setEtatPaiement(cmbPaiement->currentText());
        prod.setServiceVente("Service Commercial");
        
        // Debug
        qDebug() << "Reference:" << prod.getReference();
        qDebug() << "Type:" << prod.getType();
        qDebug() << "Montant:" << prod.getMontant();
        qDebug() << "ID Employe:" << prod.getIdEmploye();  // IMPORTANT: Vérifier l'ID
        qDebug() << "Date Creation:" << prod.getDateCreation().toString("yyyy-MM-dd");
        qDebug() << "Date Livraison:" << prod.getDateLivraisonPrevue().toString("yyyy-MM-dd");
        qDebug() << "Statut:" << prod.getStatut();
        qDebug() << "Priorite:" << prod.getPriorite();
        
        ProductionDAO dao;
        if (dao.ajouter(prod)) {
            qDebug() << "Commande ajoutee avec succes";
            NotificationWidget::show("Commande creee", prod.getReference() + " enregistree.", NotificationWidget::Success);
            QMessageBox::information(this, "Succes", "Commande ajoutee avec succes!");
            loadProductionData();
        } else {
            qDebug() << "Echec de l'ajout";
            QMessageBox::critical(this, "Erreur", "Erreur lors de l'ajout de la commande.");
        }
    }
}

void MainWindow::onModifierProduction()
{
    int row = ui->productionTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Attention", "Veuillez sélectionner une commande.");
        return;
    }
    
    QDialog d(this);
    d.setWindowTitle("Modifier Commande");
    d.setMinimumSize(500, 450);
    d.setStyleSheet(DIALOG_STYLE);
    
    QVBoxLayout lay(&d);
    lay.setContentsMargins(20, 20, 20, 20);
    QFormLayout form;
    
    QLineEdit refE(cellText(ui->productionTable, row, 1), &d);
    refE.setReadOnly(true);
    refE.setStyleSheet("background-color: #F0EBE5; color: #888; border: 2px solid #BCAAA4; border-radius:6px; padding:8px;");
    
    // Remplacer LineEdit par ComboBox pour l'employé
    QComboBox employeC(&d);
    QMap<int, QString> employeMap;
    
    // Charger les employés depuis la base de données
    QSqlQuery query(Connection::instance()->getDatabase());
    QString sql = "SELECT ID_EMPLOYE, NOM, PRENOM FROM EMPLOYES ORDER BY NOM, PRENOM";
    if (query.exec(sql)) {
        while (query.next()) {
            int id = query.value(0).toInt();
            QString nom = query.value(1).toString();
            QString prenom = query.value(2).toString();
            QString nomComplet = nom + " " + prenom;
            employeC.addItem(nomComplet, id);
            employeMap[id] = nomComplet;
        }
    }
    
    // Sélectionner l'employé actuel
    QString employeActuel = cellText(ui->productionTable, row, 2);
    int indexEmploye = employeC.findText(employeActuel);
    if (indexEmploye >= 0) {
        employeC.setCurrentIndex(indexEmploye);
    }
    
    QComboBox typeC(&d);
    {
        QSqlQuery qa(Connection::instance()->getDatabase());
        if (qa.exec("SELECT NOM FROM ARTICLES ORDER BY NOM")) {
            while (qa.next())
                typeC.addItem(qa.value(0).toString(), qa.value(0).toString());
        }
        if (typeC.count() == 0)
            typeC.addItems({"Sac à Main Cuir","Portefeuille","Ceinture","Sacoche","Porte-documents","Sac à Dos"});
    }
    // Pré-sélectionner le produit actuel
    for (int i = 0; i < typeC.count(); ++i) {
        if (typeC.itemData(i).toString() == cellText(ui->productionTable, row, 3)) {
            typeC.setCurrentIndex(i); break;
        }
    }
    
    QString montantStr = cellText(ui->productionTable, row, 4);
    montantStr.remove(" DT").remove(",").replace(" ", "");
    QDoubleSpinBox montantE(&d);
    montantE.setRange(0.01, 999999.99);
    montantE.setDecimals(2);
    montantE.setSuffix(" DT");
    montantE.setSingleStep(1.0);
    montantE.setValue(montantStr.toDouble());
    montantE.setStyleSheet(
        "QDoubleSpinBox { background:white; border:2px solid #BCAAA4; border-radius:6px; "
        "padding:8px; font-size:12px; color:#291C0E; }"
        "QDoubleSpinBox:focus { border-color:#8D6E63; }"
        "QDoubleSpinBox::up-button, QDoubleSpinBox::down-button { width:18px; }");

    QDateEdit dcE(QDate::fromString(cellText(ui->productionTable, row, 6), "dd/MM/yyyy"), &d);
    dcE.setCalendarPopup(true);
    dcE.setDisplayFormat("dd/MM/yyyy");

    QDateEdit dlE(QDate::fromString(cellText(ui->productionTable, row, 7), "dd/MM/yyyy"), &d);
    dlE.setCalendarPopup(true);
    dlE.setDisplayFormat("dd/MM/yyyy");

    QComboBox statC(&d);
    statC.addItems({"En Attente", "Planifié", "En Cours", "En Production", "Suspendu", "Terminé", "Annulé"});
    statC.setCurrentText(cellText(ui->productionTable, row, 8));

    QComboBox prioC(&d);
    prioC.addItems({"Basse", "Normale", "Urgente"});
    prioC.setCurrentText(cellText(ui->productionTable, row, 9));

    form.addRow("Référence :", &refE);
    form.addRow("Employé :", &employeC);
    form.addRow("Produit :", &typeC);
    form.addRow("Montant HT :", &montantE);
    form.addRow("Date Création :", &dcE);
    form.addRow("Date Livraison :", &dlE);
    form.addRow("Statut :", &statC);
    form.addRow("Priorité :", &prioC);

    // ComboBox client chargé depuis la BD
    QComboBox clientC(&d);
    clientC.addItem("-- Aucun client --", "");
    QString mailActuel = "";
    {
        QSqlQuery qc(Connection::instance()->getDatabase());
        if (qc.exec("SELECT EMAIL, PRENOM || ' ' || NOM FROM CLIENTS WHERE EMAIL IS NOT NULL ORDER BY NOM, PRENOM")) {
            while (qc.next()) {
                QString email = qc.value(0).toString();
                QString nom   = qc.value(1).toString();
                clientC.addItem(nom + "  <" + email + ">", email);
            }
        }
        QSqlQuery qm(Connection::instance()->getDatabase());
        qm.prepare("SELECT MAIL_CLIENT FROM COMMANDES WHERE ID_COMMANDE = :id");
        qm.bindValue(":id", cellText(ui->productionTable, row, 0).toInt());
        if (qm.exec() && qm.next()) mailActuel = qm.value(0).toString();
    }
    for (int i = 0; i < clientC.count(); ++i) {
        if (clientC.itemData(i).toString() == mailActuel) { clientC.setCurrentIndex(i); break; }
    }
    form.addRow("Client :", &clientC);

    // État paiement
    QComboBox paiementC(&d);
    paiementC.addItems({"Non payée", "Payée"});
    {
        QSqlQuery qp(Connection::instance()->getDatabase());
        qp.prepare("SELECT ETAT FROM COMMANDES WHERE ID_COMMANDE = :id");
        qp.bindValue(":id", cellText(ui->productionTable, row, 0).toInt());
        if (qp.exec() && qp.next()) {
            QString ep = qp.value(0).toString();
            int idx = paiementC.findText(ep, Qt::MatchFixedString);
            if (idx >= 0) paiementC.setCurrentIndex(idx);
        }
    }
    form.addRow("État Paiement :", &paiementC);

    lay.addLayout(&form);

    QHBoxLayout btns;
    QPushButton ok("Enregistrer", &d), cancel("Annuler", &d);
    cancel.setStyleSheet("QPushButton{background:#95877C;}");
    btns.addStretch();
    btns.addWidget(&ok);
    btns.addWidget(&cancel);
    lay.addLayout(&btns);

    connect(&ok, &QPushButton::clicked, &d, &QDialog::accept);
    connect(&cancel, &QPushButton::clicked, &d, &QDialog::reject);

    if (d.exec() == QDialog::Accepted) {
        if (refE.text().isEmpty() || employeC.currentIndex() < 0 || montantE.value() <= 0) {
            QMessageBox::warning(this, "Attention", "Champs obligatoires manquants.");
            return;
        }

        Production prod;
        prod.setIdCommande(cellText(ui->productionTable, row, 0).toInt());
        prod.setReference(refE.text());
        prod.setIdEmploye(employeC.currentData().toInt());
        prod.setType(typeC.currentData().toString());
        prod.setServiceVente(typeC.currentData().toString());
        prod.setMontant(montantE.value());
        prod.setDateCreation(dcE.date());
        prod.setDateLivraisonPrevue(dlE.date());
        prod.setStatut(statC.currentText());
        prod.setPriorite(prioC.currentText());
        prod.setMailClient(clientC.currentData().toString());
        prod.setEtatPaiement(paiementC.currentText());
        
        ProductionDAO dao;
        if (dao.modifier(prod)) {
            NotificationWidget::show("Commande modifiee", prod.getReference() + " mise a jour.", NotificationWidget::Success);
            QMessageBox::information(this, "Succes", "Commande modifiee avec succes!");
            loadProductionData();
        } else {
            QMessageBox::critical(this, "Erreur", "Erreur lors de la modification de la commande.");
        }
    }
}

void MainWindow::onSuiviProduction()
{
    int row = ui->productionTable->currentRow();
    if (row < 0) { QMessageBox::warning(this,"","Sélectionnez une commande."); return; }
    QString ref    = cellText(ui->productionTable,row,1);
    QString employe = cellText(ui->productionTable,row,2);
    QString statut = cellText(ui->productionTable,row,8);
    QString dc     = cellText(ui->productionTable,row,6);
    QString dl     = cellText(ui->productionTable,row,7);
    QString prio   = cellText(ui->productionTable,row,9);

    QString html = "<html><head><style>"
        "body{font-family:Arial;padding:10px;background:#FAFAFA;}"
        ".item{margin:12px 0;padding:12px;background:#FFF;border-left:4px solid #8D6E63;border-radius:6px;}"
        ".date{color:#8D6E63;font-weight:bold;font-size:12px;}"
        ".status{font-size:15px;font-weight:bold;margin:5px 0;}"
        ".desc{color:#666;font-size:12px;}"
        "</style></head><body>";

    auto step = [&](const QString &date, const QString &status, const QString &desc) {
        html += "<div class='item'><div class='date'>" + date + "</div>"
                "<div class='status'>" + status + "</div>"
                "<div class='desc'>" + desc + "</div></div>";
    };
    step(dc,"✅ Commande créée","La commande a été enregistrée dans le système");
    if (statut=="Planifié"||statut=="En Production"||statut=="Terminé")
        step(QDate::fromString(dc,"dd/MM/yyyy").addDays(1).toString("dd/MM/yyyy"),
             "? Planifiée","La commande a été planifiée pour la production");
    if (statut=="En Production"||statut=="Terminé")
        step(QDate::fromString(dc,"dd/MM/yyyy").addDays(3).toString("dd/MM/yyyy"),
             "? En production","La commande est en cours de fabrication");
    if (statut=="Terminé")
        step(dl,"? Terminée","La commande a été terminée et livrée avec succès");
    else
        step(dl+" (Prévu)","? Livraison prévue","Date de livraison estimée");
    html += "</body></html>";

    QDialog dlg(this); dlg.setWindowTitle("Suivi - "+ref); dlg.setMinimumSize(650,600);
    QVBoxLayout lay(&dlg); lay.setContentsMargins(20,20,20,20);
    auto *info = new QLabel(QString("<b>Ref:</b> %1 | <b>Employé:</b> %2 | <b>Priorité:</b> %3")
                            .arg(ref,employe,prio));
    info->setStyleSheet("background:#F8F8F8;border-radius:6px;padding:10px;");
    lay.addWidget(info);
    auto *view = new QTextEdit(&dlg); view->setReadOnly(true); view->setHtml(html);
    lay.addWidget(view);
    QPushButton close("Fermer",&dlg);
    connect(&close,&QPushButton::clicked,&dlg,&QDialog::accept);
    QHBoxLayout bl; bl.addStretch(); bl.addWidget(&close);
    lay.addLayout(&bl);
    dlg.exec();
}

void MainWindow::onPlanificationProduction()
{
    ProductionView *productionView = new ProductionView(this);
    if (m_ai) productionView->setNotificationAI(m_ai);
    productionView->setAttribute(Qt::WA_DeleteOnClose);
    productionView->show();
}

void MainWindow::onFactureProduction()
{
    int row = ui->productionTable->currentRow();
    if (row < 0) { QMessageBox::warning(this,"","Sélectionnez une commande."); return; }

    QString id         = cellText(ui->productionTable, row, 0);
    QString ref        = cellText(ui->productionTable, row, 1);
    QString employe    = cellText(ui->productionTable, row, 2);
    QString type       = cellText(ui->productionTable, row, 3);
    QString montantRaw = cellText(ui->productionTable, row, 4);
    QString dc         = cellText(ui->productionTable, row, 5);
    QString dl         = cellText(ui->productionTable, row, 6);
    QString statut     = cellText(ui->productionTable, row, 7);
    QString priorite   = cellText(ui->productionTable, row, 8);

    double ht     = QString(montantRaw).remove(" DT").remove(",").replace(" ","").toDouble();
    double tva    = ht * 0.19;
    double remise = ht * 0.05;
    double ttc    = ht + tva - remise;

    // Mail client depuis BD
    QString mailClient;
    {
        QSqlQuery q(Connection::instance()->getDatabase());
        q.prepare("SELECT MAIL_CLIENT FROM COMMANDES WHERE ID_COMMANDE = :id");
        q.bindValue(":id", id.toInt());
        if (q.exec() && q.next()) mailClient = q.value(0).toString();
    }

    // -- Dialogue -------------------------------------------------------------
    QDialog dlg(this);
    dlg.setWindowTitle("Facture é " + ref);
    dlg.setMinimumSize(780, 700);
    dlg.setStyleSheet("QDialog { background: #FBF5F0; }");

    QVBoxLayout *root = new QVBoxLayout(&dlg);
    root->setContentsMargins(20, 20, 20, 16);
    root->setSpacing(12);

    QScrollArea *scroll = new QScrollArea(&dlg);
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea { border:none; background:#FBF5F0; }");

    QWidget *page = new QWidget();
    page->setStyleSheet("background:white; border-radius:10px;");
    QVBoxLayout *lay = new QVBoxLayout(page);
    lay->setContentsMargins(36, 32, 36, 32);
    lay->setSpacing(20);

    // -- EN-TéTE --------------------------------------------------------------
    QHBoxLayout *hdr = new QHBoxLayout();

    QVBoxLayout *logoCol = new QVBoxLayout();
    QLabel *logoLbl = new QLabel("CUIREA");
    logoLbl->setStyleSheet("font-size:30px; font-weight:bold; color:#C4923A; letter-spacing:3px;");
    QLabel *subLbl = new QLabel("Smart Leather Goods Factory");
    subLbl->setStyleSheet("font-size:11px; color:#888;");
    logoCol->addWidget(logoLbl); logoCol->addWidget(subLbl); logoCol->addStretch();
    hdr->addLayout(logoCol);
    hdr->addStretch();

    QVBoxLayout *titleCol = new QVBoxLayout();
    titleCol->setAlignment(Qt::AlignRight);
    QLabel *facLbl = new QLabel("FACTURE");
    facLbl->setStyleSheet("font-size:34px; font-weight:bold; color:#6B2737; letter-spacing:4px;");
    facLbl->setAlignment(Qt::AlignRight);
    QLabel *numLbl = new QLabel("Né " + ref);
    numLbl->setStyleSheet("font-size:13px; color:#555;"); numLbl->setAlignment(Qt::AlignRight);
    QLabel *dateLbl = new QLabel("Date : " + QDate::currentDate().toString("dd MMMM yyyy"));
    dateLbl->setStyleSheet("font-size:12px; color:#888;"); dateLbl->setAlignment(Qt::AlignRight);
    titleCol->addWidget(facLbl); titleCol->addWidget(numLbl); titleCol->addWidget(dateLbl);
    hdr->addLayout(titleCol);
    lay->addLayout(hdr);

    // Séparateur
    auto makeSep = [&]() {
        QFrame *s = new QFrame(); s->setFrameShape(QFrame::HLine);
        s->setStyleSheet("color:#E0E0E0;"); return s;
    };
    lay->addWidget(makeSep());

    // -- INFOS ENTREPRISE / CLIENT ---------------------------------------------
    auto makeInfoBox = [](const QString &title, const QStringList &lines) {
        QWidget *box = new QWidget();
        box->setStyleSheet("background:#FBF5F0; border-radius:8px; border-left:3px solid #C4923A;");
        QVBoxLayout *bl = new QVBoxLayout(box);
        bl->setContentsMargins(14,12,14,12); bl->setSpacing(4);
        QLabel *t = new QLabel(title);
        t->setStyleSheet("font-size:10px; font-weight:bold; color:#6B2737; text-transform:uppercase; letter-spacing:1px;");
        bl->addWidget(t);
        QFrame *sep = new QFrame(); sep->setFrameShape(QFrame::HLine);
        sep->setStyleSheet("color:#C4923A;"); bl->addWidget(sep);
        for (const QString &line : lines) {
            QLabel *l = new QLabel(line); l->setStyleSheet("font-size:12px; color:#333;");
            l->setWordWrap(true); bl->addWidget(l);
        }
        return box;
    };

    QHBoxLayout *infoRow = new QHBoxLayout();
    infoRow->addWidget(makeInfoBox("", {
        "<b>CUIREA Management</b>", "Zone Industrielle, Tunis, Tunisie",
        "Tél : +216 71 000 000", "contact@cuirea.tn"
    }));
    infoRow->addSpacing(20);
    infoRow->addWidget(makeInfoBox("Facturé é", {
        mailClient.isEmpty() ? "Client interne" : mailClient,
        "Livraison prévue : " + dl
    }));
    lay->addLayout(infoRow);

    // -- TABLEAU ARTICLES ------------------------------------------------------
    QLabel *tblTitle = new QLabel("Détail de la commande");
    tblTitle->setStyleSheet("font-size:11px; font-weight:bold; color:#6B2737; text-transform:uppercase; letter-spacing:1px;");
    lay->addWidget(tblTitle);

    QTableWidget *table = new QTableWidget(1, 4, page);
    table->setHorizontalHeaderLabels({"Description", "Prix Unitaire", "Quantité", "Total"});
    table->horizontalHeader()->setStyleSheet(
        "QHeaderView::section { background:#6B2737; color:white; font-weight:bold; "
        "font-size:12px; padding:10px; border:none; }");
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    table->verticalHeader()->setVisible(false);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setShowGrid(false);
    table->setAlternatingRowColors(true);
    table->setStyleSheet(
        "QTableWidget { border:1px solid #E8D5C0; border-radius:6px; font-size:12px; }"
        "QTableWidget::item { padding:10px 12px; color:#333; }"
        "QTableWidget { alternate-background-color:#FBF5F0; }");
    table->setItem(0,0, new QTableWidgetItem(type + "  Réf. " + ref));
    table->setItem(0,1, new QTableWidgetItem(QString::number(ht,'f',2) + " DT"));
    table->setItem(0,2, new QTableWidgetItem("1"));
    table->setItem(0,3, new QTableWidgetItem(QString::number(ht,'f',2) + " DT"));
    table->setFixedHeight(table->horizontalHeader()->height() + table->rowHeight(0) + 4);
    lay->addWidget(table);

    // -- TOTAUX + QR -----------------------------------------------------------
    QHBoxLayout *totRow = new QHBoxLayout();

    // QR code à gauche
    QLabel *qrLabel = new QLabel("...");
    qrLabel->setFixedSize(110, 110);
    qrLabel->setAlignment(Qt::AlignCenter);
    qrLabel->setStyleSheet("border:2px solid #C4923A; border-radius:6px; background:white; font-size:10px; color:#888;");

    // Requéte DB pour enrichir l'URL avec les données complétes de l'article
    QString condUrl;
    {
        QString categoriePdf, couleurPdf, dimensionsPdf, prixPdf, statutArtPdf;
        QSqlQuery q2(Connection::instance()->getDatabase());
        q2.prepare("SELECT A.CATEGORIE, A.COULEUR_R, A.COULEUR_G, A.COULEUR_B, "
                   "A.LARGEUR, A.HAUTEUR, A.PROFONDEUR, A.PRIX_UNITAIRE, A.STATUT "
                   "FROM ARTICLES A "
                   "JOIN COMMANDES C ON UPPER(C.PRODUIT) = UPPER(A.NOM) "
                   "WHERE C.ID_COMMANDE = :id");
        q2.bindValue(":id", id.toInt());
        bool found = q2.exec() && q2.next();
        if (!found) {
            // fallback: search by name similarity
            q2.prepare("SELECT CATEGORIE, COULEUR_R, COULEUR_G, COULEUR_B, "
                       "LARGEUR, HAUTEUR, PROFONDEUR, PRIX_UNITAIRE, STATUT "
                       "FROM ARTICLES WHERE UPPER(NOM) = UPPER(:exacttype) "
                       "OR UPPER(NOM) LIKE UPPER(:type) "
                       "FETCH FIRST 1 ROWS ONLY");
            q2.bindValue(":exacttype", type);
            q2.bindValue(":type", "%" + type + "%");
            found = q2.exec() && q2.next();
        }
        if (found) {
            categoriePdf = q2.value(0).toString();
            int r2 = q2.value(1).toInt(), g2 = q2.value(2).toInt(), b2 = q2.value(3).toInt();
            couleurPdf = QString("rgb(%1,%2,%3)").arg(r2).arg(g2).arg(b2);
            double l2 = q2.value(4).toDouble(), h2 = q2.value(5).toDouble(), p2 = q2.value(6).toDouble();
            if (l2 > 0 || h2 > 0 || p2 > 0)
                dimensionsPdf = QString("%1x%2x%3 cm").arg(l2).arg(h2).arg(p2);
            prixPdf      = q2.value(7).toString() + " DT";
            statutArtPdf = q2.value(8).toString();
        }
        if (prixPdf.isEmpty())        prixPdf        = QString::number(ht, 'f', 2) + " DT";
        if (statutArtPdf.isEmpty())   statutArtPdf   = statut;
        if (categoriePdf.isEmpty())   categoriePdf   = "é";
        if (couleurPdf.isEmpty())     couleurPdf     = "é";
        if (dimensionsPdf.isEmpty())  dimensionsPdf  = "é";

        condUrl = QString("https://willowy-halva-d44d1d.netlify.app"
                          "?ref=%1&produit=%2&commande=%3&categorie=%4&type=%5&couleur=%6&dimensions=%7&prix=%8&statut=%9")
                  .arg(QString::fromUtf8(QUrl::toPercentEncoding(ref)),
                       QString::fromUtf8(QUrl::toPercentEncoding(type)),
                       QString::fromUtf8(QUrl::toPercentEncoding(id)),
                       QString::fromUtf8(QUrl::toPercentEncoding(categoriePdf)),
                       QString::fromUtf8(QUrl::toPercentEncoding(type)),
                       QString::fromUtf8(QUrl::toPercentEncoding(couleurPdf)),
                       QString::fromUtf8(QUrl::toPercentEncoding(dimensionsPdf)),
                       QString::fromUtf8(QUrl::toPercentEncoding(prixPdf)),
                       QString::fromUtf8(QUrl::toPercentEncoding(statutArtPdf)));
    }

    QLabel *qrLink = new QLabel(QString("<a href='%1' style='color:#C4923A;font-size:10px;'>Ouvrir la fiche</a>").arg(condUrl));
    qrLink->setOpenExternalLinks(true);
    qrLink->setAlignment(Qt::AlignCenter);

    QVBoxLayout *qrCol = new QVBoxLayout();
    qrCol->addWidget(qrLabel, 0, Qt::AlignCenter);
    qrCol->addWidget(qrLink,  0, Qt::AlignCenter);
    totRow->addLayout(qrCol);

    // Générer le QR via API
    {
        QNetworkAccessManager *mgr = new QNetworkAccessManager(page);
        QString qrApiUrl = QString("https://api.qrserver.com/v1/create-qr-code/?size=110x110&ecc=M&data=%1")
                           .arg(QString::fromUtf8(QUrl::toPercentEncoding(condUrl)));
        QNetworkReply *qrReply = mgr->get(QNetworkRequest(QUrl(qrApiUrl)));
        connect(qrReply, &QNetworkReply::finished, qrLabel, [qrLabel, qrReply]() {
            QPixmap px;
            if (px.loadFromData(qrReply->readAll()))
                qrLabel->setPixmap(px.scaled(110, 110, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            qrReply->deleteLater();
        });
    }

    totRow->addStretch();
    QWidget *totBox = new QWidget();
    totBox->setStyleSheet("background:#FBF5F0; border-radius:8px;");
    totBox->setFixedWidth(300);
    QVBoxLayout *totLay = new QVBoxLayout(totBox);
    totLay->setContentsMargins(16,14,16,14); totLay->setSpacing(6);

    auto addTotLine = [&](const QString &lbl, const QString &val) {
        QHBoxLayout *r = new QHBoxLayout();
        QLabel *l = new QLabel(lbl); QLabel *v = new QLabel(val);
        l->setStyleSheet("font-size:12px; color:#555;");
        v->setStyleSheet("font-size:12px; color:#555;");
        v->setAlignment(Qt::AlignRight);
        r->addWidget(l); r->addStretch(); r->addWidget(v);
        totLay->addLayout(r);
    };
    addTotLine("Sous-total HT",  QString::number(ht,'f',2)     + " DT");
    addTotLine("TVA (19%)",      QString::number(tva,'f',2)    + " DT");
    addTotLine("Remise (5%)",   "-" + QString::number(remise,'f',2) + " DT");

    QFrame *sepTot = new QFrame(); sepTot->setFrameShape(QFrame::HLine);
    sepTot->setStyleSheet("color:#C4923A;"); totLay->addWidget(sepTot);

    QWidget *ttcBox = new QWidget();
    ttcBox->setStyleSheet("background:#6B2737; border-radius:6px;");
    QHBoxLayout *ttcLay = new QHBoxLayout(ttcBox);
    ttcLay->setContentsMargins(12,10,12,10);
    QLabel *ttcL = new QLabel("Total TTC");
    QLabel *ttcV = new QLabel(QString::number(ttc,'f',2) + " DT");
    ttcL->setStyleSheet("color:white; font-weight:bold; font-size:14px;");
    ttcV->setStyleSheet("color:white; font-weight:bold; font-size:14px;");
    ttcV->setAlignment(Qt::AlignRight);
    ttcLay->addWidget(ttcL); ttcLay->addStretch(); ttcLay->addWidget(ttcV);
    totLay->addWidget(ttcBox);
    totRow->addWidget(totBox);
    lay->addLayout(totRow);

    // -- NOTE -----------------------------------------------------------------
    QWidget *noteBox = new QWidget();
    noteBox->setStyleSheet("background:#FBF5F0; border-left:4px solid #C4923A; border-radius:4px;");
    QVBoxLayout *noteLay = new QVBoxLayout(noteBox);
    noteLay->setContentsMargins(14,10,14,10);
    QLabel *noteT = new QLabel("Note");
    noteT->setStyleSheet("font-weight:bold; font-size:11px; color:#C4923A;");
    QLabel *noteV = new QLabel("Priorité : " + priorite + "  |  Statut : " + statut + "  |  Créée le : " + dc);
    noteV->setStyleSheet("font-size:12px; color:#555;");
    noteLay->addWidget(noteT); noteLay->addWidget(noteV);
    lay->addWidget(noteBox);

    // -- PIED DE PAGE ---------------------------------------------------------
    lay->addWidget(makeSep());
    QHBoxLayout *footer = new QHBoxLayout();
    auto makeFooterCol = [](const QString &title, const QStringList &lines) {
        QVBoxLayout *col = new QVBoxLayout();
        QLabel *t = new QLabel(title);
        t->setStyleSheet("font-size:10px; font-weight:bold; color:#6B2737; text-transform:uppercase;");
        col->addWidget(t);
        for (const QString &l : lines) {
            QLabel *lbl = new QLabel(l); lbl->setStyleSheet("font-size:11px; color:#777;");
            col->addWidget(lbl);
        }
        return col;
    };
    footer->addLayout(makeFooterCol("Contact",    {"contact@cuirea.tn", "+216 71 000 000"}));
    footer->addStretch();
    footer->addLayout(makeFooterCol("Paiement",   {"Virement bancaire", "IBAN : TN59 XXXX XXXX"}));
    footer->addStretch();
    footer->addLayout(makeFooterCol("Conditions", {"Paiement sous 30 jours", "Pénalités : 1,5%/mois"}));
    lay->addLayout(footer);

    scroll->setWidget(page);
    root->addWidget(scroll);

    // -- BOUTONS ---------------------------------------------------------------
    QHBoxLayout *btns = new QHBoxLayout();
    QPushButton *pdf   = new QPushButton("Exporter PDF", &dlg);
    QPushButton *email = new QPushButton("Envoyer Email", &dlg);
    QPushButton *close = new QPushButton("Fermer",       &dlg);
    pdf->setStyleSheet(
        "QPushButton{background:#C4923A;color:white;border:none;border-radius:6px;"
        "padding:9px 22px;font-size:13px;font-weight:bold;}"
        "QPushButton:hover{background:#A87730;}");
    email->setStyleSheet(
        "QPushButton{background:#8D6E63;color:white;border:none;border-radius:6px;"
        "padding:9px 22px;font-size:13px;font-weight:bold;}"
        "QPushButton:hover{background:#A0826D;}");
    close->setStyleSheet(
        "QPushButton{background:#6B2737;color:white;border:none;border-radius:6px;"
        "padding:9px 22px;font-size:13px;font-weight:bold;}"
        "QPushButton:hover{background:#4E1A27;}");
    btns->addStretch(); btns->addWidget(pdf); btns->addWidget(email); btns->addWidget(close);
    root->addLayout(btns);

    // HTML pour export PDF fidèle au design Qt affiché
    QString htmlPdf = QString(R"(
<html><head><meta charset='UTF-8'><style>
  * { margin:0; padding:0; box-sizing:border-box; }
  body { font-family: Arial, sans-serif; background: #FBF5F0; color: #3E1020; padding: 36px; }

  /* EN-TéTE */
  .header { display:table; width:100%%; margin-bottom:18px; }
  .header-left  { display:table-cell; vertical-align:top; }
  .header-right { display:table-cell; vertical-align:top; text-align:right; }
  .logo  { font-size:30px; font-weight:bold; color:#C4923A; letter-spacing:3px; }
  .sub   { font-size:11px; color:#888; margin-top:4px; }
  .facture-title { font-size:34px; font-weight:bold; color:#6B2737; letter-spacing:4px; }
  .facture-num   { font-size:13px; color:#555; margin-top:6px; }
  .facture-date  { font-size:12px; color:#888; margin-top:3px; }

  hr.sep { border:none; border-top:1px solid #E0E0E0; margin:16px 0; }

  /* INFOS BOXES */
  .info-row { display:table; width:100%%; margin-bottom:20px; }
  .info-box { display:table-cell; width:48%%; background:#FBF5F0;
              border-left:3px solid #C4923A; border-radius:8px;
              padding:12px 14px; font-size:12px; color:#333; vertical-align:top; }
  .info-box + .info-box { padding-left:20px; }
  .info-spacer { display:table-cell; width:4%%; }
  .info-title { font-size:10px; font-weight:bold; color:#6B2737;
                text-transform:uppercase; letter-spacing:1px; margin-bottom:6px; }
  .info-sep { border:none; border-top:1px solid #C4923A; margin:5px 0 8px 0; }

  /* SECTION TITRE */
  .section-title { font-size:11px; font-weight:bold; color:#6B2737;
                   text-transform:uppercase; letter-spacing:1px; margin-bottom:8px; }

  /* TABLEAU ARTICLES */
  table.items { width:100%%; border-collapse:collapse; border:1px solid #E8D5C0;
                border-radius:6px; margin-bottom:20px; }
  table.items th { background:#6B2737; color:white; font-size:12px; font-weight:bold;
                   padding:10px 12px; text-align:left; }
  table.items td { padding:10px 12px; font-size:12px; border-bottom:1px solid #E8D5C0; color:#333; }
  table.items tr:last-child td { border-bottom:none; }

  /* TOTAUX */
  .totals-wrap { text-align:right; margin-bottom:20px; }
  table.totals { display:inline-table; width:300px; background:#FBF5F0;
                 border-radius:8px; padding:14px 16px; }
  table.totals td { font-size:12px; color:#555; padding:4px 0; }
  table.totals td.val { text-align:right; }
  hr.tot-sep { border:none; border-top:1px solid #C4923A; margin:8px 0; }
  .ttc-row { background:#6B2737; border-radius:6px; }
  .ttc-row td { color:white; font-weight:bold; font-size:14px; padding:10px 12px; }

  /* NOTE */
  .note-box { background:#FBF5F0; border-left:4px solid #C4923A;
              border-radius:4px; padding:10px 14px; margin-bottom:20px; }
  .note-title { font-weight:bold; font-size:11px; color:#C4923A; margin-bottom:4px; }
  .note-val   { font-size:12px; color:#555; }

  /* PIED DE PAGE */
  .footer-row { display:table; width:100%%; margin-top:10px; }
  .footer-col { display:table-cell; font-size:11px; color:#777; vertical-align:top; }
  .footer-col-title { font-size:10px; font-weight:bold; color:#6B2737;
                      text-transform:uppercase; margin-bottom:4px; }
</style></head><body>

<!-- EN-TéTE -->
<div class='header'>
  <div class='header-left'>
    <div class='logo'>CUIREA</div>
    <div class='sub'>Smart Leather Goods Factory</div>
  </div>
  <div class='header-right'>
    <div class='facture-title'>FACTURE</div>
    <div class='facture-num'>N&deg; %1</div>
    <div class='facture-date'>Date : %2</div>
  </div>
</div>
<hr class='sep'/>

<!-- INFOS -->
<div class='info-row'>
  <div class='info-box'>
    <div class='info-title'>&nbsp;</div>
    <hr class='info-sep'/>
    <b>CUIREA Management</b><br/>
    Zone Industrielle, Tunis, Tunisie<br/>
    T&eacute;l : +216 71 000 000<br/>
    contact@cuirea.tn
  </div>
  <div class='info-spacer'></div>
  <div class='info-box'>
    <div class='info-title'>Factur&eacute; &agrave;</div>
    <hr class='info-sep'/>
    %3<br/>
    Livraison pr&eacute;vue : %4
  </div>
</div>

<!-- TABLEAU -->
<div class='section-title'>D&eacute;tail de la commande</div>
<table class='items'>
  <tr>
    <th>Description</th>
    <th>Prix Unitaire</th>
    <th>Quantit&eacute;</th>
    <th>Total</th>
  </tr>
  <tr>
    <td>%5 &mdash; R&eacute;f. %1</td>
    <td>%6 DT</td>
    <td>1</td>
    <td>%6 DT</td>
  </tr>
</table>

<!-- TOTAUX + QR -->
<div style='display:table;width:100%%;margin-bottom:20px;'>
  <div style='display:table-cell;vertical-align:middle;width:130px;'>
    <img src='data:image/png;base64,%13' width='100' height='100' style='border:2px solid #C4923A;border-radius:6px;'/>
  </div>
  <div style='display:table-cell;vertical-align:top;text-align:right;'>
  <table class='totals'>
    <tr><td>Sous-total HT</td><td class='val'>%6 DT</td></tr>
    <tr><td>TVA (19%%)</td>   <td class='val'>%7 DT</td></tr>
    <tr><td>Remise (5%%)</td> <td class='val'>-%8 DT</td></tr>
    <tr><td colspan='2'><hr class='tot-sep'/></td></tr>
    <tr class='ttc-row'>
      <td><b>Total TTC</b></td>
      <td class='val'><b>%9 DT</b></td>
    </tr>
  </table>
  </div>
</div>

<!-- NOTE -->
<div class='note-box'>
  <div class='note-title'>Note</div>
  <div class='note-val'>Priorit&eacute; : %10 &nbsp;|&nbsp; Statut : %11 &nbsp;|&nbsp; Cr&eacute;&eacute;e le : %12</div>
</div>

<hr class='sep'/>

<!-- PIED DE PAGE -->
<div class='footer-row'>
  <div class='footer-col'>
    <div class='footer-col-title'>Contact</div>
    contact@cuirea.tn<br/>+216 71 000 000
  </div>
  <div class='footer-col' style='text-align:center;'>
    <div class='footer-col-title'>Paiement</div>
    Virement bancaire<br/>IBAN : TN59 XXXX XXXX
  </div>
  <div class='footer-col' style='text-align:right;'>
    <div class='footer-col-title'>Conditions</div>
    Paiement sous 30 jours<br/>P&eacute;nalit&eacute;s : 1,5%%/mois
  </div>
</div>

</body></html>)")
    .arg(ref,                                          // %1
         QDate::currentDate().toString("dd MMMM yyyy"), // %2
         mailClient.isEmpty() ? "Client interne" : mailClient, // %3
         dl,                                           // %4
         type,                                         // %5
         QString::number(ht,'f',2),                    // %6
         QString::number(tva,'f',2),                   // %7
         QString::number(remise,'f',2),                // %8
         QString::number(ttc,'f',2),                   // %9
         priorite,                                     // %10
         statut,                                       // %11
         dc,                                           // %12
         QString::fromUtf8(QUrl::toPercentEncoding(condUrl))); // %13

    connect(pdf, &QPushButton::clicked, [&, page, qrLink] {
        QString fn = QFileDialog::getSaveFileName(&dlg, "Enregistrer",
            "Facture_" + ref + ".pdf", "PDF (*.pdf)");
        if (fn.isEmpty()) return;

        // Cacher "Ouvrir la fiche" dans le PDF
        qrLink->hide();
        page->repaint();
        QApplication::processEvents();

        QPixmap grab = page->grab();

        // Remettre visible dans le dialog
        qrLink->show();

        QPrinter p(QPrinter::HighResolution);
        p.setOutputFormat(QPrinter::PdfFormat);
        p.setOutputFileName(fn);
        p.setPageSize(QPageSize::A4);
        p.setPageMargins(QMarginsF(5, 5, 5, 5), QPageLayout::Millimeter);

        QPainter painter(&p);
        QRectF pageRect = p.pageRect(QPrinter::DevicePixel);
        QImage img = grab.toImage();
        QSizeF scaled(img.size().scaled(pageRect.size().toSize(), Qt::KeepAspectRatio));
        QPointF offset((pageRect.width()  - scaled.width())  / 2.0,
                       (pageRect.height() - scaled.height()) / 2.0);
        painter.drawImage(QRectF(offset, scaled), img);
        painter.end();

        QDesktopServices::openUrl(QUrl::fromLocalFile(fn));
        QMessageBox::information(&dlg, "Succès", "Facture exportée :\n" + fn);
    });
    //fonction mail client
    connect(email, &QPushButton::clicked, [&] {

        if (mailClient.isEmpty()) {
            QMessageBox::warning(&dlg, "Email manquant",
                                 "? Aucun email client associé à cette commande.");
            return;
        }

        Mail mailer;

        // -- SUBJECT ---------------------------------------------
        QString subject = "Facture CUIREA - Commande #" + ref;

        // -- BODY HTML -------------------------------------------
        QString body =
            "<html>"
            "<body style='font-family:Arial; background:#f9f9f9; padding:20px;'>"

            "<div style='max-width:600px;margin:auto;background:white;padding:25px;border-radius:10px;'>"

            "<h2 style='color:#6B2737;'>CUIREA</h2>"

            "<p>Bonjour <b>" + mailClient+ "</b>,</p>"

            "<p>"
            "Nous vous informons que votre commande a été traitée avec succès."
            "</p>"


            "<p><b>Employé responsable :</b> " + employe + "</p>"
            "<p><b>Référence commande :</b> " + ref + "</p>"
            "<p><b>Produit :</b> " + type + "</p>"

            "<hr>"

            "<p>"
            "Votre facture officielle est disponible en piéce jointe de cet email."
            "</p>"

            "<br>"

            "<p>Cordialement,</p>"
            "<p><b>L'équipe CUIREA</b></p>"

            "<hr>"

            "<p style='font-size:12px;color:gray;text-align:center;'>"
            "Cet email est généré automatiquement suite à votre commande."
            "</p>"

            "</div>"

            "<hr>"

            "<footer style='font-size:12px;color:gray;text-align:center;'>"

            "<b>CUIREA</b><br>"
            "Smart Leather Goods Factory<br>"
            "Zone Industrielle, Tunis, Tunisie<br><br>"

            "<b>Contact</b><br>"
            "Email : contact@cuirea.tn<br>"
            "Tél : +216 71 000 000<br>"

            "</footer>"

            "</body>"
            "</html>";;

        // -- FILE PATH (ATTACHMENT) -----------------------------
        // ?? This is the facture file that will be attached to the email
        QString attachmentPath = "C:/Users/USER/Desktop/facture/facture_" + ref + ".pdf";

        // -- SEND EMAIL ------------------------------------------
        bool ok = mailer.sendEmail(mailClient, subject, body, attachmentPath);

        if (ok)
            QMessageBox::information(&dlg, "Email envoyé",
                                     "? Email envoyé é " + mailClient);
        else
            QMessageBox::critical(&dlg, "Erreur",
                                  "? échec de l'envoi é " + mailClient);

    }); // FIN DU connect
    connect(close, &QPushButton::clicked, &dlg, &QDialog::accept);
    dlg.exec();
}

void MainWindow::onExcelProduction()
{
    BilanDialog bilan(this);
    bilan.exec();
}

void MainWindow::onRechercherProduction(const QString &text)
{
    if (text.isEmpty()) {
        loadProductionData();
        return;
    }

    ui->productionTable->setRowCount(0);

    ProductionDAO dao;
    QSqlQueryModel* model = dao.rechercher(text);

    if (!model) {
        qDebug() << "? Erreur lors de la recherche des commandes";
        return;
    }

    for (int i = 0; i < model->rowCount(); ++i) {
        int row = ui->productionTable->rowCount();
        ui->productionTable->insertRow(row);

        // Même mapping que loadProductionData()
        // SQL: 0=ID_COMMANDE, 1=REFERENCE, 2=EMPLOYE, 3=TYPE,
        //      4=DATE_CREATION, 5=DATE_LIVRAISON, 6=STATUT, 7=PRIORITE, 8=MONTANT

        ui->productionTable->setItem(row, 0, new QTableWidgetItem(model->data(model->index(i, 0)).toString()));
        ui->productionTable->setItem(row, 1, new QTableWidgetItem(model->data(model->index(i, 1)).toString()));
        ui->productionTable->setItem(row, 2, new QTableWidgetItem(model->data(model->index(i, 2)).toString()));
        ui->productionTable->setItem(row, 3, new QTableWidgetItem(model->data(model->index(i, 3)).toString()));
        ui->productionTable->setItem(row, 4, new QTableWidgetItem(QString::number(model->data(model->index(i, 8)).toDouble(), 'f', 2) + " DT"));
        // Col 5 : état Paiement
        {
            QString etat = model->data(model->index(i, 10)).toString();
            if (etat.isEmpty()) etat = "Non payée";
            QTableWidgetItem *etatItem = new QTableWidgetItem(etat);
            etatItem->setTextAlignment(Qt::AlignCenter);
            if (etat.toLower() == "payée" || etat.toLower() == "payee")
                etatItem->setBackground(QColor("#27AE60")), etatItem->setForeground(Qt::white);
            else
                etatItem->setBackground(QColor("#E74C3C")), etatItem->setForeground(Qt::white);
            ui->productionTable->setItem(row, 5, etatItem);
        }
        ui->productionTable->setItem(row, 6, new QTableWidgetItem(model->data(model->index(i, 4)).toDate().toString("dd/MM/yyyy")));
        {
            QDate dl = model->data(model->index(i, 5)).toDate();
            ui->productionTable->setItem(row, 7, new QTableWidgetItem(dl.isValid() ? dl.toString("dd/MM/yyyy") : "-"));
        }
        ui->productionTable->setItem(row, 8, new QTableWidgetItem(model->data(model->index(i, 6)).toString()));
        ui->productionTable->setItem(row, 9, new QTableWidgetItem(model->data(model->index(i, 7)).toString()));
        ui->productionTable->setItem(row, 10, new QTableWidgetItem(model->data(model->index(i, 9)).toString()));
    }

    delete model;
    updateProductionStatsCards();
}

void MainWindow::onProductionTableContextMenu(const QPoint &pos)
{
    QMenu menu(this);
    menu.setStyleSheet("QMenu{background:#FAF5F0;border:1px solid #BCAAA4;}"
                       "QMenu::item{padding:7px 22px;color:#291C0E;}"
                       "QMenu::item:selected{background:#8D6E63;color:white;}");
    auto *voir    = menu.addAction("Voir suivi");
    auto *modif   = menu.addAction("Modifier");
    menu.addSeparator();
    auto *supp    = menu.addAction("Supprimer");
    menu.addSeparator();
    auto *fact    = menu.addAction("Générer facture");
    auto *stats   = menu.addAction("Statistiques");

    auto *act = menu.exec(ui->productionTable->viewport()->mapToGlobal(pos));
    if      (act == voir)  onSuiviProduction();
    else if (act == modif) onModifierProduction();
    else if (act == supp)  onSupprimerProduction();
    else if (act == fact)  onFactureProduction();
    else if (act == stats) onStatistiquesProduction();
}

void MainWindow::onSupprimerProduction()
{
    int row = ui->productionTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Attention", "Sélectionnez une commande.");
        return;
    }
    
    QString ref = cellText(ui->productionTable, row, 1);
    QString employe = cellText(ui->productionTable, row, 2);
    int id = cellText(ui->productionTable, row, 0).toInt();
    
    if (QMessageBox::question(this, "Confirmer",
            QString("Supprimer la commande %1 de l'employe %2 ?").arg(ref, employe),
            QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        
        ProductionDAO dao;
        if (dao.supprimer(id)) {
            NotificationWidget::show("Commande supprimee", "La commande a ete supprimee.", NotificationWidget::Warning);
            QMessageBox::information(this, "Succes", "Commande supprimee avec succes!");
            loadProductionData();
        } else {
            QMessageBox::critical(this, "Erreur", "Erreur lors de la suppression de la commande.");
        }
    }
}

void MainWindow::onStatistiquesProduction() { afficherStatistiquesModernes(); }

void MainWindow::afficherStatistiquesModernes()
{
    // -- Palette Bordeaux & Or ------------------------------------------------
    static const QString C_BORDEAUX     = "#6B2737";
    static const QString C_OR           = "#C4923A";
    static const QString C_OR_PALE      = "#D4B896";
    static const QString C_BORDEAUX_MID = "#A0485A";
    static const QString C_CREME        = "#FBF5F0";
    static const QString C_GRIS         = "#8C7B6B";
    static const QString C_BLANC        = "#FFFFFF";
    static const QString C_BORDURE      = "#E8DDD5";

    QSqlDatabase db = Connection::instance()->getDatabase();
    if (!db.isOpen()) { QMessageBox::warning(this,"Erreur","Base de données non connectée."); return; }

    QSqlQuery query(db);
    query.exec("SELECT COUNT(*), NVL(SUM(MONTANT),0) FROM COMMANDES");
    int total = 0; double montantTotal = 0;
    if (query.next()) { total = query.value(0).toInt(); montantTotal = query.value(1).toDouble(); }
    if (total == 0) { showInfo(this,"Statistiques","Aucune commande dans la base de données."); return; }

    QMap<QString,int> statCnt, prioCnt;
    query.exec("SELECT STATUT, COUNT(*) FROM COMMANDES GROUP BY STATUT");
    while (query.next()) statCnt[query.value(0).toString()] = query.value(1).toInt();
    query.exec("SELECT PRIORITE, COUNT(*) FROM COMMANDES GROUP BY PRIORITE");
    while (query.next()) prioCnt[query.value(0).toString()] = query.value(1).toInt();

    int enAttente  = statCnt.value("En Attente", 0);
    int terminees  = statCnt.value("Terminé", 0);
    double taux    = total > 0 ? 100.0 * terminees / total : 0;
    double moyenne = total > 0 ? montantTotal / total : 0;

    // -- Dialogue -------------------------------------------------------------
    QDialog dlg(this);
    dlg.setWindowTitle("Statistiques de Production");
    dlg.setMinimumSize(1100, 750);
    dlg.setStyleSheet(QString("QDialog{background:%1;border-radius:10px;}").arg(C_CREME));

    QVBoxLayout *dlay = new QVBoxLayout(&dlg);
    dlay->setContentsMargins(0,0,0,0);
    dlay->setSpacing(0);

    // -- HEADER ---------------------------------------------------------------
    QWidget *header = new QWidget();
    header->setStyleSheet(QString("background:%1;").arg(C_BORDEAUX));
    QHBoxLayout *hdrLay = new QHBoxLayout(header);
    hdrLay->setContentsMargins(24,14,24,14);

    QLabel *titleLbl = new QLabel("STATISTIQUES DE PRODUCTION");
    titleLbl->setStyleSheet("font-size:18px;font-weight:bold;color:white;letter-spacing:2px;");

    QLabel *dateLbl = new QLabel(QDate::currentDate().toString("dd/MM/yyyy") + "  |  Tous");
    dateLbl->setStyleSheet(QString("font-size:12px;color:%1;font-weight:bold;").arg(C_OR));

    QPushButton *closeBtn = new QPushButton("✕");
    closeBtn->setFixedSize(32,32);
    closeBtn->setStyleSheet(
        "QPushButton{background:rgba(255,255,255,0.15);color:white;border:none;"
        "border-radius:16px;font-size:14px;font-weight:bold;}"
        "QPushButton:hover{background:rgba(255,255,255,0.3);}");
    connect(closeBtn, &QPushButton::clicked, &dlg, &QDialog::accept);

    hdrLay->addWidget(titleLbl);
    hdrLay->addStretch();
    hdrLay->addWidget(dateLbl);
    hdrLay->addSpacing(16);
    hdrLay->addWidget(closeBtn);
    dlay->addWidget(header);

    // -- SCROLL AREA ----------------------------------------------------------
    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet(QString("QScrollArea{border:none;background:%1;}").arg(C_CREME));
    QWidget *inner = new QWidget();
    inner->setStyleSheet(QString("background:%1;").arg(C_CREME));
    QVBoxLayout *lay = new QVBoxLayout(inner);
    lay->setSpacing(16);
    lay->setContentsMargins(24,20,24,20);

    // -- KPI CARDS ------------------------------------------------------------
    auto makeKPI = [&](const QString &icon, const QString &val, const QString &label,
                       const QString &sublabel) {
        QWidget *card = new QWidget();
        card->setStyleSheet(QString(
            "QWidget{background:%1;border-radius:10px;"
            "border-left:4px solid %2;}").arg(C_BLANC, C_OR));
        QVBoxLayout *cl = new QVBoxLayout(card);
        cl->setContentsMargins(16,14,16,14);
        cl->setSpacing(4);

        QLabel *ic = new QLabel(icon);
        ic->setStyleSheet(QString("font-size:16px;color:%1;border:none;").arg(C_OR));

        QLabel *vl = new QLabel(val);
        vl->setStyleSheet(QString("font-size:22px;font-weight:500;color:%1;border:none;").arg(C_BORDEAUX));

        QLabel *lb = new QLabel(label);
        lb->setStyleSheet(QString("font-size:12px;color:%1;border:none;").arg(C_GRIS));
        lb->setWordWrap(true);

        QLabel *sl = new QLabel(sublabel);
        sl->setStyleSheet(QString("font-size:11px;color:%1;font-weight:bold;border:none;").arg(C_OR));

        cl->addWidget(ic);
        cl->addWidget(vl);
        cl->addWidget(lb);
        cl->addWidget(sl);
        return card;
    };

    QWidget *kpiRow = new QWidget();
    kpiRow->setStyleSheet("background:transparent;");
    QHBoxLayout *kpiLay = new QHBoxLayout(kpiRow);
    kpiLay->setSpacing(12);
    kpiLay->setContentsMargins(0,0,0,0);

    double pctAttente = total > 0 ? 100.0 * enAttente / total : 0.0;
    int enCours = statCnt.value("En Cours", 0) + statCnt.value("En Production", 0);
    double pctEnCours = total > 0 ? 100.0 * enCours / total : 0.0;

    kpiLay->addWidget(makeKPI("📦",
        QString::number(total),
        "Total commandes",
        QString::number(enAttente) + " en attente ("
            + QString::number(pctAttente,'f',1) + "%)"));
    kpiLay->addWidget(makeKPI("💰",
        QLocale(QLocale::French).toString(montantTotal,'f',0) + " DT",
        "Chiffre d'affaires",
        "Moy. " + QLocale(QLocale::French).toString(moyenne,'f',0) + " DT/cmd"));
    kpiLay->addWidget(makeKPI("🔄",
        QString::number(enCours),
        "En cours de production",
        QString::number(pctEnCours,'f',1) + "% du total"));
    kpiLay->addWidget(makeKPI("✅",
        QString::number(taux,'f',1) + "%",
        "Taux complétion",
        QString::number(terminees) + " / " + QString::number(total) + " terminées"));
    lay->addWidget(kpiRow);

    // -- GRAPHIQUES -----------------------------------------------------------
    QWidget *chartsRow = new QWidget();
    chartsRow->setStyleSheet("background:transparent;");
    QHBoxLayout *chartsLay = new QHBoxLayout(chartsRow);
    chartsLay->setSpacing(16);
    chartsLay->setContentsMargins(0,0,0,0);

    auto wrapChart = [&](QChartView *cv, const QString &title) {
        QWidget *card = new QWidget();
        card->setStyleSheet(QString(
            "QWidget{background:%1;border-radius:10px;"
            "border:0.5px solid %2;}").arg(C_BLANC, C_BORDURE));
        QVBoxLayout *cl = new QVBoxLayout(card);
        cl->setContentsMargins(16,14,16,14);
        cl->setSpacing(8);
        QLabel *t = new QLabel(title);
        t->setAlignment(Qt::AlignCenter);
        t->setStyleSheet(QString("font-size:14px;font-weight:bold;color:%1;border:none;").arg(C_BORDEAUX));
        cl->addWidget(t);
        cv->setMinimumHeight(280);
        cv->setStyleSheet("background:transparent;border:none;");
        cl->addWidget(cv);
        return card;
    };

    // ── Camembert statuts ─────────────────────────────────────────────────────
    QWidget *pieCard = new QWidget();
    pieCard->setStyleSheet(QString(
        "QWidget{background:%1;border-radius:10px;border:0.5px solid %2;}").arg(C_BLANC, C_BORDURE));
    QVBoxLayout *pieLay = new QVBoxLayout(pieCard);
    pieLay->setContentsMargins(16,14,16,14);
    pieLay->setSpacing(6);

    QLabel *pieTitle = new QLabel("Répartition des statuts");
    pieTitle->setAlignment(Qt::AlignCenter);
    pieTitle->setStyleSheet(QString("font-size:14px;font-weight:bold;color:%1;border:none;").arg(C_BORDEAUX));
    pieLay->addWidget(pieTitle);

    auto *pie = new QPieSeries();
    pie->setHoleSize(0.42);
    QMap<QString,QColor> statClrs;
    statClrs["En Attente"]    = QColor(C_OR);
    statClrs["Suspendu"]      = QColor(C_OR_PALE);
    statClrs["Terminé"]       = QColor(C_BORDEAUX);
    statClrs["En Production"] = QColor(C_BORDEAUX_MID);
    statClrs["Planifié"]      = QColor("#B8956A");
    statClrs["Annulé"]        = QColor("#7A3545");
    statClrs["En Cours"]      = QColor("#8C5A6A");
    statClrs["En livraison"]  = QColor("#6D8B74");

    QLabel *clickLabel = new QLabel();
    clickLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    clickLabel->setStyleSheet(
        "QLabel { background: transparent; border: none; padding: 2px 4px; }");
    clickLabel->setWordWrap(false);
    clickLabel->setTextFormat(Qt::RichText);
    pieLay->addWidget(clickLabel); // ← en haut, juste sous le titre

    for (auto it = statCnt.begin(); it != statCnt.end(); ++it) {
        double pct = total > 0 ? 100.0 * it.value() / total : 0.0;
        auto *sl = pie->append(QString("%1%").arg(pct, 0, 'f', 1), it.value());
        sl->setColor(statClrs.value(it.key(), QColor(C_GRIS)));
        sl->setBorderColor(QColor(C_BLANC));
        sl->setLabelVisible(false);

        // Hover : explode
        QObject::connect(sl, &QPieSlice::hovered, [sl](bool on){ sl->setExploded(on); });

        // Clic : afficher "statut (XX.X%)" sous le graphique
        QString statut = it.key();
        QString pctStr = QString::number(pct, 'f', 1);
        QColor  clr    = statClrs.value(statut, QColor(C_GRIS));
        QObject::connect(sl, &QPieSlice::clicked, clickLabel,
            [clickLabel, statut, pctStr, clr]() {
                clickLabel->setText(
                    QString("<span style='color:%1;text-decoration:underline;font-weight:bold;"
                            "font-size:13px;'>%2 (%3%)</span>")
                    .arg(clr.name()).arg(statut).arg(pctStr));
                clickLabel->setStyleSheet(
                    "QLabel { background: transparent; border: none; padding: 2px 4px; }");
                clickLabel->setTextFormat(Qt::RichText);
            });
    }

    auto *pc = new QChart();
    pc->addSeries(pie);
    pc->setBackgroundBrush(QColor(C_BLANC));
    pc->setBackgroundRoundness(0);
    pc->setMargins(QMargins(4, 4, 4, 4));
    pc->setAnimationOptions(QChart::AllAnimations);
    pc->legend()->setVisible(false);

    auto *pv = new QChartView(pc);
    pv->setRenderHint(QPainter::Antialiasing);
    pv->setFixedHeight(260);
    pieLay->addWidget(pv);

    // Tableau légende custom — toujours lisible, jamais tronqué
    QWidget *legendTable = new QWidget();
    legendTable->setStyleSheet("background:transparent;border:none;");
    QGridLayout *legendGrid = new QGridLayout(legendTable);
    legendGrid->setContentsMargins(4, 4, 4, 4);
    legendGrid->setHorizontalSpacing(16);
    legendGrid->setVerticalSpacing(6);

    int col = 0, row = 0;
    for (auto it = statCnt.begin(); it != statCnt.end(); ++it) {
        double pct = total > 0 ? 100.0 * it.value() / total : 0.0;
        QColor clr = statClrs.value(it.key(), QColor(C_GRIS));

        QWidget *item = new QWidget();
        item->setStyleSheet("background:transparent;border:none;");
        QHBoxLayout *itemLay = new QHBoxLayout(item);
        itemLay->setContentsMargins(0,0,0,0);
        itemLay->setSpacing(6);

        // Pastille couleur
        QLabel *dot = new QLabel();
        dot->setFixedSize(12, 12);
        dot->setStyleSheet(QString(
            "background:%1;border-radius:6px;border:none;").arg(clr.name()));

        // Texte complet : "Statut — N (XX.X%)"
        QLabel *txt = new QLabel(QString("%1 — %2 (%3%)")
            .arg(it.key())
            .arg(it.value())
            .arg(pct, 0, 'f', 1));
        txt->setStyleSheet(QString(
            "color:%1;font-size:11px;font-weight:500;background:transparent;border:none;").arg(C_GRIS));
        txt->setWordWrap(false);

        itemLay->addWidget(dot);
        itemLay->addWidget(txt);
        itemLay->addStretch();

        legendGrid->addWidget(item, row, col);
        col++;
        if (col >= 2) { col = 0; row++; }
    }
    pieLay->addWidget(legendTable);
    chartsLay->addWidget(pieCard, 1);

    // ── Barres priorités ──────────────────────────────────────────────────────
    QWidget *barCard = new QWidget();
    barCard->setStyleSheet(QString(
        "QWidget{background:%1;border-radius:10px;border:0.5px solid %2;}").arg(C_BLANC, C_BORDURE));
    QVBoxLayout *barLay = new QVBoxLayout(barCard);
    barLay->setContentsMargins(16,14,16,14);
    barLay->setSpacing(8);

    QLabel *barTitle = new QLabel("Répartition des priorités");
    barTitle->setAlignment(Qt::AlignCenter);
    barTitle->setStyleSheet(QString("font-size:14px;font-weight:bold;color:%1;border:none;").arg(C_BORDEAUX));
    barLay->addWidget(barTitle);

    QStringList prioOrder = {"Urgente","Haute","Normale","Basse"};
    QMap<QString,QColor> prioClrs;
    prioClrs["Urgente"] = QColor(C_BORDEAUX);
    prioClrs["Haute"]   = QColor(C_BORDEAUX_MID);
    prioClrs["Normale"] = QColor(C_OR);
    prioClrs["Basse"]   = QColor(C_OR_PALE);

    QStringList cats;
    QList<int>  vals;
    for (const QString &p : prioOrder) {
        if (!prioCnt.contains(p)) continue;
        cats << p;
        vals << prioCnt[p];
    }

    int maxCnt = 0;
    for (int v : vals) maxCnt = qMax(maxCnt, v);

    auto *bc = new QChart();
    bc->setBackgroundBrush(QColor(C_BLANC));
    bc->setBackgroundRoundness(0);
    bc->setMargins(QMargins(4,4,4,4));
    bc->setAnimationOptions(QChart::SeriesAnimations);
    bc->legend()->setVisible(false); // Légende Qt masquée — tableau custom en dessous

    auto *axX = new QBarCategoryAxis();
    axX->append(cats);
    axX->setLabelsColor(QColor(C_GRIS));
    axX->setLabelsFont(QFont("Arial", 10));
    axX->setGridLineVisible(false);
    bc->addAxis(axX, Qt::AlignBottom);

    auto *axY = new QValueAxis();
    axY->setRange(0, maxCnt + 1);
    axY->setLabelFormat("%d");
    axY->setLabelsColor(QColor(C_GRIS));
    axY->setGridLineColor(QColor(C_BORDURE));
    axY->setTickCount(maxCnt + 2);
    bc->addAxis(axY, Qt::AlignLeft);

    for (int i = 0; i < cats.size(); ++i) {
        const QString &p   = cats[i];
        int            cnt = vals[i];

        auto *set = new QBarSet(p); // label court = juste le nom
        set->setColor(prioClrs.value(p, QColor(C_GRIS)));
        set->setLabelColor(QColor(C_BORDEAUX));
        set->setBorderColor(Qt::transparent);
        for (int j = 0; j < cats.size(); ++j)
            *set << (j == i ? cnt : 0);

        auto *series = new QBarSeries();
        series->setLabelsVisible(true);
        series->setLabelsPosition(QAbstractBarSeries::LabelsOutsideEnd);
        series->setLabelsFormat("@value");
        series->setBarWidth(0.5);
        series->append(set);

        bc->addSeries(series);
        series->attachAxis(axX);
        series->attachAxis(axY);
    }

    auto *bv = new QChartView(bc);
    bv->setRenderHint(QPainter::Antialiasing);
    bv->setFixedHeight(260);
    barLay->addWidget(bv);

    // Tableau légende custom priorités — toujours lisible
    QWidget *prioLegend = new QWidget();
    prioLegend->setStyleSheet("background:transparent;border:none;");
    QGridLayout *prioGrid = new QGridLayout(prioLegend);
    prioGrid->setContentsMargins(4,4,4,4);
    prioGrid->setHorizontalSpacing(16);
    prioGrid->setVerticalSpacing(6);

    int pc2 = 0, pr2 = 0;
    for (int i = 0; i < cats.size(); ++i) {
        double pct = total > 0 ? 100.0 * vals[i] / total : 0.0;
        QColor clr = prioClrs.value(cats[i], QColor(C_GRIS));

        QWidget *item = new QWidget();
        item->setStyleSheet("background:transparent;border:none;");
        QHBoxLayout *itemLay = new QHBoxLayout(item);
        itemLay->setContentsMargins(0,0,0,0);
        itemLay->setSpacing(6);

        QLabel *dot = new QLabel();
        dot->setFixedSize(12, 12);
        dot->setStyleSheet(QString("background:%1;border-radius:3px;border:none;").arg(clr.name()));

        QLabel *txt = new QLabel(QString("%1 — %2 (%3%)")
            .arg(cats[i]).arg(vals[i]).arg(pct, 0, 'f', 1));
        txt->setStyleSheet(QString(
            "color:%1;font-size:11px;font-weight:500;background:transparent;border:none;").arg(C_GRIS));

        itemLay->addWidget(dot);
        itemLay->addWidget(txt);
        itemLay->addStretch();

        prioGrid->addWidget(item, pr2, pc2);
        pc2++;
        if (pc2 >= 2) { pc2 = 0; pr2++; }
    }
    barLay->addWidget(prioLegend);
    chartsLay->addWidget(barCard, 1);

    lay->addWidget(chartsRow);
    scroll->setWidget(inner);
    dlay->addWidget(scroll, 1);

    // -- FOOTER BOUTONS -------------------------------------------------------
    QWidget *footer = new QWidget();
    footer->setStyleSheet(QString(
        "QWidget{background:%1;border-top:0.5px solid %2;}").arg(C_CREME, C_BORDURE));
    QHBoxLayout *btnRow = new QHBoxLayout(footer);
    btnRow->setContentsMargins(24,10,24,10);
    btnRow->setSpacing(10);

    QPushButton *exportBtn = new QPushButton("⬇  Exporter CSV");
    exportBtn->setStyleSheet(QString(
        "QPushButton{background:%1;color:white;border:none;border-radius:6px;"
        "padding:9px 22px;font-size:13px;font-weight:bold;}"
        "QPushButton:hover{background:#A87730;}").arg(C_OR));

    QPushButton *fermerBtn = new QPushButton("✖  Fermer");
    fermerBtn->setStyleSheet(QString(
        "QPushButton{background:%1;color:white;border:none;border-radius:6px;"
        "padding:9px 22px;font-size:13px;font-weight:bold;}"
        "QPushButton:hover{background:#4E1A27;}").arg(C_BORDEAUX));

    connect(exportBtn, &QPushButton::clicked, [&]{ onExcelProduction(); });
    connect(fermerBtn, &QPushButton::clicked, &dlg, &QDialog::accept);

    btnRow->addStretch();
    btnRow->addWidget(exportBtn);
    btnRow->addWidget(fermerBtn);
    dlay->addWidget(footer);

    dlg.exec();
}

QPixmap MainWindow::generateQRCode(const QString &, int size)
{
    QPixmap p(size,size); p.fill(Qt::white); return p;
}

// -- Articles ------------------------------------------------------------------
void MainWindow::setupArticleTable()
{
    ui->articleTable->setColumnCount(11);
    ui->articleTable->setHorizontalHeaderLabels({
        "ID","Reference","Nom","Categorie","Type","Couleur",
        "Dimensions","Prix Unitaire","Cout Fabrication","Statut","Date Creation"
    });
    ui->articleTable->setColumnHidden(0, true);
    ui->articleTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->articleTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->articleTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->articleTable->verticalHeader()->setVisible(false);
}

void MainWindow::loadArticlesFromDB()
{
    articles.clear();
    
    // Charger les articles depuis la base de données
    QList<Article> articlesDB = Article::afficher();
    
    ui->articleTable->setRowCount(articlesDB.size());
    
    for (int i = 0; i < articlesDB.size(); ++i) {
        Article a = articlesDB[i];
        articles.append(a);
        
        ui->articleTable->setItem(i, 0, new QTableWidgetItem(QString::number(a.getIdArticle())));
        ui->articleTable->setItem(i, 1, new QTableWidgetItem(a.getReference()));
        ui->articleTable->setItem(i, 2, new QTableWidgetItem(a.getNom()));
        ui->articleTable->setItem(i, 3, new QTableWidgetItem(a.getCategorie()));
        ui->articleTable->setItem(i, 4, new QTableWidgetItem(a.getType()));
        ui->articleTable->setItem(i, 5, new QTableWidgetItem(a.getCouleur()));
        ui->articleTable->setItem(i, 6, new QTableWidgetItem(a.getDimensions()));
        ui->articleTable->setItem(i, 7, new QTableWidgetItem(QString::number(a.getPrixUnitaire(), 'f', 2) + " DT"));
        ui->articleTable->setItem(i, 8, new QTableWidgetItem(QString::number(a.getCoutFabrication(), 'f', 2) + " DT"));
        ui->articleTable->setItem(i, 9, new QTableWidgetItem(a.getStatut()));
        ui->articleTable->setItem(i, 10, new QTableWidgetItem(a.getDateCreation().toString("yyyy-MM-dd")));
    }
}

void MainWindow::refreshArticleTable() { loadArticlesFromDB(); updateArticleStatistics(); }

void MainWindow::updateArticleStatistics()
{
    int dispo=0, enProd=0;
    for (const Article &a : articles) {
        if (a.getStatut()=="disponible")  ++dispo;
        if (a.getStatut()=="en_production") ++enProd;
    }
    ui->statsValueArticle1->setText(QString::number(articles.size()));
    ui->statsValueArticle2->setText(QString::number(dispo));
    ui->statsValueArticle3->setText(QString::number(enProd));
}

void MainWindow::on_btnAddArticle_clicked()
{
    ArticleDialog dlg(this, ArticleDialog::AddMode);
    int nextId = articles.size() + 1;
    dlg.setArticleData(QString("ART-%1-%2").arg(QDate::currentDate().year()).arg(nextId,4,10,QChar('0')),
                       "","Sacs","Sac à main","sac_a_main.obj",
                       141,110,99, 30.0,25.0,10.0,
                       1.0,0.0,"disponible");
    
    if (dlg.exec() == QDialog::Accepted) {
        Article article;
        article.setReference(dlg.getReference());
        article.setNom(dlg.getNom());
        article.setCategorie(dlg.getCategorie());
        article.setType(dlg.getType());
        article.setModele3D(dlg.getModele3D());
        article.setCouleurR(dlg.getCouleurR());
        article.setCouleurG(dlg.getCouleurG());
        article.setCouleurB(dlg.getCouleurB());
        article.setLargeur(dlg.getLargeur());
        article.setHauteur(dlg.getHauteur());
        article.setProfondeur(dlg.getProfondeur());
        article.setPrixUnitaire(dlg.getPrixUnitaire());
        article.setCoutFabrication(dlg.getCoutFabrication());
        article.setStatut(dlg.getStatut());
        article.setDateCreation(QDate::currentDate());
        
        // Ajouter à la base de données
        if (article.ajouter()) {
            // Sauvegarder l'image generee dans la BD
            QString photoPath = dlg.getPhotoPath();
            if (!photoPath.isEmpty()) {
                Article::updateImagePath(article.getIdArticle(), photoPath);
            }
            QMessageBox::information(this, "Succes", "Article ajoute avec succes !");
            refreshArticleTable(); // Rafraéchir l'affichage
        } else {
            QMessageBox::critical(this, "Erreur", 
                "Impossible d'ajouter l'article.\n\n"
                "Vérifiez que:\n"
                "1. La table ARTICLES existe dans la base de données\n"
                "2. La connexion à la base de données est active\n"
                "3. La référence n'existe pas déjà\n\n"
                "Consultez 'Application Output' pour plus de détails.");
        }
    }
}

void MainWindow::on_btnEditArticle_clicked()
{
    int row = ui->articleTable->currentRow();
    if (row < 0) { QMessageBox::warning(this,"Attention","Selectionnez un article."); return; }
    
    // Recuperer l'ID de l'article selectionne
    int idArticle = ui->articleTable->item(row, 0)->text().toInt();
    Article article = Article::rechercherParId(idArticle);
    
    if (article.getIdArticle() == 0) {
        QMessageBox::warning(this, "Erreur", "Article introuvable.");
        return;
    }
    
    ArticleDialog dlg(this, ArticleDialog::EditMode);
    dlg.setArticleData(article.getReference(), article.getNom(), article.getCategorie(),
                       article.getType(), article.getModele3D(),
                       article.getCouleurR(), article.getCouleurG(), article.getCouleurB(),
                       article.getLargeur(), article.getHauteur(), article.getProfondeur(),
                       article.getPrixUnitaire(), article.getCoutFabrication(), article.getStatut());
    
    if (dlg.exec() == QDialog::Accepted) {
        article.setReference(dlg.getReference());
        article.setNom(dlg.getNom());
        article.setCategorie(dlg.getCategorie());
        article.setType(dlg.getType());
        article.setModele3D(dlg.getModele3D());
        article.setCouleurR(dlg.getCouleurR());
        article.setCouleurG(dlg.getCouleurG());
        article.setCouleurB(dlg.getCouleurB());
        article.setLargeur(dlg.getLargeur());
        article.setHauteur(dlg.getHauteur());
        article.setProfondeur(dlg.getProfondeur());
        article.setPrixUnitaire(dlg.getPrixUnitaire());
        article.setCoutFabrication(dlg.getCoutFabrication());
        article.setStatut(dlg.getStatut());
        
        // Modifier dans la base de données
        if (article.modifier()) {
            // Sauvegarder l'image generee dans la BD
            QString photoPath = dlg.getPhotoPath();
            if (!photoPath.isEmpty()) {
                Article::updateImagePath(article.getIdArticle(), photoPath);
            }
            QMessageBox::information(this, "Succes", "Article modifie avec succes !");
            refreshArticleTable(); // Rafraéchir l'affichage
        } else {
            QMessageBox::critical(this, "Erreur", "Impossible de modifier l'article.");
        }
    }
}

void MainWindow::on_btnDeleteArticle_clicked()
{
    int row = ui->articleTable->currentRow();
    if (row < 0) { QMessageBox::warning(this,"Attention","Selectionnez un article."); return; }
    
    // Recuperer l'ID de l'article selectionne
    int idArticle = ui->articleTable->item(row, 0)->text().toInt();
    Article article = Article::rechercherParId(idArticle);
    
    if (article.getIdArticle() == 0) {
        QMessageBox::warning(this, "Erreur", "Article introuvable.");
        return;
    }
    
    ArticleDialog dlg(this, ArticleDialog::DeleteMode);
    dlg.setArticleData(article.getReference(), article.getNom(), article.getCategorie(),
                       article.getType(), article.getModele3D(),
                       article.getCouleurR(), article.getCouleurG(), article.getCouleurB(),
                       article.getLargeur(), article.getHauteur(), article.getProfondeur(),
                       article.getPrixUnitaire(), article.getCoutFabrication(), article.getStatut());
    
    if (dlg.exec() == QDialog::Accepted) {
        // Supprimer de la base de données
        if (article.supprimer()) {
            QMessageBox::information(this, "Succes", "Article supprime avec succes !");
            refreshArticleTable(); // Rafraéchir l'affichage
        } else {
            QMessageBox::critical(this, "Erreur", "Impossible de supprimer l'article.");
        }
    }
}

void MainWindow::on_btnViewArticle_clicked()
{
    int row = ui->articleTable->currentRow();
    if (row < 0) { QMessageBox::warning(this,"","Selectionnez un article."); return; }
    
    int idArticle = ui->articleTable->item(row, 0)->text().toInt();
    Article article = Article::rechercherParId(idArticle);
    if (article.getIdArticle() == 0) { QMessageBox::warning(this,"","Article introuvable."); return; }
    
    ArticleDialog dlg(this, ArticleDialog::ViewMode);
    dlg.setArticleData(article.getReference(), article.getNom(), article.getCategorie(),
                       article.getType(), article.getModele3D(),
                       article.getCouleurR(), article.getCouleurG(), article.getCouleurB(),
                       article.getLargeur(), article.getHauteur(), article.getProfondeur(),
                       article.getPrixUnitaire(), article.getCoutFabrication(), article.getStatut());
    dlg.setImagePath(article.getImagePath());
    dlg.exec();
}

void MainWindow::on_btnView3DArticle_clicked()
{
    int row = ui->articleTable->currentRow();
    if (row < 0) { QMessageBox::warning(this,"","Selectionnez un article pour la vue 3D."); return; }

    QString nom     = ui->articleTable->item(row, 2)->text();
    QString type    = ui->articleTable->item(row, 4)->text();
    QString couleur = ui->articleTable->item(row, 5)->text();

    QDialog dlg(this);
    dlg.setWindowTitle("Vue 3D - " + nom);
    dlg.setMinimumSize(900, 600);
    dlg.showMaximized();
    dlg.setStyleSheet("QDialog{background:#FAF5F0;}");

    QVBoxLayout *lay = new QVBoxLayout(&dlg);
    lay->setContentsMargins(8,8,8,8);

    auto *title = new QLabel(QString("VISUALISATION 3D  |  %1  -  %2").arg(nom, type));
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size:14px;font-weight:bold;color:white;padding:8px;"
                         "background:#8D6E63;border-radius:10px;border:1px solid #5D4037;");
    lay->addWidget(title);

    auto *viewer = new ArticleViewer3D(&dlg);
    
    // Appliquer la couleur RGB et dimensions depuis la BD AVANT de charger le modèle
    int row2 = ui->articleTable->currentRow();
    if (row2 >= 0 && row2 < articles.size()) {
        const Article &art = articles[row2];
        viewer->setArticleInfo(art.getNom(), type, art.getCategorie(),
                               art.getCouleurR(), art.getCouleurG(), art.getCouleurB(),
                               art.getLargeur(), art.getHauteur(), art.getProfondeur(),
                               art.getPrixUnitaire(), art.getStatut());
        
        // Charger l'image IA generee si elle existe
        QString ref = art.getReference();
        QString safeRef = ref;
        safeRef.replace("/", "_").replace("\\", "_").replace(" ", "_");
        QString imgPath = QCoreApplication::applicationDirPath() + "/photos_articles/" + safeRef + ".png";
        if (QFile::exists(imgPath)) {
            viewer->setImagePath(imgPath);
        }
    } else {
        viewer->loadModelForType(type);
    }

    lay->addWidget(viewer);

    auto *btnClose = new QPushButton("Fermer");
    btnClose->setStyleSheet("QPushButton{background:#8D6E63;color:white;border:none;"
                            "border-radius:8px;padding:10px 20px;font-weight:bold;}"
                            "QPushButton:hover{background:#A0826D;}");
    btnClose->setMaximumWidth(150);
    connect(btnClose, &QPushButton::clicked, &dlg, &QDialog::accept);
    QHBoxLayout *bl = new QHBoxLayout(); bl->addStretch(); bl->addWidget(btnClose);
    lay->addLayout(bl);

    dlg.exec();
}

void MainWindow::on_searchBoxArticle_textChanged(const QString &text)
{
    if (text.trimmed().isEmpty()) {
        // Afficher tous les articles
        for (int i = 0; i < ui->articleTable->rowCount(); ++i)
            ui->articleTable->setRowHidden(i, false);
        return;
    }
    
    // Recherche par nom (col 2) ou référence (col 1) via la classe Article
    QList<Article> resultats;
    if (text.trimmed().length() >= 2) {
        // Recherche dans la BD via requétes préparées
        QList<Article> parRef = Article::rechercherParReference(text.trimmed());
        QList<Article> parNom = Article::rechercherParNom(text.trimmed());
        
        // Fusionner sans doublons
        QSet<int> ids;
        for (const Article &a : parRef) { resultats.append(a); ids.insert(a.getIdArticle()); }
        for (const Article &a : parNom) { if (!ids.contains(a.getIdArticle())) resultats.append(a); }
        
        // Filtrer la table selon les résultats
        QSet<int> resultIds;
        for (const Article &a : resultats) resultIds.insert(a.getIdArticle());
        
        for (int i = 0; i < ui->articleTable->rowCount(); ++i) {
            int id = ui->articleTable->item(i, 0)->text().toInt();
            ui->articleTable->setRowHidden(i, !resultIds.contains(id));
        }
    } else {
        // Recherche locale simple pour 1 caractére
        for (int i = 0; i < ui->articleTable->rowCount(); ++i) {
            QString ref = ui->articleTable->item(i, 1)->text();
            QString nom = ui->articleTable->item(i, 2)->text();
            bool match = ref.contains(text, Qt::CaseInsensitive) || nom.contains(text, Qt::CaseInsensitive);
            ui->articleTable->setRowHidden(i, !match);
        }
    }
}

void MainWindow::on_btnTriArticle_clicked()
{
    QMenu menu(this);
    menu.setStyleSheet(
        "QMenu{background:#FAF5F0;border:2px solid #8D6E63;border-radius:6px;padding:8px;}"
        "QMenu::item{padding:8px 25px;color:#291C0E;border-radius:4px;}"
        "QMenu::item:selected{background:#8D6E63;color:white;}"
        "QMenu::separator{height:2px;background:#BCAAA4;margin:5px 10px;}"
    );

    // Prix
    QMenu *menuPrix = menu.addMenu("Prix");
    menuPrix->setStyleSheet(menu.styleSheet());
    auto *prixCroissant = menuPrix->addAction("Prix Croissant");
    auto *prixDecroissant = menuPrix->addAction("Prix Decroissant");
    connect(prixCroissant, &QAction::triggered, [=]{ ui->articleTable->sortItems(7, Qt::AscendingOrder); });
    connect(prixDecroissant, &QAction::triggered, [=]{ ui->articleTable->sortItems(7, Qt::DescendingOrder); });
    
    menu.addSeparator();
    
    // Categorie
    QMenu *menuCategorie = menu.addMenu("Categorie");
    menuCategorie->setStyleSheet(menu.styleSheet());
    auto *catAZ = menuCategorie->addAction("A - Z");
    auto *catZA = menuCategorie->addAction("Z - A");
    connect(catAZ, &QAction::triggered, [=]{ ui->articleTable->sortItems(3, Qt::AscendingOrder); });
    connect(catZA, &QAction::triggered, [=]{ ui->articleTable->sortItems(3, Qt::DescendingOrder); });
    
    menu.addSeparator();
    
    // Statut - Tri personnalise par disponibilite
    QMenu *menuStatut = menu.addMenu("Statut");
    menuStatut->setStyleSheet(menu.styleSheet());
    auto *dispoDabord = menuStatut->addAction("Disponibles d'abord");
    auto *nonDispoDabord = menuStatut->addAction("Non disponibles d'abord");
    menuStatut->addSeparator();
    auto *statAZ = menuStatut->addAction("A - Z");
    auto *statZA = menuStatut->addAction("Z - A");
    
    // Tri personnalisé : Disponibles d'abord
    connect(dispoDabord, &QAction::triggered, [this]() {
        std::sort(articles.begin(), articles.end(), [](const Article &a, const Article &b) {
            // Ordre: disponible (0) < en_production (1) < obsolete (2)
            auto getPriority = [](const QString &s) {
                if (s == "disponible") return 0;
                if (s == "en_production") return 1;
                return 2;
            };
            return getPriority(a.getStatut()) < getPriority(b.getStatut());
        });
        refreshArticleTable();
    });
    
    // Tri personnalisé : Non disponibles d'abord
    connect(nonDispoDabord, &QAction::triggered, [this]() {
        std::sort(articles.begin(), articles.end(), [](const Article &a, const Article &b) {
            auto getPriority = [](const QString &s) {
                if (s == "disponible") return 2;
                if (s == "en_production") return 1;
                return 0;
            };
            return getPriority(a.getStatut()) < getPriority(b.getStatut());
        });
        refreshArticleTable();
    });
    
    connect(statAZ, &QAction::triggered, [=]{ ui->articleTable->sortItems(9, Qt::AscendingOrder); });
    connect(statZA, &QAction::triggered, [=]{ ui->articleTable->sortItems(9, Qt::DescendingOrder); });

    QPoint pos = ui->btnTriArticle->mapToGlobal(QPoint(0, ui->btnTriArticle->height()));
    menu.exec(pos);
}

void MainWindow::on_btnStatistiquesArticle_clicked()
{
    if (articles.isEmpty()) { QMessageBox::information(this,"","Aucun article à analyser."); return; }

    QDialog dlg(this);
    dlg.setWindowTitle("Statistiques & Rentabilite - CUIREA");
    dlg.setMinimumSize(1200, 750);
    dlg.showMaximized();
    dlg.setStyleSheet(
        "QDialog{background:#FAF5F0;}"
        "QTabWidget::pane{border:2px solid #8D6E63;border-radius:8px;background:#FFFFFF;}"
        "QTabBar::tab{background:#D7CCC8;color:#5D4037;padding:12px 25px;border-radius:6px 6px 0 0;font-weight:bold;font-size:12px;}"
        "QTabBar::tab:selected{background:#8D6E63;color:white;}"
        "QLabel{color:#3E2723;}"
        "QGroupBox{border:2px solid #8D6E63;border-radius:10px;margin-top:10px;"
        "padding-top:10px;font-weight:bold;color:#5D4037;font-size:11px;background:#FFFFFF;}"
        "QTableWidget{background:#FFFFFF;color:#3E2723;gridline-color:#D7CCC8;border:1px solid #BCAAA4;alternate-background-color:#EFEBE9;}"
        "QHeaderView::section{background:#8D6E63;color:white;padding:6px;font-weight:bold;border:none;}"
        "QPushButton{background:#8D6E63;color:white;border:none;border-radius:8px;padding:10px 20px;font-weight:bold;}"
        "QPushButton:hover{background:#6D4C41;}"
    );

    QVBoxLayout *mainLay = new QVBoxLayout(&dlg);
    mainLay->setContentsMargins(12,12,12,10); mainLay->setSpacing(8);

    // Titre
    auto *titleLbl = new QLabel("TABLEAU DE BORD STATISTIQUES & RENTABILITE  |  CUIREA Smart Factory");
    titleLbl->setAlignment(Qt::AlignCenter);
    titleLbl->setStyleSheet("font-size:15px;font-weight:bold;color:white;padding:10px;"
                            "background:#8D6E63;border-radius:10px;");
    mainLay->addWidget(titleLbl);

    // ----------------------------------------------------------------------
    // 6 KPI PARTAGéS EN HAUT
    // ----------------------------------------------------------------------
    int total=articles.size(), dispo=0, enProd=0, obs=0;
    double totalPrix=0, totalCout=0;
    int excRent=0, bonRent=0, moyRent=0, faiRent=0;
    QMap<QString,int> nbParCat;
    QMap<QString,double> caParCat, margeParCat;

    for (const Article &a : articles) {
        if (a.getStatut()=="disponible") ++dispo;
        else if (a.getStatut()=="en_production") ++enProd;
        else ++obs;
        totalPrix += a.getPrixUnitaire();
        totalCout += a.getCoutFabrication();
        nbParCat[a.getCategorie()]++;
        caParCat[a.getCategorie()] += a.getPrixUnitaire();
        margeParCat[a.getCategorie()] += (a.getPrixUnitaire()-a.getCoutFabrication());
        double mp = a.getCoutFabrication()>0 ? (a.getPrixUnitaire()-a.getCoutFabrication())/a.getCoutFabrication()*100 : 0;
        if (mp>=50) ++excRent; else if (mp>=30) ++bonRent; else if (mp>=15) ++moyRent; else ++faiRent;
    }
    double margeGlobale = totalCout>0 ? ((totalPrix-totalCout)/totalCout)*100 : 0;
    double prixMoyen = total>0 ? totalPrix/total : 0;

    auto makeKPI = [](const QString &val, const QString &label, const QString &color) {
        auto *w = new QWidget();
        w->setStyleSheet(QString("background:%1;border-radius:10px;").arg(color));
        auto *l = new QVBoxLayout(w); l->setContentsMargins(8,8,8,8);
        auto *v = new QLabel(val); v->setAlignment(Qt::AlignCenter);
        v->setStyleSheet("font-size:20px;font-weight:bold;color:white;background:transparent;");
        auto *lb = new QLabel(label); lb->setAlignment(Qt::AlignCenter);
        lb->setStyleSheet("font-size:9px;color:rgba(255,255,255,0.85);background:transparent;");
        l->addWidget(v); l->addWidget(lb);
        w->setMinimumHeight(65); w->setMinimumWidth(100);
        return w;
    };

    QHBoxLayout *kpiLay = new QHBoxLayout(); kpiLay->setSpacing(8);
    kpiLay->addWidget(makeKPI(QString::number(total), "Total Articles", "#8D6E63"));
    kpiLay->addWidget(makeKPI(QString::number(dispo), "Disponibles", "#558B2F"));
    kpiLay->addWidget(makeKPI(QString::number(enProd), "En Production", "#EF6C00"));
    kpiLay->addWidget(makeKPI(QString::number(obs), "Obsoletes", "#C62828"));
    kpiLay->addWidget(makeKPI(QString::number(margeGlobale,'f',1)+"%", "Marge Globale", "#5D4037"));
    kpiLay->addWidget(makeKPI(QString::number(prixMoyen,'f',0)+" DT", "Prix Moyen", "#6D4C41"));
    mainLay->addLayout(kpiLay);

    // ----------------------------------------------------------------------
    // ONGLETS : Statistiques + Rentabilité
    // ----------------------------------------------------------------------
    auto *tabs = new QTabWidget();
    mainLay->addWidget(tabs, 1);

    // ------------------------------------------------------------------------
    // ONGLET 1 : STATISTIQUES
    // ------------------------------------------------------------------------
    auto *tab1 = new QWidget();
    auto *t1Lay = new QHBoxLayout(tab1); t1Lay->setSpacing(10);

    // Camembert statut
    {
        QGroupBox *gb = new QGroupBox("  Repartition par Statut");
        QVBoxLayout *l = new QVBoxLayout(gb);
        auto *pie = new QPieSeries();
        if (dispo>0) pie->append("Disponible", dispo)->setBrush(QColor("#558B2F"));
        if (enProd>0) pie->append("En Production", enProd)->setBrush(QColor("#EF6C00"));
        if (obs>0) pie->append("Obsolete", obs)->setBrush(QColor("#C62828"));
        for (auto *sl : pie->slices()) {
            sl->setLabelVisible(true);
            sl->setLabelColor(QColor("#3E2723"));
            // Afficher seulement le pourcentage sur le graphique
            sl->setLabel(QString("%1%").arg(sl->percentage()*100, 0, 'f', 1));
        }
        auto *chart = new QChart(); chart->addSeries(pie);
        chart->setBackgroundBrush(QBrush(QColor("#FFFFFF")));
        chart->legend()->setLabelColor(QColor("#3E2723"));
        chart->legend()->setAlignment(Qt::AlignBottom);
        chart->setAnimationOptions(QChart::SeriesAnimations);
        auto *cv = new QChartView(chart); cv->setRenderHint(QPainter::Antialiasing);
        l->addWidget(cv); t1Lay->addWidget(gb);
    }

    // Barres articles par catégorie
    {
        QGroupBox *gb = new QGroupBox("  Articles par Categorie");
        QVBoxLayout *l = new QVBoxLayout(gb);
        auto *bs = new QBarSet("Nb Articles"); bs->setColor(QColor("#8D6E63"));
        QStringList cats;
        for (auto it=nbParCat.begin(); it!=nbParCat.end(); ++it) { cats << it.key(); *bs << it.value(); }
        auto *series = new QBarSeries(); series->append(bs);
        auto *chart = new QChart(); chart->addSeries(series);
        chart->setBackgroundBrush(QBrush(QColor("#FFFFFF")));
        chart->legend()->setVisible(false);
        chart->setAnimationOptions(QChart::SeriesAnimations);
        auto *axX = new QBarCategoryAxis(); axX->append(cats); axX->setLabelsColor(QColor("#3E2723"));
        chart->addAxis(axX,Qt::AlignBottom); series->attachAxis(axX);
        auto *axY = new QValueAxis(); axY->setLabelFormat("%d"); axY->setLabelsColor(QColor("#3E2723"));
        chart->addAxis(axY,Qt::AlignLeft); series->attachAxis(axY);
        auto *cv = new QChartView(chart); cv->setRenderHint(QPainter::Antialiasing);
        l->addWidget(cv); t1Lay->addWidget(gb);
    }

    // Barres groupées prix vs coût par catégorie
    {
        QGroupBox *gb = new QGroupBox("  Prix vs Cout par Categorie");
        QVBoxLayout *l = new QVBoxLayout(gb);
        auto *sPrix = new QBarSet("Prix Moyen"); sPrix->setColor(QColor("#8D6E63"));
        auto *sCout = new QBarSet("Cout Moyen"); sCout->setColor(QColor("#D7CCC8"));
        QStringList cats;
        for (auto it=nbParCat.begin(); it!=nbParCat.end(); ++it) {
            cats << it.key();
            *sPrix << caParCat[it.key()]/it.value();
            double sc=0; for (const Article &a : articles) if (a.getCategorie()==it.key()) sc+=a.getCoutFabrication();
            *sCout << sc/it.value();
        }
        auto *series = new QBarSeries(); series->append(sCout); series->append(sPrix);
        auto *chart = new QChart(); chart->addSeries(series);
        chart->setBackgroundBrush(QBrush(QColor("#FFFFFF")));
        chart->legend()->setLabelColor(QColor("#3E2723"));
        chart->setAnimationOptions(QChart::SeriesAnimations);
        auto *axX = new QBarCategoryAxis(); axX->append(cats); axX->setLabelsColor(QColor("#3E2723"));
        chart->addAxis(axX,Qt::AlignBottom); series->attachAxis(axX);
        auto *axY = new QValueAxis(); axY->setLabelFormat("%.0f"); axY->setLabelsColor(QColor("#3E2723"));
        chart->addAxis(axY,Qt::AlignLeft); series->attachAxis(axY);
        auto *cv = new QChartView(chart); cv->setRenderHint(QPainter::Antialiasing);
        l->addWidget(cv); t1Lay->addWidget(gb);
    }

    tabs->addTab(tab1, "Statistiques");

    // ------------------------------------------------------------------------
    // ONGLET 2 : RENTABILITé
    // ------------------------------------------------------------------------
    auto *tab2 = new QWidget();
    auto *t2Lay = new QHBoxLayout(tab2); t2Lay->setSpacing(10);

    // Tableau détaillé rentabilité
    {
        QGroupBox *gb = new QGroupBox("  📋  Tableau Détaillé Rentabilité");
        QVBoxLayout *l = new QVBoxLayout(gb);
        auto *tbl = new QTableWidget();
        tbl->setColumnCount(7);
        tbl->setHorizontalHeaderLabels({"Nom","Catégorie","Coût (DT)","Prix (DT)","Marge (DT)","Marge (%)","Niveau"});
        tbl->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        tbl->setAlternatingRowColors(true);
        tbl->verticalHeader()->setVisible(false);
        tbl->setRowCount(articles.size());
        for (int i=0; i<articles.size(); ++i) {
            const Article &a = articles[i];
            double marge = a.getPrixUnitaire()-a.getCoutFabrication();
            double mp = a.getCoutFabrication()>0 ? marge/a.getCoutFabrication()*100 : 0;
            QString ren; QColor col;
            if (mp>=50) { ren="⭐ Excellente"; col=QColor("#2E7D32"); }
            else if (mp>=30) { ren="✓ Bonne"; col=QColor("#558B2F"); }
            else if (mp>=15) { ren="~ Moyenne"; col=QColor("#EF6C00"); }
            else if (mp>0) { ren="↓ Faible"; col=QColor("#D84315"); }
            else { ren="✗ Négative"; col=QColor("#C62828"); }
            tbl->setItem(i,0,new QTableWidgetItem(a.getNom()));
            tbl->setItem(i,1,new QTableWidgetItem(a.getCategorie()));
            tbl->setItem(i,2,new QTableWidgetItem(QString::number(a.getCoutFabrication(),'f',2)));
            tbl->setItem(i,3,new QTableWidgetItem(QString::number(a.getPrixUnitaire(),'f',2)));
            tbl->setItem(i,4,new QTableWidgetItem(QString::number(marge,'f',2)));
            tbl->setItem(i,5,new QTableWidgetItem(QString::number(mp,'f',1)+"%"));
            auto *ri = new QTableWidgetItem(ren);
            ri->setForeground(QBrush(col)); QFont f=ri->font(); f.setBold(true); ri->setFont(f);
            tbl->setItem(i,6,ri);
        }
        l->addWidget(tbl); t2Lay->addWidget(gb, 5);
    }

    // Colonne droite : camembert + courbe
    QVBoxLayout *rightLay = new QVBoxLayout(); rightLay->setSpacing(8);

    // Camembert rentabilité
    {
        QGroupBox *gb = new QGroupBox("  Repartition Rentabilite");
        QVBoxLayout *l = new QVBoxLayout(gb);
        auto *pie = new QPieSeries();
        if (excRent>0) pie->append("Excellente ≥50%", excRent)->setBrush(QColor("#2E7D32"));
        if (bonRent>0) pie->append("Bonne 30-50%", bonRent)->setBrush(QColor("#558B2F"));
        if (moyRent>0) pie->append("Moyenne 15-30%", moyRent)->setBrush(QColor("#EF6C00"));
        if (faiRent>0) pie->append("Faible <15%", faiRent)->setBrush(QColor("#C62828"));
        for (auto *sl : pie->slices()) {
            sl->setLabelVisible(true); sl->setLabelColor(QColor("#3E2723"));
            sl->setLabel(QString("%1\n%2").arg(sl->label()).arg((int)sl->value()));
        }
        auto *chart = new QChart(); chart->addSeries(pie);
        chart->setBackgroundBrush(QBrush(QColor("#FFFFFF")));
        chart->legend()->setLabelColor(QColor("#3E2723"));
        chart->legend()->setAlignment(Qt::AlignBottom);
        chart->setAnimationOptions(QChart::SeriesAnimations);
        auto *cv = new QChartView(chart); cv->setRenderHint(QPainter::Antialiasing);
        cv->setMinimumHeight(200);
        l->addWidget(cv); rightLay->addWidget(gb);
    }

    // Courbe marge % par article avec seuil 30%
    {
        QGroupBox *gb = new QGroupBox("  Courbe Marge % (seuil 30%)");
        QVBoxLayout *l = new QVBoxLayout(gb);
        auto *series = new QLineSeries(); series->setName("Marge %");
        series->setColor(QColor("#8D6E63")); series->setPen(QPen(QColor("#8D6E63"),2));
        auto *seuil = new QLineSeries(); seuil->setName("Seuil 30%");
        seuil->setColor(QColor("#558B2F")); seuil->setPen(QPen(QColor("#558B2F"),2,Qt::DashLine));
        for (int i=0; i<articles.size(); ++i) {
            double mp = articles[i].getCoutFabrication()>0
                ? (articles[i].getPrixUnitaire()-articles[i].getCoutFabrication())/articles[i].getCoutFabrication()*100 : 0;
            series->append(i, mp);
            seuil->append(i, 30.0);
        }
        auto *chart = new QChart();
        chart->addSeries(series); chart->addSeries(seuil);
        chart->setBackgroundBrush(QBrush(QColor("#FFFFFF")));
        chart->legend()->setLabelColor(QColor("#3E2723"));
        chart->setAnimationOptions(QChart::SeriesAnimations);
        chart->createDefaultAxes();
        chart->axes(Qt::Horizontal).first()->setLabelsColor(QColor("#3E2723"));
        chart->axes(Qt::Vertical).first()->setLabelsColor(QColor("#3E2723"));
        auto *cv = new QChartView(chart); cv->setRenderHint(QPainter::Antialiasing);
        cv->setMinimumHeight(200);
        l->addWidget(cv); rightLay->addWidget(gb);
    }

    t2Lay->addLayout(rightLay, 4);
    tabs->addTab(tab2, "Rentabilite");

    // Bouton fermer
    auto *btnClose = new QPushButton("Fermer");
    btnClose->setMaximumWidth(150);
    connect(btnClose, &QPushButton::clicked, &dlg, &QDialog::accept);
    QHBoxLayout *bl = new QHBoxLayout(); bl->addStretch(); bl->addWidget(btnClose);
    mainLay->addLayout(bl);

    dlg.exec();
}

void MainWindow::on_btnExportPdfArticle_clicked()
{
    if (articles.isEmpty()) {
        QMessageBox::information(this, "Export PDF", "Aucun article a exporter.");
        return;
    }

    // ----------------------------------------------------------------------
    // DIALOG DE FILTRE AVANT EXPORT
    // ----------------------------------------------------------------------
    
    QDialog filterDlg(this);
    filterDlg.setWindowTitle("Options d'Export du Catalogue");
    filterDlg.setMinimumSize(550, 480);
    filterDlg.setStyleSheet(
        "QDialog{background:#FAF5F0;}"
        "QLabel{color:#2D1810;font-size:13px;}"
        "QGroupBox{border:2px solid #5D4037;border-radius:10px;margin-top:10px;"
        "padding:15px 20px 15px 20px;font-weight:bold;color:#3E2723;background:#FFFFFF;font-size:13px;}"
        "QGroupBox::title{subcontrol-origin:margin;left:15px;padding:0 8px;color:#3E2723;font-weight:bold;font-size:13px;}"
        "QCheckBox{color:#2D1810;font-size:13px;padding:6px 0px;}"
        "QCheckBox::indicator{width:18px;height:18px;border:2px solid #5D4037;border-radius:3px;background:white;}"
        "QCheckBox::indicator:checked{background:#5D4037;border-color:#3E2723;}"
        "QComboBox{background:#FFFFFF;border:2px solid #5D4037;border-radius:6px;"
        "padding:8px 12px;color:#2D1810;font-size:13px;min-width:200px;}"
        "QComboBox::drop-down{border:none;width:25px;}"
        "QComboBox QAbstractItemView{background:#FFFFFF;color:#2D1810;selection-background-color:#D7CCC8;}"
        "QPushButton{background:#5D4037;color:white;border:none;border-radius:8px;"
        "padding:12px 25px;font-weight:bold;font-size:13px;}"
        "QPushButton:hover{background:#8D6E63;}"
        "QPushButton#exportBtn{background:qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #2E7D32,stop:1 #4CAF50);}"
        "QPushButton#exportBtn:hover{background:#43A047;}"
    );

    QVBoxLayout *filterLay = new QVBoxLayout(&filterDlg);
    filterLay->setContentsMargins(25,20,25,20);
    filterLay->setSpacing(15);

    auto *filterTitle = new QLabel("CONFIGURATION DE L'EXPORT PDF");
    filterTitle->setAlignment(Qt::AlignCenter);
    filterTitle->setStyleSheet("font-size:15px;font-weight:bold;color:white;padding:12px;"
                               "background:#5D4037;border-radius:10px;");
    filterLay->addWidget(filterTitle);

    QGroupBox *filterBox = new QGroupBox("Filtres");
    QVBoxLayout *filterBoxLay = new QVBoxLayout(filterBox);
    filterBoxLay->setSpacing(8);
    filterBoxLay->setContentsMargins(10,15,10,10);
    
    auto *cbTous = new QCheckBox("Tous les articles");
    cbTous->setChecked(true);
    auto *cbDispoOnly = new QCheckBox("Disponibles uniquement");
    cbDispoOnly->setEnabled(false);
    cbDispoOnly->setChecked(false);
    
    QHBoxLayout *catLay = new QHBoxLayout();
    catLay->setSpacing(10);
    auto *catLabel = new QLabel("Categorie:");
    auto *catCombo = new QComboBox();
    catCombo->addItems({"Toutes categories", "Sacs", "Portefeuilles", "Ceintures", "Accessoires", "Chaussures"});
    catCombo->setEnabled(false);
    catLay->addWidget(catLabel);
    catLay->addWidget(catCombo, 1);
    
    filterBoxLay->addWidget(cbTous);
    filterBoxLay->addWidget(cbDispoOnly);
    filterBoxLay->addLayout(catLay);
    filterLay->addWidget(filterBox);

    QGroupBox *displayBox = new QGroupBox("Affichage");
    QVBoxLayout *displayLay = new QVBoxLayout(displayBox);
    displayLay->setSpacing(8);
    displayLay->setContentsMargins(10,15,10,10);
    auto *cbImages = new QCheckBox("Inclure les images generees");
    cbImages->setChecked(true);
    auto *cbSections = new QCheckBox("Organiser par categorie (sections)");
    cbSections->setChecked(true);
    displayLay->addWidget(cbImages);
    displayLay->addWidget(cbSections);
    filterLay->addWidget(displayBox);

    // Logique d'activation/desactivation des filtres
    connect(cbTous, &QCheckBox::toggled, [=](bool checked) {
        cbDispoOnly->setEnabled(!checked);
        catCombo->setEnabled(!checked);
        catLabel->setEnabled(!checked);
        if (checked) { 
            cbDispoOnly->setChecked(false); 
            catCombo->setCurrentIndex(0); 
        }
    });
    
    // Permettre de selectionner une categorie meme si "Disponibles uniquement" n'est pas coche
    connect(cbDispoOnly, &QCheckBox::toggled, [=](bool) {
        // Les deux filtres peuvent etre utilises ensemble ou separement
    });

    filterLay->addStretch();

    QHBoxLayout *btnFilterLay = new QHBoxLayout();
    auto *btnContinue = new QPushButton("Generer le Catalogue");
    btnContinue->setObjectName("exportBtn");
    auto *btnAnnuler = new QPushButton("Annuler");
    btnFilterLay->addStretch();
    btnFilterLay->addWidget(btnContinue);
    btnFilterLay->addWidget(btnAnnuler);
    btnFilterLay->addStretch();
    filterLay->addLayout(btnFilterLay);

    connect(btnAnnuler, &QPushButton::clicked, &filterDlg, &QDialog::reject);
    connect(btnContinue, &QPushButton::clicked, &filterDlg, &QDialog::accept);

    if (filterDlg.exec() != QDialog::Accepted) return;

    bool showAll = cbTous->isChecked();
    bool dispoOnly = cbDispoOnly->isChecked();
    QString catFilter = catCombo->currentText();
    bool includeImages = cbImages->isChecked();
    bool organizeByCategory = cbSections->isChecked();

    // ----------------------------------------------------------------------
    // FILTRER LES ARTICLES ET CHARGER LES CHEMINS D'IMAGES
    // ----------------------------------------------------------------------
    
    QString photosDir = QCoreApplication::applicationDirPath() + "/photos_articles/";
    
    QList<Article> filteredArticles;
    for (Article a : articles) {
        if (!showAll) {
            if (dispoOnly && a.getStatut() != "disponible") continue;
            if (catFilter != "Toutes categories" && a.getCategorie() != catFilter) continue;
        }
        // Chercher l'image generee pour cet article
        if (a.getImagePath().isEmpty()) {
            QString safeRef = a.getReference();
            safeRef.replace("/", "_").replace("\\", "_").replace(" ", "_");
            QString imgPath = photosDir + safeRef + ".png";
            if (QFile::exists(imgPath)) {
                a.setImagePath(imgPath);
            }
        }
        filteredArticles.append(a);
    }

    if (filteredArticles.isEmpty()) {
        QMessageBox::warning(this, "Export PDF", "Aucun article ne correspond aux filtres.");
        return;
    }

    // ----------------------------------------------------------------------
    // CALCUL DES STATISTIQUES
    // ----------------------------------------------------------------------
    
    int dispo=0, enProd=0;
    double totalPrix=0, totalMarge=0;
    QMap<QString, int> countByCat;
    
    for (const Article &a : filteredArticles) {
        if (a.getStatut()=="disponible") ++dispo;
        else if (a.getStatut()=="en_production") ++enProd;
        totalPrix += a.getPrixUnitaire();
        if (a.getCoutFabrication() > 0)
            totalMarge += (a.getPrixUnitaire() - a.getCoutFabrication()) / a.getCoutFabrication() * 100;
        countByCat[a.getCategorie()]++;
    }
    double prixMoy = filteredArticles.size() > 0 ? totalPrix / filteredArticles.size() : 0;
    double margeMoy = filteredArticles.size() > 0 ? totalMarge / filteredArticles.size() : 0;

    // Helper : convertir une image locale en data URI base64
    auto imageToDataUri = [](const QString &imagePath) -> QString {
        if (imagePath.isEmpty() || !QFile::exists(imagePath)) return "";
        QFile f(imagePath);
        if (!f.open(QIODevice::ReadOnly)) return "";
        QByteArray data = f.readAll();
        f.close();
        if (data.isEmpty()) return "";
        QString mime = imagePath.toLower().endsWith(".jpg") || imagePath.toLower().endsWith(".jpeg") 
                     ? "image/jpeg" : "image/png";
        return QString("data:%1;base64,%2").arg(mime, QString::fromLatin1(data.toBase64()));
    };

    // ----------------------------------------------------------------------
    // GÉNÉRATION DU HTML CATALOGUE MODERNE (PLEINE PAGE)
    // ----------------------------------------------------------------------
    
    QString html = R"(<!DOCTYPE html><html><head><meta charset='UTF-8'>
<style>
@page { size: A4; margin: 8mm; }
* { box-sizing: border-box; }
body { font-family: 'Segoe UI', Arial, sans-serif; margin: 0; padding: 0;
       background: #FAFAFA; color: #2D1810; font-size: 10pt; width: 100%; }
.header { background: linear-gradient(135deg, #5D4037 0%, #3E2723 100%); 
          color: white; padding: 20px 25px; margin-bottom: 15px; }
.logo { font-size: 28pt; font-weight: bold; letter-spacing: 3px; }
.header-info { float: right; text-align: right; font-size: 10pt; color: #D7CCC8; }
.kpi-row { display: flex; justify-content: space-between; margin: 12px 10px; gap: 10px; }
.kpi { flex: 1; text-align: center; padding: 12px 8px;
       background: white; border: 2px solid #8D6E63; border-radius: 8px; }
.kpi-val { font-size: 20pt; font-weight: bold; color: #3E2723; }
.kpi-lbl { font-size: 9pt; color: #5D4037; margin-top: 3px; font-weight: 600; }
.section-title { font-size: 13pt; font-weight: bold; color: #3E2723; 
                 padding: 10px 15px; margin: 15px 10px 10px;
                 border-left: 4px solid #5D4037; 
                 background: #EFEBE9; border-radius: 0 6px 6px 0; }
.grid { display: flex; flex-wrap: wrap; padding: 0 5px; gap: 12px; justify-content: flex-start; }
.product-card { width: 170px; background: white; border: 2px solid #D7CCC8; 
                border-radius: 10px; overflow: hidden; page-break-inside: avoid; }
.product-img { width: 100%; height: 140px; background: #F5F5F5; 
               display: flex; align-items: center; justify-content: center;
               border-bottom: 2px solid #EFEBE9; position: relative; }
.product-img img { max-width: 100%; max-height: 135px; object-fit: contain; }
.color-box { width: 100%; height: 140px; display: flex; align-items: center; 
             justify-content: center; font-size: 24pt; color: white; font-weight: bold; }
.product-info { padding: 10px; }
.product-name { font-size: 11pt; font-weight: bold; color: #2D1810; 
                margin-bottom: 4px; line-height: 1.2; 
                max-height: 2.4em; overflow: hidden; }
.product-ref { font-size: 8pt; color: #6D4C41; margin-bottom: 6px; }
.product-type { font-size: 8pt; color: #8D6E63; margin-bottom: 8px; }
.product-price { font-size: 14pt; font-weight: bold; color: #3E2723; }
.product-marge { font-size: 9pt; font-weight: bold; padding: 2px 8px; 
                 border-radius: 4px; display: inline-block; margin-top: 5px; }
.marge-high { background: #C8E6C9; color: #1B5E20; }
.marge-mid { background: #FFE0B2; color: #E65100; }
.marge-low { background: #FFCDD2; color: #B71C1C; }
.status-badge { position: absolute; top: 5px; right: 5px; font-size: 7pt; 
                padding: 2px 6px; border-radius: 3px; font-weight: bold; }
.status-dispo { background: #4CAF50; color: white; }
.status-prod { background: #FF9800; color: white; }
.status-obs { background: #F44336; color: white; }
.footer { margin-top: 15px; padding: 12px 15px; border-top: 2px solid #5D4037;
          font-size: 9pt; color: #5D4037; display: flex; justify-content: space-between; }
</style></head><body>)";

    // Header
    html += QString(R"(<div class='header'>
<span class='logo'>CUIREA</span>
<span class='header-info'>Catalogue Articles en Cuir<br><strong>%1 articles</strong> - %2</span>
<div style='clear:both;'></div></div>)")
        .arg(filteredArticles.size()).arg(QDate::currentDate().toString("dd MMMM yyyy"));

    // KPI Row
    html += QString(R"(<div class='kpi-row'>
<div class='kpi'><div class='kpi-val'>%1</div><div class='kpi-lbl'>Articles</div></div>
<div class='kpi'><div class='kpi-val'>%2</div><div class='kpi-lbl'>Catégories</div></div>
<div class='kpi'><div class='kpi-val'>%3 DT</div><div class='kpi-lbl'>Prix moyen</div></div>
<div class='kpi'><div class='kpi-val'>%4%%</div><div class='kpi-lbl'>Marge moy.</div></div>
</div>)").arg(filteredArticles.size()).arg(countByCat.size())
        .arg(prixMoy, 0, 'f', 0).arg(margeMoy, 0, 'f', 0);

    // Helper pour générer une card article (design grille moderne)
    auto buildArticleCard = [&](const Article &a) -> QString {
        double marge = a.getCoutFabrication() > 0 
            ? (a.getPrixUnitaire() - a.getCoutFabrication()) / a.getCoutFabrication() * 100 : 0;
        QString margeClass = marge >= 50 ? "marge-high" : marge >= 25 ? "marge-mid" : "marge-low";
        QString statusClass = a.getStatut() == "disponible" ? "status-dispo"
                            : a.getStatut() == "en_production" ? "status-prod" : "status-obs";
        QString statusText = a.getStatut() == "disponible" ? "DISPO"
                           : a.getStatut() == "en_production" ? "PROD" : "OBS";

        QString imgHtml;
        if (includeImages) {
            QString imgUri = imageToDataUri(a.getImagePath());
            if (!imgUri.isEmpty()) {
                imgHtml = QString("<img src='%1' alt='%2'>").arg(imgUri, a.getNom());
            } else {
                imgHtml = QString("<div class='color-box' style='background:rgb(%1,%2,%3);'>%4</div>")
                          .arg(a.getCouleurR()).arg(a.getCouleurG()).arg(a.getCouleurB())
                          .arg(a.getCategorie().left(3).toUpper());
            }
        } else {
            imgHtml = QString("<div class='color-box' style='background:rgb(%1,%2,%3);'>%4</div>")
                      .arg(a.getCouleurR()).arg(a.getCouleurG()).arg(a.getCouleurB())
                      .arg(a.getCategorie().left(3).toUpper());
        }

        return QString(R"(<div class='product-card'>
<div class='product-img'>%1<span class='status-badge %2'>%3</span></div>
<div class='product-info'>
    <div class='product-name'>%4</div>
    <div class='product-ref'>%5</div>
    <div class='product-type'>%6 | %7</div>
    <div class='product-price'>%8 DT</div>
    <span class='product-marge %9'>Marge: %10%</span>
</div></div>)")
            .arg(imgHtml).arg(statusClass).arg(statusText)
            .arg(a.getNom()).arg(a.getReference())
            .arg(a.getType()).arg(a.getDimensions())
            .arg(a.getPrixUnitaire(), 0, 'f', 2).arg(margeClass).arg(marge, 0, 'f', 0);
    };

    // Générer les cards en grille
    if (organizeByCategory) {
        QStringList categories = {"Sacs", "Portefeuilles", "Ceintures", "Accessoires", "Chaussures"};
        for (const QString &cat : categories) {
            QList<Article> artsCat;
            for (const Article &a : filteredArticles)
                if (a.getCategorie() == cat) artsCat.append(a);
            if (artsCat.isEmpty()) continue;
            
            html += QString("<div class='section-title'>%1 (%2 articles)</div>").arg(cat).arg(artsCat.size());
            html += "<div class='grid'>";
            for (const Article &a : artsCat) html += buildArticleCard(a);
            html += "</div>";
        }
        // Autres catégories
        QList<Article> autres;
        for (const Article &a : filteredArticles)
            if (!categories.contains(a.getCategorie())) autres.append(a);
        if (!autres.isEmpty()) {
            html += QString("<div class='section-title'>Autres (%1)</div>").arg(autres.size());
            html += "<div class='grid'>";
            for (const Article &a : autres) html += buildArticleCard(a);
            html += "</div>";
        }
    } else {
        html += "<div class='section-title'>Catalogue des Produits</div>";
        html += "<div class='grid'>";
        for (const Article &a : filteredArticles) html += buildArticleCard(a);
        html += "</div>";
    }

    // Footer
    html += QString(R"(<div class='footer'>
<span><strong>CUIREA</strong> - Maroquinerie de qualite</span>
<span>contact@cuirea.tn | +216 XX XXX XXX</span>
<span>Genere le %1</span>
</div></body></html>)").arg(QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm"));

    // ----------------------------------------------------------------------
    // DIALOG DE PREVIEW AVANT EXPORT
    // ----------------------------------------------------------------------
    
    QDialog previewDlg(this);
    previewDlg.setWindowTitle("Apercu du Catalogue PDF - CUIREA");
    previewDlg.setMinimumSize(950, 750);
    previewDlg.setStyleSheet(
        "QDialog{background:#FAF5F0;}"
        "QLabel{color:#3E2723;}"
        "QPushButton{background:#8D6E63;color:white;border:none;border-radius:8px;"
        "padding:12px 28px;font-weight:bold;font-size:13px;}"
        "QPushButton:hover{background:#A0826D;}"
        "QPushButton#exportBtn{background:qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #2E7D32,stop:1 #43A047);}"
        "QPushButton#exportBtn:hover{background:#4CAF50;}"
        "QPushButton#cancelBtn{background:#C62828;}"
        "QPushButton#cancelBtn:hover{background:#E53935;}"
    );

    QVBoxLayout *previewLay = new QVBoxLayout(&previewDlg);
    previewLay->setContentsMargins(15,15,15,15);
    previewLay->setSpacing(12);

    auto *previewTitle = new QLabel("APERCU DU CATALOGUE PDF");
    previewTitle->setAlignment(Qt::AlignCenter);
    previewTitle->setStyleSheet("font-size:16px;font-weight:bold;color:white;padding:12px;"
                                "background:#8D6E63;border-radius:10px;");
    previewLay->addWidget(previewTitle);

    auto *previewBrowser = new QTextBrowser();
    previewBrowser->setHtml(html);
    previewBrowser->setStyleSheet("QTextBrowser{background:white;border:2px solid #8D6E63;"
                                  "border-radius:10px;padding:10px;}");
    previewBrowser->setOpenExternalLinks(false);
    previewLay->addWidget(previewBrowser, 1);

    auto *infoLbl = new QLabel(QString("%1 articles  -  Prix moyen: %2 DT  -  Marge moyenne: %3%")
                               .arg(filteredArticles.size())
                               .arg(prixMoy, 0, 'f', 2)
                               .arg(margeMoy, 0, 'f', 1));
    infoLbl->setAlignment(Qt::AlignCenter);
    infoLbl->setStyleSheet("color:#5D4037;font-size:12px;padding:8px;background:#EFEBE9;border-radius:6px;");
    previewLay->addWidget(infoLbl);

    QHBoxLayout *btnLay = new QHBoxLayout();
    btnLay->setSpacing(15);

    auto *btnExport = new QPushButton("Exporter en PDF");
    btnExport->setObjectName("exportBtn");
    btnExport->setMinimumWidth(200);

    auto *btnCancel = new QPushButton("Annuler");
    btnCancel->setObjectName("cancelBtn");
    btnCancel->setMinimumWidth(130);

    btnLay->addStretch();
    btnLay->addWidget(btnExport);
    btnLay->addWidget(btnCancel);
    btnLay->addStretch();
    previewLay->addLayout(btnLay);

    QString selectedFile;
    bool exportConfirmed = false;

    connect(btnExport, &QPushButton::clicked, [&]() {
        QString fn = QFileDialog::getSaveFileName(&previewDlg, "Exporter Catalogue PDF",
                                                   QDir::homePath() + "/Catalogue_CUIREA_" + 
                                                   QDateTime::currentDateTime().toString("yyyyMMdd_HHmm") + ".pdf",
                                                   "PDF (*.pdf)");
        if (!fn.isEmpty()) {
            selectedFile = fn;
            exportConfirmed = true;
            previewDlg.accept();
        }
    });

    connect(btnCancel, &QPushButton::clicked, &previewDlg, &QDialog::reject);

    if (previewDlg.exec() == QDialog::Accepted && exportConfirmed && !selectedFile.isEmpty()) {
        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(selectedFile);
        printer.setPageSize(QPageSize::A4);
        printer.setPageMargins(QMarginsF(10, 10, 10, 10), QPageLayout::Millimeter);

        QTextDocument doc;
        doc.setHtml(html);
        // Définir la largeur du document pour correspondre à la largeur de la page A4
        QSizeF pageSize = printer.pageRect(QPrinter::Point).size();
        doc.setPageSize(pageSize);
        doc.print(&printer);

        QMessageBox::information(this, "Export Reussi", 
            QString("Le catalogue PDF a ete exporte avec succes !\n\n"
                    "Fichier: %1\n"
                    "%2 articles exportes\n"
                    "Prix moyen: %3 DT\n"
                    "Marge moyenne: %4%")
            .arg(selectedFile)
            .arg(filteredArticles.size())
            .arg(prixMoy, 0, 'f', 2)
            .arg(margeMoy, 0, 'f', 1));
    }
}

void MainWindow::on_btnAnalyseRentabilite_clicked()
{
    if (articles.isEmpty()) { QMessageBox::information(this,"","Aucun article à analyser."); return; }

    QDialog dlg(this);
    dlg.setWindowTitle("?? Analyse de Rentabilité Avancée - CUIREA");
    dlg.setMinimumSize(1150, 780);
    dlg.setStyleSheet(
        "QDialog{background:#1A1A2E;}"
        "QLabel{color:#E0E0E0;}"
        "QGroupBox{border:2px solid #8D6E63;border-radius:10px;margin-top:10px;"
        "padding-top:10px;font-weight:bold;color:#FFCC80;font-size:11px;}"
        "QTableWidget{background:#16213E;color:white;gridline-color:#2A2A4A;border:none;}"
        "QHeaderView::section{background:#8D6E63;color:white;padding:6px;font-weight:bold;border:none;}"
        "QTableWidget{alternate-background-color:#1E2A4A;}"
        "QPushButton{background:#8D6E63;color:white;border:none;border-radius:8px;"
        "padding:8px 20px;font-weight:bold;}"
        "QPushButton:hover{background:#A0826D;}"
    );

    QVBoxLayout *mainLay = new QVBoxLayout(&dlg);
    mainLay->setContentsMargins(12,12,12,10); mainLay->setSpacing(8);

    // Titre
    auto *titleLbl = new QLabel("??  ANALYSE DE RENTABILITé AVANCéE  |  CUIREA Smart Factory");
    titleLbl->setAlignment(Qt::AlignCenter);
    titleLbl->setStyleSheet("font-size:14px;font-weight:bold;color:#FFCC80;padding:8px;"
                            "background:#0F3460;border-radius:10px;border:1px solid #8D6E63;");
    mainLay->addWidget(titleLbl);

    // -- Calculs ------------------------------------------------------------
    double totalMarge=0, totalPrix=0, totalCout=0;
    int exc=0, bon=0, moy=0, fai=0;
    QMap<QString,double> margeParCat;
    QMap<QString,int> nbParCat;
    QList<QPair<double,QString>> topMarges; // pour top 3

    for (const Article &a : articles) {
        double marge = a.getPrixUnitaire() - a.getCoutFabrication();
        double mp = a.getCoutFabrication()>0 ? marge/a.getCoutFabrication()*100 : 0;
        totalMarge += marge; totalPrix += a.getPrixUnitaire(); totalCout += a.getCoutFabrication();
        margeParCat[a.getCategorie()] += marge;
        nbParCat[a.getCategorie()]++;
        topMarges.append({mp, a.getNom()});
        if (mp>=50) ++exc; else if (mp>=30) ++bon; else if (mp>=15) ++moy; else ++fai;
    }
    std::sort(topMarges.begin(), topMarges.end(), [](auto &a, auto &b){ return a.first > b.first; });
    double margeGlobale = totalCout>0 ? (totalMarge/totalCout)*100 : 0;

    // -- KPI Cards ----------------------------------------------------------
    auto makeKPI = [](const QString &val, const QString &lbl, const QString &col) {
        auto *w = new QWidget();
        w->setStyleSheet(QString("background:%1;border-radius:10px;").arg(col));
        auto *l = new QVBoxLayout(w); l->setContentsMargins(8,8,8,8);
        auto *v = new QLabel(val); v->setAlignment(Qt::AlignCenter);
        v->setStyleSheet("font-size:20px;font-weight:bold;color:white;");
        auto *lb = new QLabel(lbl); lb->setAlignment(Qt::AlignCenter);
        lb->setStyleSheet("font-size:9px;color:rgba(255,255,255,0.8);");
        l->addWidget(v); l->addWidget(lb); w->setMinimumHeight(65);
        return w;
    };
    QHBoxLayout *kpiLay = new QHBoxLayout();
    kpiLay->addWidget(makeKPI(QString::number(totalMarge,'f',0)+" DT","Marge Totale","#1B5E20"));
    kpiLay->addWidget(makeKPI(QString::number(margeGlobale,'f',1)+"%","Marge Globale","#4A148C"));
    kpiLay->addWidget(makeKPI(QString::number(totalPrix/qMax((int)articles.size(),1),'f',0)+" DT","Prix Moyen","#006064"));
    kpiLay->addWidget(makeKPI(QString::number(totalCout/qMax((int)articles.size(),1),'f',0)+" DT","Coét Moyen","#BF360C"));
    kpiLay->addWidget(makeKPI(QString::number(exc)+" ?","Excellente =50%","#1B5E20"));
    kpiLay->addWidget(makeKPI(QString::number(fai)+" ?","Faible <15%","#B71C1C"));
    mainLay->addLayout(kpiLay);

    // -- Corps : tableau + graphiques ---------------------------------------
    QHBoxLayout *bodyLay = new QHBoxLayout(); bodyLay->setSpacing(8);

    // Tableau détaillé
    QGroupBox *tblBox = new QGroupBox("  ??  Détail par Article");
    QVBoxLayout *tblLay = new QVBoxLayout(tblBox);
    auto *tbl = new QTableWidget();
    tbl->setColumnCount(7);
    tbl->setHorizontalHeaderLabels({"Nom","Categorie","Cout (DT)","Prix (DT)","Marge (DT)","Marge (%)","Niveau"});
    tbl->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tbl->setAlternatingRowColors(true);
    tbl->verticalHeader()->setVisible(false);
    tbl->setSelectionBehavior(QAbstractItemView::SelectRows);
    tbl->setRowCount(articles.size());

    for (int i=0; i<articles.size(); ++i) {
        const Article &a = articles[i];
        double marge = a.getPrixUnitaire()-a.getCoutFabrication();
        double mp = a.getCoutFabrication()>0 ? marge/a.getCoutFabrication()*100 : 0;
        QString ren; QColor col;
        if      (mp>=50){ ren="?? Excellente"; col=QColor("#A5D6A7"); }
        else if (mp>=30){ ren="? Bonne";      col=QColor("#80CBC4"); }
        else if (mp>=15){ ren="~ Moyenne";     col=QColor("#FFE082"); }
        else if (mp> 0) { ren="? Faible";     col=QColor("#FFAB91"); }
        else            { ren="? Négative";    col=QColor("#EF9A9A"); }

        tbl->setItem(i,0,new QTableWidgetItem(a.getNom()));
        tbl->setItem(i,1,new QTableWidgetItem(a.getCategorie()));
        tbl->setItem(i,2,new QTableWidgetItem(QString::number(a.getCoutFabrication(),'f',2)));
        tbl->setItem(i,3,new QTableWidgetItem(QString::number(a.getPrixUnitaire(),'f',2)));
        tbl->setItem(i,4,new QTableWidgetItem(QString::number(marge,'f',2)));
        tbl->setItem(i,5,new QTableWidgetItem(QString::number(mp,'f',1)+"%"));
        auto *ri = new QTableWidgetItem(ren);
        ri->setForeground(QBrush(col));
        QFont f=ri->font(); f.setBold(true); ri->setFont(f);
        tbl->setItem(i,6,ri);
    }
    tblLay->addWidget(tbl);
    bodyLay->addWidget(tblBox, 5);

    // Graphiques droite
    QVBoxLayout *rightLay = new QVBoxLayout(); rightLay->setSpacing(8);

    // Barres groupées : prix vs coét par catégorie
    {
        QGroupBox *gb = new QGroupBox("  ??  Prix vs Coét par Catégorie");
        QVBoxLayout *l = new QVBoxLayout(gb);
        auto *sPrix = new QBarSet("Prix Moyen"); sPrix->setColor(QColor("#FFCC80"));
        auto *sCout = new QBarSet("Coét Moyen"); sCout->setColor(QColor("#EF9A9A"));
        QStringList cats;
        for (auto it=nbParCat.begin(); it!=nbParCat.end(); ++it) {
            cats << it.key();
            double sp=0, sc=0;
            for (const Article &a : articles) {
                if (a.getCategorie()==it.key()) { sp+=a.getPrixUnitaire(); sc+=a.getCoutFabrication(); }
            }
            *sPrix << sp/it.value();
            *sCout << sc/it.value();
        }
        auto *series = new QBarSeries(); series->append(sCout); series->append(sPrix);
        auto *chart = new QChart(); chart->addSeries(series);
        chart->setBackgroundBrush(QBrush(QColor("#16213E")));
        chart->legend()->setLabelColor(Qt::white);
        chart->setAnimationOptions(QChart::SeriesAnimations);
        auto *axX = new QBarCategoryAxis(); axX->append(cats); axX->setLabelsColor(Qt::white);
        chart->addAxis(axX,Qt::AlignBottom); series->attachAxis(axX);
        auto *axY = new QValueAxis(); axY->setLabelFormat("%.0f"); axY->setLabelsColor(Qt::white);
        chart->addAxis(axY,Qt::AlignLeft); series->attachAxis(axY);
        auto *cv = new QChartView(chart); cv->setRenderHint(QPainter::Antialiasing);
        cv->setMinimumHeight(220);
        l->addWidget(cv); rightLay->addWidget(gb);
    }

    // Courbe marge % par article
    {
        QGroupBox *gb = new QGroupBox("  Courbe Marge % par Article");
        QVBoxLayout *l = new QVBoxLayout(gb);
        auto *series = new QLineSeries(); series->setName("Marge %");
        series->setColor(QColor("#FFCC80")); series->setPen(QPen(QColor("#FFCC80"),2));
        auto *seuil = new QLineSeries(); seuil->setName("Seuil 30%");
        seuil->setColor(QColor("#4CAF50")); seuil->setPen(QPen(QColor("#4CAF50"),1,Qt::DashLine));
        for (int i=0; i<articles.size(); ++i) {
            double mp = articles[i].getCoutFabrication()>0
                ? (articles[i].getPrixUnitaire()-articles[i].getCoutFabrication())
                  /articles[i].getCoutFabrication()*100 : 0;
            series->append(i, mp);
            seuil->append(i, 30.0);
        }
        auto *chart = new QChart();
        chart->addSeries(series); chart->addSeries(seuil);
        chart->setBackgroundBrush(QBrush(QColor("#16213E")));
        chart->legend()->setLabelColor(Qt::white);
        chart->setAnimationOptions(QChart::SeriesAnimations);
        chart->createDefaultAxes();
        chart->axes(Qt::Horizontal).first()->setLabelsColor(Qt::white);
        chart->axes(Qt::Vertical).first()->setLabelsColor(Qt::white);
        auto *cv = new QChartView(chart); cv->setRenderHint(QPainter::Antialiasing);
        cv->setMinimumHeight(220);
        l->addWidget(cv); rightLay->addWidget(gb);
    }

    bodyLay->addLayout(rightLay, 4);
    mainLay->addLayout(bodyLay);

    // Bouton fermer
    auto *btnClose = new QPushButton("Fermer");
    btnClose->setMaximumWidth(150);
    connect(btnClose, &QPushButton::clicked, &dlg, &QDialog::accept);
    QHBoxLayout *bl = new QHBoxLayout(); bl->addStretch(); bl->addWidget(btnClose);
    mainLay->addLayout(bl);

    dlg.exec();
}

void MainWindow::on_btnAideDecision_clicked()
{
    QDialog dlg(this);
    dlg.setWindowTitle("CUIREA - Aide a la Decision Tarifaire");
    dlg.setMinimumSize(1200, 750);
    dlg.showMaximized();
    dlg.setStyleSheet(
        "QDialog{background:#FAF5F0;color:#3E2723;}"
        "QTabWidget::pane{border:2px solid #8D6E63;border-radius:8px;background:#FFFFFF;}"
        "QTabBar::tab{background:#D7CCC8;color:#5D4037;padding:14px 40px;border-radius:6px 6px 0 0;font-weight:bold;font-size:14px;}"
        "QTabBar::tab:selected{background:#8D6E63;color:white;}"
        "QGroupBox{border:2px solid #8D6E63;border-radius:10px;margin-top:10px;padding-top:10px;font-weight:bold;color:#5D4037;font-size:11px;background:#FFFFFF;}"
        "QLabel{color:#3E2723;}"
        "QComboBox,QDoubleSpinBox,QSlider{background:#FFFFFF;border:2px solid #8D6E63;border-radius:6px;padding:6px;color:#3E2723;font-size:12px;}"
        "QComboBox::drop-down{border:none;}"
        "QComboBox QAbstractItemView{background:#FFFFFF;color:#3E2723;selection-background-color:#D7CCC8;}"
        "QTableWidget{background:#FFFFFF;color:#3E2723;gridline-color:#D7CCC8;border:none;alternate-background-color:#FAF5F0;}"
        "QHeaderView::section{background:#8D6E63;color:white;padding:6px;font-weight:bold;border:none;}"
        "QPushButton{background:qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #8D6E63,stop:1 #A0826D);color:white;border:none;border-radius:8px;padding:10px 20px;font-weight:bold;font-size:12px;}"
        "QPushButton:hover{background:qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #A0826D,stop:1 #BCAAA4);}"
        "QSlider::groove:horizontal{background:#D7CCC8;height:8px;border-radius:4px;}"
        "QSlider::handle:horizontal{background:#8D6E63;width:20px;height:20px;border-radius:10px;margin:-6px 0;}"
        "QSlider::sub-page:horizontal{background:#8D6E63;border-radius:4px;}"
    );

    QVBoxLayout *mainLay = new QVBoxLayout(&dlg);
    mainLay->setContentsMargins(15,15,15,12);
    mainLay->setSpacing(10);

    // Titre
    auto *titleLbl = new QLabel("AIDE A LA DECISION TARIFAIRE  |  Algorithme k-NN Pondere (k=5)");
    titleLbl->setAlignment(Qt::AlignCenter);
    titleLbl->setStyleSheet("font-size:17px;font-weight:bold;color:white;padding:14px;background:#8D6E63;border-radius:12px;border:2px solid #5D4037;");
    mainLay->addWidget(titleLbl);

    auto *srcLbl = new QLabel("Base : REF_ARTICLES_MARCHE (50 articles tunisiens)  |  Distance = |Cout_ref - Cout| + Penalite_Type(20) + Penalite_Couleur(5)  |  Prix = 60% k-NN + 25% Ratio_Type + 15% Ratio_Couleur");
    srcLbl->setAlignment(Qt::AlignCenter);
    srcLbl->setStyleSheet("font-size:10px;color:#5D4037;padding:4px;");
    mainLay->addWidget(srcLbl);

    auto *tabs = new QTabWidget();
    mainLay->addWidget(tabs);

    // ══════════════════════════════════════════════════════════════════════════
    // ONGLET 1 : PRÉDICTION & SIMULATION
    // ══════════════════════════════════════════════════════════════════════════
    auto *tab1 = new QWidget();
    auto *tab1Lay = new QHBoxLayout(tab1);
    tab1Lay->setSpacing(15);

    // Panneau Gauche
    QVBoxLayout *leftLay = new QVBoxLayout();
    leftLay->setSpacing(10);

    QGroupBox *formBox = new QGroupBox("  Caracteristiques de l'Article");
    QFormLayout *formLay = new QFormLayout(formBox);
    formLay->setSpacing(12);
    auto *cbCat = new QComboBox();
    cbCat->addItems({"Sacs","Portefeuilles","Ceintures","Accessoires","Chaussures"});
    auto *cbType = new QComboBox();
    cbType->addItems({"Sac à main","Sac bandoulière","Tote bag","Sac à dos","Portefeuille long","Portefeuille compact","Porte-cartes","Ceinture classique","Ceinture fashion","Ceinture luxe","Porte-monnaie","Pochette","Bandoulière","Porte-clés"});
    auto *cbCouleur = new QComboBox();
    cbCouleur->addItems({"Noir","Marron","Camel","Cognac","Beige","Blanc","Rouge","Bleu Marine","Gris","Vert"});
    formLay->addRow("Catégorie :", cbCat);
    formLay->addRow("Type :", cbType);
    formLay->addRow("Couleur :", cbCouleur);
    leftLay->addWidget(formBox);

    QGroupBox *sliderBox = new QGroupBox("  Cout de Fabrication (Slider Interactif)");
    QVBoxLayout *sliderLay = new QVBoxLayout(sliderBox);
    auto *sliderCout = new QSlider(Qt::Horizontal);
    sliderCout->setRange(5, 200);
    sliderCout->setValue(30);
    auto *lblSliderVal = new QLabel("Coût : 30 DT");
    lblSliderVal->setAlignment(Qt::AlignCenter);
    lblSliderVal->setStyleSheet("font-size:20px;font-weight:bold;color:#5D4037;padding:10px;");
    sliderLay->addWidget(sliderCout);
    sliderLay->addWidget(lblSliderVal);
    leftLay->addWidget(sliderBox);

    QGroupBox *resBox = new QGroupBox("  Resultats Temps Reel");
    QVBoxLayout *resLay = new QVBoxLayout(resBox);
    resLay->setSpacing(8);
    
    auto *lblPrixPredit = new QLabel("Prix Prédit : — DT");
    lblPrixPredit->setAlignment(Qt::AlignCenter);
    lblPrixPredit->setStyleSheet("font-size:26px;font-weight:bold;color:white;padding:14px;background:#8D6E63;border-radius:12px;border:2px solid #5D4037;");
    
    auto *lblMarge = new QLabel("Marge : — DT (—%)");
    lblMarge->setAlignment(Qt::AlignCenter);
    lblMarge->setStyleSheet("font-size:15px;font-weight:bold;color:#2E7D32;padding:8px;");
    
    auto *margeBar = new QProgressBar();
    margeBar->setRange(0, 200);
    margeBar->setValue(0);
    margeBar->setFormat("Marge: %v%");
    margeBar->setStyleSheet("QProgressBar{background:#EFEBE9;border:2px solid #8D6E63;border-radius:6px;height:26px;font-weight:bold;color:#3E2723;}QProgressBar::chunk{background:qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #B71C1C,stop:0.25 #F57F17,stop:0.5 #4CAF50,stop:1 #1B5E20);}");
    
    auto *lblSeuil = new QLabel("Seuil Rentabilité (×2) : — DT");
    lblSeuil->setAlignment(Qt::AlignCenter);
    lblSeuil->setStyleSheet("font-size:12px;color:#5D4037;padding:5px;");
    
    auto *lblReco = new QLabel("Déplacez le slider pour voir la prédiction");
    lblReco->setWordWrap(true);
    lblReco->setAlignment(Qt::AlignCenter);
    lblReco->setStyleSheet("font-size:11px;color:#5D4037;padding:10px;background:#EFEBE9;border-radius:8px;border:1px solid #8D6E63;");
    
    resLay->addWidget(lblPrixPredit);
    resLay->addWidget(lblMarge);
    resLay->addWidget(margeBar);
    resLay->addWidget(lblSeuil);
    resLay->addWidget(lblReco);
    leftLay->addWidget(resBox);
    leftLay->addStretch();
    tab1Lay->addLayout(leftLay, 3);

    // Panneau Droit : Graphiques
    QVBoxLayout *rightLay = new QVBoxLayout();
    rightLay->setSpacing(10);

    QGroupBox *barChartBox = new QGroupBox("  Comparaison Prix Actuel vs Prix Predit (6 premiers articles)");
    QVBoxLayout *barChartLay = new QVBoxLayout(barChartBox);
    auto *setActuel = new QBarSet("Prix Actuel");
    auto *setPredit = new QBarSet("Prix Prédit");
    setActuel->setColor(QColor("#8D6E63"));
    setPredit->setColor(QColor("#FFCC80"));
    QStringList noms;
    int maxArt = qMin((int)articles.size(), 6);
    for (int i = 0; i < maxArt; ++i) {
        const Article &a = articles[i];
        auto res = Article::predirePrixAvance(a.getCategorie(), a.getType(), a.getCouleur(), a.getCoutFabrication());
        *setActuel << a.getPrixUnitaire();
        *setPredit << res.prixPredit;
        noms << (a.getNom().length() > 8 ? a.getNom().left(8) + "…" : a.getNom());
    }
    auto *barSeries = new QBarSeries();
    barSeries->append(setActuel);
    barSeries->append(setPredit);
    auto *barChart = new QChart();
    barChart->addSeries(barSeries);
    barChart->setBackgroundBrush(QBrush(QColor("#FFFFFF")));
    barChart->legend()->setLabelColor(QColor("#3E2723"));
    barChart->setAnimationOptions(QChart::SeriesAnimations);
    auto *axX = new QBarCategoryAxis();
    axX->append(noms);
    axX->setLabelsColor(QColor("#3E2723"));
    barChart->addAxis(axX, Qt::AlignBottom);
    barSeries->attachAxis(axX);
    auto *axY = new QValueAxis();
    axY->setLabelFormat("%.0f DT");
    axY->setLabelsColor(QColor("#3E2723"));
    barChart->addAxis(axY, Qt::AlignLeft);
    barSeries->attachAxis(axY);
    auto *barChartView = new QChartView(barChart);
    barChartView->setRenderHint(QPainter::Antialiasing);
    barChartView->setMinimumHeight(220);
    barChartLay->addWidget(barChartView);
    rightLay->addWidget(barChartBox);

    QGroupBox *curveBox = new QGroupBox("  Courbe Prix/Cout (point rouge = position actuelle)");
    QVBoxLayout *curveLay = new QVBoxLayout(curveBox);
    auto *lineSeries = new QLineSeries();
    lineSeries->setName("Prix Prédit");
    lineSeries->setColor(QColor("#FFCC80"));
    lineSeries->setPen(QPen(QColor("#FFCC80"), 3));
    auto *seuilSeries = new QLineSeries();
    seuilSeries->setName("Seuil ×2");
    seuilSeries->setColor(QColor("#4CAF50"));
    seuilSeries->setPen(QPen(QColor("#4CAF50"), 2, Qt::DashLine));
    auto *pointSeries = new QScatterSeries();
    pointSeries->setName("Position");
    pointSeries->setMarkerSize(18);
    pointSeries->setColor(QColor("#F44336"));
    for (int c = 5; c <= 200; c += 5) {
        auto r = Article::predirePrixAvance("Sacs", "Sac à main", "Noir", c);
        lineSeries->append(c, r.prixPredit);
        seuilSeries->append(c, c * 2.0);
    }
    auto r0 = Article::predirePrixAvance("Sacs", "Sac à main", "Noir", 30);
    pointSeries->append(30, r0.prixPredit);
    auto *lineChart = new QChart();
    lineChart->addSeries(lineSeries);
    lineChart->addSeries(seuilSeries);
    lineChart->addSeries(pointSeries);
    lineChart->setBackgroundBrush(QBrush(QColor("#FFFFFF")));
    lineChart->legend()->setLabelColor(QColor("#3E2723"));
    lineChart->setAnimationOptions(QChart::AllAnimations);
    lineChart->createDefaultAxes();
    lineChart->axes(Qt::Horizontal).first()->setTitleText("Coût (DT)");
    lineChart->axes(Qt::Horizontal).first()->setLabelsColor(QColor("#3E2723"));
    lineChart->axes(Qt::Vertical).first()->setTitleText("Prix (DT)");
    lineChart->axes(Qt::Vertical).first()->setLabelsColor(QColor("#3E2723"));
    auto *lineChartView = new QChartView(lineChart);
    lineChartView->setRenderHint(QPainter::Antialiasing);
    lineChartView->setMinimumHeight(280);
    curveLay->addWidget(lineChartView);
    rightLay->addWidget(curveBox);
    tab1Lay->addLayout(rightLay, 5);
    tabs->addTab(tab1, "Prediction & Simulation");

    // ══════════════════════════════════════════════════════════════════════════
    // ONGLET 2 : REVUE DES PRIX
    // ══════════════════════════════════════════════════════════════════════════
    auto *tab2 = new QWidget();
    auto *tab2Lay = new QVBoxLayout(tab2);
    tab2Lay->setSpacing(12);

    // KPI en haut
    int nbArticles = articles.size();
    double caTotal = 0, gainPotentiel = 0;
    int nbAjuster = 0;
    for (const Article &a : articles) {
        caTotal += a.getPrixUnitaire();
        auto res = Article::predirePrixAvance(a.getCategorie(), a.getType(), a.getCouleur(), a.getCoutFabrication());
        double ecart = res.prixPredit - a.getPrixUnitaire();
        if (qAbs(ecart) > a.getPrixUnitaire() * 0.05) {
            ++nbAjuster;
            if (ecart > 0) gainPotentiel += ecart;
        }
    }

    auto makeKPI = [](const QString &val, const QString &label, const QString &color) {
        auto *w = new QWidget();
        w->setStyleSheet(QString("background:%1;border-radius:12px;").arg(color));
        auto *lay = new QVBoxLayout(w);
        auto *vl = new QLabel(val);
        vl->setAlignment(Qt::AlignCenter);
        vl->setStyleSheet("font-size:26px;font-weight:bold;color:white;");
        auto *ll = new QLabel(label);
        ll->setAlignment(Qt::AlignCenter);
        ll->setStyleSheet("font-size:11px;color:rgba(255,255,255,0.85);");
        lay->addWidget(vl);
        lay->addWidget(ll);
        w->setMinimumHeight(85);
        return w;
    };

    QHBoxLayout *kpiLay = new QHBoxLayout();
    kpiLay->addWidget(makeKPI(QString::number(nbArticles), "📦 Articles", "#8D6E63"));
    kpiLay->addWidget(makeKPI(QString::number(caTotal, 'f', 0) + " DT", "💰 CA Total", "#6D4C41"));
    kpiLay->addWidget(makeKPI(QString::number(nbAjuster), "⚠️ Prix à Ajuster", "#A1887F"));
    kpiLay->addWidget(makeKPI(QString::number(gainPotentiel, 'f', 0) + " DT", "📈 Gain Potentiel", "#5D4037"));
    tab2Lay->addLayout(kpiLay);

    QGroupBox *tblBox = new QGroupBox("  Revue des Prix par Article");
    QVBoxLayout *tblLay = new QVBoxLayout(tblBox);
    auto *revueTbl = new QTableWidget();
    revueTbl->setColumnCount(6);
    revueTbl->setHorizontalHeaderLabels({"Nom", "Catégorie", "Prix Actuel", "Prix IA", "Écart", "Action"});
    revueTbl->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    revueTbl->setAlternatingRowColors(true);
    revueTbl->verticalHeader()->setVisible(false);
    revueTbl->setRowCount(articles.size());

    for (int i = 0; i < articles.size(); ++i) {
        const Article &a = articles[i];
        auto res = Article::predirePrixAvance(a.getCategorie(), a.getType(), a.getCouleur(), a.getCoutFabrication());
        double ecart = res.prixPredit - a.getPrixUnitaire();
        double ecartPct = a.getPrixUnitaire() > 0 ? (ecart / a.getPrixUnitaire()) * 100 : 0;

        QString action;
        QColor actionColor;
        if (ecartPct > 5) {
            action = QString("📈 Augmenter de %1 DT").arg(QString::number(ecart, 'f', 0));
            actionColor = QColor("#4CAF50");
        } else if (ecartPct < -5) {
            action = QString("📉 Réduire de %1 DT").arg(QString::number(qAbs(ecart), 'f', 0));
            actionColor = QColor("#F44336");
        } else {
            action = "✅ Prix correct";
            actionColor = QColor("#2196F3");
        }

        revueTbl->setItem(i, 0, new QTableWidgetItem(a.getNom()));
        revueTbl->setItem(i, 1, new QTableWidgetItem(a.getCategorie()));
        revueTbl->setItem(i, 2, new QTableWidgetItem(QString::number(a.getPrixUnitaire(), 'f', 2) + " DT"));
        revueTbl->setItem(i, 3, new QTableWidgetItem(QString::number(res.prixPredit, 'f', 2) + " DT"));
        auto *ecartItem = new QTableWidgetItem(QString("%1%").arg(QString::number(ecartPct, 'f', 1)));
        ecartItem->setForeground(QBrush(ecartPct > 0 ? QColor("#4CAF50") : ecartPct < 0 ? QColor("#F44336") : Qt::white));
        revueTbl->setItem(i, 4, ecartItem);
        auto *actionItem = new QTableWidgetItem(action);
        actionItem->setForeground(QBrush(actionColor));
        QFont f = actionItem->font();
        f.setBold(true);
        actionItem->setFont(f);
        revueTbl->setItem(i, 5, actionItem);
    }
    tblLay->addWidget(revueTbl);
    tab2Lay->addWidget(tblBox);

    // Graphiques en bas
    QHBoxLayout *chartsLay = new QHBoxLayout();

    QGroupBox *pieBox = new QGroupBox("  CA par Categorie");
    QVBoxLayout *pieLay = new QVBoxLayout(pieBox);
    auto *pieSeries = new QPieSeries();
    QMap<QString, double> caByCat;
    for (const Article &a : articles) caByCat[a.getCategorie()] += a.getPrixUnitaire();
    QStringList colors = {"#8D6E63", "#A1887F", "#6D4C41", "#5D4037", "#BCAAA4"};
    int ci = 0;
    for (auto it = caByCat.begin(); it != caByCat.end(); ++it, ++ci) {
        auto *slice = pieSeries->append(it.key(), it.value());
        slice->setColor(QColor(colors[ci % colors.size()]));
        slice->setLabelVisible(true);
        slice->setLabel(QString("%1\n%2 DT").arg(it.key()).arg(QString::number(it.value(), 'f', 0)));
        slice->setLabelColor(QColor("#3E2723"));
    }
    auto *pieChart = new QChart();
    pieChart->addSeries(pieSeries);
    pieChart->setBackgroundBrush(QBrush(QColor("#FFFFFF")));
    pieChart->legend()->setVisible(false);
    auto *pieView = new QChartView(pieChart);
    pieView->setRenderHint(QPainter::Antialiasing);
    pieLay->addWidget(pieView);
    chartsLay->addWidget(pieBox);

    QGroupBox *margeBox = new QGroupBox("  Marge Moyenne par Categorie");
    QVBoxLayout *margeLay = new QVBoxLayout(margeBox);
    auto *margeSet = new QBarSet("Marge %");
    margeSet->setColor(QColor("#FFCC80"));
    QMap<QString, QPair<double, int>> margeByCat;
    for (const Article &a : articles) {
        if (a.getCoutFabrication() > 0) {
            double marge = (a.getPrixUnitaire() - a.getCoutFabrication()) / a.getCoutFabrication() * 100;
            margeByCat[a.getCategorie()].first += marge;
            margeByCat[a.getCategorie()].second++;
        }
    }
    QStringList catNames;
    for (auto it = margeByCat.begin(); it != margeByCat.end(); ++it) {
        double avg = it.value().second > 0 ? it.value().first / it.value().second : 0;
        *margeSet << avg;
        catNames << it.key();
    }
    auto *margeSeries = new QBarSeries();
    margeSeries->append(margeSet);
    auto *margeChart = new QChart();
    margeChart->addSeries(margeSeries);
    margeChart->setBackgroundBrush(QBrush(QColor("#FFFFFF")));
    margeChart->legend()->setVisible(false);
    auto *margeAxX = new QBarCategoryAxis();
    margeAxX->append(catNames);
    margeAxX->setLabelsColor(QColor("#3E2723"));
    margeChart->addAxis(margeAxX, Qt::AlignBottom);
    margeSeries->attachAxis(margeAxX);
    auto *margeAxY = new QValueAxis();
    margeAxY->setLabelFormat("%.0f%%");
    margeAxY->setLabelsColor(QColor("#3E2723"));
    margeChart->addAxis(margeAxY, Qt::AlignLeft);
    margeSeries->attachAxis(margeAxY);
    auto *margeView = new QChartView(margeChart);
    margeView->setRenderHint(QPainter::Antialiasing);
    margeLay->addWidget(margeView);
    chartsLay->addWidget(margeBox);
    tab2Lay->addLayout(chartsLay);
    tabs->addTab(tab2, "Revue des Prix");

    // Bouton Fermer
    auto *btnClose = new QPushButton("Fermer");
    btnClose->setMaximumWidth(150);
    connect(btnClose, &QPushButton::clicked, &dlg, &QDialog::accept);
    QHBoxLayout *bl = new QHBoxLayout();
    bl->addStretch();
    bl->addWidget(btnClose);
    mainLay->addLayout(bl);

    // Connexions Slider
    auto updatePrediction = [&]() {
        double cout = sliderCout->value();
        lblSliderVal->setText(QString("Cout : %1 DT").arg(cout));
        auto res = Article::predirePrixAvance(cbCat->currentText(), cbType->currentText(), cbCouleur->currentText(), cout);
        lblPrixPredit->setText(QString("Prix Prédit : %1 DT").arg(QString::number(res.prixPredit, 'f', 2)));
        double marge = res.prixPredit - cout;
        double margePct = cout > 0 ? (marge / cout) * 100 : 0;
        lblMarge->setText(QString("Marge : %1 DT (%2%)").arg(QString::number(marge, 'f', 2)).arg(QString::number(margePct, 'f', 1)));
        margeBar->setValue(qMin(200, (int)margePct));
        double seuil = cout * 2.0;
        QString seuilStatus = res.prixPredit >= seuil ? "✅ Atteint" : "⚠️ Non atteint";
        lblSeuil->setText(QString("Seuil Rentabilité (×2) : %1 DT  %2").arg(QString::number(seuil, 'f', 2)).arg(seuilStatus));
        lblReco->setText(res.recommandation);
        lineSeries->clear();
        seuilSeries->clear();
        pointSeries->clear();
        for (int c = 5; c <= 200; c += 5) {
            auto r = Article::predirePrixAvance(cbCat->currentText(), cbType->currentText(), cbCouleur->currentText(), c);
            lineSeries->append(c, r.prixPredit);
            seuilSeries->append(c, c * 2.0);
        }
        pointSeries->append(cout, res.prixPredit);
    };

    connect(sliderCout, &QSlider::valueChanged, [&](int) { updatePrediction(); });
    connect(cbCat, QOverload<int>::of(&QComboBox::currentIndexChanged), [&](int) { updatePrediction(); });
    connect(cbType, QOverload<int>::of(&QComboBox::currentIndexChanged), [&](int) { updatePrediction(); });
    connect(cbCouleur, QOverload<int>::of(&QComboBox::currentIndexChanged), [&](int) { updatePrediction(); });
    updatePrediction();

    dlg.exec();
}

// Voice Recognition (SAPI)
#ifdef Q_OS_WIN
void MainWindow::initSAPI()
{
    // Utiliser le recognizer partagé Windows (déjà configuré avec le micro système)
    if (FAILED(CoCreateInstance(CLSID_SpSharedRecognizer, nullptr, CLSCTX_LOCAL_SERVER,
                                IID_ISpRecognizer, (void**)&spRecognizer))) {
        QMessageBox::critical(this, "Vocal", "Impossible d'initialiser SAPI.\nVérifiez que la reconnaissance vocale Windows est activée.");
        spRecognizer = nullptr;
        return;
    }
    if (FAILED(spRecognizer->CreateRecoContext(&spRecoContext))) {
        spRecognizer->Release(); spRecognizer = nullptr;
        return;
    }
    spRecoContext->SetInterest(SPFEI(SPEI_RECOGNITION), SPFEI(SPEI_RECOGNITION));
    spRecoContext->SetNotifyWin32Event();
    spRecoContext->CreateGrammar(1, &spGrammar);
    spGrammar->LoadDictation(nullptr, SPLO_STATIC);
    spGrammar->SetDictationState(SPRS_ACTIVE);
}
void MainWindow::stopSAPI()
{
    if (spGrammar)     { spGrammar->SetDictationState(SPRS_INACTIVE); spGrammar->Release();     spGrammar = nullptr; }
    if (spRecoContext) { spRecoContext->Release(); spRecoContext = nullptr; }
    if (spRecognizer)  { spRecognizer->Release();  spRecognizer  = nullptr; }
}

#endif

//send email


// ------------------------------------------------------------------------------
// MéTHODES DE GESTION DES PERMISSIONS ET AUTHENTIFICATION
// ------------------------------------------------------------------------------

void MainWindow::applyUserPermissions()
{
    UserSession &session = UserSession::instance();
    
    if (!session.isLoggedIn()) {
        return;
    }
    
    // Masquer/Afficher les onglets selon les permissions
    hideTabsBasedOnPermissions();
    
    // Activer/Désactiver les boutons CRUD
    updateCRUDButtons();
    
    // Personnaliser le menu - DéSACTIVé
    // customizeMenuBar();
}

void MainWindow::hideTabsBasedOnPermissions()
{
    UserSession &session = UserSession::instance();
    
    // Masquer les boutons de navigation selon les permissions
    ui->btnEmployees->setVisible(session.canAccessEmployees());
    ui->btnClients->setVisible(session.canAccessClients());
    ui->btnSuppliers->setVisible(session.canAccessFournisseurs());
    ui->btnRawMaterials->setVisible(session.canAccessMatieres());
    ui->btnProduction->setVisible(session.canAccessProduction());
    ui->btnProducts->setVisible(session.canAccessArticles());
    
    // Rediriger vers la premiére page accessible
    if (!session.canAccessEmployees() && ui->stackedWidget->currentIndex() == 0) {
        if (session.canAccessClients()) {
            on_btnClients_clicked();
        } else if (session.canAccessArticles()) {
            on_btnProducts_clicked();
        } else if (session.canAccessMatieres()) {
            on_btnRawMaterials_clicked();
        } else if (session.canAccessFournisseurs()) {
            on_btnSuppliers_clicked();
        } else if (session.canAccessProduction()) {
            on_btnProduction_clicked();
        }
    }
}

void MainWindow::updateCRUDButtons()
{
    UserSession &session = UserSession::instance();
    
    // Boutons Employés
    if (ui->btnAdd) ui->btnAdd->setEnabled(session.canCreate() && session.canAccessEmployees());
    if (ui->btnEdit) ui->btnEdit->setEnabled(session.canEdit() && session.canAccessEmployees());
    if (ui->btnDelete) ui->btnDelete->setEnabled(session.canDelete() && session.canAccessEmployees());
    if (ui->btnExport) ui->btnExport->setEnabled(session.canExport() && session.canAccessEmployees());
    if (ui->btnStatistics) ui->btnStatistics->setEnabled(session.canViewStatistics() && session.canAccessEmployees());
    
    // Boutons Clients
    if (ui->btnAddClient) ui->btnAddClient->setEnabled(session.canCreate() && session.canAccessClients());
    if (ui->btnEditClient) ui->btnEditClient->setEnabled(session.canEdit() && session.canAccessClients());
    if (ui->btnDeleteClient) ui->btnDeleteClient->setEnabled(session.canDelete() && session.canAccessClients());
    if (ui->btnExportClient) ui->btnExportClient->setEnabled(session.canExport() && session.canAccessClients());
    
    // Boutons Matières
    if (ui->btnAddMatiere) ui->btnAddMatiere->setEnabled(session.canCreate() && session.canAccessMatieres());
    if (ui->btnEditMatiere) ui->btnEditMatiere->setEnabled(session.canEdit() && session.canAccessMatieres());
    if (ui->btnDeleteMatiere) ui->btnDeleteMatiere->setEnabled(session.canDelete() && session.canAccessMatieres());
    if (ui->btnExportMatiere) ui->btnExportMatiere->setEnabled(session.canExport() && session.canAccessMatieres());
    if (ui->btnStatistiques) ui->btnStatistiques->setEnabled(session.canViewStatistics() && session.canAccessMatieres());
    
    // Boutons Fournisseurs
    if (ui->btnAddFournisseur) ui->btnAddFournisseur->setEnabled(session.canCreate() && session.canAccessFournisseurs());
    if (ui->btnEditFournisseur) ui->btnEditFournisseur->setEnabled(session.canEdit() && session.canAccessFournisseurs());
    if (ui->btnDeleteFournisseur) ui->btnDeleteFournisseur->setEnabled(session.canDelete() && session.canAccessFournisseurs());
    if (ui->btnExportFournisseur) ui->btnExportFournisseur->setEnabled(session.canExport() && session.canAccessFournisseurs());
    if (ui->btnStatsFournisseur) ui->btnStatsFournisseur->setEnabled(session.canViewStatistics() && session.canAccessFournisseurs());
    
    // Boutons Production
    if (ui->btnCreerProduction) ui->btnCreerProduction->setEnabled(session.canCreate() && session.canAccessProduction());
    if (ui->btnModifierProduction) ui->btnModifierProduction->setEnabled(session.canEdit() && session.canAccessProduction());
    if (ui->btnSupprimerProduction) ui->btnSupprimerProduction->setEnabled(session.canDelete() && session.canAccessProduction());
    if (ui->btnExcelProduction) ui->btnExcelProduction->setEnabled(session.canExport() && session.canAccessProduction());
    if (ui->btnStatistiquesProduction) ui->btnStatistiquesProduction->setEnabled(session.canViewStatistics() && session.canAccessProduction());
    
    // Boutons Articles
    if (ui->btnAddArticle) ui->btnAddArticle->setEnabled(session.canCreate() && session.canAccessArticles());
    if (ui->btnEditArticle) ui->btnEditArticle->setEnabled(session.canEdit() && session.canAccessArticles());
    if (ui->btnDeleteArticle) ui->btnDeleteArticle->setEnabled(session.canDelete() && session.canAccessArticles());
    if (ui->btnExportPdfArticle) ui->btnExportPdfArticle->setEnabled(session.canExport() && session.canAccessArticles());
    if (ui->btnStatistiquesArticle) ui->btnStatistiquesArticle->setEnabled(session.canViewStatistics() && session.canAccessArticles());
}

void MainWindow::customizeMenuBar()
{
    UserSession &session = UserSession::instance();
    
    // Ajouter un menu utilisateur
    QMenu *userMenu = menuBar()->addMenu(QString("?? %1").arg(session.getNomComplet()));
    
    // Style pour le menu déroulant
    userMenu->setStyleSheet(
        "QMenu { "
        "    background-color: #FAF5F0; "
        "    border: 2px solid #8D6E63; "
        "    border-radius: 6px; "
        "    padding: 8px; "
        "}"
        "QMenu::item { "
        "    padding: 8px 25px; "
        "    color: #291C0E; "
        "    border-radius: 4px; "
        "    background-color: transparent; "
        "}"
        "QMenu::item:selected { "
        "    background-color: #8D6E63; "
        "    color: white; "
        "}"
        "QMenu::separator { "
        "    height: 2px; "
        "    background: #BCAAA4; "
        "    margin: 5px 10px; "
        "}"
    );
    
    QAction *profileAction = userMenu->addAction("?? Mon Profil");
    connect(profileAction, &QAction::triggered, this, &MainWindow::showUserProfile);
    
    QAction *changePasswordAction = userMenu->addAction("?? Changer le mot de passe");
    connect(changePasswordAction, &QAction::triggered, this, &MainWindow::changePassword);
    
    userMenu->addSeparator();
    
    QAction *logoutAction = userMenu->addAction("?? Déconnexion");
    connect(logoutAction, &QAction::triggered, this, &MainWindow::logout);
}

void MainWindow::showUserProfile()
{
    UserSession &session = UserSession::instance();
    
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Profil Utilisateur");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStyleSheet(MSGBOX_STYLE);
    
    QString profileInfo = QString(
        "<h3 style='color:#8D6E63;'>Informations du Profil</h3>"
        "<table style='width:100%; margin-top:10px;'>"
        "<tr><td><b>Matricule:</b></td><td>%1</td></tr>"
        "<tr><td><b>Nom complet:</b></td><td>%2</td></tr>"
        "<tr><td><b>Email:</b></td><td>%3</td></tr>"
        "<tr><td><b>Poste:</b></td><td>%4</td></tr>"
        "<tr><td><b>Département:</b></td><td>%5</td></tr>"
        "<tr><td><b>Rôle système:</b></td><td><span style='color:#8D6E63;font-weight:bold;'>%6</span></td></tr>"
        "</table>"
    ).arg(session.getMatricule())
     .arg(session.getNomComplet())
     .arg(session.getEmail())
     .arg(session.getPoste())
     .arg(session.getDepartement())
     .arg(session.getRoleString());
    
    msgBox.setText(profileInfo);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

void MainWindow::changePassword()
{
    QMessageBox::information(this, "Changer le mot de passe",
                            "Fonctionnalité à venir.\n"
                            "Contactez l'administrateur pour changer votre mot de passe.");
}

void MainWindow::logout()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Déconnexion");
    msgBox.setText("étes-vous sér de vouloir vous déconnecter?");
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setStyleSheet(MSGBOX_STYLE);
    
    if (msgBox.exec() == QMessageBox::Yes) {
        UserSession::instance().logout();
        close();
        
        // Relancer le login
        QTimer::singleShot(100, []() {
            LoginDialog loginDlg;
            if (loginDlg.exec() == QDialog::Accepted) {
                MainWindow *mainWin = new MainWindow();
                mainWin->applyUserPermissions();
                mainWin->show();
            } else {
                QApplication::quit();
            }
        });
    }
}


// ══════════════════════════════════════════════════════════════════════════
// SYSTÈME DE POINTAGE RFID - Intégration complète
// ══════════════════════════════════════════════════════════════════════════

#include "pointagedialog.h"
#include <QSerialPort>
#include <QTime>

// ─────────────────────────────────────────────────────────────────────────────
//  setupArduinoMultiCartes — point d'entrée, lance la découverte automatique
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::setupArduinoMultiCartes()
{
    // ── Indicateur connexion overlay ──────────────────────────────────────
    m_arduinoIndicator = new QLabel(this);
    m_arduinoIndicator->setFixedSize(200, 24);
    m_arduinoIndicator->setAlignment(Qt::AlignCenter);
    m_arduinoIndicator->setStyleSheet(
        "QLabel { border-radius:12px; font-size:11px; font-weight:bold; "
        "color:white; background:#888; }");
    m_arduinoIndicator->setText("● Recherche Arduino...");
    m_arduinoIndicator->setVisible(false);
    m_arduinoIndicator->raise();

    // ── Timer fin de journée → marquer absents à 23h59 ───────────────────
    m_timerAbsences = new QTimer(this);
    connect(m_timerAbsences, &QTimer::timeout, [this]() {
        QTime now = QTime::currentTime();
        if (now.hour() == 23 && now.minute() == 59)
            m_pointage.marquerAbsentsJournee();
    });
    m_timerAbsences->start(60000);

    // ── Miroir LCD Qt ─────────────────────────────────────────────────────
    setupKeypadSimulator();

    // ── ArduinoMonitor (scénarios temp + balance) ─────────────────────────
    m_arduinoMonitor = new ArduinoMonitor(this);

    connect(m_arduinoMonitor, &ArduinoMonitor::temperatureAlert, this,
            [this](double /*tempMatiere*/, const QString &message) {
        SystemNotification::instance().show("⚠️ ALERTE TEMPÉRATURE", message,
                                            NotificationWidget::Warning);
        NotificationHistoryItem item;
        item.title       = "⚠️ Alerte Température";
        item.message     = message;
        item.type        = NotificationWidget::Warning;
        item.timestamp   = QDateTime::currentDateTime();
        item.read        = false;
        item.aiGenerated = false;
        NotificationHistory::instance().add(item);
        if (m_bell) m_bell->refresh();
    });

    connect(m_arduinoMonitor, &ArduinoMonitor::temperatureUpdated, this,
            [this](double tempMatiere) {
        if (!m_tempIndicator) return;
        m_tempIndicator->setText(QString("🌡 %1°C").arg(tempMatiere, 0, 'f', 1));
        QString bg = (tempMatiere > 30.0) ? "#D32F2F"
                   : (tempMatiere > 25.0) ? "#F57C00"
                                          : "#388E3C";
        m_tempIndicator->setStyleSheet(
            QString("QLabel { background-color:%1; color:white; border-radius:16px; "
                    "font-size:11px; font-weight:bold; padding:4px 8px; }").arg(bg));
    });

    // ── ArduinoManager — découverte asynchrone ────────────────────────────
    m_arduinoManager = new ArduinoManager(this);

    connect(m_arduinoManager, &ArduinoManager::cardIdentified, this,
            [this](const QString &role, const QString &port) {
        qDebug() << "🎯 Carte identifiée:" << role << "sur" << port;
    });

    connect(m_arduinoManager, &ArduinoManager::cardMissing, this,
            [this](const QString &role) {
        qDebug() << "⚠️ Carte manquante:" << role;
    });

    connect(m_arduinoManager, &ArduinoManager::discoveryComplete,
            this, &MainWindow::onArduinoDiscoveryComplete);

    m_arduinoManager->scanAndIdentify();
}

// ─────────────────────────────────────────────────────────────────────────────
//  onArduinoDiscoveryComplete — appelé quand toutes les cartes sont identifiées
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::onArduinoDiscoveryComplete()
{
    m_arduinoTempBalance = m_arduinoManager->arduinoTempBalance();
    m_arduinoLivraison   = m_arduinoManager->arduinoLivraison();
    m_arduinoPointage    = m_arduinoManager->arduinoPointage();

    // ── Résumé indicateur ─────────────────────────────────────────────────
    int nbConnected = (m_arduinoTempBalance ? 1 : 0)
                    + (m_arduinoLivraison   ? 1 : 0)
                    + (m_arduinoPointage    ? 1 : 0);

    if (nbConnected == 3) {
        m_arduinoIndicator->setText("● 3 Arduino connectés");
        m_arduinoIndicator->setStyleSheet(
            "QLabel { border-radius:12px; font-size:11px; font-weight:bold; "
            "color:white; background:#27AE60; }");
    } else if (nbConnected > 0) {
        m_arduinoIndicator->setText(QString("● %1/3 Arduino").arg(nbConnected));
        m_arduinoIndicator->setStyleSheet(
            "QLabel { border-radius:12px; font-size:11px; font-weight:bold; "
            "color:white; background:#F57C00; }");
    } else {
        m_arduinoIndicator->setText("● Non connecté");
        m_arduinoIndicator->setStyleSheet(
            "QLabel { border-radius:12px; font-size:11px; font-weight:bold; "
            "color:white; background:#E74C3C; }");
    }

    // ── Carte TEMP_BALANCE → ArduinoMonitor ───────────────────────────────
    if (m_arduinoTempBalance) {
        m_arduinoMonitor->setArduinoTempBalance(m_arduinoTempBalance);
        m_arduinoMonitor->startTemperatureMonitoring();
        qDebug() << "✅ Surveillance température démarrée";
    }

    // ── Carte LIVRAISON → port série pour expédition ──────────────────────
    if (m_arduinoLivraison) {
        m_serialLivraison = m_arduinoLivraison->connection()->getSerial();
        connect(m_serialLivraison, &QSerialPort::readyRead,
                this, &MainWindow::recevoir_donnee);
        qDebug() << "✅ Carte LIVRAISON prête sur" << m_arduinoLivraison->getPortName();
    }

    // ── Carte POINTAGE → port série pour RFID ────────────────────────────
    if (m_arduinoPointage) {
        m_serialPointage = m_arduinoPointage->connection()->getSerial();
        connect(m_serialPointage, &QSerialPort::readyRead,
                this, &MainWindow::recevoir_donnee);
        qDebug() << "✅ Carte POINTAGE prête sur" << m_arduinoPointage->getPortName();
    }

    qDebug() << "🏁 Initialisation Arduino terminée:" << m_arduinoManager->statusSummary();
}

// ─────────────────────────────────────────────────────────────────────────────
//  setupArduinoPointage — conservé pour compatibilité (ne fait plus rien)
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::setupArduinoPointage()
{
    // Remplacé par setupArduinoMultiCartes() — ne pas appeler directement
}

// ─────────────────────────────────────────────────────────────────────────────
//  recevoir_donnee — lit les données des cartes LIVRAISON et POINTAGE
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::recevoir_donnee()
{
    // Lire depuis la carte LIVRAISON
    if (m_serialLivraison && m_serialLivraison->bytesAvailable() > 0) {
        static QByteArray bufLivraison;
        bufLivraison += m_arduinoLivraison->read_from_arduino();
        while (bufLivraison.contains('\n')) {
            int idx = bufLivraison.indexOf('\n');
            QString msg = QString::fromUtf8(bufLivraison.left(idx)).trimmed();
            bufLivraison.remove(0, idx + 1);
            if (!msg.isEmpty()) traiterMessageArduino(msg);
        }
    }

    // Lire depuis la carte POINTAGE
    if (m_serialPointage && m_serialPointage->bytesAvailable() > 0) {
        static QByteArray bufPointage;
        bufPointage += m_arduinoPointage->read_from_arduino();
        while (bufPointage.contains('\n')) {
            int idx = bufPointage.indexOf('\n');
            QString msg = QString::fromUtf8(bufPointage.left(idx)).trimmed();
            bufPointage.remove(0, idx + 1);
            if (!msg.isEmpty()) traiterMessageArduino(msg);
        }
    }
}

// ── inputmbarky ────────────────────────────────
void MainWindow::traiterMessageArduino(const QString &msg)
{
    qDebug() << "Arduino msg:" << msg;

    // ── TEMP:xx.x,yy.y — Données température (2 DHT11) ──────────────────
    if (msg.startsWith("TEMP:")) {
        // Transmettre à ArduinoMonitor pour traitement
        if (m_arduinoMonitor) {
            QStringList parts = msg.mid(5).split(',');
            if (parts.size() == 2) {
                bool ok1, ok2;
                double tempMatiere = parts[0].toDouble(&ok1);
                double tempAmbiance = parts[1].toDouble(&ok2);
                if (ok1 && ok2) {
                    // Appeler directement le slot de traitement
                    QMetaObject::invokeMethod(m_arduinoMonitor, 
                        "onTemperatureReceived",
                        Qt::QueuedConnection,
                        Q_ARG(double, tempMatiere),
                        Q_ARG(double, tempAmbiance));
                }
            }
        }
        return;
    }

    // ── WEIGHT:xx.xx — Données poids (HX711) ────────────────────────────
    if (msg.startsWith("WEIGHT:")) {
        bool ok;
        double weight = msg.mid(7).toDouble(&ok);
        if (ok) {
            // Transmettre le poids à ArduinoMonitor via la carte TEMP_BALANCE
            if (m_arduinoTempBalance)
                emit m_arduinoTempBalance->weightStable(weight);
            qDebug() << "Weight received:" << weight << "kg";
        }
        return;
    }

    // ── INPUT:xxx — saisie en cours, miroir LCD ──────────────────────────
    if (msg.startsWith("INPUT:")) {
        QString saisi = msg.mid(6);
        m_keypadBuffer = saisi;
        if (m_lcdLigne2) m_lcdLigne2->setText("Saisie: " + saisi);
        return;
    }

    // ── CLEAR — réinitialiser LCD ────────────────────────────────────────
    if (msg == "CLEAR") {
        m_keypadBuffer.clear();
        if (m_lcdLigne1) m_lcdLigne1->setText("Systeme pret");
        if (m_lcdLigne2) m_lcdLigne2->setText("Saisir Ref + #");
        return;
    }

    // ── '1' — confirmation servos (ignorer) ─────────────────────────────
    if (msg == "1") return;

    // ── ID:xxx — chercher en BDD et mettre à jour statut ────────────────────
    if (msg.startsWith("ID:")) {
        QString id = msg.mid(3).trimmed();
        if (m_lcdLigne1) m_lcdLigne1->setText("Recherche...");
        if (m_lcdLigne2) m_lcdLigne2->setText(id);

        QSqlQuery q(Connection::instance()->getDatabase());
        q.prepare("SELECT ID_COMMANDE FROM COMMANDES "
                  "WHERE REFERENCE = :id OR TO_CHAR(ID_COMMANDE) = :id2");
        q.bindValue(":id",  id);
        q.bindValue(":id2", id);

        if (q.exec() && q.next()) {
            // Trouvé → marquer Terminé + date livraison + envoyer '1' aux servos
            QString dateAuj = QDate::currentDate().toString("dd/MM/yyyy");
            QSqlQuery upd(Connection::instance()->getDatabase());
            upd.prepare("UPDATE COMMANDES SET STATUT = 'Termin\u00e9', "
                        "DATE_LIVRAISON = TO_DATE(:dl, 'DD/MM/YYYY') "
                        "WHERE REFERENCE = :id OR TO_CHAR(ID_COMMANDE) = :id2");
            upd.bindValue(":dl",  dateAuj);
            upd.bindValue(":id",  id);
            upd.bindValue(":id2", id);
            upd.exec();

            // Envoyer '1' à la carte LIVRAISON (servos)
            if (m_arduinoLivraison && m_arduinoLivraison->isConnected())
                m_arduinoLivraison->write_to_arduino("1");

            loadProductionData();
            m_keypadBuffer.clear();

            // LCD "Commande terminee" → retour home après 3s
            if (m_lcdLigne1) m_lcdLigne1->setText("Commande terminee");
            if (m_lcdLigne2) m_lcdLigne2->setText(id);

            QTimer::singleShot(3000, this, [this]() {
                if (m_lcdLigne1) m_lcdLigne1->setText("Systeme pret");
                if (m_lcdLigne2) m_lcdLigne2->setText("Saisir ID + #");
            });

            NotificationWidget::show(
                "✅ Commande terminée",
                "Référence " + id + " marquée Terminé — " + dateAuj,
                NotificationWidget::Success
            );
        } else {
            // Invalide → envoyer '2' à la carte LIVRAISON, rester en attente
            if (m_arduinoLivraison && m_arduinoLivraison->isConnected())
                m_arduinoLivraison->write_to_arduino("2");

            if (m_lcdLigne1) m_lcdLigne1->setText("ID invalide");
            if (m_lcdLigne2) m_lcdLigne2->setText("Ressaisir + #");
        }
        return;
    }

    // ── UID: — pointage RFID employé ────────────────────────────────────
    if (msg.contains("UID:")) {
        QString uid = msg.section("UID:", -1).trimmed();
        bool ok = m_pointage.marquerPresent(uid);
        
        if (ok && !m_pointage.estDejaPointe()) {
            // Pointage reussi - Envoyer nom a Arduino
            QString prenom = m_pointage.getDernierPrenom();
            QString nom = m_pointage.getDernierNom();
            QString heure = QTime::currentTime().toString("HH:mm");
            
            // Envoyer "OK:Prenom Nom" a Arduino POINTAGE
            if (m_arduinoPointage && m_arduinoPointage->isConnected())
                m_arduinoPointage->write_to_arduino(
                    QString("OK:%1 %2").arg(prenom).arg(nom).toUtf8());
            
            SystemNotification::instance().show(
                "Pointage CUIREA",
                QString("%1 %2 est arrive(e) a %3").arg(prenom).arg(nom).arg(heure),
                NotificationWidget::Success,
                5000
            );
            
            populateEmployeeTable();
            
        } else if (ok && m_pointage.estDejaPointe()) {
            // Deja pointe aujourd'hui - sortie
            QString prenom = m_pointage.getDernierPrenom();
            QString nom = m_pointage.getDernierNom();
            QString heure = QTime::currentTime().toString("HH:mm");
            
            // Envoyer "BYE:Prenom Nom" a Arduino POINTAGE
            if (m_arduinoPointage && m_arduinoPointage->isConnected())
                m_arduinoPointage->write_to_arduino(
                    QString("BYE:%1 %2").arg(prenom).arg(nom).toUtf8());
            
            SystemNotification::instance().show(
                "Pointage CUIREA",
                QString("%1 %2 a quitte a %3").arg(prenom).arg(nom).arg(heure),
                NotificationWidget::Info,
                5000
            );
            
        } else {
            // Badge inconnu
            if (m_arduinoPointage && m_arduinoPointage->isConnected())
                m_arduinoPointage->write_to_arduino("NO");
            
            QString heure = QTime::currentTime().toString("HH:mm");
            SystemNotification::instance().show(
                "Alerte Securite CUIREA",
                QString("Tentative d'acces refusee a %1 - Carte inconnue (UID: %2)").arg(heure).arg(uid),
                NotificationWidget::Critical,
                8000
            );
        }
    }
}

// ── Expédier via bouton Qt ────────────────────────────────────────────────
void MainWindow::expedierActionArduino()
{
    // CAS 1 : carte LIVRAISON non connectée → tentative reconnexion
    if (!m_arduinoLivraison || !m_arduinoLivraison->isConnected()) {
        // Relancer la découverte si le manager existe
        if (m_arduinoManager) {
            QMessageBox::warning(this, "Expédition impossible",
                "Carte d'expédition non connectée.\n"
                "Vérifiez que l'Arduino LIVRAISON est branché.");
        }
        return;
    }

    // CAS 2 : aucune ligne sélectionnée
    int row = ui->productionTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Attention", "Veuillez sélectionner une commande.");
        return;
    }

    QString ref    = cellText(ui->productionTable, row, 1);
    QString statut = cellText(ui->productionTable, row, 8);
    QString mail   = cellText(ui->productionTable, row, 10);

    qDebug() << "[Expedier] ref=" << ref << "statut=" << statut;

    // CAS 3 : statut != Terminé
    bool estTermine = (statut.trimmed().normalized(QString::NormalizationForm_C)
                       == QString("Termin\u00e9").normalized(QString::NormalizationForm_C));

    if (!estTermine) {
        QString msgErr;
        if (statut.contains("livraison", Qt::CaseInsensitive))
            msgErr = QString("Référence : %1\nCette commande est déjà en livraison.").arg(ref);
        else
            msgErr = QString("Référence     : %1\nStatut actuel : %2\n"
                             "La commande doit être Terminé pour être expédiée.").arg(ref, statut);
        QMessageBox::critical(this, "Expédition impossible", msgErr);
        return;
    }

    // CAS 4 : statut == Terminé → déclencher les moteurs (carte LIVRAISON)
    qDebug() << "[Expedier] Envoi '1' → carte LIVRAISON pour" << ref;
    m_arduinoLivraison->write_to_arduino("1");

    QString dateAujourdhui = QDate::currentDate().toString("dd/MM/yyyy");
    QSqlQuery upd(Connection::instance()->getDatabase());
    upd.prepare("UPDATE COMMANDES SET STATUT = 'En livraison', "
                "DATE_LIVRAISON = TO_DATE(:dl, 'DD/MM/YYYY') "
                "WHERE REFERENCE = :ref");
    upd.bindValue(":dl",  dateAujourdhui);
    upd.bindValue(":ref", ref);
    if (!upd.exec())
        qDebug() << "Erreur UPDATE expédition:" << upd.lastError().text();

    loadProductionData();

    if (m_lcdLigne1) m_lcdLigne1->setText("Expedition OK");
    if (m_lcdLigne2) m_lcdLigne2->setText(ref);

    QTimer::singleShot(2000, this, [this]() {
        if (m_lcdLigne1) m_lcdLigne1->setText("Systeme pret");
        if (m_lcdLigne2) m_lcdLigne2->setText("Saisir Ref + #");
    });

    QMessageBox::information(this, "Expédition lancée",
        QString("Référence : %1\nClient    : %2\n"
                "Statut    : En livraison\nDate      : %3")
            .arg(ref, mail.isEmpty() ? "—" : mail, dateAujourdhui));
}

// ── Miroir LCD Qt (overlay sous l'indicateur Arduino) ────────────────────
void MainWindow::setupKeypadSimulator()
{
    if (m_lcdLigne1 != nullptr) return; // guard anti-doublon

    // LCD comme overlay — enfant de la MainWindow, positionné sous m_arduinoIndicator
    QFrame *lcd = new QFrame(this);
    lcd->setFixedSize(240, 56);
    lcd->setStyleSheet("background:#000; border:2px solid #333; border-radius:4px;");
    QVBoxLayout *lcdLay = new QVBoxLayout(lcd);
    lcdLay->setContentsMargins(8, 4, 8, 4);
    lcdLay->setSpacing(2);

    m_lcdLigne1 = new QLabel("Systeme pret", lcd);
    m_lcdLigne1->setStyleSheet(
        "color:#00FF00; font-family:'Courier New'; font-size:12px; font-weight:bold;");
    m_lcdLigne2 = new QLabel("Saisir Ref + #", lcd);
    m_lcdLigne2->setStyleSheet(
        "color:#00FF00; font-family:'Courier New'; font-size:12px;");
    lcdLay->addWidget(m_lcdLigne1);
    lcdLay->addWidget(m_lcdLigne2);

    // Position initiale : sous l'indicateur Arduino
    int indY = m_arduinoIndicator
        ? (m_bell ? m_bell->height() + 10 : 6) + m_arduinoIndicator->height() + 6
        : 50;
    lcd->move(width() - lcd->width() - 12, indY);
    lcd->setVisible(false); // visible seulement sur la page Production
    lcd->raise();
}

// ── Ouvrir calendrier pointage ────────────────────────────────────────────
void MainWindow::ouvrirCalendrierPointage(int idEmploye, const QString &nom)
{
    PointageDialog dlg(idEmploye, nom, this);
    dlg.exec();
}

// ── Slot bouton Pointage (dans module Employés) ──────────────────────────
void MainWindow::on_btnPointage_clicked()
{
    QModelIndex idx = ui->employeeTable->currentIndex();
    if (!idx.isValid()) {
        QMessageBox::warning(this, "⚠️ Attention",
            "Veuillez sélectionner un employé.");
        return;
    }
    
    // Récupérer ID et Nom depuis le modèle
    auto *model = ui->employeeTable->model();
    int   id    = model->index(idx.row(), 13).data().toInt();  // colonne ID
    QString nom = model->index(idx.row(), 1).data().toString()  // NOM
                + " "
                + model->index(idx.row(), 2).data().toString(); // PRENOM
    
    ouvrirCalendrierPointage(id, nom);
}
