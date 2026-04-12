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
#include <QHttpMultiPart>
#include <QStandardPaths>
#include <QMouseEvent>
#include <QEvent>
#include "employeedialog.h"
#include "clientmanagerdialog.h"
#include "matieredialog.h"
#include "fournisseurdialog.h"
#include "smsfournisseurdialog.h"
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
// ?? Shared style constants ????????????????????????????????????????????????????
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

// ?? Helper: show a styled info message box ????????????????????????????????????
static void showInfo(QWidget *parent, const QString &title, const QString &text)
{
    QMessageBox mb(parent);
    mb.setWindowTitle(title);
    mb.setText(text);
    mb.setIcon(QMessageBox::Information);
    mb.setStyleSheet(MSGBOX_STYLE);
    mb.exec();
}

// ?? Helper: get text from a table cell safely ????????????????????????????????
static QString cellText(QTableWidget *t, int row, int col)
{
    auto *item = t->item(row, col);
    return item ? item->text() : QString();
}

// ?? Helper: filter table rows by search text ?????????????????????????????????
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

// ?????????????????????????????????????????????????????????????????????????????
// FloatingAIButton � cercle anim� style Meta AI
// ?????????????????????????????????????????????????????????????????????????????
FloatingAIButton::FloatingAIButton(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(64, 64);
    // Pas de WA_TranslucentBackground � on peint le fond nous-m�mes
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

    // Ombre port�e
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

    // Cercle int�rieur beige CUIREA
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

// ?????????????????????????????????????????????????????????????????????????????

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , isEditMode(false)
    , editingRow(-1)
{
    ui->setupUi(this);
    setWindowTitle("CUIREA - Management System");
    
    // Initialiser la d�tection de d�fauts (Mode API Cloud - Hugging Face Spaces)
    networkManager = new QNetworkAccessManager(this);
    apiUrl = "https://ahmedomar10-detection-cuir.hf.space";
    detectionResultLabel = nullptr;
    detectionProgress = nullptr;
    
    // Initialiser les classes extraites pour mati�res premi�res
    matiereDetection = new MatiereDetection(this, ui->matiereTable, networkManager, apiUrl);
    voiceMatieres = new VoiceMatieres(this, ui->matiereTable);
    //=========================MAPFOURNISSEUR
    mapService = new Map(this);

        connect(mapService, &Map::coordinatesReady, this, [=](double lat, double lon){
            openMap(lat, lon);
        });
    // ?? Employee table ??????????????????????????????????????????????????????
    ui->employeeTable->verticalHeader()->setVisible(false);
    // Colonnes: 0=Matricule, 1=Nom, 2=Pr�nom, 3=CIN, 4=DateNaissance, 5=Sexe,
    //           6=Adresse, 7=T�l�phone, 8=Email, 9=Poste, 10=D�partement,
    //           11=DateEmbauche, 12=Photo, 13=ID
    ui->employeeTable->setColumnCount(14);
    ui->employeeTable->setHorizontalHeaderLabels({"Matricule", "Nom", "Pr�nom", "CIN", 
                                                   "Date Naissance", "Sexe", "Adresse", 
                                                   "T�l�phone", "Email", "Poste", 
                                                   "D�partement", "Date Embauche", "Photo", "ID"});
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
    
    // Permettre de d�s�lectionner en cliquant sur une zone vide du tableau
    ui->employeeTable->viewport()->installEventFilter(this);
    
    // Masquer le panneau de profil au d�marrage
    ui->employeeProfilePanel->setVisible(false);
    
    // Recherche en temps r�el avec crit�res
    connect(ui->searchBox, &QLineEdit::textChanged, this, [this](const QString &text) {
        QString searchTerm = text.trimmed();
        
        if (searchTerm.isEmpty()) {
            // Afficher tous les employ�s
            populateEmployeeTable();
            return;
        }
        
        // Recherche intelligente
        Employe e;
        QSqlQueryModel* model = nullptr;
        
        // D�tecter le type de recherche
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
            // Recherche par pr�nom
            QString prenom = searchTerm.mid(7).trimmed();
            model = e.rechercherParCritere("prenom", prenom);
            ui->searchBox->setPlaceholderText("? Recherche par pr�nom...");
        } else if (searchTerm.startsWith("dept:")) {
            // Recherche par d�partement
            QString dept = searchTerm.mid(5).trimmed();
            model = e.rechercherParCritere("departement", dept);
            ui->searchBox->setPlaceholderText("? Recherche par d�partement...");
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
        
        // Afficher les r�sultats - IMPORTANT: afficher toutes les 15 colonnes
        int n = model->rowCount();
        ui->employeeTable->setRowCount(n);
        
        // Le mod�le retourne maintenant 12 colonnes dans cet ordre:
        // 0=ID, 1=Matricule, 2=Nom, 3=Pr�nom, 4=CIN, 5=DateNaissance, 6=Sexe,
        // 7=D�partement, 8=Poste, 9=T�l�phone, 10=Email, 11=Photo
        
        // Le tableau a 14 colonnes dans cet ordre:
        // 0=Matricule, 1=Nom, 2=Pr�nom, 3=CIN, 4=DateNaissance, 5=Sexe,
        // 6=Adresse, 7=T�l�phone, 8=Email, 9=Poste,
        // 10=D�partement, 11=DateEmbauche, 12=Photo, 13=ID
        
        for (int i = 0; i < n; ++i) {
            // Mapper les colonnes du mod�le vers le tableau
            ui->employeeTable->setItem(i, 0, new QTableWidgetItem(model->data(model->index(i, 1)).toString())); // Matricule
            ui->employeeTable->setItem(i, 1, new QTableWidgetItem(model->data(model->index(i, 2)).toString())); // Nom
            ui->employeeTable->setItem(i, 2, new QTableWidgetItem(model->data(model->index(i, 3)).toString())); // Pr�nom
            ui->employeeTable->setItem(i, 3, new QTableWidgetItem(model->data(model->index(i, 4)).toString())); // CIN
            ui->employeeTable->setItem(i, 4, new QTableWidgetItem(model->data(model->index(i, 5)).toString())); // Date Naissance
            ui->employeeTable->setItem(i, 5, new QTableWidgetItem(model->data(model->index(i, 6)).toString())); // Sexe
            ui->employeeTable->setItem(i, 6, new QTableWidgetItem("")); // Adresse (cach�e)
            ui->employeeTable->setItem(i, 7, new QTableWidgetItem(model->data(model->index(i, 9)).toString())); // T�l�phone
            ui->employeeTable->setItem(i, 8, new QTableWidgetItem(model->data(model->index(i, 10)).toString())); // Email
            ui->employeeTable->setItem(i, 9, new QTableWidgetItem(model->data(model->index(i, 8)).toString())); // Poste
            ui->employeeTable->setItem(i, 10, new QTableWidgetItem(model->data(model->index(i, 7)).toString())); // D�partement
            ui->employeeTable->setItem(i, 11, new QTableWidgetItem("")); // Date Embauche (cach�e)
            ui->employeeTable->setItem(i, 12, new QTableWidgetItem(model->data(model->index(i, 11)).toString())); // Photo
            ui->employeeTable->setItem(i, 13, new QTableWidgetItem(model->data(model->index(i, 0)).toString())); // ID
        }
        
        delete model;
    });

    // ?? Raw materials ???????????????????????????????????????????????????????
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

    // ?? Client extra buttons ????????????????????????????????????????????????
    connect(ui->btnFidelityClassification, &QPushButton::clicked, this, &MainWindow::on_btnFidelityClassification_clicked);

    // ?? Fournisseurs ????????????????????????????????????????????????????????
    ui->fournisseurTable->verticalHeader()->setVisible(false);
    setupFournisseurTable();

    // ?? Production ??????????????????????????????????????????????????????????
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
    connect(ui->searchBoxProduction, &QLineEdit::textChanged, this, &MainWindow::onRechercherProduction);

    // ?? Timer retard notifications ???????????????????????????????????????????
    m_retardTimer = new QTimer(this);
    connect(m_retardTimer, &QTimer::timeout, this, &MainWindow::checkRetards);
    m_retardTimer->start(20000); // v�rifie toutes les 20 secondes
    QTimer::singleShot(3000, this, &MainWindow::checkRetards); // 1er check 3s apr�s d�marrage

    // ?? Pipeline IA Gemini pour les notifications intelligentes ??????????????
    const QString geminiKey = EnvLoader::get("GEMINI_API_KEY");
    if (!geminiKey.isEmpty()) {
        m_pipeline = new NotificationPipeline(geminiKey, this);
        m_pipeline->start();
    }

    // ?? Articles ????????????????????????????????????????????????????????????
    setupArticleTable();
    loadArticlesFromDB();
    updateArticleStatistics();

    // ?? Initial page ????????????????????????????????????????????????????????
    ui->stackedWidget->setCurrentIndex(0);
    ui->btnEmployees->setStyleSheet(NAV_ACTIVE_STYLE);
    
    // ?? AI floating button ???????????????????????????????????????????????????
    m_aiWidget = new AIChatWidget(this);
    m_aiWidget->setContext("Gestion des Employ�s");

    m_floatingBtn = new FloatingAIButton(this);
    connect(m_floatingBtn, &FloatingAIButton::clicked, this, [this]() {
        m_aiWidget->toggleChat();
    });

    // Forcer le plein �cran au d�marrage
    showMaximized();

    // Positionner et afficher le bouton flottant apr�s que la fen�tre soit visible
    QTimer::singleShot(100, this, [this]() {
        // Juste apr�s la sidebar (180px) en bas
        m_floatingBtn->move(190, height() - 84);
        m_floatingBtn->show();
        m_floatingBtn->raise();
    });
}

MainWindow::~MainWindow() 
{
    // Arr�ter l'API Python via taskkill (processus d�tach�)
    QProcess::execute("cmd.exe", QStringList() << "/c" << "taskkill /f /im python.exe >nul 2>&1");
    delete ui; 
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    // G�rer le clic sur une zone vide du tableau employ�s
    if (obj == ui->employeeTable->viewport() && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        QModelIndex index = ui->employeeTable->indexAt(mouseEvent->pos());
        
        // Si on clique sur une zone vide (pas sur une cellule)
        if (!index.isValid()) {
            ui->employeeTable->clearSelection();
            ui->employeeProfilePanel->setVisible(false);
            return true; // Ev�nement trait�
        }
    }
    
    // Passer l'�v�nement au gestionnaire parent
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    if (m_floatingBtn) {
        // Juste apr�s la sidebar (180px) en bas
        m_floatingBtn->move(190, height() - 84);
        m_floatingBtn->show();
        m_floatingBtn->raise();
    }
}

// ?? Navigation helpers ????????????????????????????????????????????????????????
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
}

void MainWindow::on_btnEmployees_clicked()  
{ 
    switchPage(0, ui->btnEmployees,  "CUIREA - Gestion des Employ�s");  
    m_aiWidget->setContext("Gestion des Employ�s"); 
    // Masquer le panneau de profil et d�s�lectionner
    ui->employeeProfilePanel->setVisible(false);
    ui->employeeTable->clearSelection();
}

void MainWindow::on_btnClients_clicked()    
{ 
    switchPage(1, ui->btnClients,    "CUIREA - Gestion des Clients"); 
    m_aiWidget->setContext("Gestion des Clients");
    // Masquer le panneau de profil employ� quand on change de page
    ui->employeeProfilePanel->setVisible(false);
    afficherClients();
}

void MainWindow::on_btnProducts_clicked()   
{ 
    switchPage(5, ui->btnProducts,   "CUIREA - Gestion des Articles"); 
    m_aiWidget->setContext("Gestion des Articles"); 
    // Masquer le panneau de profil employ� quand on change de page
    ui->employeeProfilePanel->setVisible(false);
}

void MainWindow::on_btnRawMaterials_clicked()
{ 
    switchPage(2, ui->btnRawMaterials,"CUIREA - Mati�res Premi�res"); 
    m_aiWidget->setContext("Gestion des Mati�res Premi�res"); 
    // Masquer le panneau de profil employ� quand on change de page
    ui->employeeProfilePanel->setVisible(false);
}

void MainWindow::on_btnSuppliers_clicked()  
{ 
    switchPage(3, ui->btnSuppliers,  ""); 
    m_aiWidget->setContext("Gestion des Fournisseurs"); 
    // Masquer le panneau de profil employ� quand on change de page
    ui->employeeProfilePanel->setVisible(false);
}
void MainWindow::on_btnProduction_clicked() 
{ 
    switchPage(4, ui->btnProduction, "CUIREA - Gestion de la Production");
    updateProductionStatsCards();
    m_aiWidget->setContext("Gestion de la Production");
    // Masquer le panneau de profil employ� quand on change de page
    ui->employeeProfilePanel->setVisible(false);
    // R�initialiser pour re-notifier � chaque visite de l'onglet
    m_notifiedIds.clear();
    QTimer::singleShot(3000, this, &MainWindow::checkRetards);
}

// ?? Employee CRUD ?????????????????????????????????????????????????????????????
void MainWindow::on_btnAdd_clicked()
{
    EmployeeDialog dlg(this, EmployeeDialog::AddMode);
    if (dlg.exec() == QDialog::Accepted) {
        Employe e;
        
        // Mode BD - toujours essayer d'ajouter dans la base de donn�es
        e.setMatricule(dlg.getMatricule());
        e.setNom(dlg.getNom());
        e.setPrenom(dlg.getPrenom());
        e.setCin(dlg.getCin());
        e.setDateNaissance(dlg.getDateNaissance());
        e.setSexe(dlg.getSexe());
        e.setAdresse(dlg.getAdresse());
        e.setTelephone(dlg.getTelephone());
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
            QMessageBox::information(this, "Succ�s", 
                QString("Employ� ajout� avec succ�s !\n\n"
                       "Identifiants de connexion:\n"
                       "Matricule: %1\n"
                       "R�le: %2\n"
                       "Compte actif: %3")
                .arg(dlg.getMatricule())
                .arg(dlg.getRoleSysteme())
                .arg(dlg.isActif() ? "Oui" : "Non"));
        } else {
            QMessageBox::critical(this, "Erreur", 
                "Impossible d'ajouter l'employ�.\n"
                "V�rifiez que la table EMPLOYES existe dans la base de donn�es.");
        }
    }
}

void MainWindow::on_btnExport_clicked()
{
    int row = ui->employeeTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Attention", 
            "Veuillez s�lectionner un employ� pour g�n�rer sa fiche de paie.");
        return;
    }
    
    // R�cup�rer les donn�es de l'employ� s�lectionn�
    QString matricule = cellText(ui->employeeTable, row, 0);
    QString nom = cellText(ui->employeeTable, row, 1);
    QString prenom = cellText(ui->employeeTable, row, 2);
    QString cin = cellText(ui->employeeTable, row, 3);
    QString poste = cellText(ui->employeeTable, row, 9);
    QString departement = cellText(ui->employeeTable, row, 10);
    QString dateEmbaucheStr = cellText(ui->employeeTable, row, 11);
    QDate dateEmbauche = QDate::fromString(dateEmbaucheStr, "dd/MM/yyyy");
    
    // Ouvrir le dialog de fiche de paie
    FichePaieDialog dlg(matricule, nom, prenom, cin, poste, departement, dateEmbauche, this);
    dlg.exec();
}

void MainWindow::on_btnEdit_clicked()
{
    int row = ui->employeeTable->currentRow();
    if (row < 0) { QMessageBox::warning(this,"","Veuillez s�lectionner un employ� � modifier."); return; }
    
    EmployeeDialog dlg(this, EmployeeDialog::EditMode);
    // Colonnes: 0=Matricule, 1=Nom, 2=Pr�nom, 3=CIN, 4=DateNaissance, 5=Sexe,
    //           6=Adresse, 7=T�l�phone, 8=Email, 9=Poste, 10=D�partement,
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
        
        // Mode BD - toujours modifier dans la base de donn�es
        e.setId(employeeId.toInt());
        e.setMatricule(dlg.getMatricule());
        e.setNom(dlg.getNom());
        e.setPrenom(dlg.getPrenom());
        e.setCin(dlg.getCin());
        e.setDateNaissance(dlg.getDateNaissance());
        e.setSexe(dlg.getSexe());
        e.setAdresse(dlg.getAdresse());
        e.setTelephone(dlg.getTelephone());
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
            QMessageBox::information(this, "Succ�s", "Employ� modifi� avec succ�s !");
        } else {
            QMessageBox::critical(this, "Erreur", 
                "Impossible de modifier l'employ�.\n"
                "V�rifiez que la table EMPLOYES existe dans la base de donn�es.");
        }
    }
}

void MainWindow::on_btnDelete_clicked()
{
    int row = ui->employeeTable->currentRow();
    if (row < 0) { QMessageBox::warning(this,"","Veuillez s�lectionner un employ� � supprimer."); return; }
    
    EmployeeDialog dlg(this, EmployeeDialog::DeleteMode);
    // Colonnes: 0=Matricule, 1=Nom, 2=Pr�nom, 3=CIN, 4=DateNaissance, 5=Sexe,
    //           6=Adresse, 7=T�l�phone, 8=Email, 9=Poste, 10=D�partement,
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
        
        // Mode BD - toujours supprimer de la base de donn�es
        Employe e;
        if (e.supprimer(id)) {
            populateEmployeeTable();
            QMessageBox::information(this, "Succ�s", "Employ� supprim� avec succ�s !");
        } else {
            QMessageBox::critical(this, "Erreur", 
                QString("Impossible de supprimer l'employ�.\n"
                       "ID: %1\n"
                       "V�rifiez que la table EMPLOYES existe dans la base de donn�es.").arg(id));
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
        auto *desc = sub->addAction("? D�croissant (Z ? A)");
        connect(asc, &QAction::triggered, [=]{ ui->employeeTable->sortItems(col, Qt::AscendingOrder); });
        connect(desc, &QAction::triggered, [=]{ ui->employeeTable->sortItems(col, Qt::DescendingOrder); });
    };

    addSortOptions("Matricule", 1);
    addSortOptions("Nom", 2);
    addSortOptions("Pr�nom", 3);

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
    
    // Si aucune ligne s�lectionn�e, masquer le panneau
    if (row < 0) {
        ui->employeeProfilePanel->setVisible(false);
        return;
    }
    
    // V�rifier que la ligne a des donn�es valides
    if (ui->employeeTable->item(row, 1) == nullptr) {
        ui->employeeProfilePanel->setVisible(false);
        return;
    }
    
    // Afficher le panneau et remplir les informations
    ui->employeeProfilePanel->setVisible(true);
    
    // Remplir les labels avec les donn�es de l'employ� s�lectionn�
    // Colonnes: 0=Matricule, 1=Nom, 2=Pr�nom, 3=CIN, 4=DateNaissance, 5=Sexe,
    //           6=Adresse, 7=T�l�phone, 8=Email, 9=Poste, 10=D�partement,
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
    
    // R�cup�rer la photo depuis la base de donn�es
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

// ?? Client CRUD ???????????????????????????????????????????????????????????????
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
    QStringList headers = { "ID","Nom", "Pr�nom", "Sexe", "CIN", "Pays", "Ville", "Adresse", "Email", "Date Inscription"};
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
            QMessageBox::information(this, "Succ�s", "Client ajout� avec succ�s !");
        } else {
            QMessageBox::critical(this, "Erreur",
                                  "Impossible d'ajouter le client.\n"
                                  "V�rifiez que la table CLIENTS existe dans la base de donn�es.");
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
        QMessageBox::warning(this, "Erreur", "Veuillez s�lectionner un client � modifier.");
        return;
    }

    int id = ui->clientTable->item(row, 0)->text().toInt();

    ClientManagerDialog dlg(this, ClientManagerDialog::EditMode);
    dlg.setEditingId(id);
    dlg.setClientData(
        ui->clientTable->item(row, 1)->text(),  // Nom
        ui->clientTable->item(row, 2)->text(),  // Pr�nom
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
            QMessageBox::information(this, "Succ�s", "Client modifi� avec succ�s !");
        } else {
            QMessageBox::critical(this, "Erreur", "Echec de modification du client !");
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
        QMessageBox::warning(this, "Erreur", "Veuillez s�lectionner un client � supprimer.");
        return;
    }

    int id = ui->clientTable->item(row, 0)->text().toInt();

    ClientManagerDialog dlg(this, ClientManagerDialog::DeleteMode);
    dlg.setDeleteId(id);
    dlg.setClientData(
        ui->clientTable->item(row, 1)->text(),  // Nom
        ui->clientTable->item(row, 2)->text(),  // Pr�nom
        ui->clientTable->item(row, 3)->text(),  // Sexe
        ui->clientTable->item(row, 4)->text(),  // CIN
        ui->clientTable->item(row, 5)->text(),  // Pays
        ui->clientTable->item(row, 6)->text(),  // Ville
        "", ""
        );

    if(dlg.exec() == QDialog::Accepted) {
        if(Client().supprimer(id)) {
            afficherClients();
            QMessageBox::information(this, "Succ�s", "Client supprim� avec succ�s !");
        } else {
            QMessageBox::critical(this, "Erreur", "Echec de suppression du client !");
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
    QAction *dateDesc = dateMenu->addAction("D�croissant");
    // ?? ALPHA SUBMENU
    QMenu *alphaMenu = menu->addMenu("?? Alphab�tique");
    QAction *alphaAsc  = alphaMenu->addAction("Croissant");
    QAction *alphaDesc = alphaMenu->addAction("D�croissant");
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
    QString filePath = QFileDialog::getSaveFileName(
        this,
        "Exporter les clients",
        "",
        "CSV (*.csv);;PDF (*.pdf);;Word (*.docx)"
    );

    if (filePath.isEmpty())
        return;

    if (Client::exporterListe(ui->clientTable, filePath)) {
        QMessageBox::information(this, "Succ�s", "Export r�ussi !");
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

// ?? Raw Materials ?????????????????????????????????????????????????????????????
void MainWindow::setupMatiereTable()
{
    Matiere matiereTmp;
    QSqlQueryModel* model = matiereTmp.afficher();
    if (!model) {
        return;
    }
    ui->matiereTable->setRowCount(0);
    ui->matiereTable->setColumnCount(7);
    ui->matiereTable->setHorizontalHeaderLabels({"MODULE", "REFERENCE", "TYPE", "QUANTITE ACTUELLE", "SEUIL", "DATE D'EXPIRATION", "PHOTO"});
    const int rowCount = model->rowCount();
    for (int row = 0; row < rowCount; ++row) {
        ui->matiereTable->insertRow(row);
        // Cr�er les items de base
        ui->matiereTable->setItem(row, 0, new QTableWidgetItem(model->data(model->index(row, 1)).toString()));
        ui->matiereTable->setItem(row, 1, new QTableWidgetItem(model->data(model->index(row, 2)).toString()));
        ui->matiereTable->setItem(row, 2, new QTableWidgetItem(model->data(model->index(row, 3)).toString()));
        ui->matiereTable->setItem(row, 3, new QTableWidgetItem(model->data(model->index(row, 4)).toString() + " m�"));
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
            photoItem->setText("�");
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
        QMessageBox::information(this, "Succ�s", "Mati�re ajout�e avec succ�s!");
        setupMatiereTable();
    } else {
        QMessageBox::critical(this, "Erreur", "Erreur lors de l'ajout de la mati�re.");
    }
}
void MainWindow::onEditMatiere()
{
    int row = ui->matiereTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "", "Veuillez s�lectionner une mati�re � modifier.");
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
        QMessageBox::information(this, "Succ�s", "Mati�re mise � jour avec succ�s!");
        setupMatiereTable();
    } else {
        QMessageBox::critical(this, "Erreur", "Erreur lors de la modification de la mati�re.");
    }
}
void MainWindow::onDeleteMatiere()
{
    int row = ui->matiereTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "", "Veuillez s�lectionner une mati�re � supprimer.");
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
        QMessageBox::information(this, "Succ�s", "Mati�re supprim�e avec succ�s!");
        setupMatiereTable();
    } else {
        QMessageBox::critical(this, "Erreur", "Erreur lors de la suppression de la mati�re.");
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
        double qty   = cellText(ui->matiereTable, r, 3).remove(" m�").toDouble();
        int    seuil = cellText(ui->matiereTable, r, 4).toInt();
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
    // Construction HTML identique � l'image
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
      .footer { font-size: 10px; color: #666; margin-top: 25px;
                padding: 12px 0; border-top: 2px solid #E8DED3;
                display: flex; justify-content: space-between; }
      .footer-right { font-weight: 600; }
    </style></head><body>
    )";
    // En-t�te avec logo
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
    //Statistiques g�n�rales
    html += R"(<div class="section-title">STATISTIQUES GENERALES</div>)";
    html += QString(R"(
    <div class="cards">
      <div class="card card-total">
        <div class="card-icon">??</div>
        <div class="card-title">Total<br>Matieres</div>
        <div class="card-value">%1</div>
        <div class="card-label">Actives</div>
      </div>
      <div class="card card-critique">
        <div class="card-icon">??</div>
        <div class="card-title">Stock<br>Critique</div>
        <div class="card-value">%2</div>
        <div class="card-label">Items a reapprovisionner</div>
      </div>
      <div class="card card-expires">
        <div class="card-icon">?</div>
        <div class="card-title">Expires</div>
        <div class="card-value">%3</div>
        <div class="card-label">Expires</div>
      </div>
      <div class="card card-proche">
        <div class="card-icon">??</div>
        <div class="card-title">< 30 Jours</div>
        <div class="card-value">%4</div>
        <div class="card-label">A surveiller</div>
      </div>
      <div class="card card-ok">
        <div class="card-icon">?</div>
        <div class="card-title">Stock<br>OK</div>
        <div class="card-value">%5</div>
        <div class="card-label">Stock OK</div>
      </div>
    </div>
    )").arg(total).arg(critique).arg(expires).arg(proche30j).arg(stockOK);
    // R�partition par type (graphique + tableau)
    html += R"(<div class="section-title">REPARTITION PAR TYPE</div>)";
    html += R"(<div class="charts-row">)";
    // Graphique � barres
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
    // Tableau r�capitulatif
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
    //Liste d�taill�e
    html += R"(<div class="section-title">LISTE DETAILLEE DES MATIERES</div>)";
    html += R"(
    <table class="detail-table">
      <tr>
        <th>Module</th><th>Reference</th><th>Type</th>
        <th>Quantite</th><th>Seuil</th><th>Expiration</th><th>Statut</th>
      </tr>
    )";
    for (int r = 0; r < total; ++r) {
        double qty   = cellText(ui->matiereTable, r, 3).remove(" m�").toDouble();
        int    seuil = cellText(ui->matiereTable, r, 4).toInt();
        QString badge, label;
        if      (qty < seuil * 0.5) { badge = "badge-critique"; label = "Critique"; }
        else if (qty < seuil)       { badge = "badge-normal";   label = "Normal";   }
        html += QString(R"(
        <tr>
          <td><span class="icon-leather">??</span>%1</td>
          <td>%2</td><td>%3</td><td>%4</td><td>%5</td><td>%6</td>
          <td><span class="%7">%8</span></td>
        </tr>
        )").arg(cellText(ui->matiereTable, r, 0))
           .arg(cellText(ui->matiereTable, r, 1))
           .arg(cellText(ui->matiereTable, r, 2))
           .arg(cellText(ui->matiereTable, r, 3))
           .arg(cellText(ui->matiereTable, r, 4))
           .arg(cellText(ui->matiereTable, r, 5))
           .arg(badge).arg(label);
    }
    html += R"(</table>)";
    //Footer
    html += R"(
    <div class="footer">
      <div>CUIREA Management System � Rapport genere automatiquement</div>
      <div class="footer-right">1 / 1</div>
    </div>
    )";
    html += R"(</div></body></html>)";
    //G�n�ration PDF
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
    dlg.setWindowTitle("Statistiques - Mati�res Premi�res");
    dlg.setMinimumSize(950, 650);
    dlg.setStyleSheet("QDialog { background-color: #F5F0EB; }");
    QVBoxLayout mainLay(&dlg);
    mainLay.setContentsMargins(25, 25, 25, 25);
    mainLay.setSpacing(20);
    // Titre
    auto *title = new QLabel("?? STATISTIQUES DES MATIERES PREMIERES");
    title->setStyleSheet("font-size: 18px; font-weight: bold; color: #6D4C41; padding: 8px;");
    title->setAlignment(Qt::AlignCenter);
    mainLay.addWidget(title);
    // Calcul des stats
    int total = ui->matiereTable->rowCount();
    int critique = 0, normal = 0, eleve = 0, expires = 0, proche30j = 0;
    QMap<QString, int> parType;
    for (int r = 0; r < total; ++r) {
        double qty = cellText(ui->matiereTable, r, 3).remove(" m�").toDouble();
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
    cardsLay->addWidget(createCard("Total Mati�res", QString::number(total), "#2C2416", "#E0D5CC"));
    cardsLay->addWidget(createCard("Stock Critique", QString::number(critique), "#C0392B", "#FDECEA"));
    cardsLay->addWidget(createCard("Expir�s", QString::number(expires), "#8B4513", "#FEF5E7"));
    cardsLay->addWidget(createCard("< 30 jours", QString::number(proche30j), "#D4A574", "#FFF8E1"));
    mainLay.addLayout(cardsLay);
    // Graphiques
    QHBoxLayout *chartsLay = new QHBoxLayout();
    chartsLay->setSpacing(20);
    // Donut Chart - R�partition stock (CRITICITE) 
    auto *pieSeries = new QPieSeries();
    pieSeries->setHoleSize(0.5);  // Donut chart
    auto *sliceCrit = pieSeries->append("Critique", critique);
    sliceCrit->setBrush(QColor("#B33A3A"));
    sliceCrit->setLabelVisible(false);
    auto *sliceElev = pieSeries->append("Elev�", eleve);
    sliceElev->setBrush(QColor("#E67E22"));
    sliceElev->setLabelVisible(false);
    
    auto *sliceNorm = pieSeries->append("Normal", normal);
    sliceNorm->setBrush(QColor("#95A472"));
    sliceNorm->setLabelVisible(false);

    auto *pieChart = new QChart();
    pieChart->addSeries(pieSeries);
    pieChart->setTitle("R�partition des Stocks (CRITICITE)");
    pieChart->setTitleFont(QFont("Arial", 12, QFont::Bold));
    pieChart->setBackgroundBrush(QColor("#FFFFFF"));
    pieChart->setBackgroundRoundness(10);
    
    // L�gende personnalis�e
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

    // ?? Bar chart - Par type ??????????????????????????????????
    auto *barSet = new QBarSet("Quantit�");
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
    barChart->setTitle("R�partition par Type");
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
            << "Tri par Quantit� (Croissant)" << "Tri par Quantit� (D�croissant)"
            << "Tri par Seuil (Croissant)" << "Tri par Seuil (D�croissant)"
            << "Tri par Date d'expiration (Plus proche)" << "Tri par Date d'expiration (Plus lointaine)";
    
    bool ok;
    QString choice = QInputDialog::getItem(this, "Tri des Mati�res", 
                                          "Choisissez le crit�re de tri:", 
                                          options, 0, false, &ok);
    
    if (!ok || choice.isEmpty()) return;
    
    // Structure pour stocker ligne compl�te avec donn�es UserRole
    struct RowData {
        QStringList texts;
        QVariant photoUrl;
        int originalId;
    };
    
    // Collecter toutes les donn�es
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
    
    // Trier selon le crit�re
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
    } else if (choice.contains("Quantit�")) {
        bool desc = choice.contains("D�croissant");
        std::sort(rows.begin(), rows.end(), [desc](const RowData &a, const RowData &b) {
            double qtyA = QString(a.texts[3]).remove(" m�").toDouble();
            double qtyB = QString(b.texts[3]).remove(" m�").toDouble();
            return desc ? qtyA > qtyB : qtyA < qtyB;
        });
    } else if (choice.contains("Seuil")) {
        bool desc = choice.contains("D�croissant");
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
    
    // R�appliquer les donn�es tri�es avec UserRole
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
    
    QMessageBox::information(this, "Tri effectu�", 
                           QString("Les mati�res ont �t� tri�es par: %1").arg(choice));
}

void MainWindow::onRechercheTriMatiere()
{
    QDialog dlg(this);
    dlg.setWindowTitle("Recherche Avanc�e");
    dlg.setMinimumWidth(500);
    dlg.setStyleSheet(DIALOG_STYLE);

    QVBoxLayout lay(&dlg);
    lay.setSpacing(12);
    lay.setContentsMargins(20, 20, 20, 20);

    auto *title = new QLabel("?? Recherche de Mati�res Premi�res");
    title->setStyleSheet("font-size: 16px; font-weight: bold; color: #8D6E63;");
    title->setAlignment(Qt::AlignCenter);

    QFormLayout form;
    
    auto *txtNom = new QLineEdit(&dlg);
    txtNom->setPlaceholderText("Ex: cuir, peau...");
    
    auto *txtRef = new QLineEdit(&dlg);
    txtRef->setPlaceholderText("Ex: dd, aaaa...");
    
    auto *cmbType = new QComboBox(&dlg);
    cmbType->addItems({"Tous", "Cuir", "Peau de Veau", "Ficelin�e", "Quincaillerie"});
    
    auto *cmbStock = new QComboBox(&dlg);
    cmbStock->addItems({"Tous", "Critique", "Normal", "Elev�"});
    
    auto *txtSeuil = new QLineEdit(&dlg);
    txtSeuil->setPlaceholderText("Ex: 100, >50, <200");
    
    auto *cmbPeremption = new QComboBox(&dlg);
    cmbPeremption->addItems({"Tous", "Expir�", "< 30 jours", "< 90 jours", "> 90 jours"});
    
    auto *cmbPhoto = new QComboBox(&dlg);
    cmbPhoto->addItems({"Tous", "Avec photo", "Sans photo"});

    form.addRow("Nom / Module :", txtNom);
    form.addRow("R�f�rence :", txtRef);
    form.addRow("Type :", cmbType);
    form.addRow("Niveau Stock :", cmbStock);
    form.addRow("Seuil :", txtSeuil);
    form.addRow("P�remption :", cmbPeremption);
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

        // Filtre r�f�rence
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
            int qty = cellText(ui->matiereTable, r, 3).remove(" m�").toDouble();
            int seuil = cellText(ui->matiereTable, r, 4).toInt();
            if (stockFilter == "Critique" && qty >= seuil) match = false;
            if (stockFilter == "Normal" && (qty < seuil || qty > seuil * 2)) match = false;
            if (stockFilter == "Elev�" && qty <= seuil * 2) match = false;
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

        // Filtre p�remption
        if (peremptionFilter != "Tous" && match) {
            QString dateStr = cellText(ui->matiereTable, r, 5);
            QDate expDate = QDate::fromString(dateStr, "yyyy-MM-dd");
            int daysLeft = QDate::currentDate().daysTo(expDate);
            
            if (peremptionFilter == "Expir�" && daysLeft >= 0) match = false;
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
        QString("?? %1 mati�re(s) trouv�e(s)").arg(visibleCount), 5000);
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
        if (quantite < seuil) ++critical; // Stock critique si quantit� < seuil
    }
    ui->statsValueMatiere1->setText(QString::number(total));
    ui->statsValueMatiere2->setText(QString::number(critical));
}

// ?? Suppliers ?????????????????????????????????????????????????????????????????
void MainWindow::setupFournisseurTable()
{
    ui->fournisseurTable->setColumnHidden(0, true);
    refreshFournisseurTable();
    updateFournisseurStatistics();
}

void MainWindow::refreshFournisseurTable()
{
    // Charger depuis la base de donn�es
    FournisseurData f;
    QSqlQueryModel* model = f.afficher();
    
    if (!model) {
        return;
    }
    
    // Charger depuis le mod�le BD
    int n = model->rowCount();
    ui->fournisseurTable->setRowCount(n);
    
    for (int i = 0; i < n; ++i) {
        for (int col = 0; col < 9; ++col) {
            QString value = model->data(model->index(i, col)).toString();
            ui->fournisseurTable->setItem(i, col, new QTableWidgetItem(value));
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
            QMessageBox::information(this, "Succ�s", "Fournisseur ajout� avec succ�s dans la base de donn�es!");
        } else {
            QMessageBox::critical(this, "Erreur", 
                "Impossible d'ajouter le fournisseur.\n"
                "V�rifiez que la table FOURNISSEURS existe dans la base de donn�es.");
        }
    }
}

void MainWindow::on_btnEditFournisseur_clicked()
{
    int row = ui->fournisseurTable->currentRow();
    if (row < 0) { QMessageBox::warning(this,"","Veuillez s�lectionner un fournisseur � modifier."); return; }
    
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
            QMessageBox::information(this, "Succ�s", "Fournisseur modifi� avec succ�s!");
        } else {
            QMessageBox::critical(this, "Erreur", 
                "Impossible de modifier le fournisseur.\n"
                "V�rifiez que la table FOURNISSEURS existe dans la base de donn�es.");
        }
    }
}

void MainWindow::on_btnDeleteFournisseur_clicked()
{
    int row = ui->fournisseurTable->currentRow();
    if (row < 0) { QMessageBox::warning(this,"","Veuillez s�lectionner un fournisseur � supprimer."); return; }
    
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
            QMessageBox::information(this, "Succ�s", "Fournisseur supprim� avec succ�s!");
        } else {
            QMessageBox::critical(this, "Erreur", 
                QString("Impossible de supprimer le fournisseur.\n"
                       "ID: %1\n"
                       "V�rifiez que la table FOURNISSEURS existe dans la base de donn�es.").arg(id));
        }
    }
}

void MainWindow::on_btnSmsFournisseur_clicked()
{
    int row = ui->fournisseurTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "", "Veuillez s�lectionner un fournisseur pour envoyer un SMS.");
        return;
    }

    QString nomEntreprise = ui->fournisseurTable->item(row, 1) ? ui->fournisseurTable->item(row, 1)->text() : "";
    QString telephone     = ui->fournisseurTable->item(row, 3) ? ui->fournisseurTable->item(row, 3)->text() : "";

    SmsFournisseurDialog dlg(nomEntreprise, telephone, this);
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
            "<table><thead><tr><th>ID</th><th>Nom Entreprise</th><th>Email</th><th>T�l�phone</th>"
            "<th>Type Produit</th><th>Condition Paiement</th><th>Statut</th></tr></thead><tbody>";

    for (const FournisseurData &f : fournisseursData) {
        html += QString("<tr><td>%1</td><td><b>%2</b></td><td>%3</td><td>%4</td>"
                        "<td>%5</td><td>%6</td><td>%7</td></tr>")
                .arg(f.getId(), f.getNomEntreprise(), f.getEmail(), f.getTelephone(),
                     f.getTypeProduit(), f.getConditionPaiement(), f.getStatut());
    }
    html += "</tbody></table></div>";

    // Section 2: Historique des transactions (donn�es statiques)
    html += "<div class='section'><h2>?? Historique des Transactions</h2>"
            "<table><thead><tr><th>Date</th><th>Fournisseur</th><th>Type</th><th>Montant (TND)</th><th>R�f�rence</th></tr></thead><tbody>"
            "<tr><td>15/01/2025</td><td>Leather Premium Co.</td><td>Paiement</td><td>5,000.00</td><td>PAY-2025-001</td></tr>"
            "<tr><td>10/01/2025</td><td>Textile Supplies Ltd</td><td>Facture</td><td>12,500.00</td><td>INV-2025-003</td></tr>"
            "<tr><td>05/01/2025</td><td>Metal Accessories Inc</td><td>Paiement</td><td>7,500.00</td><td>PAY-2025-002</td></tr>"
            "<tr><td>28/12/2024</td><td>Leather Premium Co.</td><td>Facture</td><td>8,200.00</td><td>INV-2024-125</td></tr>"
            "<tr><td>20/12/2024</td><td>Textile Supplies Ltd</td><td>Paiement</td><td>8,200.00</td><td>PAY-2024-089</td></tr>"
            "<tr><td>15/12/2024</td><td>Zipper World</td><td>Facture</td><td>3,500.00</td><td>INV-2024-118</td></tr>"
            "<tr><td>10/12/2024</td><td>Button Factory</td><td>Paiement</td><td>1,800.00</td><td>PAY-2024-075</td></tr>"
            "</tbody></table></div>";

    // Section 3: Liste des commandes effectu�es
    html += "<div class='section'><h2>?? Liste des Commandes Effectu�es</h2>"
            "<table><thead><tr><th>N� Commande</th><th>Date</th><th>Fournisseur</th><th>Produit</th><th>Quantit�</th><th>Montant (TND)</th></tr></thead><tbody>"
            "<tr><td>CMD-2025-015</td><td>10/01/2025</td><td>Leather Premium Co.</td><td>Cuir Premium</td><td>500 m�</td><td>12,500.00</td></tr>"
            "<tr><td>CMD-2024-098</td><td>28/12/2024</td><td>Textile Supplies Ltd</td><td>Tissu Doublure</td><td>300 m</td><td>8,200.00</td></tr>"
            "<tr><td>CMD-2024-087</td><td>15/12/2024</td><td>Zipper World</td><td>Fermetures Eclair</td><td>1000 pcs</td><td>3,500.00</td></tr>"
            "<tr><td>CMD-2024-076</td><td>05/12/2024</td><td>Button Factory</td><td>Boutons M�tal</td><td>2000 pcs</td><td>1,800.00</td></tr>"
            "<tr><td>CMD-2024-065</td><td>28/11/2024</td><td>Metal Accessories Inc</td><td>Boucles</td><td>500 pcs</td><td>4,200.00</td></tr>"
            "</tbody></table></div>";

    // Section 4: R�sum� financier
    html += "<div class='summary'>"
            "<h2 style='color:#8D6E63;margin-top:0;'>?? R�sum� Financier</h2>"
            "<p><b>?? Montant Total Pay�:</b> 26,000.00 TND</p>"
            "<p><b>?? Montant Total Factur�:</b> 38,500.00 TND</p>"
            "<p><b>? Solde Restant:</b> 12,500.00 TND</p>"
            "<p><b>?? Nombre de Transactions:</b> 7</p>"
            "<p><b>?? Nombre de Commandes:</b> 5</p>"
            "</div>";

    html += "<div style='text-align:center;margin-top:30px;color:#999;font-size:9px;'>"
            "Document g�n�r� automatiquement par CUIREA Management System<br>"
            "Pour usage interne uniquement - Confidentiel</div>"
            "</body></html>";

    QTextDocument doc; doc.setHtml(html); doc.print(&printer);
    QMessageBox::information(this,"Succ�s","Historique des transactions export�: "+fn);
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
        "?? D�lai Moyen", 
        "?? Retards (%)", 
        "? Annulations (%)", 
        "?? Retours (%)", 
        "? Score", 
        "Fiabilit�"
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
        score -= (p.delaiMoyen - 3) * 2;  // P�nalit� d�lai
        score -= p.tauxRetard * 1.5;       // P�nalit� retards
        score -= p.tauxAnnulation * 3;     // P�nalit� annulations
        score -= p.tauxRetour * 2;         // P�nalit� retours
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
        "?? <b>R�sum�:</b> Sur %1 fournisseurs analys�s, le score moyen de fiabilit� est de <b>%2/100</b>. "
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
    
    // Essayer d'utiliser la m�thode rechercher de la BD
    FournisseurData f;
    QSqlQueryModel* model = f.rechercher(text);
    
    if (!model || model->rowCount() == 0) {
        // Fallback sur le filtrage local
        filterTable(ui->fournisseurTable, text);
        if (model) delete model;
        return;
    }
    
    // Afficher les r�sultats de la recherche BD
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
        auto *desc = sub->addAction("? D�croissant (Z ? A)");
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

// ?? Production ????????????????????????????????????????????????????????????????
void MainWindow::setupProductionTable()
{
    int widths[] = {50,120,150,120,100,110,110,100,80,150};
    for (int i = 0; i < 10; ++i) ui->productionTable->setColumnWidth(i, widths[i]);
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
            {1, "REFERENCE"},
            {3, "PRODUIT"},
            {4, "MONTANT"},
            {5, "DATE_CREATION"},
            {6, "DATE_LIVRAISON"},
            {7, "STATUT"},
            {8, "PRIORITE"}
        };
        if (!colToSql.contains(logicalIndex)) return;

        // Alterner ASC/DESC si m�me colonne
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
        ProductionDAO dao;
        QSqlQueryModel *model = dao.trierPar(colToSql[logicalIndex], m_productionSortAsc);
        if (!model) return;

        ui->productionTable->setRowCount(0);
        for (int i = 0; i < model->rowCount(); ++i) {
            int row = ui->productionTable->rowCount();
            ui->productionTable->insertRow(row);
            ui->productionTable->setItem(row, 0, new QTableWidgetItem(model->data(model->index(i, 0)).toString()));
            ui->productionTable->setItem(row, 1, new QTableWidgetItem(model->data(model->index(i, 1)).toString()));
            ui->productionTable->setItem(row, 2, new QTableWidgetItem(model->data(model->index(i, 2)).toString()));
            ui->productionTable->setItem(row, 3, new QTableWidgetItem(model->data(model->index(i, 3)).toString()));
            ui->productionTable->setItem(row, 4, new QTableWidgetItem(QString::number(model->data(model->index(i, 8)).toDouble(), 'f', 2) + " DT"));
            ui->productionTable->setItem(row, 5, new QTableWidgetItem(model->data(model->index(i, 4)).toDate().toString("dd/MM/yyyy")));
            QDate dl = model->data(model->index(i, 5)).toDate();
            ui->productionTable->setItem(row, 6, new QTableWidgetItem(dl.isValid() ? dl.toString("dd/MM/yyyy") : "-"));
            ui->productionTable->setItem(row, 7, new QTableWidgetItem(model->data(model->index(i, 6)).toString()));
            ui->productionTable->setItem(row, 8, new QTableWidgetItem(model->data(model->index(i, 7)).toString()));
        }
        delete model;
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
void MainWindow::loadProductionData()
{
    ui->productionTable->setRowCount(0);
    
    ProductionDAO dao;
    QSqlQueryModel* model = dao.afficher();
    
    if (!model) {
        return;
    }
    
    for (int i = 0; i < model->rowCount(); ++i) {
        int row = ui->productionTable->rowCount();
        ui->productionTable->insertRow(row);
        
        // Nouveau mapping selon la requ�te SQL:
        // 0=ID_COMMANDE, 1=REFERENCE, 2=EMPLOYE, 3=TYPE, 4=DATE_CREATION, 5=DATE_LIVRAISON, 6=STATUT, 7=PRIORITE, 8=MONTANT
        
        // Colonne 0: ID_COMMANDE (cach� mais utilis� pour modifier)
        ui->productionTable->setItem(row, 0, new QTableWidgetItem(model->data(model->index(i, 0)).toString()));
        // Colonne 1: R�f�rence
        ui->productionTable->setItem(row, 1, new QTableWidgetItem(model->data(model->index(i, 1)).toString()));
        // Colonne 2: Employ�
        ui->productionTable->setItem(row, 2, new QTableWidgetItem(model->data(model->index(i, 2)).toString()));
        // Colonne 3: Type
        ui->productionTable->setItem(row, 3, new QTableWidgetItem(model->data(model->index(i, 3)).toString()));
        // Colonne 4: Montant
        ui->productionTable->setItem(row, 4, new QTableWidgetItem(QString::number(model->data(model->index(i, 8)).toDouble(), 'f', 2) + " DT"));
        // Colonne 5: Date Cr�ation
        ui->productionTable->setItem(row, 5, new QTableWidgetItem(model->data(model->index(i, 4)).toDate().toString("dd/MM/yyyy")));
        // Colonne 6: Date Livraison
        QDate dateLivraison = model->data(model->index(i, 5)).toDate();
        QString dateLivraisonStr = dateLivraison.isValid() ? dateLivraison.toString("dd/MM/yyyy") : "-";
        ui->productionTable->setItem(row, 6, new QTableWidgetItem(dateLivraisonStr));
        // Colonne 7: Statut
        ui->productionTable->setItem(row, 7, new QTableWidgetItem(model->data(model->index(i, 6)).toString()));
        // Colonne 8: Priorit�
        ui->productionTable->setItem(row, 8, new QTableWidgetItem(model->data(model->index(i, 7)).toString()));
        // Colonne 9: Mail Client
        ui->productionTable->setItem(row, 9, new QTableWidgetItem(model->data(model->index(i, 9)).toString()));
    }
    
    delete model;
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
    // Statuts qui d�clenchent la bulle : En Attente, En Cours, Suspendu
    // On exclut : Exp�di�, Termin�, Annul� (et variantes sans accent)
    query.prepare(
        "SELECT ID_COMMANDE, REFERENCE, DATE_LIVRAISON, STATUT, PRIORITE "
        "FROM COMMANDES "
        "WHERE DATE_LIVRAISON IS NOT NULL "
        "AND DATE_LIVRAISON < :today "
        "AND UPPER(STATUT) NOT IN ("
        "  'TERMINE','TERMINE',"
        "  'ANNULE','ANNULE'"
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

        QString titre = QString("? Retard � %1").arg(ref);
        QString msg   = QString("Livraison pr�vue le %1\n%2 jour(s) de retard � Statut : %3")
                            .arg(dl.toString("dd/MM/yyyy"))
                            .arg(jours)
                            .arg(statut);

        NotificationWidget::show(titre, msg, ntype);
    }
}

void MainWindow::onTrierProduction()
{
    // ?? Dialogue de tri avanc� multi-crit�res ????????????????????????????????
    QDialog dlg(this);
    dlg.setWindowTitle("Tri avanc� � Production");
    dlg.setFixedSize(420, 340);
    dlg.setStyleSheet(
        "QDialog{background:#FBF5F0;border-radius:8px;}"
        "QLabel{color:#3E1020;font-size:12px;font-weight:bold;}"
        "QComboBox{background:white;border:1px solid #C4923A;border-radius:5px;"
        "padding:6px 10px;font-size:12px;color:#3E1020;min-height:28px;}"
        "QComboBox::drop-down{border:none;width:20px;}"
        "QComboBox QAbstractItemView{background:white;color:#3E1020;"
        "selection-background-color:#6B2737;selection-color:white;border:1px solid #C4923A;}"
        "QGroupBox{background:white;border:1px solid #E8DDD5;border-radius:8px;padding:8px;margin-top:6px;}"
        "QGroupBox::title{color:#6B2737;font-size:11px;font-weight:bold;"
        "subcontrol-origin:margin;left:10px;padding:0 4px;}"
    );

    QVBoxLayout *root = new QVBoxLayout(&dlg);
    root->setContentsMargins(20,16,20,16);
    root->setSpacing(12);

    // Titre
    QLabel *title = new QLabel("Trier les commandes par :");
    title->setStyleSheet("font-size:14px;font-weight:bold;color:#6B2737;");
    root->addWidget(title);

    // Colonnes disponibles
    QStringList colLabels = {"R�f�rence","Employ�","Produit","Montant HT",
                             "Date Cr�ation","Date Livraison","Statut","Priorit�","Mail Client"};
    QStringList colSql    = {"REFERENCE","","PRODUIT","MONTANT",
                             "DATE_CREATION","DATE_LIVRAISON","STATUT","PRIORITE",""};

    // Crit�re 1
    QGroupBox *g1 = new QGroupBox("Crit�re principal");
    QHBoxLayout *l1 = new QHBoxLayout(g1); l1->setSpacing(8);
    QComboBox *col1 = new QComboBox(); QComboBox *ord1 = new QComboBox();
    for (int i = 0; i < colLabels.size(); ++i)
        if (!colSql[i].isEmpty()) col1->addItem(colLabels[i], colSql[i]);
    col1->setCurrentIndex(3); // Montant par d�faut
    ord1->addItem("?  D�croissant", false);
    ord1->addItem("?  Croissant",   true);
    ord1->setFixedWidth(130);
    l1->addWidget(col1, 1); l1->addWidget(ord1);
    root->addWidget(g1);

    // Crit�re 2
    QGroupBox *g2 = new QGroupBox("Crit�re secondaire (optionnel)");
    QHBoxLayout *l2 = new QHBoxLayout(g2); l2->setSpacing(8);
    QComboBox *col2 = new QComboBox(); QComboBox *ord2 = new QComboBox();
    col2->addItem("� Aucun �", "");
    for (int i = 0; i < colLabels.size(); ++i)
        if (!colSql[i].isEmpty()) col2->addItem(colLabels[i], colSql[i]);
    ord2->addItem("?  D�croissant", false);
    ord2->addItem("?  Croissant",   true);
    ord2->setFixedWidth(130);
    l2->addWidget(col2, 1); l2->addWidget(ord2);
    root->addWidget(g2);

    root->addStretch();

    // Boutons
    QHBoxLayout *btns = new QHBoxLayout(); btns->setSpacing(10);
    QPushButton *btnReset  = new QPushButton("?  R�initialiser");
    QPushButton *btnApply  = new QPushButton("?  Appliquer");
    QPushButton *btnCancel = new QPushButton("Annuler");
    btnApply->setStyleSheet(
        "QPushButton{background:#6B2737;color:white;border:none;border-radius:6px;"
        "padding:9px 20px;font-size:13px;font-weight:bold;}"
        "QPushButton:hover{background:#4E1A27;}");
    btnReset->setStyleSheet(
        "QPushButton{background:#C4923A;color:white;border:none;border-radius:6px;"
        "padding:9px 16px;font-size:12px;font-weight:bold;}"
        "QPushButton:hover{background:#A87730;}");
    btnCancel->setStyleSheet(
        "QPushButton{background:#E8DDD5;color:#3E1020;border:none;border-radius:6px;"
        "padding:9px 16px;font-size:12px;}"
        "QPushButton:hover{background:#D4C8BC;}");

    connect(btnReset, &QPushButton::clicked, [&]{
        loadProductionData();
        ui->productionTable->horizontalHeader()->setSortIndicatorShown(false);
        dlg.accept();
    });
    connect(btnCancel, &QPushButton::clicked, &dlg, &QDialog::reject);
    connect(btnApply, &QPushButton::clicked, [&]{
        QString c1 = col1->currentData().toString();
        bool    a1 = ord1->currentData().toBool();
        QString c2 = col2->currentData().toString();
        bool    a2 = ord2->currentData().toBool();

        // Construire la requ�te SQL avec 1 ou 2 crit�res
        QString sql =
            "SELECT C.ID_COMMANDE, C.REFERENCE, (E.NOM || ' ' || E.PRENOM) AS EMPLOYE, "
            "C.PRODUIT, C.DATE_CREATION, C.DATE_LIVRAISON, C.STATUT, C.PRIORITE, C.MONTANT, C.MAIL_CLIENT "
            "FROM COMMANDES C LEFT JOIN EMPLOYES E ON C.ID_EMPLOYE = E.ID_EMPLOYE "
            "ORDER BY C." + c1 + (a1 ? " ASC" : " DESC");
        if (!c2.isEmpty())
            sql += ", C." + c2 + (a2 ? " ASC" : " DESC");

        QSqlQueryModel model;
        model.setQuery(sql, Connection::instance()->getDatabase());
        if (model.lastError().isValid()) { dlg.reject(); return; }

        // Indicateur visuel sur le header
        static const QMap<QString,int> sqlToCol = {
            {"REFERENCE",1},{"PRODUIT",3},{"MONTANT",4},
            {"DATE_CREATION",5},{"DATE_LIVRAISON",6},{"STATUT",7},{"PRIORITE",8}
        };
        if (sqlToCol.contains(c1)) {
            ui->productionTable->horizontalHeader()->setSortIndicatorShown(true);
            ui->productionTable->horizontalHeader()->setSortIndicator(
                sqlToCol[c1], a1 ? Qt::AscendingOrder : Qt::DescendingOrder);
        }

        ui->productionTable->setRowCount(0);
        for (int i = 0; i < model.rowCount(); ++i) {
            int row = ui->productionTable->rowCount();
            ui->productionTable->insertRow(row);
            ui->productionTable->setItem(row,0,new QTableWidgetItem(model.data(model.index(i,0)).toString()));
            ui->productionTable->setItem(row,1,new QTableWidgetItem(model.data(model.index(i,1)).toString()));
            ui->productionTable->setItem(row,2,new QTableWidgetItem(model.data(model.index(i,2)).toString()));
            ui->productionTable->setItem(row,3,new QTableWidgetItem(model.data(model.index(i,3)).toString()));
            ui->productionTable->setItem(row,4,new QTableWidgetItem(QString::number(model.data(model.index(i,8)).toDouble(),'f',2)+" DT"));
            ui->productionTable->setItem(row,5,new QTableWidgetItem(model.data(model.index(i,4)).toDate().toString("dd/MM/yyyy")));
            QDate dl = model.data(model.index(i,5)).toDate();
            ui->productionTable->setItem(row,6,new QTableWidgetItem(dl.isValid()?dl.toString("dd/MM/yyyy"):"-"));
            ui->productionTable->setItem(row,7,new QTableWidgetItem(model.data(model.index(i,6)).toString()));
            ui->productionTable->setItem(row,8,new QTableWidgetItem(model.data(model.index(i,7)).toString()));
            ui->productionTable->setItem(row,9,new QTableWidgetItem(model.data(model.index(i,9)).toString()));
        }
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
    // Optimisation: 1 seule requ�te au lieu de 4
    QSqlQuery query(Connection::instance()->getDatabase());
    
    QString sql = "SELECT "
                  "COUNT(*) AS total, "
                  "SUM(CASE WHEN STATUT = 'En Production' THEN 1 ELSE 0 END) AS en_prod, "
                  "SUM(CASE WHEN STATUT = 'Termin�' THEN 1 ELSE 0 END) AS terminees, "
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
    
    // Mettre � jour les labels
    ui->statsValueProduction1->setText(QString::number(total));
    ui->statsValueProduction2->setText(QString::number(enProd));
    ui->statsValueProduction3->setText(QString::number(terminees));
    ui->statsValueProduction4->setText(QString::number(montantTotal, 'f', 2) + " DT");
    
    qDebug() << "?? Statistiques mises � jour:";
    qDebug() << "   Total:" << total;
    qDebug() << "   En Production:" << enProd;
    qDebug() << "   Termin�es:" << terminees;
    qDebug() << "   Montant Total:" << montantTotal << "DT";
}

void MainWindow::onCreerProduction()
{
    ProductionDialog dlg(this, ProductionDialog::AddMode);
    if (dlg.exec() == QDialog::Accepted) {
        qDebug() << "========== AJOUT COMMANDE ==========";
        
        // Cr�er l'objet Production
        Production prod;
        
        // G�n�rer une r�f�rence unique
        QString ref = dlg.getReference();
        if (ref.isEmpty()) {
            ref = QString("PROD-%1-%2")
                .arg(QDate::currentDate().year())
                .arg(QTime::currentTime().toString("HHmmss"));
        }
        
        prod.setReference(ref);
        prod.setType(dlg.getProduit()); // Le nom du produit (ex: "Sac � Main Cuir") va dans PRODUIT
        prod.setServiceVente(dlg.getProduit());
        prod.setMontant(dlg.getQuantite().toDouble()); // getQuantite() contient le prix saisi
        prod.setIdEmploye(dlg.getEmployeId());  // R�cup�rer l'ID de l'employ� s�lectionn�
        prod.setMailClient(dlg.getMailClient());
        
        // G�rer les dates
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
        prod.setEtatPaiement("Non pay�e");
        prod.setServiceVente("Service Commercial");
        
        // Debug
        qDebug() << "Reference:" << prod.getReference();
        qDebug() << "Type:" << prod.getType();
        qDebug() << "Montant:" << prod.getMontant();
        qDebug() << "ID Employe:" << prod.getIdEmploye();  // IMPORTANT: V�rifier l'ID
        qDebug() << "Date Creation:" << prod.getDateCreation().toString("yyyy-MM-dd");
        qDebug() << "Date Livraison:" << prod.getDateLivraisonPrevue().toString("yyyy-MM-dd");
        qDebug() << "Statut:" << prod.getStatut();
        qDebug() << "Priorite:" << prod.getPriorite();
        
        ProductionDAO dao;
        if (dao.ajouter(prod)) {
            qDebug() << "? Commande ajout�e avec succ�s";
            QMessageBox::information(this, "Succ�s", "Commande ajout�e avec succ�s!");
            loadProductionData();
        } else {
            qDebug() << "? Echec de l'ajout";
            QMessageBox::critical(this, "Erreur", "Erreur lors de l'ajout de la commande.");
        }
    }
}

void MainWindow::onModifierProduction()
{
    int row = ui->productionTable->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Attention", "Veuillez s�lectionner une commande.");
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
    
    // Remplacer LineEdit par ComboBox pour l'employ�
    QComboBox employeC(&d);
    QMap<int, QString> employeMap;
    
    // Charger les employ�s depuis la base de donn�es
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
    
    // S�lectionner l'employ� actuel
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
            typeC.addItems({"Sac � Main Cuir","Portefeuille","Ceinture","Sacoche","Porte-documents","Sac � Dos"});
    }
    // Pr�-s�lectionner le produit actuel
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

    QDateEdit dcE(QDate::fromString(cellText(ui->productionTable, row, 5), "dd/MM/yyyy"), &d);
    dcE.setCalendarPopup(true);
    dcE.setDisplayFormat("dd/MM/yyyy");

    QDateEdit dlE(QDate::fromString(cellText(ui->productionTable, row, 6), "dd/MM/yyyy"), &d);
    dlE.setCalendarPopup(true);
    dlE.setDisplayFormat("dd/MM/yyyy");

    QComboBox statC(&d);
    statC.addItems({"En Attente", "Planifi�", "En Cours", "En Production", "Suspendu", "Termin�", "Annul�"});
    statC.setCurrentText(cellText(ui->productionTable, row, 7));

    QComboBox prioC(&d);
    prioC.addItems({"Basse", "Normale", "Urgente"});
    prioC.setCurrentText(cellText(ui->productionTable, row, 8));

    form.addRow("R�f�rence *:", &refE);
    form.addRow("Employ� *:", &employeC);
    form.addRow("Produit:", &typeC);
    form.addRow("Montant HT *:", &montantE);
    form.addRow("Date Cr�ation:", &dcE);
    form.addRow("Date Livraison:", &dlE);
    form.addRow("Statut:", &statC);
    form.addRow("Priorit�:", &prioC);

    // ComboBox client charg� depuis la BD
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
        
        ProductionDAO dao;
        if (dao.modifier(prod)) {
            QMessageBox::information(this, "Succ�s", "Commande modifi�e avec succ�s!");
            loadProductionData(); // Actualiser l'affichage
        } else {
            QMessageBox::critical(this, "Erreur", "Erreur lors de la modification de la commande.");
        }
    }
}

void MainWindow::onSuiviProduction()
{
    int row = ui->productionTable->currentRow();
    if (row < 0) { QMessageBox::warning(this,"","S�lectionnez une commande."); return; }
    QString ref    = cellText(ui->productionTable,row,1);
    QString employe = cellText(ui->productionTable,row,2);
    QString statut = cellText(ui->productionTable,row,7);
    QString dc     = cellText(ui->productionTable,row,5);
    QString dl     = cellText(ui->productionTable,row,6);
    QString prio   = cellText(ui->productionTable,row,8);

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
    step(dc,"? Commande cr��e","La commande a �t� enregistr�e dans le syst�me");
    if (statut=="Planifi�"||statut=="En Production"||statut=="Termin�")
        step(QDate::fromString(dc,"dd/MM/yyyy").addDays(1).toString("dd/MM/yyyy"),
             "? Planifi�e","La commande a �t� planifi�e pour la production");
    if (statut=="En Production"||statut=="Termin�")
        step(QDate::fromString(dc,"dd/MM/yyyy").addDays(3).toString("dd/MM/yyyy"),
             "? En production","La commande est en cours de fabrication");
    if (statut=="Termin�")
        step(dl,"? Termin�e","La commande a �t� termin�e et livr�e avec succ�s");
    else
        step(dl+" (Pr�vu)","? Livraison pr�vue","Date de livraison estim�e");
    html += "</body></html>";

    QDialog dlg(this); dlg.setWindowTitle("Suivi - "+ref); dlg.setMinimumSize(650,600);
    QVBoxLayout lay(&dlg); lay.setContentsMargins(20,20,20,20);
    auto *info = new QLabel(QString("<b>Ref:</b> %1 | <b>Employ�:</b> %2 | <b>Priorit�:</b> %3")
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
    // Ouvrir la vue compl�te de production avec ProductionView
    ProductionView *productionView = new ProductionView(this);
    if (m_pipeline)
        productionView->setNotificationPipeline(m_pipeline);
    productionView->setAttribute(Qt::WA_DeleteOnClose);
    productionView->show();
}

void MainWindow::onFactureProduction()
{
    int row = ui->productionTable->currentRow();
    if (row < 0) { QMessageBox::warning(this,"","S�lectionnez une commande."); return; }

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

    // ?? Dialogue ?????????????????????????????????????????????????????????????
    QDialog dlg(this);
    dlg.setWindowTitle("Facture � " + ref);
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

    // ?? EN-TETE ??????????????????????????????????????????????????????????????
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
    QLabel *numLbl = new QLabel("N� " + ref);
    numLbl->setStyleSheet("font-size:13px; color:#555;"); numLbl->setAlignment(Qt::AlignRight);
    QLabel *dateLbl = new QLabel("Date : " + QDate::currentDate().toString("dd MMMM yyyy"));
    dateLbl->setStyleSheet("font-size:12px; color:#888;"); dateLbl->setAlignment(Qt::AlignRight);
    titleCol->addWidget(facLbl); titleCol->addWidget(numLbl); titleCol->addWidget(dateLbl);
    hdr->addLayout(titleCol);
    lay->addLayout(hdr);

    // S�parateur
    auto makeSep = [&]() {
        QFrame *s = new QFrame(); s->setFrameShape(QFrame::HLine);
        s->setStyleSheet("color:#E0E0E0;"); return s;
    };
    lay->addWidget(makeSep());

    // ?? INFOS ENTREPRISE / CLIENT ?????????????????????????????????????????????
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
        "T�l : +216 71 000 000", "contact@cuirea.tn"
    }));
    infoRow->addSpacing(20);
    infoRow->addWidget(makeInfoBox("Factur� �", {
        mailClient.isEmpty() ? "Client interne" : mailClient,
        "Livraison pr�vue : " + dl
    }));
    lay->addLayout(infoRow);

    // ?? TABLEAU ARTICLES ??????????????????????????????????????????????????????
    QLabel *tblTitle = new QLabel("D�tail de la commande");
    tblTitle->setStyleSheet("font-size:11px; font-weight:bold; color:#6B2737; text-transform:uppercase; letter-spacing:1px;");
    lay->addWidget(tblTitle);

    QTableWidget *table = new QTableWidget(1, 4, page);
    table->setHorizontalHeaderLabels({"Description", "Prix Unitaire", "Quantit�", "Total"});
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
    table->setItem(0,0, new QTableWidgetItem(type + " � R�f. " + ref));
    table->setItem(0,1, new QTableWidgetItem(QString::number(ht,'f',2) + " DT"));
    table->setItem(0,2, new QTableWidgetItem("1"));
    table->setItem(0,3, new QTableWidgetItem(QString::number(ht,'f',2) + " DT"));
    table->setFixedHeight(table->horizontalHeader()->height() + table->rowHeight(0) + 4);
    lay->addWidget(table);

    // ?? TOTAUX ????????????????????????????????????????????????????????????????
    QHBoxLayout *totRow = new QHBoxLayout();
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

    // ?? NOTE ?????????????????????????????????????????????????????????????????
    QWidget *noteBox = new QWidget();
    noteBox->setStyleSheet("background:#FBF5F0; border-left:4px solid #C4923A; border-radius:4px;");
    QVBoxLayout *noteLay = new QVBoxLayout(noteBox);
    noteLay->setContentsMargins(14,10,14,10);
    QLabel *noteT = new QLabel("Note");
    noteT->setStyleSheet("font-weight:bold; font-size:11px; color:#C4923A;");
    QLabel *noteV = new QLabel("Priorit� : " + priorite + "  |  Statut : " + statut + "  |  Cr��e le : " + dc);
    noteV->setStyleSheet("font-size:12px; color:#555;");
    noteLay->addWidget(noteT); noteLay->addWidget(noteV);
    lay->addWidget(noteBox);

    // Build condUrl — use production data directly + DB lookup for extra details
    QString condUrl;
    {
        QString categoriePdf, couleurPdf, dimensionsPdf, prixPdf, statutArtPdf;
        QSqlQuery q2(Connection::instance()->getDatabase());
        // Try to find matching article — search by type/nom/reference
        q2.prepare("SELECT CATEGORIE, COULEUR_R, COULEUR_G, COULEUR_B, "
                   "LARGEUR, HAUTEUR, PROFONDEUR, PRIX_UNITAIRE, STATUT "
                   "FROM ARTICLES WHERE UPPER(NOM) LIKE UPPER(:type) "
                   "OR UPPER(TYPE) LIKE UPPER(:type2) "
                   "OR UPPER(REFERENCE) LIKE UPPER(:ref) "
                   "OR ROWNUM = 1");
        q2.bindValue(":type",  "%" + type + "%");
        q2.bindValue(":type2", "%" + type + "%");
        q2.bindValue(":ref",   "%" + ref  + "%");
        if (q2.exec() && q2.next()) {
            categoriePdf  = q2.value(0).toString();
            int r = q2.value(1).toInt(), g = q2.value(2).toInt(), b = q2.value(3).toInt();
            if (r > 0 || g > 0 || b > 0)
                couleurPdf = QString("rgb(%1,%2,%3)").arg(r).arg(g).arg(b);
            double l = q2.value(4).toDouble(), h = q2.value(5).toDouble(), p = q2.value(6).toDouble();
            if (l > 0 || h > 0 || p > 0)
                dimensionsPdf = QString("%1x%2x%3 cm").arg(l).arg(h).arg(p);
            prixPdf      = q2.value(7).toString() + " DT";
            statutArtPdf = q2.value(8).toString();
        }
        // Fallback: use production data if article not found
        if (prixPdf.isEmpty()) prixPdf = QString::number(ht, 'f', 2) + " DT";
        if (statutArtPdf.isEmpty()) statutArtPdf = statut;

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
    // QR CODE CONDITIONS
    {
        QHBoxLayout *qrRow = new QHBoxLayout();
        qrRow->setSpacing(20);

        // Fetch article details from DB
        QString categorie, couleur, dimensions, prix, statutArt;
        {
            QSqlQuery q(Connection::instance()->getDatabase());
            q.prepare("SELECT CATEGORIE, COULEUR_R, COULEUR_G, COULEUR_B, "
                      "LARGEUR, HAUTEUR, PROFONDEUR, PRIX_UNITAIRE, STATUT "
                      "FROM ARTICLES WHERE UPPER(NOM) LIKE UPPER(:type) "
                      "OR UPPER(TYPE) LIKE UPPER(:type2) "
                      "OR UPPER(REFERENCE) LIKE UPPER(:ref)");
            q.bindValue(":type",  "%" + type + "%");
            q.bindValue(":type2", "%" + type + "%");
            q.bindValue(":ref",   "%" + ref  + "%");
            if (q.exec() && q.next()) {
                categorie  = q.value(0).toString();
                int r = q.value(1).toInt(), g = q.value(2).toInt(), b = q.value(3).toInt();
                couleur    = QString("rgb(%1,%2,%3)").arg(r).arg(g).arg(b);
                double l = q.value(4).toDouble(), h = q.value(5).toDouble(), p = q.value(6).toDouble();
                dimensions = QString("%1x%2x%3 cm").arg(l).arg(h).arg(p);
                prix       = q.value(7).toString() + " DT";
                statutArt  = q.value(8).toString();
            }
        }

        // Build Netlify URL with article params

        // QR label
        QLabel *qrLabel = new QLabel("...");
        qrLabel->setFixedSize(110, 110);
        qrLabel->setAlignment(Qt::AlignCenter);
        qrLabel->setStyleSheet("border:2px solid #C4923A;border-radius:6px;background:white;font-size:10px;color:#888;");

        QVBoxLayout *qrTextCol = new QVBoxLayout();
        QLabel *qrTitle = new QLabel("Scannez pour la fiche article");
        qrTitle->setStyleSheet("font-size:12px;font-weight:bold;color:#6B2737;");
        QLabel *qrDesc = new QLabel(
            "La fiche contient :\n"
            "� Caract�ristiques de l'article\n"
            "� Temp�rature & humidit�\n"
            "� Protection lumi�re\n"
            "� A�ration\n"
            "� Entretien r�gulier\n"
            "� Sources de chaleur");
        qrDesc->setStyleSheet("font-size:11px;color:#555;");
        qrDesc->setWordWrap(true);
        qrTextCol->addWidget(qrTitle);
        qrTextCol->addWidget(qrDesc);
        qrTextCol->addStretch();
        qrRow->addWidget(qrLabel);
        qrRow->addLayout(qrTextCol);

        // Clickable link below QR
        QLabel *qrLink = new QLabel(QString("<a href='%1' style='color:#C4923A;font-size:11px;'>Ouvrir la fiche article</a>").arg(condUrl));
        qrLink->setOpenExternalLinks(true);
        qrLink->setAlignment(Qt::AlignLeft);

        QVBoxLayout *qrContainer = new QVBoxLayout();
        qrContainer->setSpacing(6);
        qrContainer->addLayout(qrRow);
        qrContainer->addWidget(qrLink);
        lay->addLayout(qrContainer);

        // Generate QR directly
        QNetworkAccessManager *mgr = new QNetworkAccessManager(page);
        QString qrApiUrl = QString("https://api.qrserver.com/v1/create-qr-code/?size=110x110&ecc=M&data=%1")
                           .arg(QString::fromUtf8(QUrl::toPercentEncoding(condUrl)));
        QNetworkReply *qrReply = mgr->get(QNetworkRequest(QUrl(qrApiUrl)));
        connect(qrReply, &QNetworkReply::finished, qrLabel, [qrLabel, qrReply]() {
            QPixmap px;
            if (px.loadFromData(qrReply->readAll()))
                qrLabel->setPixmap(px);
            qrReply->deleteLater();
        });
    }

    // ?? PIED DE PAGE ?????????????????????????????????????????????????????????
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
    footer->addLayout(makeFooterCol("Conditions", {"Paiement sous 30 jours", "P�nalit�s : 1,5%/mois"}));
    lay->addLayout(footer);

    scroll->setWidget(page);
    root->addWidget(scroll);

    // ?? BOUTONS ???????????????????????????????????????????????????????????????
    QHBoxLayout *btns = new QHBoxLayout();
    QPushButton *pdf   = new QPushButton("Exporter PDF", &dlg);
    QPushButton *email = new QPushButton("Envoyer Email", &dlg);
    QPushButton *close = new QPushButton("Fermer",        &dlg);
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

    // HTML pour export PDF � fid�le au design Qt affich�
    QString htmlPdf = QString(R"(
<html><head><meta charset='UTF-8'><style>
  * { margin:0; padding:0; box-sizing:border-box; }
  body { font-family: Arial, sans-serif; background: #FBF5F0; color: #3E1020; padding: 36px; }

  /* EN-TETE */
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

<!-- EN-TETE -->
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

<!-- TOTAUX -->
<div class='totals-wrap'>
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

<!-- NOTE -->
<div class='note-box'>
  <div class='note-title'>Note</div>
  <div class='note-val'>Priorit&eacute; : %10 &nbsp;|&nbsp; Statut : %11 &nbsp;|&nbsp; Cr&eacute;&eacute;e le : %12</div>
</div>

<!-- QR CODE -->
<div style='margin:16px 0;padding:14px;background:#FBF5F0;border-left:4px solid #C4923A;border-radius:6px;display:table;width:100%%;'>
  <div style='display:table-cell;vertical-align:middle;width:120px;'>
    <img src='data:image/png;base64,%13' width='100' height='100'/>
  </div>
  <div style='display:table-cell;vertical-align:middle;padding-left:16px;'>
    <div style='font-weight:bold;color:#6B2737;font-size:13px;margin-bottom:6px;'>Scannez pour la fiche article</div>
    <div style='font-size:11px;color:#555;line-height:1.6;'>
      Caract&eacute;ristiques &bull; Temp&eacute;rature &amp; humidit&eacute;<br/>
      Protection lumi&egrave;re &bull; A&eacute;ration<br/>
      Entretien r&eacute;gulier &bull; Sources de chaleur
    </div>
  </div>
</div>

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

    connect(pdf, &QPushButton::clicked, [&, condUrl] {
        QString fn = QFileDialog::getSaveFileName(&dlg, "Enregistrer",
            "Facture_" + ref + ".pdf", "PDF (*.pdf)");
        if (fn.isEmpty()) return;

        // Download QR image as base64 then generate PDF
        QNetworkAccessManager *pdfMgr = new QNetworkAccessManager(&dlg);
        QString qrUrl = QString("https://api.qrserver.com/v1/create-qr-code/?size=120x120&ecc=M&data=%1")
                        .arg(QString::fromUtf8(QUrl::toPercentEncoding(condUrl)));
        QNetworkReply *r = pdfMgr->get(QNetworkRequest(QUrl(qrUrl)));
        connect(r, &QNetworkReply::finished, &dlg, [r, fn, htmlPdf, &dlg]() {
            QString qrBase64 = QString::fromLatin1(r->readAll().toBase64());
            r->deleteLater();
            QString finalHtml = htmlPdf;
            finalHtml.replace("%13", qrBase64);
            QPrinter p(QPrinter::HighResolution);
            p.setOutputFormat(QPrinter::PdfFormat);
            p.setOutputFileName(fn);
            p.setPageSize(QPageSize::A4);
            QTextDocument doc; doc.setHtml(finalHtml); doc.print(&p);
            QMessageBox::information(&dlg, "Succ�s", "Facture export�e :\n" + fn);
        });
    });
    //fonction mail client
    connect(email, &QPushButton::clicked, [&] {

        if (mailClient.isEmpty()) {
            QMessageBox::warning(&dlg, "Email manquant",
                                 "? Aucun email client associ� � cette commande.");
            return;
        }

        Mail mailer;

        // ?? SUBJECT ?????????????????????????????????????????????
        QString subject = "Facture CUIREA - Commande #" + ref;

        // ?? BODY HTML ???????????????????????????????????????????
        QString body =
            "<html>"
            "<body style='font-family:Arial; background:#f9f9f9; padding:20px;'>"

            "<h2 style='color:#6B2737;'>FACTURE CUIREA</h2>"

            "<p><b>R�f�rence :</b> " + ref + "</p>"
            "<p><b>Employ� :</b> " + employe + "</p>"

            "<hr>"

            "<h3>Client</h3>"
            "<p>" + (mailClient.isEmpty() ? "Client interne" : mailClient) + "</p>"

            "<h3>D�tails de la commande</h3>"

            "<table border='1' cellpadding='8' cellspacing='0' style='border-collapse:collapse; width:100%;'>"
            "<tr style='background:#6B2737;color:white;'>"
            "<th>Description</th>"
            "<th>Prix HT</th>"
            "<th>Quantit�</th>"
            "<th>Total</th>"
            "</tr>"

            "<tr>"
            "<td>" + type + " � R�f " + ref + "</td>"
            "<td>" + QString::number(ht,'f',2) + " DT</td>"
            "<td>1</td>"
            "<td>" + QString::number(ht,'f',2) + " DT</td>"
            "</tr>"
            "</table>"

            "<h3>Totaux</h3>"
            "<p>? Sous-total HT : " + QString::number(ht,'f',2) + " DT</p>"
            "<p>? TVA (19%) : " + QString::number(tva,'f',2) + " DT</p>"
            "<p>? Remise (5%) : -" + QString::number(remise,'f',2) + " DT</p>"

            "<h2 style='color:#2c3e50;'>Total TTC : " + QString::number(ttc,'f',2) + " DT</h2>"

            "<hr>"

            "<p><b>Date cr�ation :</b> " + dc + "</p>"
            "<p><b>Date livraison :</b> " + dl + "</p>"
            "<p><b>Statut :</b> " + statut + "</p>"
            "<p><b>Priorit� :</b> " + priorite + "</p>"

            "<hr>"

            "<footer style='font-size:12px;color:gray;text-align:center;'>"
            "<b>CUIREA</b><br>"
            "Smart Leather Goods Factory<br>"
            "Zone Industrielle, Tunis, Tunisie<br>"
            "contact@cuirea.tn | +216 71 000 000"
            "</footer>"

            "</body>"
            "</html>";

        // ?? SEND EMAIL ??????????????????????????????????????????
        bool ok = mailer.sendEmail(mailClient, subject, body);

        if (ok)
            QMessageBox::information(&dlg, "Email envoy�",
                                     "? Email envoy� � " + mailClient);
        else
            QMessageBox::critical(&dlg, "Erreur",
                                  "? Echec de l'envoi � " + mailClient);

    }); // ?? FIN DU connect PROPREMENT

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

        // M�me mapping que loadProductionData()
        // SQL: 0=ID_COMMANDE, 1=REFERENCE, 2=EMPLOYE, 3=TYPE,
        //      4=DATE_CREATION, 5=DATE_LIVRAISON, 6=STATUT, 7=PRIORITE, 8=MONTANT

        // Col 0 : ID (cach�)
        ui->productionTable->setItem(row, 0, new QTableWidgetItem(model->data(model->index(i, 0)).toString()));
        // Col 1 : R�f�rence
        ui->productionTable->setItem(row, 1, new QTableWidgetItem(model->data(model->index(i, 1)).toString()));
        // Col 2 : Employ�
        ui->productionTable->setItem(row, 2, new QTableWidgetItem(model->data(model->index(i, 2)).toString()));
        // Col 3 : Type
        ui->productionTable->setItem(row, 3, new QTableWidgetItem(model->data(model->index(i, 3)).toString()));
        // Col 4 : Montant
        ui->productionTable->setItem(row, 4, new QTableWidgetItem(QString::number(model->data(model->index(i, 8)).toDouble(), 'f', 2) + " DT"));
        // Col 5 : Date Cr�ation
        ui->productionTable->setItem(row, 5, new QTableWidgetItem(model->data(model->index(i, 4)).toDate().toString("dd/MM/yyyy")));
        // Col 6 : Date Livraison
        QDate dateLivraison = model->data(model->index(i, 5)).toDate();
        ui->productionTable->setItem(row, 6, new QTableWidgetItem(dateLivraison.isValid() ? dateLivraison.toString("dd/MM/yyyy") : "-"));
        // Col 7 : Statut
        ui->productionTable->setItem(row, 7, new QTableWidgetItem(model->data(model->index(i, 6)).toString()));
        // Col 8 : Priorit�
        ui->productionTable->setItem(row, 8, new QTableWidgetItem(model->data(model->index(i, 7)).toString()));
        // Col 9 : Mail Client
        ui->productionTable->setItem(row, 9, new QTableWidgetItem(model->data(model->index(i, 9)).toString()));
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
    auto *fact    = menu.addAction("G�n�rer facture");
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
        QMessageBox::warning(this, "Attention", "S�lectionnez une commande.");
        return;
    }
    
    QString ref = cellText(ui->productionTable, row, 1);
    QString employe = cellText(ui->productionTable, row, 2);
    int id = cellText(ui->productionTable, row, 0).toInt();
    
    if (QMessageBox::question(this, "Confirmer",
            QString("Supprimer la commande %1 de l'employ� %2 ?").arg(ref, employe),
            QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        
        ProductionDAO dao;
        if (dao.supprimer(id)) {
            QMessageBox::information(this, "Succ�s", "Commande supprim�e avec succ�s!");
            loadProductionData(); // Actualiser l'affichage
        } else {
            QMessageBox::critical(this, "Erreur", "Erreur lors de la suppression de la commande.");
        }
    }
}

void MainWindow::onStatistiquesProduction() { afficherStatistiquesModernes(); }

void MainWindow::afficherStatistiquesModernes()
{
    // ?? Palette Bordeaux & Or ????????????????????????????????????????????????
    static const QString C_BORDEAUX     = "#6B2737";
    static const QString C_OR           = "#C4923A";
    static const QString C_OR_PALE      = "#D4B896";
    static const QString C_BORDEAUX_MID = "#A0485A";
    static const QString C_CREME        = "#FBF5F0";
    static const QString C_GRIS         = "#8C7B6B";
    static const QString C_BLANC        = "#FFFFFF";
    static const QString C_BORDURE      = "#E8DDD5";

    QSqlDatabase db = Connection::instance()->getDatabase();
    if (!db.isOpen()) { QMessageBox::warning(this,"Erreur","Base de donn�es non connect�e."); return; }

    QSqlQuery query(db);
    query.exec("SELECT COUNT(*), NVL(SUM(MONTANT),0) FROM COMMANDES");
    int total = 0; double montantTotal = 0;
    if (query.next()) { total = query.value(0).toInt(); montantTotal = query.value(1).toDouble(); }
    if (total == 0) { showInfo(this,"Statistiques","Aucune commande dans la base de donn�es."); return; }

    QMap<QString,int> statCnt, prioCnt;
    query.exec("SELECT STATUT, COUNT(*) FROM COMMANDES GROUP BY STATUT");
    while (query.next()) statCnt[query.value(0).toString()] = query.value(1).toInt();
    query.exec("SELECT PRIORITE, COUNT(*) FROM COMMANDES GROUP BY PRIORITE");
    while (query.next()) prioCnt[query.value(0).toString()] = query.value(1).toInt();

    int enAttente  = statCnt.value("En Attente", 0);
    int terminees  = statCnt.value("Termin�", 0);
    double taux    = total > 0 ? 100.0 * terminees / total : 0;
    double moyenne = total > 0 ? montantTotal / total : 0;

    // ?? Dialogue ?????????????????????????????????????????????????????????????
    QDialog dlg(this);
    dlg.setWindowTitle("Statistiques de Production");
    dlg.setMinimumSize(1000, 680);
    dlg.setStyleSheet(QString("QDialog{background:%1;border-radius:10px;}").arg(C_CREME));

    QVBoxLayout *dlay = new QVBoxLayout(&dlg);
    dlay->setContentsMargins(0,0,0,0);
    dlay->setSpacing(0);

    // ?? HEADER ???????????????????????????????????????????????????????????????
    QWidget *header = new QWidget();
    header->setStyleSheet(QString("background:%1;").arg(C_BORDEAUX));
    QHBoxLayout *hdrLay = new QHBoxLayout(header);
    hdrLay->setContentsMargins(24,14,24,14);

    QLabel *titleLbl = new QLabel("STATISTIQUES DE PRODUCTION");
    titleLbl->setStyleSheet("font-size:18px;font-weight:bold;color:white;letter-spacing:2px;");

    QLabel *dateLbl = new QLabel(QDate::currentDate().toString("dd/MM/yyyy") + "  |  Tous");
    dateLbl->setStyleSheet(QString("font-size:12px;color:%1;font-weight:bold;").arg(C_OR));

    QPushButton *closeBtn = new QPushButton("?");
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

    // ?? SCROLL AREA ??????????????????????????????????????????????????????????
    QScrollArea *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet(QString("QScrollArea{border:none;background:%1;}").arg(C_CREME));
    QWidget *inner = new QWidget();
    inner->setStyleSheet(QString("background:%1;").arg(C_CREME));
    QVBoxLayout *lay = new QVBoxLayout(inner);
    lay->setSpacing(16);
    lay->setContentsMargins(24,20,24,20);

    // ?? KPI CARDS ????????????????????????????????????????????????????????????
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

    kpiLay->addWidget(makeKPI("??",
        QString::number(total),
        "Total commandes",
        QString::number(enAttente) + " en attente"));
    kpiLay->addWidget(makeKPI("??",
        QLocale(QLocale::French).toString(montantTotal,'f',0) + " DT",
        "Chiffre d'affaires",
        "+12% ce mois"));
    kpiLay->addWidget(makeKPI("??",
        QLocale(QLocale::French).toString(moyenne,'f',0) + " DT",
        "Montant moyen",
        "par commande"));
    kpiLay->addWidget(makeKPI("?",
        QString::number(taux,'f',1) + "%",
        "Taux compl�tion",
        QString::number(terminees) + " / " + QString::number(total) + " termin�es"));
    lay->addWidget(kpiRow);

    // ?? GRAPHIQUES ???????????????????????????????????????????????????????????
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

    // Camembert statuts
    auto *pie = new QPieSeries();
    pie->setHoleSize(0.45);
    QMap<QString,QColor> statClrs;
    statClrs["En Attente"]    = QColor(C_OR);
    statClrs["Suspendu"]      = QColor(C_OR_PALE);
    statClrs["Termin�"]       = QColor(C_BORDEAUX);
    statClrs["En Production"] = QColor(C_BORDEAUX_MID);
    statClrs["Planifi�"]      = QColor("#B8956A");
    statClrs["Annul�"]        = QColor("#7A3545");
    for (auto it = statCnt.begin(); it != statCnt.end(); ++it) {
        auto *sl = pie->append(it.key(), it.value());
        sl->setColor(statClrs.value(it.key(), QColor(C_GRIS)));
        sl->setBorderColor(QColor(C_BLANC));
        QObject::connect(sl, &QPieSlice::hovered, [sl](bool on){ sl->setExploded(on); });
    }
    auto *pc = new QChart();
    pc->addSeries(pie);
    pc->setBackgroundBrush(QColor(C_BLANC));
    pc->setBackgroundRoundness(0);
    pc->setMargins(QMargins(0,0,0,0));
    pc->setAnimationOptions(QChart::AllAnimations);
    // Texte central du donut
    QGraphicsTextItem *centerText = new QGraphicsTextItem(pc);
    centerText->setHtml(QString("<div style='text-align:center;'>"
        "<b style='color:%1;font-size:13px;'>Statuts</b><br/>"
        "<span style='color:%2;font-size:11px;'>%3 types</span></div>")
        .arg(C_BORDEAUX, C_GRIS).arg(statCnt.size()));
    pc->legend()->setAlignment(Qt::AlignBottom);
    pc->legend()->setFont(QFont("Arial", 9));
    pc->legend()->setLabelColor(QColor(C_GRIS));
    auto *pv = new QChartView(pc);
    pv->setRenderHint(QPainter::Antialiasing);
    chartsLay->addWidget(wrapChart(pv, "R�partition des statuts"), 1);

    // Barres priorit�s � 1 set par priorit�, 1 valeur chacun, cat�gories distinctes
    QStringList prioOrder = {"Urgente","Haute","Normale","Basse"};
    QMap<QString,QColor> prioClrs;
    prioClrs["Urgente"] = QColor(C_BORDEAUX);
    prioClrs["Haute"]   = QColor(C_BORDEAUX_MID);
    prioClrs["Normale"] = QColor(C_OR);
    prioClrs["Basse"]   = QColor(C_OR_PALE);

    // Une s�rie par priorit� pr�sente, chacune avec 1 seule valeur
    // et attach�e � sa propre cat�gorie via QBarCategoryAxis
    QStringList cats;
    QList<QBarSet*> sets;
    for (const QString &p : prioOrder) {
        if (!prioCnt.contains(p)) continue;
        cats << p;
        auto *set = new QBarSet(p);
        set->setColor(prioClrs.value(p, QColor(C_GRIS)));
        set->setLabelColor(Qt::white);
        set->setBorderColor(Qt::transparent);
        *set << prioCnt[p];
        sets << set;
    }

    // Utiliser une s�rie par set pour que chaque barre soit dans sa propre cat�gorie
    auto *bc = new QChart();
    bc->setBackgroundBrush(QColor(C_BLANC));
    bc->setBackgroundRoundness(0);
    bc->setMargins(QMargins(0,0,0,0));
    bc->setAnimationOptions(QChart::SeriesAnimations);

    auto *axX = new QBarCategoryAxis();
    axX->append(cats);
    axX->setLabelsColor(QColor(C_GRIS));
    axX->setGridLineVisible(false);
    bc->addAxis(axX, Qt::AlignBottom);

    auto *axY = new QValueAxis();
    axY->setLabelFormat("%d");
    axY->setLabelsColor(QColor(C_GRIS));
    axY->setGridLineColor(QColor(C_BORDURE));
    bc->addAxis(axY, Qt::AlignLeft);

    // Une QBarSeries par set � chaque s�rie a 1 set avec 1 valeur ? 1 barre par cat�gorie
    for (int i = 0; i < sets.size(); ++i) {
        auto *series = new QBarSeries();
        series->setLabelsVisible(true);
        series->setLabelsPosition(QAbstractBarSeries::LabelsCenter);
        series->setLabelsFormat("@value");
        // Remplir avec 0 pour les autres cat�gories, valeur r�elle pour la sienne
        auto *set = new QBarSet(sets[i]->label());
        set->setColor(sets[i]->color());
        set->setLabelColor(Qt::white);
        set->setBorderColor(Qt::transparent);
        for (int j = 0; j < cats.size(); ++j)
            *set << (j == i ? sets[i]->at(0) : 0);
        series->append(set);
        bc->addSeries(series);
        series->attachAxis(axX);
        series->attachAxis(axY);
    }
    bc->legend()->setAlignment(Qt::AlignBottom);
    bc->legend()->setFont(QFont("Arial", 9));
    bc->legend()->setLabelColor(QColor(C_GRIS));
    auto *bv = new QChartView(bc);
    bv->setRenderHint(QPainter::Antialiasing);
    qDeleteAll(sets); // lib�rer les sets temporaires (les copies ont �t� faites dans chaque s�rie)
    chartsLay->addWidget(wrapChart(bv, "R�partition des priorit�s"), 1);

    lay->addWidget(chartsRow);
    scroll->setWidget(inner);
    dlay->addWidget(scroll, 1);

    // ?? FOOTER BOUTONS ???????????????????????????????????????????????????????
    QWidget *footer = new QWidget();
    footer->setStyleSheet(QString(
        "QWidget{background:%1;border-top:0.5px solid %2;}").arg(C_CREME, C_BORDURE));
    QHBoxLayout *btnRow = new QHBoxLayout(footer);
    btnRow->setContentsMargins(24,10,24,10);
    btnRow->setSpacing(10);

    QPushButton *exportBtn = new QPushButton("?  Exporter CSV");
    exportBtn->setStyleSheet(QString(
        "QPushButton{background:%1;color:white;border:none;border-radius:6px;"
        "padding:9px 22px;font-size:13px;font-weight:bold;}"
        "QPushButton:hover{background:#A87730;}").arg(C_OR));

    QPushButton *fermerBtn = new QPushButton("?  Fermer");
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

// ?? Articles ??????????????????????????????????????????????????????????????????
void MainWindow::setupArticleTable()
{
    ui->articleTable->setColumnCount(11);
    ui->articleTable->setHorizontalHeaderLabels({
        "ID","R�f�rence","Nom","Cat�gorie","Type","Couleur",
        "Dimensions","Prix Unitaire","Co�t Fabrication","Statut","Date Cr�ation"
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
    
    // Charger les articles depuis la base de donn�es
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
        ui->articleTable->setItem(i, 7, new QTableWidgetItem(QString::number(a.getPrixUnitaire(), 'f', 2) + " �"));
        ui->articleTable->setItem(i, 8, new QTableWidgetItem(QString::number(a.getCoutFabrication(), 'f', 2) + " �"));
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
                       "","Sacs","Sac � main","sac_a_main.obj",
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
        
        // Ajouter � la base de donn�es
        if (article.ajouter()) {
            QMessageBox::information(this, "Succ�s", "Article ajout� avec succ�s !");
            refreshArticleTable(); // Rafra�chir l'affichage
        } else {
            QMessageBox::critical(this, "Erreur", 
                "Impossible d'ajouter l'article.\n\n"
                "V�rifiez que:\n"
                "1. La table ARTICLES existe dans la base de donn�es\n"
                "2. La connexion � la base de donn�es est active\n"
                "3. La r�f�rence n'existe pas d�j�\n\n"
                "Consultez 'Application Output' pour plus de d�tails.");
        }
    }
}

void MainWindow::on_btnEditArticle_clicked()
{
    int row = ui->articleTable->currentRow();
    if (row < 0) { QMessageBox::warning(this,"Attention","S�lectionnez un article."); return; }
    
    // R�cup�rer l'ID de l'article s�lectionn�
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
        
        // Modifier dans la base de donn�es
        if (article.modifier()) {
            QMessageBox::information(this, "Succ�s", "Article modifi� avec succ�s !");
            refreshArticleTable(); // Rafra�chir l'affichage
        } else {
            QMessageBox::critical(this, "Erreur", "Impossible de modifier l'article.");
        }
    }
}

void MainWindow::on_btnDeleteArticle_clicked()
{
    int row = ui->articleTable->currentRow();
    if (row < 0) { QMessageBox::warning(this,"Attention","S�lectionnez un article."); return; }
    
    // R�cup�rer l'ID de l'article s�lectionn�
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
        // Supprimer de la base de donn�es
        if (article.supprimer()) {
            QMessageBox::information(this, "Succ�s", "Article supprim� avec succ�s !");
            refreshArticleTable(); // Rafra�chir l'affichage
        } else {
            QMessageBox::critical(this, "Erreur", "Impossible de supprimer l'article.");
        }
    }
}

void MainWindow::on_btnViewArticle_clicked()
{
    int row = ui->articleTable->currentRow();
    if (row < 0) { QMessageBox::warning(this,"","S�lectionnez un article."); return; }
    
    int idArticle = ui->articleTable->item(row, 0)->text().toInt();
    Article article = Article::rechercherParId(idArticle);
    if (article.getIdArticle() == 0) { QMessageBox::warning(this,"","Article introuvable."); return; }
    
    ArticleDialog dlg(this, ArticleDialog::ViewMode);
    dlg.setArticleData(article.getReference(), article.getNom(), article.getCategorie(),
                       article.getType(), article.getModele3D(),
                       article.getCouleurR(), article.getCouleurG(), article.getCouleurB(),
                       article.getLargeur(), article.getHauteur(), article.getProfondeur(),
                       article.getPrixUnitaire(), article.getCoutFabrication(), article.getStatut());
    dlg.exec();
}

void MainWindow::on_btnView3DArticle_clicked()
{
    int row = ui->articleTable->currentRow();
    if (row < 0) { QMessageBox::warning(this,"","S�lectionnez un article pour la vue 3D."); return; }

    QString nom     = ui->articleTable->item(row, 2)->text();
    QString type    = ui->articleTable->item(row, 4)->text();
    QString couleur = ui->articleTable->item(row, 5)->text();

    QDialog dlg(this);
    dlg.setWindowTitle("?? Vue 3D - " + nom);
    dlg.setMinimumSize(900, 600);
    dlg.showMaximized();
    dlg.setStyleSheet("QDialog{background:#1A1A2E;}");

    QVBoxLayout *lay = new QVBoxLayout(&dlg);
    lay->setContentsMargins(8,8,8,8);

    auto *title = new QLabel(QString("??  VISUALISATION 3D  |  %1  �  %2").arg(nom, type));
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size:14px;font-weight:bold;color:#FFCC80;padding:8px;"
                         "background:#0F3460;border-radius:10px;border:1px solid #8D6E63;");
    lay->addWidget(title);

    auto *viewer = new ArticleViewer3D(&dlg);
    viewer->loadModelForType(type);

    // Appliquer la couleur RGB depuis la BD
    int row2 = ui->articleTable->currentRow();
    if (row2 >= 0 && row2 < articles.size()) {
        const Article &art = articles[row2];
        viewer->setColor(art.getCouleurR(), art.getCouleurG(), art.getCouleurB());
        viewer->setDimensions(art.getLargeur(), art.getHauteur(), art.getProfondeur());
    }

    lay->addWidget(viewer);

    auto *btnClose = new QPushButton("?  Fermer");
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
    
    // Recherche par nom (col 2) ou r�f�rence (col 1) via la classe Article
    QList<Article> resultats;
    if (text.trimmed().length() >= 2) {
        // Recherche dans la BD via requ�tes pr�par�es
        QList<Article> parRef = Article::rechercherParReference(text.trimmed());
        QList<Article> parNom = Article::rechercherParNom(text.trimmed());
        
        // Fusionner sans doublons
        QSet<int> ids;
        for (const Article &a : parRef) { resultats.append(a); ids.insert(a.getIdArticle()); }
        for (const Article &a : parNom) { if (!ids.contains(a.getIdArticle())) resultats.append(a); }
        
        // Filtrer la table selon les r�sultats
        QSet<int> resultIds;
        for (const Article &a : resultats) resultIds.insert(a.getIdArticle());
        
        for (int i = 0; i < ui->articleTable->rowCount(); ++i) {
            int id = ui->articleTable->item(i, 0)->text().toInt();
            ui->articleTable->setRowHidden(i, !resultIds.contains(id));
        }
    } else {
        // Recherche locale simple pour 1 caract�re
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
    QMenu *menuPrix = menu.addMenu("?? Prix");
    menuPrix->setStyleSheet(menu.styleSheet());
    auto *prixCroissant = menuPrix->addAction("? Prix Croissant");
    auto *prixDecroissant = menuPrix->addAction("? Prix D�croissant");
    connect(prixCroissant, &QAction::triggered, [=]{ ui->articleTable->sortItems(7, Qt::AscendingOrder); });
    connect(prixDecroissant, &QAction::triggered, [=]{ ui->articleTable->sortItems(7, Qt::DescendingOrder); });
    
    menu.addSeparator();
    
    // Cat�gorie
    QMenu *menuCategorie = menu.addMenu("?? Cat�gorie");
    menuCategorie->setStyleSheet(menu.styleSheet());
    auto *catAZ = menuCategorie->addAction("? A ? Z");
    auto *catZA = menuCategorie->addAction("? Z ? A");
    connect(catAZ, &QAction::triggered, [=]{ ui->articleTable->sortItems(3, Qt::AscendingOrder); });
    connect(catZA, &QAction::triggered, [=]{ ui->articleTable->sortItems(3, Qt::DescendingOrder); });
    
    menu.addSeparator();
    
    // Statut
    QMenu *menuStatut = menu.addMenu("?? Statut");
    menuStatut->setStyleSheet(menu.styleSheet());
    auto *statAZ = menuStatut->addAction("? A ? Z");
    auto *statZA = menuStatut->addAction("? Z ? A");
    connect(statAZ, &QAction::triggered, [=]{ ui->articleTable->sortItems(9, Qt::AscendingOrder); });
    connect(statZA, &QAction::triggered, [=]{ ui->articleTable->sortItems(9, Qt::DescendingOrder); });

    QPoint pos = ui->btnTriArticle->mapToGlobal(QPoint(0, ui->btnTriArticle->height()));
    menu.exec(pos);
}

void MainWindow::on_btnStatistiquesArticle_clicked()
{
    if (articles.isEmpty()) { QMessageBox::information(this,"","Aucun article � analyser."); return; }

    QDialog *dlg = new QDialog(this);
    dlg->setWindowTitle("?? Tableau de Bord Statistiques - CUIREA");
    dlg->setMinimumSize(1150, 780);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->setStyleSheet(
        "QDialog{background:#1A1A2E;}"
        "QLabel{color:#E0E0E0;}"
        "QGroupBox{border:2px solid #8D6E63;border-radius:10px;margin-top:10px;"
        "padding-top:10px;font-weight:bold;color:#FFCC80;font-size:11px;}"
        "QPushButton{background:#8D6E63;color:white;border:none;border-radius:8px;"
        "padding:8px 20px;font-weight:bold;}"
        "QPushButton:hover{background:#A0826D;}"
    );

    QVBoxLayout *mainLay = new QVBoxLayout(dlg);
    mainLay->setContentsMargins(12,12,12,10); mainLay->setSpacing(8);

    // Titre
    auto *titleLbl = new QLabel("??  TABLEAU DE BORD STATISTIQUES ARTICLES  |  CUIREA Smart Factory");
    titleLbl->setAlignment(Qt::AlignCenter);
    titleLbl->setStyleSheet("font-size:14px;font-weight:bold;color:#FFCC80;padding:8px;"
                            "background:#0F3460;border-radius:10px;border:1px solid #8D6E63;");
    mainLay->addWidget(titleLbl);

    // ?? KPI Cards ??????????????????????????????????????????????????????????
    int total=articles.size(), dispo=0, enProd=0, obs=0;
    double totalPrix=0, totalCout=0;
    QMap<QString,int> parCat;
    QMap<QString,double> margeCat;

    for (const Article &a : articles) {
        if (a.getStatut()=="disponible")    ++dispo;
        else if (a.getStatut()=="en_production") ++enProd;
        else ++obs;
        totalPrix += a.getPrixUnitaire();
        totalCout += a.getCoutFabrication();
        parCat[a.getCategorie()]++;
        margeCat[a.getCategorie()] += (a.getPrixUnitaire()-a.getCoutFabrication());
    }
    double margeGlobale = totalCout>0 ? ((totalPrix-totalCout)/totalCout)*100 : 0;

    auto makeKPI = [](const QString &val, const QString &label, const QString &color) {
        auto *w = new QWidget();
        w->setStyleSheet(QString("background:%1;border-radius:10px;").arg(color));
        auto *l = new QVBoxLayout(w);
        auto *v = new QLabel(val); v->setAlignment(Qt::AlignCenter);
        v->setStyleSheet("font-size:22px;font-weight:bold;color:white;");
        auto *lb = new QLabel(label); lb->setAlignment(Qt::AlignCenter);
        lb->setStyleSheet("font-size:10px;color:rgba(255,255,255,0.8);");
        l->addWidget(v); l->addWidget(lb);
        w->setMinimumHeight(70);
        return w;
    };

    QHBoxLayout *kpiLay = new QHBoxLayout();
    kpiLay->addWidget(makeKPI(QString::number(total), "Total Articles", "#0F3460"));
    kpiLay->addWidget(makeKPI(QString::number(dispo), "Disponibles", "#1B5E20"));
    kpiLay->addWidget(makeKPI(QString::number(enProd), "En Production", "#E65100"));
    kpiLay->addWidget(makeKPI(QString::number(obs), "Obsol�tes", "#B71C1C"));
    kpiLay->addWidget(makeKPI(QString::number(margeGlobale,'f',1)+"%", "Marge Globale", "#4A148C"));
    kpiLay->addWidget(makeKPI(QString::number(totalPrix/qMax(total,1),'f',0)+" DT", "Prix Moyen", "#006064"));
    mainLay->addLayout(kpiLay);

    // ?? Ligne 1 : Camembert statut + Barres cat�gorie ??????????????????????
    QHBoxLayout *row1 = new QHBoxLayout(); row1->setSpacing(8);

    // Camembert statut
    {
        QGroupBox *gb = new QGroupBox("  ??  R�partition par Statut");
        QVBoxLayout *l = new QVBoxLayout(gb);
        auto *pie = new QPieSeries();
        pie->append("Disponible", dispo)->setBrush(QColor("#4CAF50"));
        pie->append("En Production", enProd)->setBrush(QColor("#FF9800"));
        if (obs>0) pie->append("Obsol�te", obs)->setBrush(QColor("#F44336"));
        for (auto *sl : pie->slices()) {
            sl->setLabelVisible(true);
            sl->setLabelColor(Qt::white);
            sl->setLabel(QString("%1\n%2%").arg(sl->label())
                         .arg(QString::number(sl->percentage()*100,'f',1)));
        }
        auto *chart = new QChart(); chart->addSeries(pie);
        chart->setBackgroundBrush(QBrush(QColor("#16213E")));
        chart->setTitleBrush(QBrush(Qt::white));
        chart->legend()->setLabelColor(Qt::white);
        chart->setAnimationOptions(QChart::SeriesAnimations);
        chart->legend()->setAlignment(Qt::AlignBottom);
        auto *cv = new QChartView(chart); cv->setRenderHint(QPainter::Antialiasing);
        cv->setMinimumHeight(260);
        l->addWidget(cv); row1->addWidget(gb);
    }

    // Barres cat�gorie
    {
        QGroupBox *gb = new QGroupBox("  ??  Articles par Cat�gorie");
        QVBoxLayout *l = new QVBoxLayout(gb);
        auto *bs = new QBarSet("Nb Articles"); bs->setColor(QColor("#8D6E63"));
        QStringList cats;
        for (auto it=parCat.begin(); it!=parCat.end(); ++it) {
            cats << it.key(); *bs << it.value();
        }
        auto *series = new QBarSeries(); series->append(bs);
        auto *chart = new QChart(); chart->addSeries(series);
        chart->setBackgroundBrush(QBrush(QColor("#16213E")));
        chart->setTitleBrush(QBrush(Qt::white));
        chart->legend()->setVisible(false);
        chart->setAnimationOptions(QChart::SeriesAnimations);
        auto *axX = new QBarCategoryAxis(); axX->append(cats);
        axX->setLabelsColor(Qt::white);
        chart->addAxis(axX,Qt::AlignBottom); series->attachAxis(axX);
        auto *axY = new QValueAxis(); axY->setLabelFormat("%d");
        axY->setLabelsColor(Qt::white);
        chart->addAxis(axY,Qt::AlignLeft); series->attachAxis(axY);
        auto *cv = new QChartView(chart); cv->setRenderHint(QPainter::Antialiasing);
        cv->setMinimumHeight(260);
        l->addWidget(cv); row1->addWidget(gb);
    }

    // Barres marge par cat�gorie
    {
        QGroupBox *gb = new QGroupBox("  ??  Marge Totale par Cat�gorie (DT)");
        QVBoxLayout *l = new QVBoxLayout(gb);
        auto *bs = new QBarSet("Marge DT"); bs->setColor(QColor("#FFCC80"));
        QStringList cats;
        for (auto it=margeCat.begin(); it!=margeCat.end(); ++it) {
            cats << it.key(); *bs << it.value();
        }
        auto *series = new QBarSeries(); series->append(bs);
        auto *chart = new QChart(); chart->addSeries(series);
        chart->setBackgroundBrush(QBrush(QColor("#16213E")));
        chart->legend()->setVisible(false);
        chart->setAnimationOptions(QChart::SeriesAnimations);
        auto *axX = new QBarCategoryAxis(); axX->append(cats);
        axX->setLabelsColor(Qt::white);
        chart->addAxis(axX,Qt::AlignBottom); series->attachAxis(axX);
        auto *axY = new QValueAxis(); axY->setLabelFormat("%.0f");
        axY->setLabelsColor(Qt::white);
        chart->addAxis(axY,Qt::AlignLeft); series->attachAxis(axY);
        auto *cv = new QChartView(chart); cv->setRenderHint(QPainter::Antialiasing);
        cv->setMinimumHeight(260);
        l->addWidget(cv); row1->addWidget(gb);
    }

    mainLay->addLayout(row1);

    // ?? Ligne 2 : Courbe prix vs co�t + Scatter rentabilit� ???????????????
    QHBoxLayout *row2 = new QHBoxLayout(); row2->setSpacing(8);

    // Courbe prix vs co�t
    {
        QGroupBox *gb = new QGroupBox("  ??  Evolution Prix Unitaire vs Co�t Fabrication");
        QVBoxLayout *l = new QVBoxLayout(gb);
        auto *sPrix = new QLineSeries(); sPrix->setName("Prix Unitaire");
        sPrix->setColor(QColor("#FFCC80")); sPrix->setPen(QPen(QColor("#FFCC80"),2));
        auto *sCout = new QLineSeries(); sCout->setName("Co�t Fabrication");
        sCout->setColor(QColor("#EF9A9A")); sCout->setPen(QPen(QColor("#EF9A9A"),2));
        for (int i=0; i<articles.size(); ++i) {
            sPrix->append(i, articles[i].getPrixUnitaire());
            sCout->append(i, articles[i].getCoutFabrication());
        }
        auto *chart = new QChart();
        chart->addSeries(sPrix); chart->addSeries(sCout);
        chart->setBackgroundBrush(QBrush(QColor("#16213E")));
        chart->legend()->setLabelColor(Qt::white);
        chart->setAnimationOptions(QChart::SeriesAnimations);
        chart->createDefaultAxes();
        chart->axes(Qt::Horizontal).first()->setLabelsColor(Qt::white);
        chart->axes(Qt::Vertical).first()->setLabelsColor(Qt::white);
        auto *cv = new QChartView(chart); cv->setRenderHint(QPainter::Antialiasing);
        cv->setMinimumHeight(220);
        l->addWidget(cv); row2->addWidget(gb,3);
    }

    // Camembert rentabilit�
    {
        QGroupBox *gb = new QGroupBox("  ??  R�partition Rentabilit�");
        QVBoxLayout *l = new QVBoxLayout(gb);
        int exc=0,bon=0,moy=0,fai=0;
        for (const Article &a : articles) {
            double mp = a.getCoutFabrication()>0
                ? (a.getPrixUnitaire()-a.getCoutFabrication())/a.getCoutFabrication()*100 : 0;
            if (mp>=50) ++exc; else if (mp>=30) ++bon;
            else if (mp>=15) ++moy; else ++fai;
        }
        auto *pie = new QPieSeries();
        if (exc>0) pie->append("Excellente ?50%",exc)->setBrush(QColor("#1B5E20"));
        if (bon>0) pie->append("Bonne 30-50%",bon)->setBrush(QColor("#4CAF50"));
        if (moy>0) pie->append("Moyenne 15-30%",moy)->setBrush(QColor("#FF9800"));
        if (fai>0) pie->append("Faible <15%",fai)->setBrush(QColor("#F44336"));
        for (auto *sl : pie->slices()) {
            sl->setLabelVisible(true); sl->setLabelColor(Qt::white);
            sl->setLabel(QString("%1\n%2%").arg(sl->label())
                         .arg(QString::number(sl->percentage()*100,'f',0)));
        }
        auto *chart = new QChart(); chart->addSeries(pie);
        chart->setBackgroundBrush(QBrush(QColor("#16213E")));
        chart->legend()->setLabelColor(Qt::white);
        chart->legend()->setAlignment(Qt::AlignBottom);
        chart->setAnimationOptions(QChart::SeriesAnimations);
        auto *cv = new QChartView(chart); cv->setRenderHint(QPainter::Antialiasing);
        cv->setMinimumHeight(220);
        l->addWidget(cv); row2->addWidget(gb,2);
    }

    mainLay->addLayout(row2);

    // Bouton fermer
    auto *btnClose = new QPushButton("?  Fermer");
    btnClose->setMaximumWidth(150);
    connect(btnClose, &QPushButton::clicked, dlg, &QDialog::accept);
    QHBoxLayout *bl = new QHBoxLayout();
    bl->addStretch(); bl->addWidget(btnClose);
    mainLay->addLayout(bl);

    dlg->exec();
}

void MainWindow::on_btnExportPdfArticle_clicked()
{
    QString fn = QFileDialog::getSaveFileName(this,"Exporter Catalogue PDF",
                                               QDir::homePath()+"/Catalogue_Articles.pdf","PDF (*.pdf)");
    if (fn.isEmpty()) return;

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fn);
    printer.setPageSize(QPageSize::A4);
    printer.setPageMargins(QMarginsF(15,15,15,15), QPageLayout::Millimeter);

    int dispo=0, enProd=0;
    for (const Article &a : articles) {
        if (a.getStatut()=="disponible")   ++dispo;
        if (a.getStatut()=="en_production") ++enProd;
    }

    QString html = QString(
        "<html><head><style>body{font-family:Arial;color:#291C0E;}"
        "h1{text-align:center;color:#8D6E63;border-bottom:3px solid #8D6E63;padding-bottom:8px;}"
        ".hdr{text-align:center;color:#666;font-size:12px;margin-bottom:15px;}"
        "table{width:100%%;border-collapse:collapse;font-size:10px;}"
        "th{background:#8D6E63;color:white;padding:8px;text-align:left;}"
        "td{padding:6px;border-bottom:1px solid #F0E6DA;}"
        "tr:nth-child(even){background:#FFF8F0;}"
        "</style></head><body>"
        "<h1>CATALOGUE DES ARTICLES</h1>"
        "<div class='hdr'>CUIREA - Export: %1<br>Total: %2 | Disponibles: %3 | En production: %4</div>"
        "<table><thead><tr><th>R�f.</th><th>Nom</th><th>Cat�gorie</th><th>Type</th>"
        "<th>Couleur</th><th>Dimensions</th><th>Prix</th><th>Statut</th></tr></thead><tbody>"
    ).arg(QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm"))
     .arg(articles.size()).arg(dispo).arg(enProd);

    for (const Article &a : articles) {
        QString st = a.getStatut()=="disponible" ? "Disponible"
                   : a.getStatut()=="en_production" ? "En Production" : "Obsol�te";
        html += QString("<tr><td>%1</td><td><b>%2</b></td><td>%3</td><td>%4</td>"
                        "<td>%5</td><td>%6</td><td>%7 �</td><td>%8</td></tr>")
                .arg(a.getReference(),a.getNom(),a.getCategorie(),a.getType(),
                     a.getCouleur(),a.getDimensions(),
                     QString::number(a.getPrixUnitaire(),'f',2), st);
    }
    html += "</tbody></table></body></html>";

    QTextDocument doc; doc.setHtml(html); doc.print(&printer);
    QMessageBox::information(this,"","Catalogue export�: "+fn);
}

void MainWindow::on_btnAnalyseRentabilite_clicked()
{
    if (articles.isEmpty()) { QMessageBox::information(this,"","Aucun article � analyser."); return; }

    QDialog dlg(this);
    dlg.setWindowTitle("?? Analyse de Rentabilit� Avanc�e - CUIREA");
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
    auto *titleLbl = new QLabel("??  ANALYSE DE RENTABILITE AVANCEE  |  CUIREA Smart Factory");
    titleLbl->setAlignment(Qt::AlignCenter);
    titleLbl->setStyleSheet("font-size:14px;font-weight:bold;color:#FFCC80;padding:8px;"
                            "background:#0F3460;border-radius:10px;border:1px solid #8D6E63;");
    mainLay->addWidget(titleLbl);

    // ?? Calculs ????????????????????????????????????????????????????????????
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

    // ?? KPI Cards ??????????????????????????????????????????????????????????
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
    kpiLay->addWidget(makeKPI(QString::number(totalCout/qMax((int)articles.size(),1),'f',0)+" DT","Co�t Moyen","#BF360C"));
    kpiLay->addWidget(makeKPI(QString::number(exc)+" ?","Excellente ?50%","#1B5E20"));
    kpiLay->addWidget(makeKPI(QString::number(fai)+" ?","Faible <15%","#B71C1C"));
    mainLay->addLayout(kpiLay);

    // ?? Corps : tableau + graphiques ???????????????????????????????????????
    QHBoxLayout *bodyLay = new QHBoxLayout(); bodyLay->setSpacing(8);

    // Tableau d�taill�
    QGroupBox *tblBox = new QGroupBox("  ??  D�tail par Article");
    QVBoxLayout *tblLay = new QVBoxLayout(tblBox);
    auto *tbl = new QTableWidget();
    tbl->setColumnCount(7);
    tbl->setHorizontalHeaderLabels({"Nom","Cat�gorie","Co�t (DT)","Prix (DT)","Marge (DT)","Marge (%)","Niveau"});
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
        else            { ren="? N�gative";    col=QColor("#EF9A9A"); }

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

    // Barres group�es : prix vs co�t par cat�gorie
    {
        QGroupBox *gb = new QGroupBox("  ??  Prix vs Co�t par Cat�gorie");
        QVBoxLayout *l = new QVBoxLayout(gb);
        auto *sPrix = new QBarSet("Prix Moyen"); sPrix->setColor(QColor("#FFCC80"));
        auto *sCout = new QBarSet("Co�t Moyen"); sCout->setColor(QColor("#EF9A9A"));
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
        QGroupBox *gb = new QGroupBox("  ??  Courbe Marge % par Article");
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
    auto *btnClose = new QPushButton("?  Fermer");
    btnClose->setMaximumWidth(150);
    connect(btnClose, &QPushButton::clicked, &dlg, &QDialog::accept);
    QHBoxLayout *bl = new QHBoxLayout(); bl->addStretch(); bl->addWidget(btnClose);
    mainLay->addLayout(bl);

    dlg.exec();
}

void MainWindow::on_btnAideDecision_clicked()
{
    QDialog dlg(this);
    dlg.setWindowTitle("?? CUIREA AI Pricing Engine - Syst�me Intelligent de Pr�diction");
    dlg.setMinimumSize(1100, 700);
    dlg.showMaximized();
    dlg.setStyleSheet(
        "QDialog{background:#1A1A2E;color:white;}"
        "QTabWidget::pane{border:2px solid #8D6E63;border-radius:8px;background:#16213E;}"
        "QTabBar::tab{background:#0F3460;color:#BCAAA4;padding:10px 18px;border-radius:6px 6px 0 0;font-weight:bold;}"
        "QTabBar::tab:selected{background:#8D6E63;color:white;}"
        "QGroupBox{border:2px solid #8D6E63;border-radius:10px;margin-top:10px;"
        "padding-top:10px;font-weight:bold;color:#FFCC80;font-size:11px;}"
        "QLabel{color:#E0E0E0;}"
        "QComboBox,QDoubleSpinBox,QSlider{background:#16213E;border:2px solid #8D6E63;"
        "border-radius:6px;padding:6px;color:white;font-size:12px;}"
        "QComboBox::drop-down{border:none;}"
        "QComboBox QAbstractItemView{background:#16213E;color:white;selection-background-color:#8D6E63;}"
        "QTableWidget{background:#16213E;color:white;gridline-color:#333;border:none;}"
        "QHeaderView::section{background:#8D6E63;color:white;padding:6px;font-weight:bold;border:none;}"
        "QTableWidget{alternate-background-color:#1E2A4A;}"
        "QPushButton{background:qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #8D6E63,stop:1 #A0826D);"
        "color:white;border:none;border-radius:8px;padding:10px 20px;font-weight:bold;font-size:12px;}"
        "QPushButton:hover{background:qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #A0826D,stop:1 #BCAAA4);}"
        "QSlider::groove:horizontal{background:#0F3460;height:6px;border-radius:3px;}"
        "QSlider::handle:horizontal{background:#FFCC80;width:16px;height:16px;border-radius:8px;margin:-5px 0;}"
        "QSlider::sub-page:horizontal{background:#8D6E63;border-radius:3px;}"
    );

    QVBoxLayout *mainLay = new QVBoxLayout(&dlg);
    mainLay->setContentsMargins(12,12,12,10); mainLay->setSpacing(8);

    // ?? Titre ??????????????????????????????????????????????????????????????
    auto *titleLbl = new QLabel("??  CUIREA AI PRICING ENGINE  |  Syst�me Intelligent Multi-Algorithmes");
    titleLbl->setAlignment(Qt::AlignCenter);
    titleLbl->setStyleSheet("font-size:15px;font-weight:bold;color:#FFCC80;padding:10px;"
                            "background:#0F3460;border-radius:10px;border:1px solid #8D6E63;");
    mainLay->addWidget(titleLbl);

    auto *srcLbl = new QLabel("?? BD R�f�rence : Usine LEATHER PRO (50 articles)  |  "
                              "Algorithmes : R�gression Lin�aire + k-NN + Moyenne Pond�r�e  |  "
                              "Fonctions : Pr�diction � Simulation � Segmentation � Alertes � Export PDF");
    srcLbl->setAlignment(Qt::AlignCenter);
    srcLbl->setStyleSheet("font-size:10px;color:#BCAAA4;padding:3px;");
    mainLay->addWidget(srcLbl);

    // ?? Onglets ????????????????????????????????????????????????????????????
    auto *tabs = new QTabWidget();
    mainLay->addWidget(tabs);

    QHBoxLayout *bodyLay = new QHBoxLayout();
    bodyLay->setSpacing(10);

    // ??????????????????????????????????????????????????????????????????????
    // PANNEAU GAUCHE : Formulaire + R�sultats
    // ??????????????????????????????????????????????????????????????????????
    QVBoxLayout *leftLay = new QVBoxLayout();
    leftLay->setSpacing(8);

    // Formulaire
    QGroupBox *formBox = new QGroupBox("  ??  Caract�ristiques de l'Article");
    QFormLayout *formLay = new QFormLayout(formBox);
    formLay->setSpacing(8);

    auto *cbCat = new QComboBox();
    cbCat->addItems({"Sacs","Portefeuilles","Ceintures","Accessoires","Chaussures"});
    auto *cbType = new QComboBox();
    cbType->addItems({"Sac � main","Sac bandouli�re","Tote bag","Sac � dos",
                      "Portefeuille long","Portefeuille compact","Porte-cartes",
                      "Ceinture classique","Ceinture fashion","Ceinture luxe",
                      "Porte-monnaie","Pochette","Bandouli�re","Porte-cl�s"});
    auto *cbCouleur = new QComboBox();
    cbCouleur->addItems({"Noir","Marron","Camel","Cognac","Beige","Blanc",
                         "Rouge","Bleu Marine","Gris","Vert","Autre"});
    auto *spinCout = new QDoubleSpinBox();
    spinCout->setRange(1,99999); spinCout->setDecimals(2);
    spinCout->setSuffix(" DT"); spinCout->setValue(30.0);

    formLay->addRow("Cat�gorie :", cbCat);
    formLay->addRow("Type :", cbType);
    formLay->addRow("Couleur :", cbCouleur);
    formLay->addRow("Co�t Fabrication :", spinCout);
    leftLay->addWidget(formBox);

    // Bouton pr�dire
    auto *btnPredire = new QPushButton("??  LANCER LA PREDICTION");
    btnPredire->setMinimumHeight(45);
    leftLay->addWidget(btnPredire);

    // R�sultat principal
    auto *lblPrix = new QLabel("�");
    lblPrix->setAlignment(Qt::AlignCenter);
    lblPrix->setStyleSheet("font-size:32px;font-weight:bold;color:#FFCC80;"
                           "background:#0F3460;border-radius:10px;padding:15px;"
                           "border:2px solid #8D6E63;");
    lblPrix->setMinimumHeight(80);
    leftLay->addWidget(lblPrix);

    // Intervalle de confiance
    auto *lblInterval = new QLabel("Intervalle de confiance : �");
    lblInterval->setAlignment(Qt::AlignCenter);
    lblInterval->setStyleSheet("font-size:11px;color:#BCAAA4;padding:4px;");
    leftLay->addWidget(lblInterval);

    // D�tail des algorithmes
    QGroupBox *algoBox = new QGroupBox("  ??  D�tail des Algorithmes");
    QVBoxLayout *algoLay = new QVBoxLayout(algoBox);
    auto *lblAlgo = new QLabel("Lancez une pr�diction pour voir le d�tail.");
    lblAlgo->setStyleSheet("color:#BCAAA4;font-size:11px;padding:5px;");
    lblAlgo->setWordWrap(true);
    algoLay->addWidget(lblAlgo);
    leftLay->addWidget(algoBox);

    // Recommandation
    auto *lblReco = new QLabel("�");
    lblReco->setAlignment(Qt::AlignCenter);
    lblReco->setWordWrap(true);
    lblReco->setStyleSheet("font-size:12px;font-weight:bold;color:#A5D6A7;"
                           "background:#1B5E20;border-radius:8px;padding:10px;"
                           "border:1px solid #4CAF50;");
    lblReco->setMinimumHeight(50);
    leftLay->addWidget(lblReco);

    leftLay->addStretch();
    bodyLay->addLayout(leftLay, 4);

    // ??????????????????????????????????????????????????????????????????????
    // PANNEAU DROIT : Tableau de positionnement + graphique
    // ??????????????????????????????????????????????????????????????????????
    QVBoxLayout *rightLay = new QVBoxLayout();
    rightLay->setSpacing(8);

    QGroupBox *tableBox = new QGroupBox("  ??  Positionnement des Articles Existants");
    QVBoxLayout *tableLay = new QVBoxLayout(tableBox);

    auto *tbl = new QTableWidget();
    tbl->setColumnCount(7);
    tbl->setHorizontalHeaderLabels({"Nom","Cat�gorie","Co�t","Prix Actuel",
                                    "Prix Pr�dit","Ecart","Conseil"});
    tbl->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tbl->setAlternatingRowColors(true);
    tbl->verticalHeader()->setVisible(false);
    tbl->setSelectionBehavior(QAbstractItemView::SelectRows);
    tbl->setStyleSheet("QTableWidget{alternate-background-color:#1E2A4A;}");

    // Remplir le tableau
    tbl->setRowCount(articles.size());
    for (int i = 0; i < articles.size(); ++i) {
        const Article &a = articles[i];
        auto res = Article::predirePrixAvance(a.getCategorie(), a.getType(),
                                              a.getCouleur(), a.getCoutFabrication());
        double ecartP = a.getPrixUnitaire() > 0
            ? ((res.prixPredit - a.getPrixUnitaire()) / a.getPrixUnitaire()) * 100.0
            : 0.0;

        QString conseil;
        QColor couleurConseil;
        if (ecartP > 15)       { conseil = "?? Augmenter"; couleurConseil = QColor("#A5D6A7"); }
        else if (ecartP < -15) { conseil = "?? R�duire";   couleurConseil = QColor("#EF9A9A"); }
        else                   { conseil = "? Optimal";   couleurConseil = QColor("#80DEEA"); }

        tbl->setItem(i,0,new QTableWidgetItem(a.getNom()));
        tbl->setItem(i,1,new QTableWidgetItem(a.getCategorie()));
        tbl->setItem(i,2,new QTableWidgetItem(QString::number(a.getCoutFabrication(),'f',2)+" DT"));
        tbl->setItem(i,3,new QTableWidgetItem(QString::number(a.getPrixUnitaire(),'f',2)+" DT"));
        tbl->setItem(i,4,new QTableWidgetItem(QString::number(res.prixPredit,'f',2)+" DT"));
        tbl->setItem(i,5,new QTableWidgetItem(QString::number(ecartP,'f',1)+"%"));
        auto *ci = new QTableWidgetItem(conseil);
        ci->setForeground(QBrush(couleurConseil));
        QFont f=ci->font(); f.setBold(true); ci->setFont(f);
        tbl->setItem(i,6,ci);
    }
    tableLay->addWidget(tbl);
    rightLay->addWidget(tableBox);

    // Graphique barres : prix actuel vs pr�dit
    QGroupBox *chartBox = new QGroupBox("  ??  Comparaison Prix Actuel vs Prix Pr�dit");
    QVBoxLayout *chartLay = new QVBoxLayout(chartBox);

    auto *setActuel = new QBarSet("Prix Actuel");
    auto *setPredit = new QBarSet("Prix Pr�dit");
    setActuel->setColor(QColor("#8D6E63"));
    setPredit->setColor(QColor("#FFCC80"));
    QStringList noms;
    int maxArt = qMin((int)articles.size(), 6);
    for (int i = 0; i < maxArt; ++i) {
        const Article &a = articles[i];
        auto res = Article::predirePrixAvance(a.getCategorie(), a.getType(),
                                              a.getCouleur(), a.getCoutFabrication());
        *setActuel << a.getPrixUnitaire();
        *setPredit << res.prixPredit;
        noms << (a.getNom().length() > 10 ? a.getNom().left(10)+"�" : a.getNom());
    }
    auto *barSeries = new QBarSeries();
    barSeries->append(setActuel);
    barSeries->append(setPredit);
    auto *chart = new QChart();
    chart->addSeries(barSeries);
    chart->setTitle("Prix Actuel vs Prix Pr�dit (DT)");
    chart->setTitleBrush(QBrush(QColor("#FFCC80")));
    chart->setBackgroundBrush(QBrush(QColor("#16213E")));
    chart->legend()->setLabelColor(Qt::white);
    auto *axX = new QBarCategoryAxis(); axX->append(noms);
    axX->setLabelsColor(Qt::white);
    chart->addAxis(axX, Qt::AlignBottom); barSeries->attachAxis(axX);
    auto *axY = new QValueAxis(); axY->setLabelFormat("%.0f");
    axY->setLabelsColor(Qt::white);
    chart->addAxis(axY, Qt::AlignLeft); barSeries->attachAxis(axY);
    chart->setAnimationOptions(QChart::SeriesAnimations);
    auto *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumHeight(200);
    chartLay->addWidget(chartView);
    rightLay->addWidget(chartBox);

    bodyLay->addLayout(rightLay, 6);

    // Onglet 1 : contenu dans le tab
    auto *tab1 = new QWidget();
    tab1->setLayout(bodyLay);
    tabs->addTab(tab1, "??  Pr�diction IA");

    // ??????????????????????????????????????????????????????????????????????
    // ONGLET 2 : SIMULATION WHAT-IF
    // ??????????????????????????????????????????????????????????????????????
    auto *tab2 = new QWidget();
    auto *tab2OuterLay = new QVBoxLayout(tab2);
    auto *scroll2 = new QScrollArea();
    scroll2->setWidgetResizable(true);
    scroll2->setStyleSheet("QScrollArea{border:none;background:transparent;}");
    auto *tab2Inner = new QWidget();
    auto *t2Lay = new QVBoxLayout(tab2Inner);
    t2Lay->setSpacing(10); t2Lay->setContentsMargins(8,8,8,8);

    auto *t2Title = new QLabel("??  SIMULATION WHAT-IF : Impact du Co�t sur le Prix de Vente");
    t2Title->setAlignment(Qt::AlignCenter);
    t2Title->setStyleSheet("font-size:13px;font-weight:bold;color:#FFCC80;padding:8px;"
                           "background:#0F3460;border-radius:8px;");
    t2Lay->addWidget(t2Title);

    QHBoxLayout *simTopLay = new QHBoxLayout();

    // Formulaire simulation
    QGroupBox *simFormBox = new QGroupBox("  ??  Param�tres");
    QFormLayout *simForm = new QFormLayout(simFormBox); simForm->setSpacing(8);
    auto *simCbCat    = new QComboBox(); simCbCat->addItems({"Sacs","Portefeuilles","Ceintures","Accessoires"});
    auto *simCbType   = new QComboBox(); simCbType->addItems({"Sac � main","Sac bandouli�re","Portefeuille long","Ceinture classique","Porte-monnaie","Pochette"});
    auto *simCbCouleur= new QComboBox(); simCbCouleur->addItems({"Noir","Marron","Camel","Beige","Rouge"});
    auto *sliderCout  = new QSlider(Qt::Horizontal);
    sliderCout->setRange(5, 200); sliderCout->setValue(30);
    auto *lblSliderVal = new QLabel("Co�t : 30 DT");
    lblSliderVal->setStyleSheet("color:#FFCC80;font-weight:bold;font-size:13px;");
    simForm->addRow("Cat�gorie :", simCbCat);
    simForm->addRow("Type :", simCbType);
    simForm->addRow("Couleur :", simCbCouleur);
    simForm->addRow("Co�t (DT) :", sliderCout);
    simForm->addRow("", lblSliderVal);
    simTopLay->addWidget(simFormBox, 2);

    // R�sultats temps r�el
    QGroupBox *simResBox = new QGroupBox("  ??  R�sultats Temps R�el");
    QVBoxLayout *simResLay = new QVBoxLayout(simResBox);
    auto *lblSimPrix  = new QLabel("Prix Pr�dit : �");
    lblSimPrix->setStyleSheet("font-size:20px;font-weight:bold;color:#FFCC80;padding:6px;");
    auto *lblSimMarge = new QLabel("Marge : �");
    lblSimMarge->setStyleSheet("font-size:14px;color:#A5D6A7;padding:4px;");
    auto *lblSimSeg   = new QLabel("Segment : �");
    lblSimSeg->setStyleSheet("font-size:13px;color:#80DEEA;padding:4px;");
    auto *lblSimReco  = new QLabel("�");
    lblSimReco->setWordWrap(true);
    lblSimReco->setStyleSheet("font-size:11px;color:#FFE082;padding:6px;"
                              "background:#1A1A2E;border-radius:6px;border:1px solid #8D6E63;");
    simResLay->addWidget(lblSimPrix); simResLay->addWidget(lblSimMarge);
    simResLay->addWidget(lblSimSeg);  simResLay->addWidget(lblSimReco);
    simResLay->addStretch();
    simTopLay->addWidget(simResBox, 2);
    t2Lay->addLayout(simTopLay);

    // Courbe dynamique
    QGroupBox *simChartBox = new QGroupBox("  ??  Courbe Prix Pr�dit vs Co�t (temps r�el)");
    QVBoxLayout *simChartLay = new QVBoxLayout(simChartBox);
    auto *simSeries      = new QLineSeries(); simSeries->setName("Prix Pr�dit");
    simSeries->setColor(QColor("#FFCC80")); simSeries->setPen(QPen(QColor("#FFCC80"),2));
    auto *simSeuilSeries = new QLineSeries(); simSeuilSeries->setName("Seuil Rentabilit� �2");
    simSeuilSeries->setColor(QColor("#4CAF50")); simSeuilSeries->setPen(QPen(QColor("#4CAF50"),1,Qt::DashLine));
    for (int c=5; c<=200; c+=5) {
        auto r = Article::predirePrixAvance("Sacs","Sac � main","Noir",c);
        simSeries->append(c, r.prixPredit);
        simSeuilSeries->append(c, c*2.0);
    }
    auto *simChart = new QChart();
    simChart->addSeries(simSeries); simChart->addSeries(simSeuilSeries);
    simChart->setBackgroundBrush(QBrush(QColor("#16213E")));
    simChart->legend()->setLabelColor(Qt::white);
    simChart->setAnimationOptions(QChart::AllAnimations);
    simChart->createDefaultAxes();
    simChart->axes(Qt::Horizontal).first()->setLabelsColor(Qt::white);
    simChart->axes(Qt::Vertical).first()->setLabelsColor(Qt::white);
    auto *simChartView = new QChartView(simChart);
    simChartView->setRenderHint(QPainter::Antialiasing);
    simChartView->setMinimumHeight(280);
    simChartLay->addWidget(simChartView);
    t2Lay->addWidget(simChartBox);
    scroll2->setWidget(tab2Inner);
    tab2OuterLay->addWidget(scroll2);
    tabs->addTab(tab2, "??  Simulation What-If");

    // ??????????????????????????????????????????????????????????????????????
    // ONGLET 3 : ALERTES INTELLIGENTES
    // ??????????????????????????????????????????????????????????????????????
    auto *tab3 = new QWidget();
    auto *tab3OuterLay = new QVBoxLayout(tab3);
    auto *scroll3 = new QScrollArea();
    scroll3->setWidgetResizable(true);
    scroll3->setStyleSheet("QScrollArea{border:none;background:transparent;}");
    auto *tab3Inner = new QWidget();
    auto *t3Lay = new QVBoxLayout(tab3Inner);
    t3Lay->setSpacing(8); t3Lay->setContentsMargins(8,8,8,8);

    auto *t3Title = new QLabel("??  ALERTES INTELLIGENTES : D�tection des Articles Mal Evalu�s");
    t3Title->setAlignment(Qt::AlignCenter);
    t3Title->setStyleSheet("font-size:13px;font-weight:bold;color:#FFCC80;padding:8px;"
                           "background:#0F3460;border-radius:8px;");
    t3Lay->addWidget(t3Title);

    // KPI alertes
    int sousEval=0, surEval=0, optimal=0;
    for (const Article &a : articles) {
        auto res = Article::predirePrixAvance(a.getCategorie(),a.getType(),a.getCouleur(),a.getCoutFabrication());
        double ecartP = a.getPrixUnitaire()>0 ? ((res.prixPredit-a.getPrixUnitaire())/a.getPrixUnitaire())*100 : 0;
        if (ecartP>15) ++sousEval; else if (ecartP<-15) ++surEval; else ++optimal;
    }
    auto makeAlertKPI = [](const QString &v, const QString &l, const QString &c) {
        auto *w = new QWidget(); w->setStyleSheet(QString("background:%1;border-radius:10px;").arg(c));
        auto *lay = new QVBoxLayout(w);
        auto *vl = new QLabel(v); vl->setAlignment(Qt::AlignCenter);
        vl->setStyleSheet("font-size:26px;font-weight:bold;color:white;");
        auto *ll = new QLabel(l); ll->setAlignment(Qt::AlignCenter);
        ll->setStyleSheet("font-size:10px;color:rgba(255,255,255,0.8);");
        lay->addWidget(vl); lay->addWidget(ll); w->setMinimumHeight(75); return w;
    };
    QHBoxLayout *alertKpiLay = new QHBoxLayout();
    alertKpiLay->addWidget(makeAlertKPI(QString::number(sousEval),"?? Sous-�valu�s\n(Prix trop bas)","#1B5E20"));
    alertKpiLay->addWidget(makeAlertKPI(QString::number(surEval),"?? Sur-�valu�s\n(Prix trop haut)","#B71C1C"));
    alertKpiLay->addWidget(makeAlertKPI(QString::number(optimal),"? Prix Optimaux","#006064"));
    alertKpiLay->addWidget(makeAlertKPI(QString::number(articles.size()),"Total Articles","#4A148C"));
    t3Lay->addLayout(alertKpiLay);

    // Tableau alertes
    QGroupBox *alertTblBox = new QGroupBox("  ??  D�tail des Alertes par Article");
    QVBoxLayout *alertTblLay = new QVBoxLayout(alertTblBox);
    auto *alertTbl = new QTableWidget();
    alertTbl->setColumnCount(7);
    alertTbl->setHorizontalHeaderLabels({"Alerte","Nom","Cat�gorie","Prix Actuel","Prix March�","Ecart %","Action Recommand�e"});
    alertTbl->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    alertTbl->setAlternatingRowColors(true); alertTbl->verticalHeader()->setVisible(false);
    alertTbl->setRowCount(articles.size());
    for (int i=0; i<articles.size(); ++i) {
        const Article &a = articles[i];
        auto res = Article::predirePrixAvance(a.getCategorie(),a.getType(),a.getCouleur(),a.getCoutFabrication());
        double ecartP = a.getPrixUnitaire()>0 ? ((res.prixPredit-a.getPrixUnitaire())/a.getPrixUnitaire())*100 : 0;
        QString alerte, action; QColor col;
        if (ecartP>25)       { alerte="?? CRITIQUE"; action=QString("+%1 DT recommand�").arg(QString::number(res.prixPredit-a.getPrixUnitaire(),'f',0)); col=QColor("#EF9A9A"); }
        else if (ecartP>15)  { alerte="?? ATTENTION"; action=QString("Envisager +%1 DT").arg(QString::number(res.prixPredit-a.getPrixUnitaire(),'f',0)); col=QColor("#FFE082"); }
        else if (ecartP<-25) { alerte="?? CRITIQUE"; action=QString("-%1 DT recommand�").arg(QString::number(a.getPrixUnitaire()-res.prixPredit,'f',0)); col=QColor("#EF9A9A"); }
        else if (ecartP<-15) { alerte="?? ATTENTION"; action=QString("Envisager -%1 DT").arg(QString::number(a.getPrixUnitaire()-res.prixPredit,'f',0)); col=QColor("#FFE082"); }
        else                 { alerte="?? OPTIMAL"; action="Maintenir le prix actuel"; col=QColor("#A5D6A7"); }
        auto *ai = new QTableWidgetItem(alerte); ai->setForeground(QBrush(col));
        QFont f=ai->font(); f.setBold(true); ai->setFont(f);
        alertTbl->setItem(i,0,ai);
        alertTbl->setItem(i,1,new QTableWidgetItem(a.getNom()));
        alertTbl->setItem(i,2,new QTableWidgetItem(a.getCategorie()));
        alertTbl->setItem(i,3,new QTableWidgetItem(QString::number(a.getPrixUnitaire(),'f',2)+" DT"));
        alertTbl->setItem(i,4,new QTableWidgetItem(QString::number(res.prixPredit,'f',2)+" DT"));
        alertTbl->setItem(i,5,new QTableWidgetItem(QString::number(ecartP,'f',1)+"%"));
        alertTbl->setItem(i,6,new QTableWidgetItem(action));
    }
    alertTblLay->addWidget(alertTbl);
    t3Lay->addWidget(alertTblBox);
    scroll3->setWidget(tab3Inner);
    tab3OuterLay->addWidget(scroll3);
    tabs->addTab(tab3, "??  Alertes Intelligentes");

    // ??????????????????????????????????????????????????????????????????????
    // ONGLET 4 : OUTILS AVANCES
    // ??????????????????????????????????????????????????????????????????????
    auto *tab4 = new QWidget();
    auto *tab4OuterLay = new QVBoxLayout(tab4);
    auto *scroll4 = new QScrollArea();
    scroll4->setWidgetResizable(true);
    scroll4->setStyleSheet("QScrollArea{border:none;background:transparent;}");
    auto *tab4Inner = new QWidget();
    auto *t4Lay = new QVBoxLayout(tab4Inner);
    t4Lay->setSpacing(8); t4Lay->setContentsMargins(8,8,8,8);

    auto *t4Title = new QLabel("??  OUTILS AVANCES : Optimisation � Sensibilit� � Matrice � Historique");
    t4Title->setAlignment(Qt::AlignCenter);
    t4Title->setStyleSheet("font-size:13px;font-weight:bold;color:#FFCC80;padding:8px;"
                           "background:#0F3460;border-radius:8px;");
    t4Lay->addWidget(t4Title);

    QHBoxLayout *advTopLay = new QHBoxLayout(); advTopLay->setSpacing(8);

    // ?? Bloc 1 : Optimisation du co�t ?????????????????????????????????????
    QGroupBox *optBox = new QGroupBox("  ??  Optimisation du Co�t");
    QVBoxLayout *optLay = new QVBoxLayout(optBox);
    QFormLayout *optForm = new QFormLayout();
    auto *optCbCat    = new QComboBox(); optCbCat->addItems({"Sacs","Portefeuilles","Ceintures","Accessoires"});
    auto *optCbType   = new QComboBox(); optCbType->addItems({"Sac � main","Sac bandouli�re","Portefeuille long","Ceinture classique","Porte-monnaie"});
    auto *optCbCouleur= new QComboBox(); optCbCouleur->addItems({"Noir","Marron","Camel","Beige","Rouge"});
    auto *optSpinMarge= new QDoubleSpinBox(); optSpinMarge->setRange(10,300); optSpinMarge->setValue(50); optSpinMarge->setSuffix(" %");
    optForm->addRow("Cat�gorie :", optCbCat);
    optForm->addRow("Type :", optCbType);
    optForm->addRow("Couleur :", optCbCouleur);
    optForm->addRow("Marge Objectif :", optSpinMarge);
    optLay->addLayout(optForm);
    auto *btnOpt = new QPushButton("??  Calculer Co�t Max");
    auto *lblOptResult = new QLabel("�");
    lblOptResult->setAlignment(Qt::AlignCenter);
    lblOptResult->setWordWrap(true);
    lblOptResult->setStyleSheet("font-size:13px;font-weight:bold;color:#FFCC80;"
                                "background:#0F3460;border-radius:8px;padding:10px;"
                                "border:1px solid #8D6E63;");
    lblOptResult->setMinimumHeight(80);
    optLay->addWidget(btnOpt); optLay->addWidget(lblOptResult); optLay->addStretch();
    advTopLay->addWidget(optBox, 1);

    // ?? Bloc 2 : Analyse de sensibilit� ???????????????????????????????????
    QGroupBox *sensBox = new QGroupBox("  ??  Analyse de Sensibilit�");
    QVBoxLayout *sensLay = new QVBoxLayout(sensBox);
    QFormLayout *sensForm = new QFormLayout();
    auto *sensCbCat    = new QComboBox(); sensCbCat->addItems({"Sacs","Portefeuilles","Ceintures","Accessoires"});
    auto *sensCbType   = new QComboBox(); sensCbType->addItems({"Sac � main","Sac bandouli�re","Portefeuille long","Ceinture classique","Porte-monnaie"});
    auto *sensCbCouleur= new QComboBox(); sensCbCouleur->addItems({"Noir","Marron","Camel","Beige","Rouge"});
    auto *sensSpinCout = new QDoubleSpinBox(); sensSpinCout->setRange(1,500); sensSpinCout->setValue(30); sensSpinCout->setSuffix(" DT");
    sensForm->addRow("Cat�gorie :", sensCbCat);
    sensForm->addRow("Type :", sensCbType);
    sensForm->addRow("Couleur :", sensCbCouleur);
    sensForm->addRow("Co�t Base :", sensSpinCout);
    sensLay->addLayout(sensForm);
    auto *btnSens = new QPushButton("??  Analyser Sensibilit�");

    // Graphique barres horizontales pour la sensibilit�
    auto *sensBarSet = new QBarSet("Impact sur le Prix (%)");
    sensBarSet->setColor(QColor("#FFCC80"));
    *sensBarSet << 0 << 0 << 0 << 0;
    auto *sensSeries = new QBarSeries(); sensSeries->append(sensBarSet);
    auto *sensChart = new QChart(); sensChart->addSeries(sensSeries);
    sensChart->setBackgroundBrush(QBrush(QColor("#16213E")));
    sensChart->legend()->setVisible(false);
    sensChart->setAnimationOptions(QChart::SeriesAnimations);
    QStringList sensLabels = {"Cat�gorie","Type","Couleur","k-NN"};
    auto *sensAxX = new QBarCategoryAxis(); sensAxX->append(sensLabels); sensAxX->setLabelsColor(Qt::white);
    sensChart->addAxis(sensAxX, Qt::AlignBottom); sensSeries->attachAxis(sensAxX);
    auto *sensAxY = new QValueAxis(); sensAxY->setLabelFormat("%.1f"); sensAxY->setLabelsColor(Qt::white);
    sensChart->addAxis(sensAxY, Qt::AlignLeft); sensSeries->attachAxis(sensAxY);
    auto *sensChartView = new QChartView(sensChart);
    sensChartView->setRenderHint(QPainter::Antialiasing);
    sensChartView->setMinimumHeight(180);
    sensLay->addWidget(btnSens); sensLay->addWidget(sensChartView);
    advTopLay->addWidget(sensBox, 2);
    t4Lay->addLayout(advTopLay);

    // ?? Bloc 3 : Matrice de d�cision ??????????????????????????????????????
    QGroupBox *matBox = new QGroupBox("  ???  Matrice de D�cision : Ratio Prix/Co�t par Cat�gorie � Couleur");
    QVBoxLayout *matLay = new QVBoxLayout(matBox);
    auto *matTbl = new QTableWidget();
    matTbl->setAlternatingRowColors(true); matTbl->verticalHeader()->setVisible(true);
    // Remplir la matrice
    auto matrice = Article::matriceDecision();
    QStringList cats = matrice.keys();
    QSet<QString> couleursSet;
    for (auto &cat : cats) for (auto &col : matrice[cat].keys()) couleursSet.insert(col);
    QStringList couleurs = couleursSet.values();
    matTbl->setRowCount(cats.size()); matTbl->setColumnCount(couleurs.size());
    matTbl->setHorizontalHeaderLabels(couleurs);
    matTbl->setVerticalHeaderLabels(cats);
    matTbl->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    for (int r=0; r<cats.size(); ++r) {
        for (int c=0; c<couleurs.size(); ++c) {
            double ratio = matrice[cats[r]].value(couleurs[c], 0.0);
            auto *item = new QTableWidgetItem(ratio>0 ? QString("�%1").arg(QString::number(ratio,'f',2)) : "�");
            item->setTextAlignment(Qt::AlignCenter);
            if (ratio>=3.0)      item->setBackground(QBrush(QColor("#1B5E20")));
            else if (ratio>=2.5) item->setBackground(QBrush(QColor("#2E7D32")));
            else if (ratio>=2.0) item->setBackground(QBrush(QColor("#F57F17")));
            else if (ratio>0)    item->setBackground(QBrush(QColor("#B71C1C")));
            item->setForeground(QBrush(Qt::white));
            matTbl->setItem(r, c, item);
        }
    }
    matLay->addWidget(matTbl);
    t4Lay->addWidget(matBox);

    // ?? Bloc 4 : Historique des pr�dictions ???????????????????????????????
    QGroupBox *histBox = new QGroupBox("  ??  Historique des Pr�dictions");
    QVBoxLayout *histLay = new QVBoxLayout(histBox);
    auto *histTbl = new QTableWidget();
    histTbl->setColumnCount(7);
    histTbl->setHorizontalHeaderLabels({"Date","Cat�gorie","Type","Couleur","Co�t","Prix Pr�dit","Segment"});
    histTbl->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    histTbl->setAlternatingRowColors(true); histTbl->verticalHeader()->setVisible(false);
    // Charger l'historique depuis la BD
    {
        QSqlQuery hq(Connection::instance()->getDatabase());
        hq.prepare("SELECT TO_CHAR(DATE_PREDICTION,'DD/MM/YYYY HH24:MI'), CATEGORIE, TYPE_ARTICLE, "
                   "COULEUR, COUT_FABRICATION, PRIX_PREDIT, SEGMENT "
                   "FROM HISTORIQUE_PREDICTIONS ORDER BY DATE_PREDICTION DESC FETCH FIRST 20 ROWS ONLY");
        if (hq.exec()) {
            int row=0;
            while (hq.next()) {
                histTbl->insertRow(row);
                for (int c=0; c<7; ++c)
                    histTbl->setItem(row, c, new QTableWidgetItem(hq.value(c).toString()));
                ++row;
            }
        }
        if (histTbl->rowCount()==0) {
            histTbl->insertRow(0);
            histTbl->setItem(0,0,new QTableWidgetItem("Aucune pr�diction sauvegard�e"));
        }
    }
    histLay->addWidget(histTbl);
    t4Lay->addWidget(histBox);
    scroll4->setWidget(tab4Inner);
    tab4OuterLay->addWidget(scroll4);
    tabs->addTab(tab4, "??  Optimisation & Strat�gie");

    // Bouton fermer
    auto *btnClose = new QPushButton("?  Fermer");
    btnClose->setMaximumWidth(150);
    connect(btnClose, &QPushButton::clicked, &dlg, &QDialog::accept);
    QHBoxLayout *bl = new QHBoxLayout();
    bl->addStretch(); bl->addWidget(btnClose);
    mainLay->addLayout(bl);

    // ?? Connexion bouton Pr�dire ??????????????????????????????????????????
    connect(btnPredire, &QPushButton::clicked, [&]() {
        double cout = spinCout->value();
        auto res = Article::predirePrixAvance(cbCat->currentText(),
                                              cbType->currentText(),
                                              cbCouleur->currentText(), cout);

        lblPrix->setText(QString("??  %1 DT").arg(QString::number(res.prixPredit,'f',2)));
        lblInterval->setText(QString("Intervalle de confiance : [ %1 DT  �  %2 DT ]   |   "
                                     "Confiance : %3   |   R�f. utilis�es : %4")
                             .arg(QString::number(res.prixMin,'f',2))
                             .arg(QString::number(res.prixMax,'f',2))
                             .arg(res.niveauConfiance)
                             .arg(res.nbArticlesRef));

        lblAlgo->setText(
            QString("?? R�gression Cat�gorie  ?  %1 DT\n"
                    "?? R�gression Type       ?  %2 DT\n"
                    "?? R�gression Couleur    ?  %3 DT\n"
                    "?? k-NN (3 voisins)      ?  %4 DT\n"
                    "?????????????????????????????????\n"
                    "? Prix Final (pond�r�)  ?  %5 DT\n"
                    "?? Marge estim�e         ?  %6 %")
            .arg(QString::number(res.prixCategorie,'f',2))
            .arg(QString::number(res.prixType,'f',2))
            .arg(QString::number(res.prixCouleur,'f',2))
            .arg(QString::number(res.prixKNN,'f',2))
            .arg(QString::number(res.prixPredit,'f',2))
            .arg(QString::number(res.margeEstimee,'f',1))
        );
        lblAlgo->setStyleSheet("color:#E0E0E0;font-size:11px;padding:5px;"
                               "font-family:'Courier New',monospace;");

        lblReco->setText(res.recommandation);

        // Couleur du r�sultat selon la marge
        QString couleurPrix = res.margeEstimee >= 80 ? "#FFCC80"
                            : res.margeEstimee >= 50 ? "#A5D6A7"
                            : res.margeEstimee >= 30 ? "#FFE082"
                            : "#EF9A9A";
        lblPrix->setStyleSheet(QString("font-size:32px;font-weight:bold;color:%1;"
                                       "background:#0F3460;border-radius:10px;padding:15px;"
                                       "border:2px solid #8D6E63;").arg(couleurPrix));
    });

    // ?? Connexion slider simulation (onglet 2) ????????????????????????????
    auto updateSim = [&]() {
        double cout = sliderCout->value();
        lblSliderVal->setText(QString("Co�t : %1 DT").arg(cout));
        auto res = Article::predirePrixAvance(simCbCat->currentText(), simCbType->currentText(),
                                              simCbCouleur->currentText(), cout);
        lblSimPrix->setText(QString("Prix Pr�dit : %1 DT").arg(QString::number(res.prixPredit,'f',2)));
        lblSimMarge->setText(QString("Marge : %1 DT  (%2%)")
                             .arg(QString::number(res.prixPredit-cout,'f',2))
                             .arg(QString::number(res.margeEstimee,'f',1)));
        QString seg;
        if (res.margeEstimee>=150) seg="?? LUXE";
        else if (res.margeEstimee>=80) seg="? PREMIUM";
        else if (res.margeEstimee>=40) seg="? STANDARD";
        else seg="? ENTREE DE GAMME";
        lblSimSeg->setText(QString("Segment : %1").arg(seg));
        lblSimReco->setText(res.recommandation);
        // Mettre � jour la courbe
        simSeries->clear(); simSeuilSeries->clear();
        for (int c=5; c<=200; c+=5) {
            auto r = Article::predirePrixAvance(simCbCat->currentText(), simCbType->currentText(),
                                                simCbCouleur->currentText(), c);
            simSeries->append(c, r.prixPredit);
            simSeuilSeries->append(c, c*2.0);
        }
    };
    connect(sliderCout,    &QSlider::valueChanged,                              [&](int){ updateSim(); });
    connect(simCbCat,      QOverload<int>::of(&QComboBox::currentIndexChanged), [&](int){ updateSim(); });
    connect(simCbType,     QOverload<int>::of(&QComboBox::currentIndexChanged), [&](int){ updateSim(); });
    connect(simCbCouleur,  QOverload<int>::of(&QComboBox::currentIndexChanged), [&](int){ updateSim(); });
    // ?? Connexion bouton Optimisation (onglet 4) ??????????????????????????
    connect(btnOpt, &QPushButton::clicked, [&]() {
        double margeObj = optSpinMarge->value();
        double coutMax = Article::optimiserCout(optCbCat->currentText(), optCbType->currentText(),
                                                 optCbCouleur->currentText(), margeObj);
        auto res = Article::predirePrixAvance(optCbCat->currentText(), optCbType->currentText(),
                                              optCbCouleur->currentText(), coutMax);
        lblOptResult->setText(
            QString("? Co�t Maximum : %1 DT\n"
                    "?? Prix de Vente : %2 DT\n"
                    "?? Marge Obtenue : %3%")
            .arg(QString::number(coutMax,'f',2))
            .arg(QString::number(res.prixPredit,'f',2))
            .arg(QString::number(res.margeEstimee,'f',1))
        );
    });
    // ?? Connexion bouton Sensibilit� (onglet 4)
    connect(btnSens, &QPushButton::clicked, [&]() {
        double cout = sensSpinCout->value();
        auto base = Article::predirePrixAvance(sensCbCat->currentText(), sensCbType->currentText(),
                                               sensCbCouleur->currentText(), cout);
        // Impact de chaque variable : variation de �1 cat�gorie/type/couleur
        double impactCat  = qAbs(base.prixCategorie - base.prixPredit) / base.prixPredit * 100;
        double impactType = qAbs(base.prixType      - base.prixPredit) / base.prixPredit * 100;
        double impactCol  = qAbs(base.prixCouleur   - base.prixPredit) / base.prixPredit * 100;
        double impactKNN  = qAbs(base.prixKNN       - base.prixPredit) / base.prixPredit * 100;
        sensBarSet->replace(0, impactCat);
        sensBarSet->replace(1, impactType);
        sensBarSet->replace(2, impactCol);
        sensBarSet->replace(3, impactKNN);
        double maxImpact = std::max({impactCat, impactType, impactCol, impactKNN});
        sensAxY->setRange(0, maxImpact * 1.2 + 1);
    });
    //Sauvegarde automatique lors d'une pr�diction (onglet 1)
    // On reconnecte btnPredire pour aussi sauvegarder
    connect(btnPredire, &QPushButton::clicked, [&]() {
        auto res = Article::predirePrixAvance(cbCat->currentText(), cbType->currentText(),
                                              cbCouleur->currentText(), spinCout->value());
        Article::sauvegarderPrediction(res, cbCat->currentText(), cbType->currentText(),
                                        cbCouleur->currentText(), spinCout->value());
    });
    dlg.exec();
}
// Voice Recognition (SAPI)
#ifdef Q_OS_WIN
void MainWindow::initSAPI()
{
    // Utiliser le recognizer partag� Windows (d�j� configur� avec le micro syst�me)
    if (FAILED(CoCreateInstance(CLSID_SpSharedRecognizer, nullptr, CLSCTX_LOCAL_SERVER,
                                IID_ISpRecognizer, (void**)&spRecognizer))) {
        QMessageBox::critical(this, "Vocal", "Impossible d'initialiser SAPI.\nV�rifiez que la reconnaissance vocale Windows est activ�e.");
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


// ??????????????????????????????????????????????????????????????????????????????
// METHODES DE GESTION DES PERMISSIONS ET AUTHENTIFICATION
// ??????????????????????????????????????????????????????????????????????????????

void MainWindow::applyUserPermissions()
{
    UserSession &session = UserSession::instance();
    
    if (!session.isLoggedIn()) {
        return;
    }
    
    // Masquer/Afficher les onglets selon les permissions
    hideTabsBasedOnPermissions();
    
    // Activer/D�sactiver les boutons CRUD
    updateCRUDButtons();
    
    // Personnaliser le menu - DESACTIVE
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
    
    // Rediriger vers la premi�re page accessible
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
    
    // Boutons Employ�s
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
    
    // Boutons Mati�res
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
    
    // Style pour le menu d�roulant
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
    
    QAction *profileAction = userMenu->addAction("? Mon Profil");
    connect(profileAction, &QAction::triggered, this, &MainWindow::showUserProfile);
    
    QAction *changePasswordAction = userMenu->addAction("?? Changer le mot de passe");
    connect(changePasswordAction, &QAction::triggered, this, &MainWindow::changePassword);
    
    userMenu->addSeparator();
    
    QAction *logoutAction = userMenu->addAction("?? D�connexion");
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
        "<tr><td><b>Role systéme:</b></td><td><span style='color:#8D6E63;font-weight:bold;'>%6</span></td></tr>"
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
                            "Fonctionnalit� � venir.\n"
                            "Contactez l'administrateur pour changer votre mot de passe.");
}

void MainWindow::logout()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("D�connexion");
    msgBox.setText("Etes-vous s�r de vouloir vous d�connecter?");
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
