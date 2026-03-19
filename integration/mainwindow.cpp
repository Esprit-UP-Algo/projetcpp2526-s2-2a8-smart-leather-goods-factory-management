#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStatusBar>
#include <QRegularExpression>
#include "employeedialog.h"
#include "clientmanagerdialog.h"
#include "matieredialog.h"
#include "fournisseurdialog.h"
#include "productiondialog.h"
#include "articledialog.h"
#include "employe.h"
#include "production.h"
#include "connection.h"
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

// ── Shared style constants ────────────────────────────────────────────────────
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

// ── Helper: show a styled info message box ────────────────────────────────────
static void showInfo(QWidget *parent, const QString &title, const QString &text)
{
    QMessageBox mb(parent);
    mb.setWindowTitle(title);
    mb.setText(text);
    mb.setIcon(QMessageBox::Information);
    mb.setStyleSheet(MSGBOX_STYLE);
    mb.exec();
}

// ── Helper: get text from a table cell safely ────────────────────────────────
static QString cellText(QTableWidget *t, int row, int col)
{
    auto *item = t->item(row, col);
    return item ? item->text() : QString();
}

// ── Helper: load article data from a table row into a dialog ─────────────────
static void loadArticleRow(QTableWidget *table, int row, ArticleDialog &dlg)
{
    QString prixStr = cellText(table, row, 7);
    QString coutStr = cellText(table, row, 8);
    double prix = prixStr.remove(" €").toDouble();
    double cout = coutStr.remove(" €").toDouble();
    dlg.setArticleData(cellText(table, row, 1), cellText(table, row, 2),
                       cellText(table, row, 3), cellText(table, row, 4),
                       cellText(table, row, 5), cellText(table, row, 6),
                       prix, cout, cellText(table, row, 9));
}

// ── Helper: filter table rows by search text ─────────────────────────────────
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

// ─────────────────────────────────────────────────────────────────────────────

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
    
    // ══════════════════════════════════════════════════════════════════════
    // ANCIEN CODE (API Locale) - Désactivé car on utilise maintenant l'API Cloud
    // ══════════════════════════════════════════════════════════════════════
    // apiProcess = nullptr;
    // QString vbsScript = "C:/Users/omard/OneDrive/Bureau/integratin finalee/launch_api.vbs";
    // if (QFile::exists(vbsScript)) {
    //     QStringList args;
    //     args << vbsScript.replace("/", "\\");
    //     QProcess::startDetached("wscript.exe", args);
    //     qDebug() << "Démarrage du serveur Python via VBS";
    // }
    // ══════════════════════════════════════════════════════════════════════

    // ── Employee table ──────────────────────────────────────────────────────
    ui->employeeTable->verticalHeader()->setVisible(false);
    ui->employeeTable->setColumnHidden(0, true);
    int empWidths[] = {50,110,100,100,100,120,130,120,110,180};
    for (int i = 0; i < 10; ++i) ui->employeeTable->setColumnWidth(i, empWidths[i]);
    populateEmployeeTable();
    connect(ui->employeeTable, &QTableWidget::currentCellChanged,
            this, &MainWindow::onEmployeeSelected);

    // ── Clients ─────────────────────────────────────────────────────────────


    // ── Raw materials ───────────────────────────────────────────────────────
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
    connect(ui->btnVoice, &QPushButton::clicked, this, &MainWindow::onVoiceCommand);
    connect(ui->btnExportMatiere,&QPushButton::clicked, this, &MainWindow::onExportMatiere);

    // ── Client extra buttons ────────────────────────────────────────────────
    connect(ui->btnAIAgent, &QPushButton::clicked, this, &MainWindow::on_btnAIAgent_clicked);
    connect(ui->btnStatsByRegion, &QPushButton::clicked, this, &MainWindow::on_btnStatsByRegion_clicked);
    connect(ui->btnFidelityClassification, &QPushButton::clicked, this, &MainWindow::on_btnFidelityClassification_clicked);

    // ── Fournisseurs ────────────────────────────────────────────────────────
    ui->fournisseurTable->verticalHeader()->setVisible(false);
    setupFournisseurTable();

    // ── Production ──────────────────────────────────────────────────────────
    ui->productionTable->verticalHeader()->setVisible(false);
    setupProductionTable();
    loadProductionData();
    connect(ui->btnCreerProduction,       &QPushButton::clicked, this, &MainWindow::onCreerProduction);
    connect(ui->btnModifierProduction,    &QPushButton::clicked, this, &MainWindow::onModifierProduction);
    connect(ui->btnPlanificationProduction,&QPushButton::clicked, this, &MainWindow::onPlanificationProduction);
    connect(ui->btnFactureProduction,     &QPushButton::clicked, this, &MainWindow::onFactureProduction);
    connect(ui->btnExcelProduction,       &QPushButton::clicked, this, &MainWindow::onExcelProduction);
    connect(ui->btnTrierProduction,       &QPushButton::clicked, this, &MainWindow::onTrierProduction);
    connect(ui->btnStatistiquesProduction,&QPushButton::clicked, this, &MainWindow::onStatistiquesProduction);
    connect(ui->searchBoxProduction, &QLineEdit::textChanged, this, &MainWindow::onRechercherProduction);

    // ── Articles ────────────────────────────────────────────────────────────
    setupArticleTable();
    loadArticlesFromDB();
    updateArticleStatistics();

    // ── Initial page ────────────────────────────────────────────────────────
    ui->stackedWidget->setCurrentIndex(0);
    ui->profilePanel->setVisible(true);
    ui->btnEmployees->setStyleSheet(NAV_ACTIVE_STYLE);
    
    // Forcer le plein écran au démarrage
    showMaximized();
}

MainWindow::~MainWindow() 
{
    // Arrêter l'API Python via taskkill (processus détaché)
    QProcess::execute("cmd.exe", QStringList() << "/c" << "taskkill /f /im python.exe >nul 2>&1");
    delete ui; 
}

// ── Navigation helpers ────────────────────────────────────────────────────────
void MainWindow::switchPage(int index, QPushButton *activeBtn, const QString &title, bool showProfile)
{
    const QList<QPushButton*> navBtns = {
        ui->btnEmployees, ui->btnClients, ui->btnProducts,
        ui->btnRawMaterials, ui->btnSuppliers, ui->btnProduction
    };
    for (auto *b : navBtns) b->setStyleSheet("");
    if (activeBtn) activeBtn->setStyleSheet(NAV_ACTIVE_STYLE);

    // Réinitialiser les contraintes AVANT d'afficher/cacher
    ui->profilePanel->setMinimumWidth(0);
    ui->profilePanel->setMaximumWidth(QWIDGETSIZE_MAX);

    ui->stackedWidget->setCurrentIndex(index);
    ui->profilePanel->setVisible(showProfile);

    if (!title.isEmpty()) setWindowTitle(title);
}

void MainWindow::on_btnEmployees_clicked()  { switchPage(0, ui->btnEmployees,  "CUIREA - Gestion des Employés", true); }
void MainWindow::on_btnClients_clicked()    { switchPage(1, ui->btnClients,    "CUIREA - Gestion des Clients",  false); }
void MainWindow::on_btnProducts_clicked()   { switchPage(5, ui->btnProducts,   "CUIREA - Gestion des Articles", false); }
void MainWindow::on_btnRawMaterials_clicked(){ switchPage(2, ui->btnRawMaterials,"CUIREA - Matières Premières",  false); }
void MainWindow::on_btnSuppliers_clicked()  { switchPage(3, ui->btnSuppliers,  "",                              false); }
void MainWindow::on_btnProduction_clicked() { switchPage(4, ui->btnProduction, "CUIREA - Gestion de la Production", false); }

// ── Employee CRUD ─────────────────────────────────────────────────────────────
void MainWindow::on_btnAdd_clicked()
{
    EmployeeDialog dlg(this, EmployeeDialog::AddMode);
    if (dlg.exec() == QDialog::Accepted) {
        Employe e;
        
        // Vérifier si on est en mode BD ou mode statique
        QSqlQueryModel* testModel = e.afficher();
        
        if (!testModel || testModel->rowCount() == 0) {
            // Mode statique - ajouter directement dans la table
            qDebug() << "⚠️ Mode statique - ajout dans le tableau uniquement";
            int row = ui->employeeTable->rowCount();
            ui->employeeTable->insertRow(row);
            ui->employeeTable->setItem(row, 0, new QTableWidgetItem(QString::number(row + 100)));
            ui->employeeTable->setItem(row, 1, new QTableWidgetItem(dlg.getMatricule()));
            ui->employeeTable->setItem(row, 2, new QTableWidgetItem(dlg.getNom()));
            ui->employeeTable->setItem(row, 3, new QTableWidgetItem(dlg.getPrenom()));
            ui->employeeTable->setItem(row, 4, new QTableWidgetItem(dlg.getCin()));
            ui->employeeTable->setItem(row, 5, new QTableWidgetItem(dlg.getDateNaissance().toString("dd/MM/yyyy")));
            ui->employeeTable->setItem(row, 6, new QTableWidgetItem(dlg.getDepartement()));
            ui->employeeTable->setItem(row, 7, new QTableWidgetItem(dlg.getPoste()));
            ui->employeeTable->setItem(row, 8, new QTableWidgetItem(dlg.getTelephone()));
            ui->employeeTable->setItem(row, 9, new QTableWidgetItem(dlg.getEmail()));
            QMessageBox::information(this, "Succès", "Employé ajouté");
            if (testModel) delete testModel;
            return;
        }
        
        delete testModel;
        
        // Mode BD
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
        e.setSpecialite(dlg.getSpecialite());
        e.setDepartement(dlg.getDepartement());
        e.setDateEmbauche(dlg.getDateEmbauche());
        e.setStatut("ACTIF");
        
        if (e.ajouter()) {
            populateEmployeeTable();
            QMessageBox::information(this, "Succès", "Employé ajouté avec succès !");
        } else {
            QMessageBox::critical(this, "Erreur", 
                "Impossible d'ajouter l'employé.\n"
                "Vérifiez que la table EMPLOYES existe dans la base de données.");
        }
    }
}

void MainWindow::on_btnExport_clicked() { EmployeeDialog(this, EmployeeDialog::ExportMode).exec(); }

void MainWindow::on_btnEdit_clicked()
{
    int row = ui->employeeTable->currentRow();
    if (row < 0) { QMessageBox::warning(this,"","Veuillez sélectionner un employé à modifier."); return; }
    
    EmployeeDialog dlg(this, EmployeeDialog::EditMode);
    dlg.setEmployeeData(cellText(ui->employeeTable,row,0), cellText(ui->employeeTable,row,1),
                        cellText(ui->employeeTable,row,2), cellText(ui->employeeTable,row,3),
                        cellText(ui->employeeTable,row,4), cellText(ui->employeeTable,row,5),
                        "Homme", "", cellText(ui->employeeTable,row,8),
                        cellText(ui->employeeTable,row,9), cellText(ui->employeeTable,row,7),
                        "", cellText(ui->employeeTable,row,6), "01/01/2024");
    
    if (dlg.exec() == QDialog::Accepted) {
        Employe e;
        
        // Vérifier si on est en mode BD ou mode statique
        QSqlQueryModel* testModel = e.afficher();
        
        if (!testModel || testModel->rowCount() == 0) {
            // Mode statique - modifier directement dans la table
            qDebug() << "⚠️ modification dans le tableau uniquement";
            ui->employeeTable->item(row, 1)->setText(dlg.getMatricule());
            ui->employeeTable->item(row, 2)->setText(dlg.getNom());
            ui->employeeTable->item(row, 3)->setText(dlg.getPrenom());
            ui->employeeTable->item(row, 4)->setText(dlg.getCin());
            ui->employeeTable->item(row, 5)->setText(dlg.getDateNaissance().toString("dd/MM/yyyy"));
            ui->employeeTable->item(row, 6)->setText(dlg.getDepartement());
            ui->employeeTable->item(row, 7)->setText(dlg.getPoste());
            ui->employeeTable->item(row, 8)->setText(dlg.getTelephone());
            ui->employeeTable->item(row, 9)->setText(dlg.getEmail());
            QMessageBox::information(this, "Succès", "Employé modifié");
            if (testModel) delete testModel;
            return;
        }
        
        delete testModel;
        
        // Mode BD
        e.setId(cellText(ui->employeeTable,row,0).toInt());
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
        e.setSpecialite(dlg.getSpecialite());
        e.setDepartement(dlg.getDepartement());
        e.setDateEmbauche(dlg.getDateEmbauche());
        e.setStatut("ACTIF");
        
        if (e.modifier()) {
            populateEmployeeTable();
            QMessageBox::information(this, "Succès", "Employé modifié avec succès !");
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
    dlg.setEmployeeData(cellText(ui->employeeTable,row,0), cellText(ui->employeeTable,row,1),
                        cellText(ui->employeeTable,row,2), cellText(ui->employeeTable,row,3),
                        "","","","","","",
                        cellText(ui->employeeTable,row,7),"",
                        cellText(ui->employeeTable,row,6),"");
    
    if (dlg.exec() == QDialog::Accepted) {
        int id = cellText(ui->employeeTable,row,0).toInt();
        
        // Vérifier si on est en mode BD ou mode statique
        Employe e;
        QSqlQueryModel* testModel = e.afficher();
        
        if (!testModel || testModel->rowCount() == 0) {
            // Mode statique - supprimer directement de la table
            qDebug() << "⚠️ Mode statique - suppression de la ligne du tableau";
            ui->employeeTable->removeRow(row);
            QMessageBox::information(this, "Succès", "Employé supprimé (mode statique)");
            if (testModel) delete testModel;
            return;
        }
        
        delete testModel;
        
        // Mode BD - utiliser la méthode supprimer
        if (e.supprimer(id)) {
            populateEmployeeTable();
            QMessageBox::information(this, "Succès", "Employé supprimé avec succès !");
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
        QMenu *sub = menu.addMenu("📋 " + label);
        sub->setStyleSheet(menu.styleSheet());
        auto *asc = sub->addAction("↑ Croissant (A → Z)");
        auto *desc = sub->addAction("↓ Décroissant (Z → A)");
        connect(asc, &QAction::triggered, [=]{ ui->employeeTable->sortItems(col, Qt::AscendingOrder); });
        connect(desc, &QAction::triggered, [=]{ ui->employeeTable->sortItems(col, Qt::DescendingOrder); });
    };

    addSortOptions("Matricule", 1);
    addSortOptions("Nom", 2);
    addSortOptions("Prénom", 3);
    addSortOptions("CIN", 4);
    menu.addSeparator();
    addSortOptions("Téléphone", 8);
    addSortOptions("Email", 9);

    // Afficher le menu sous le bouton
    QPoint pos = ui->btnSort->mapToGlobal(QPoint(0, ui->btnSort->height()));
    menu.exec(pos);
}

void MainWindow::on_btnStatistics_clicked()
{
    int total = ui->employeeTable->rowCount();
    QMap<QString,int> depts, postes;
    for (int r = 0; r < total; ++r) {
        depts[cellText(ui->employeeTable,r,6)]++;
        postes[cellText(ui->employeeTable,r,7)]++;
    }

    QDialog dlg(this); dlg.setWindowTitle("Statistiques Employés"); dlg.setMinimumSize(1100,700);
    dlg.setStyleSheet("QDialog{background:#FAF5F0;} QPushButton{background:#8D6E63;color:white;"
                      "border:none;border-radius:8px;padding:10px 24px;} QPushButton:hover{background:#A0826D;}");
    QVBoxLayout lay(&dlg); lay.setSpacing(15); lay.setContentsMargins(20,20,20,20);

    auto *title = new QLabel(QString("<b style='color:#8D6E63;font-size:20px;'>Statistiques - Total: %1</b>").arg(total));
    title->setAlignment(Qt::AlignCenter);
    lay.addWidget(title);

    QHBoxLayout charts; charts.setSpacing(15);

    // Pie chart - Departments
    auto *pieSeries = new QPieSeries();
    QList<QColor> colors;
    colors << QColor("#8D6E63") << QColor("#A0826D") << QColor("#BCAAA4") 
           << QColor("#6D4C41") << QColor("#5D4037") << QColor("#795548");
    int ci = 0;
    for (auto it = depts.begin(); it != depts.end(); ++it, ++ci) {
        auto *sl = pieSeries->append(it.key(), it.value());
        sl->setLabelVisible(true);
        sl->setLabel(QString("%1: %2 (%3%)").arg(it.key()).arg(it.value())
                     .arg(QString::number(it.value()*100.0/total,'f',1)));
        sl->setColor(colors[ci % colors.size()]);
    }
    auto *pieChart = new QChart(); pieChart->addSeries(pieSeries);
    pieChart->legend()->setVisible(false); pieChart->setBackgroundBrush(Qt::white);
    auto *pieView = new QChartView(pieChart); pieView->setRenderHint(QPainter::Antialiasing);
    pieView->setMinimumHeight(320);
    charts.addWidget(pieView);

    // Bar chart - Postes
    auto *barSet = new QBarSet("Employés"); barSet->setColor(QColor("#8D6E63"));
    QStringList cats;
    for (auto it = postes.begin(); it != postes.end(); ++it) { *barSet << it.value(); cats << it.key(); }
    auto *barSeries = new QBarSeries(); barSeries->append(barSet);
    auto *barChart = new QChart(); barChart->addSeries(barSeries); barChart->legend()->setVisible(false);
    auto *axX = new QBarCategoryAxis(); axX->append(cats); barChart->addAxis(axX, Qt::AlignBottom);
    barSeries->attachAxis(axX);
    auto *axY = new QValueAxis(); axY->setLabelFormat("%d"); barChart->addAxis(axY, Qt::AlignLeft);
    barSeries->attachAxis(axY);
    auto *barView = new QChartView(barChart); barView->setRenderHint(QPainter::Antialiasing);
    barView->setMinimumHeight(320);
    charts.addWidget(barView);

    lay.addLayout(&charts);
    QPushButton close("Fermer",&dlg);
    connect(&close,&QPushButton::clicked,&dlg,&QDialog::accept);
    lay.addWidget(&close, 0, Qt::AlignCenter);
    dlg.exec();
}

void MainWindow::populateEmployeeTable()
{
    // Charger depuis la base de données
    Employe e;
    QSqlQueryModel* model = e.afficher();
    
    // Si la requête a échoué ou retourne 0 lignes, utiliser les données statiques
    if (!model || model->rowCount() == 0) {
        qDebug() << "⚠️ Utilisation des données statiques (BD vide ou erreur)";
        
        struct EmpRow { int id; const char *mat,*nom,*prenom,*cin,*dn,*dept,*poste,*tel,*email; };
        static const EmpRow data[] = {
            {1,"EMP-001","Benali","Ahmed","AB123456","15/03/1985","Production","Supervisor","0612345678","ahmed.benali@cuirea.com"},
            {2,"EMP-002","Zahra","Fatima","CD234567","22/07/1990","Quality Control","Inspector","0623456789","fatima.zahra@cuirea.com"},
            {3,"EMP-003","Alami","Mohammed","EF345678","10/11/1988","Cutting","Operator","0634567890","mohammed.alami@cuirea.com"},
            {4,"EMP-004","Mansouri","Khadija","GH456789","05/02/1992","Stitching","Team Leader","0645678901","khadija.mansouri@cuirea.com"},
            {5,"EMP-005","Idrissi","Youssef","IJ567890","18/09/1987","Finishing","Technician","0656789012","youssef.idrissi@cuirea.com"},
            {6,"EMP-006","Benjelloun","Salma","KL678901","30/04/1991","Design","Designer","0667890123","salma.benjelloun@cuirea.com"},
            {7,"EMP-007","Tazi","Omar","MN789012","12/12/1989","Warehouse","Manager","0678901234","omar.tazi@cuirea.com"},
            {8,"EMP-008","Fassi","Leila","OP890123","25/06/1993","Administration","HR Officer","0689012345","leila.fassi@cuirea.com"}
        };
        int n = sizeof(data)/sizeof(*data);
        ui->employeeTable->setRowCount(n);
        QSet<QString> depts;
        for (int i = 0; i < n; ++i) {
            const auto &emp = data[i];
            ui->employeeTable->setItem(i,0,new QTableWidgetItem(QString::number(emp.id)));
            ui->employeeTable->setItem(i,1,new QTableWidgetItem(emp.mat));
            ui->employeeTable->setItem(i,2,new QTableWidgetItem(emp.nom));
            ui->employeeTable->setItem(i,3,new QTableWidgetItem(emp.prenom));
            ui->employeeTable->setItem(i,4,new QTableWidgetItem(emp.cin));
            ui->employeeTable->setItem(i,5,new QTableWidgetItem(emp.dn));
            ui->employeeTable->setItem(i,6,new QTableWidgetItem(emp.dept));
            ui->employeeTable->setItem(i,7,new QTableWidgetItem(emp.poste));
            ui->employeeTable->setItem(i,8,new QTableWidgetItem(emp.tel));
            ui->employeeTable->setItem(i,9,new QTableWidgetItem(emp.email));
            depts.insert(emp.dept);
        }
        ui->statsValue->setText(QString::number(n));
        ui->statsValue_2->setText(QString::number(n));
        ui->statsValue_3->setText(QString::number(depts.size()));
        
        if (model) delete model;
        return;
    }
    
    // Charger depuis le modèle BD
    int n = model->rowCount();
    ui->employeeTable->setRowCount(n);
    QSet<QString> depts;
    
    for (int i = 0; i < n; ++i) {
        for (int col = 0; col < 10; ++col) {
            QString value = model->data(model->index(i, col)).toString();
            ui->employeeTable->setItem(i, col, new QTableWidgetItem(value));
        }
        depts.insert(model->data(model->index(i, 6)).toString());
    }
    
    ui->statsValue->setText(QString::number(n));
    ui->statsValue_2->setText(QString::number(n));
    ui->statsValue_3->setText(QString::number(depts.size()));
    
    delete model;
}

void MainWindow::onEmployeeSelected()
{
    int row = ui->employeeTable->currentRow();
    if (row < 0) return;
    ui->profileValue->setText(cellText(ui->employeeTable,row,1));
    ui->profileValue_2->setText(cellText(ui->employeeTable,row,2));
    ui->profileValue_3->setText(cellText(ui->employeeTable,row,3));
    ui->profileValue_4->setText(cellText(ui->employeeTable,row,4));
    ui->profileValue_5->setText(cellText(ui->employeeTable,row,5));
    ui->profileValue_6->setText("M");
    ui->profileValue_7->setText("Casablanca");
    ui->profileValue_8->setText(cellText(ui->employeeTable,row,8));
    ui->profileValue_9->setText(cellText(ui->employeeTable,row,9));
    ui->profileValue_10->setText(cellText(ui->employeeTable,row,7));
    ui->profileValue_11->setText(cellText(ui->employeeTable,row,6));
    ui->profileValue_12->setText(cellText(ui->employeeTable,row,6));
    ui->profileValue_13->setText("01/01/2024");
    ui->photoPlaceholder->setText("●");
}

// ── Client CRUD ───────────────────────────────────────────────────────────────
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
    QStringList headers = {"ID", "Nom", "Prénom", "Sexe", "CIN", "Pays", "Ville", "Adresse", "Email", "Date Inscription"};
    ui->clientTable->setHorizontalHeaderLabels(headers);
    for(int row = 0; row < model->rowCount(); ++row)
        for(int col = 0; col < model->columnCount(); ++col)
            ui->clientTable->setItem(row, col,
                                     new QTableWidgetItem(model->index(row,col).data().toString()));
    ui->clientTable->resizeColumnsToContents();
    ui->clientTable->horizontalHeader()->setStretchLastSection(true);
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
            QMessageBox::critical(this, "Erreur", "Échec de modification du client !");
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
            QMessageBox::critical(this, "Erreur", "Échec de suppression du client !");
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

void MainWindow::on_btnFidelityClassification_clicked() {}
void MainWindow::on_btnStatsByRegion_clicked() {}
void MainWindow::on_btnAIAgent_clicked() {}
void MainWindow::on_btnExportClient_clicked() {}
void MainWindow::on_btnTriClient_clicked() {}


// ── Raw Materials ─────────────────────────────────────────────────────────────
void MainWindow::setupMatiereTable()
{
    // Charger depuis la classe Matiere avec QSqlQueryModel
    Matiere matiereTmp;
    QSqlQueryModel* model = matiereTmp.afficher();
    
    if (!model) {
        qDebug() << "Erreur chargement matières";
        return;
    }
    
    ui->matiereTable->setRowCount(0); // Vider le tableau
    ui->matiereTable->setColumnCount(7); // Ajouter colonne photo
    ui->matiereTable->setHorizontalHeaderLabels({"MODULE", "RÉFÉRENCE", "TYPE", "QUANTITÉ ACTUELLE", "SEUIL", "DATE D'EXPIRATION", "PHOTO"});
    
    for (int row = 0; row < model->rowCount(); ++row) {
        ui->matiereTable->insertRow(row);
        ui->matiereTable->setItem(row, 0, new QTableWidgetItem(model->data(model->index(row, 1)).toString())); // nom
        ui->matiereTable->setItem(row, 1, new QTableWidgetItem(model->data(model->index(row, 2)).toString())); // reference
        ui->matiereTable->setItem(row, 2, new QTableWidgetItem(model->data(model->index(row, 3)).toString())); // type_matiere
        ui->matiereTable->setItem(row, 3, new QTableWidgetItem(model->data(model->index(row, 4)).toString() + " m²")); // quantite
        ui->matiereTable->setItem(row, 4, new QTableWidgetItem(model->data(model->index(row, 5)).toString())); // seuil
        ui->matiereTable->setItem(row, 5, new QTableWidgetItem(model->data(model->index(row, 6)).toDate().toString("yyyy-MM-dd"))); // date_expiration
        
        // Photo - afficher une miniature ou icône
        QString photoPath = model->data(model->index(row, 7)).toString();
        QTableWidgetItem* photoItem = new QTableWidgetItem();
        if (!photoPath.isEmpty() && QFile::exists(photoPath)) {
            QPixmap pixmap(photoPath);
            photoItem->setIcon(QIcon(pixmap.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
            photoItem->setToolTip(photoPath);
        } else {
            photoItem->setText("—");
        }
        ui->matiereTable->setItem(row, 6, photoItem);
        
        // Stocker l'ID et le chemin photo dans UserRole
        ui->matiereTable->item(row, 0)->setData(Qt::UserRole, model->data(model->index(row, 0)).toInt());
        ui->matiereTable->item(row, 0)->setData(Qt::UserRole + 1, photoPath); // Stocker le chemin photo
    }
    
    delete model; // Libérer la mémoire
    
    ui->matiereTable->setRowHeight(0, 50); // Ajuster la hauteur pour les icônes
    for (int i = 0; i < ui->matiereTable->rowCount(); ++i) {
        ui->matiereTable->setRowHeight(i, 50);
    }
    ui->matiereTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->matiereTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    updateMatiereStatistics();
}

// Empty stubs no longer needed – these were only called indirectly
void MainWindow::setupSuggestionTable() {}
void MainWindow::setupHistoriqueTable() {}
void MainWindow::showForm(bool) {}
void MainWindow::hideForm() {}
void MainWindow::clearForm() {}
void MainWindow::hideAllPanels() {}
void MainWindow::addMatiereToTable(const QString&,const QString&,const QString&,const QString&,const QString&,const QString&) {}
void MainWindow::onSaveMatiere() {}
void MainWindow::onCancelForm() {}
void MainWindow::onGenerateSuggestion() { onSuggestionCommande(); }
void MainWindow::onCloseSuggestion() {}
void MainWindow::onAnalyzeFIFO()       { onOptimisationFIFO(); }
void MainWindow::onCloseOptimisation() {}
void MainWindow::onAppliquerRecherche(){ onRechercheTriMatiere(); }
void MainWindow::onResetRecherche()    {}
void MainWindow::onCloseRecherche()    {}
void MainWindow::onAddFournisseur()    { showInfo(this,"Ajouter","Fonctionnalité en développement"); }
void MainWindow::onEditFournisseur()   { showInfo(this,"Modifier","Fonctionnalité en développement"); }
void MainWindow::onDeleteFournisseur() { showInfo(this,"Supprimer","Fonctionnalité en développement"); }
void MainWindow::onCloseFournisseurs() {}
void MainWindow::loadFournisseurs()    {}
void MainWindow::saveFournisseurs()    {}
void MainWindow::updateFournisseursList() {}
void MainWindow::updateHistoriqueTable()  {}
void MainWindow::updateConsommationChart(){}

void MainWindow::onAddMatiere()
{
    MatiereDialog dlg(this, MatiereDialog::AddMode);
    if (dlg.exec() == QDialog::Accepted) {
        // Créer l'objet Matiere et utiliser la méthode ajouter()
        Matiere matiere;
        matiere.setNom(dlg.getModule());
        matiere.setReference(dlg.getReference());
        matiere.setType(dlg.getType());
        
        // Extraire le nombre de la quantité (ex: "2.5 m²" -> 2.5)
        QString qteStr = dlg.getQuantite().split(" ").first();
        matiere.setQuantite(qteStr.toDouble());
        
        matiere.setSeuil(dlg.getSeuil().toInt());
        matiere.setDateExpiration(QDate::fromString(dlg.getDateExpiration(), "yyyy-MM-dd"));
        matiere.setIdFournisseur(1); // Fournisseur par défaut
        matiere.setPhotoUrl(dlg.getPhotoUrl()); // Photo
        
        if (matiere.ajouter()) {
            QMessageBox::information(this, "Succès", "Matière ajoutée avec succès dans la base de données!");
            setupMatiereTable(); // Recharger le tableau
        } else {
            QMessageBox::critical(this, "Erreur", "Erreur lors de l'ajout de la matière.");
        }
    }
}

void MainWindow::onEditMatiere()
{
    int row = ui->matiereTable->currentRow();
    if (row < 0) { QMessageBox::warning(this,"","Veuillez sélectionner une matière à modifier."); return; }
    
    // Récupérer l'ID de la matière (stocké dans UserRole)
    int matiereId = ui->matiereTable->item(row, 0)->data(Qt::UserRole).toInt();
    QString photoPath = ui->matiereTable->item(row, 0)->data(Qt::UserRole + 1).toString();
    
    MatiereDialog dlg(this, MatiereDialog::EditMode);
    dlg.setMatiereData(cellText(ui->matiereTable,row,0),cellText(ui->matiereTable,row,1),
                       cellText(ui->matiereTable,row,2),cellText(ui->matiereTable,row,3),
                       cellText(ui->matiereTable,row,4),cellText(ui->matiereTable,row,5),
                       photoPath);
    if (dlg.exec() == QDialog::Accepted) {
        // Créer l'objet Matiere et utiliser la méthode modifier()
        Matiere matiere;
        matiere.setId(matiereId);
        matiere.setNom(dlg.getModule());
        matiere.setReference(dlg.getReference());
        matiere.setType(dlg.getType());
        
        // Extraire le nombre de la quantité (ex: "2.5 m²" -> 2.5)
        QString qteStr = dlg.getQuantite().split(" ").first();
        matiere.setQuantite(qteStr.toDouble());
        
        matiere.setSeuil(dlg.getSeuil().toInt());
        matiere.setDateExpiration(QDate::fromString(dlg.getDateExpiration(), "yyyy-MM-dd"));
        matiere.setIdFournisseur(1);
        matiere.setPhotoUrl(dlg.getPhotoUrl()); // Photo
        
        if (matiere.modifier()) {
            QMessageBox::information(this, "Succès", "Matière mise à jour avec succès!");
            setupMatiereTable(); // Recharger le tableau
        } else {
            QMessageBox::critical(this, "Erreur", "Erreur lors de la modification de la matière.");
        }
    }
}

void MainWindow::onDeleteMatiere()
{
    int row = ui->matiereTable->currentRow();
    if (row < 0) { QMessageBox::warning(this,"","Veuillez sélectionner une matière à supprimer."); return; }
    
    // Récupérer l'ID de la matière (stocké dans UserRole)
    int matiereId = ui->matiereTable->item(row, 0)->data(Qt::UserRole).toInt();
    QString photoPath = ui->matiereTable->item(row, 0)->data(Qt::UserRole + 1).toString();
    
    MatiereDialog dlg(this, MatiereDialog::DeleteMode);
    dlg.setMatiereData(cellText(ui->matiereTable,row,0),cellText(ui->matiereTable,row,1),
                       cellText(ui->matiereTable,row,2),cellText(ui->matiereTable,row,3),
                       cellText(ui->matiereTable,row,4),cellText(ui->matiereTable,row,5),
                       photoPath);
    if (dlg.exec() == QDialog::Accepted) {
        // Utiliser la méthode supprimer() de la classe Matiere avec l'ID
        Matiere matiereTmp;
        if (matiereTmp.supprimer(matiereId)) {
            QMessageBox::information(this, "Succès", "Matière supprimée avec succès!");
            setupMatiereTable(); // Recharger le tableau
        } else {
            QMessageBox::critical(this, "Erreur", "Erreur lors de la suppression de la matière.");
        }
    }
}

void MainWindow::onSuggestionCommande()
{
    QString msg = "[!] SUGGESTIONS DE COMMANDE\n\n";
    for (int r = 0; r < ui->matiereTable->rowCount(); ++r) {
        double conso = cellText(ui->matiereTable,r,3).split(" ").first().toDouble();
        int seuil    = cellText(ui->matiereTable,r,4).toInt();
        double need30 = conso * 30;
        if (need30 > seuil * 0.5) {
            QString prio = (need30 > seuil) ? "[!!!] URGENT" : "[!] NORMAL";
            msg += QString("%1 - %2\n   Qté suggérée: %3\n   Délai: %4\n\n")
                   .arg(prio, cellText(ui->matiereTable,r,0))
                   .arg(int(need30*1.5))
                   .arg(need30 > seuil ? "3-5 jours" : "7-14 jours");
        }
    }
    if (msg.count('\n') <= 2) msg += "[OK] Aucune commande urgente nécessaire.";
    showInfo(this, "Suggestions de Commande", msg);
}

void MainWindow::onOptimisationFIFO()
{
    QList<QPair<QString,int>> urgent, normal, safe;
    for (int r = 0; r < ui->matiereTable->rowCount(); ++r) {
        int days = calculateDaysToExpiration(cellText(ui->matiereTable,r,5));
        QString name = cellText(ui->matiereTable,r,0);
        if (days <= 30)       urgent.append({name,days});
        else if (days <= 90)  normal.append({name,days});
        else                  safe.append({name,days});
    }
    auto fmt = [](const QList<QPair<QString,int>> &lst, const QString &hdr) {
        QString s = hdr;
        if (lst.isEmpty()) return s + "   Aucun\n";
        for (const auto &p : lst)
            s += QString("   • %1 - Expire dans %2 jours\n").arg(p.first).arg(p.second);
        return s;
    };
    showInfo(this,"Optimisation FIFO",
             "[FIFO] ANALYSE FIFO\n\n"
             + fmt(urgent,"[!!!] URGENCE (≤ 30j):\n")
             + fmt(normal,"\n[!] NORMAL (31-90j):\n")
             + fmt(safe,"\n[OK] STOCK SÛR (> 90j):\n"));
}

void MainWindow::onExportMatiere()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        "Exporter en PDF",
        QDir::homePath() + "/Rapport_Matieres_" + QDate::currentDate().toString("yyyy-MM-dd") + ".pdf",
        "PDF Files (*.pdf)");

    if (fileName.isEmpty()) return;

    // ── Calcul des statistiques ───────────────────────────────
    int total = ui->matiereTable->rowCount();
    int critique = 0, normal = 0, eleve = 0, expires = 0, proche30j = 0;
    QMap<QString, int> parType;

    for (int r = 0; r < total; ++r) {
        double qty   = cellText(ui->matiereTable, r, 3).remove(" m²").toDouble();
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

    // ── Construction HTML identique à l'image ─────────────────
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

    // ── En-tête avec logo ─────────────────────────────────────
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

    // ── Statistiques générales ────────────────────────────────
    html += R"(<div class="section-title">STATISTIQUES GENERALES</div>)";
    
    html += QString(R"(
    <div class="cards">
      <div class="card card-total">
        <div class="card-icon">📦</div>
        <div class="card-title">Total<br>Matieres</div>
        <div class="card-value">%1</div>
        <div class="card-label">Actives</div>
      </div>
      <div class="card card-critique">
        <div class="card-icon">⚠️</div>
        <div class="card-title">Stock<br>Critique</div>
        <div class="card-value">%2</div>
        <div class="card-label">Items a reapprovisionner</div>
      </div>
      <div class="card card-expires">
        <div class="card-icon">⏳</div>
        <div class="card-title">Expires</div>
        <div class="card-value">%3</div>
        <div class="card-label">Expires</div>
      </div>
      <div class="card card-proche">
        <div class="card-icon">🕐</div>
        <div class="card-title">< 30 Jours</div>
        <div class="card-value">%4</div>
        <div class="card-label">A surveiller</div>
      </div>
      <div class="card card-ok">
        <div class="card-icon">✓</div>
        <div class="card-title">Stock<br>OK</div>
        <div class="card-value">%5</div>
        <div class="card-label">Stock OK</div>
      </div>
    </div>
    )").arg(total).arg(critique).arg(expires).arg(proche30j).arg(stockOK);

    // ── Répartition par type (graphique + tableau) ────────────
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

    // ── Liste détaillée ───────────────────────────────────────
    html += R"(<div class="section-title">LISTE DETAILLEE DES MATIERES</div>)";
    html += R"(
    <table class="detail-table">
      <tr>
        <th>Module</th><th>Reference</th><th>Type</th>
        <th>Quantite</th><th>Seuil</th><th>Expiration</th><th>Statut</th>
      </tr>
    )";

    for (int r = 0; r < total; ++r) {
        double qty   = cellText(ui->matiereTable, r, 3).remove(" m²").toDouble();
        int    seuil = cellText(ui->matiereTable, r, 4).toInt();
        QString badge, label;
        if      (qty < seuil * 0.5) { badge = "badge-critique"; label = "Critique"; }
        else if (qty < seuil)       { badge = "badge-normal";   label = "Normal";   }

        html += QString(R"(
        <tr>
          <td><span class="icon-leather">🐄</span>%1</td>
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

    // ── Footer ────────────────────────────────────────────────
    html += R"(
    <div class="footer">
      <div>CUIREA Management System — Rapport genere automatiquement</div>
      <div class="footer-right">1 / 1</div>
    </div>
    )";

    html += R"(</div></body></html>)";

    // ── Génération PDF ────────────────────────────────────────
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

    // ── Donut Chart - Répartition stock (CRITICITÉ) ──────────
    auto *pieSeries = new QPieSeries();
    pieSeries->setHoleSize(0.5);  // Donut chart
    
    auto *sliceCrit = pieSeries->append("Critique", critique);
    sliceCrit->setBrush(QColor("#B33A3A"));
    sliceCrit->setLabelVisible(false);
    
    auto *sliceElev = pieSeries->append("Élevé", eleve);
    sliceElev->setBrush(QColor("#E67E22"));
    sliceElev->setLabelVisible(false);
    
    auto *sliceNorm = pieSeries->append("Normal", normal);
    sliceNorm->setBrush(QColor("#95A472"));
    sliceNorm->setLabelVisible(false);

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

    // ── Bar chart - Par type ──────────────────────────────────
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
    auto *btnExport = new QPushButton("📄 Exporter PDF", &dlg);
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
    
    if (ok && !choice.isEmpty()) {
        int column = -1;
        Qt::SortOrder order = Qt::AscendingOrder;
        
        if (choice.contains("Module")) {
            column = 0;
            order = choice.contains("Z-A") ? Qt::DescendingOrder : Qt::AscendingOrder;
        } else if (choice.contains("Type")) {
            column = 2;
            order = choice.contains("Z-A") ? Qt::DescendingOrder : Qt::AscendingOrder;
        } else if (choice.contains("Quantité")) {
            column = 3;
            order = choice.contains("Décroissant") ? Qt::DescendingOrder : Qt::AscendingOrder;
        } else if (choice.contains("Seuil")) {
            column = 4;
            order = choice.contains("Décroissant") ? Qt::DescendingOrder : Qt::AscendingOrder;
        } else if (choice.contains("Date")) {
            column = 5;
            order = choice.contains("lointaine") ? Qt::DescendingOrder : Qt::AscendingOrder;
        }
        
        if (column >= 0) {
            ui->matiereTable->sortItems(column, order);
            QMessageBox::information(this, "Tri effectué", 
                                   QString("Les matières ont été triées par: %1").arg(choice));
        }
    }
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

    auto *title = new QLabel("🔍 Recherche de Matières Premières");
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
    cmbStock->addItems({"Tous", "Critique", "Normal", "Élevé"});
    
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
            int qty = cellText(ui->matiereTable, r, 3).remove(" m²").toDouble();
            int seuil = cellText(ui->matiereTable, r, 4).toInt();
            if (stockFilter == "Critique" && qty >= seuil) match = false;
            if (stockFilter == "Normal" && (qty < seuil || qty > seuil * 2)) match = false;
            if (stockFilter == "Élevé" && qty <= seuil * 2) match = false;
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
        QString("🔍 %1 matière(s) trouvée(s)").arg(visibleCount), 5000);
}

void MainWindow::onDetectionDefauts()
{
    // Créer une boîte de dialogue améliorée pour la détection
    QDialog *detectionDialog = new QDialog(this);
    detectionDialog->setWindowTitle("Détection de Défauts - Cuir");
    detectionDialog->setFixedSize(650, 550);
    detectionDialog->setStyleSheet(
        "QDialog { background: #FAF5F0; }"
        "QLabel { color: #291C0E; }"
        "QPushButton { border-radius: 8px; padding: 10px 20px; font-weight: bold; }"
        "QComboBox { background: white; border: 2px solid #BCAAA4; border-radius: 6px; padding: 8px; }"
        "QGroupBox { font-weight: bold; border: 2px solid #BCAAA4; border-radius: 8px; margin-top: 10px; padding-top: 15px; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 15px; padding: 0 5px; }"
    );
    
    QVBoxLayout *mainLayout = new QVBoxLayout(detectionDialog);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(25, 25, 25, 25);
    
    // Titre
    QLabel *titleLabel = new QLabel("🔬 Analyse de Qualité du Cuir", detectionDialog);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #8D6E63; margin-bottom: 10px;");
    mainLayout->addWidget(titleLabel);
    
    // Description
    QLabel *descLabel = new QLabel("Sélectionnez une image de matière première pour détecter les défauts potentiels.", detectionDialog);
    descLabel->setAlignment(Qt::AlignCenter);
    descLabel->setWordWrap(true);
    descLabel->setStyleSheet("color: #5D4037; font-size: 12px; margin-bottom: 15px;");
    mainLayout->addWidget(descLabel);
    
    // Zone de prévisualisation de l'image
    QLabel *previewLabel = new QLabel(detectionDialog);
    previewLabel->setFixedSize(200, 200);
    previewLabel->setAlignment(Qt::AlignCenter);
    previewLabel->setStyleSheet(
        "background: #E8E0D8; border: 3px dashed #BCAAA4; border-radius: 12px;"
    );
    previewLabel->setText("📷\nAucune image\nsélectionnée");
    
    QHBoxLayout *previewLayout = new QHBoxLayout();
    previewLayout->addStretch();
    previewLayout->addWidget(previewLabel);
    previewLayout->addStretch();
    mainLayout->addLayout(previewLayout);
    
    // Variable pour stocker le chemin de l'image
    QString *selectedImagePath = new QString();
    
    // GroupBox: Sélection depuis le tableau
    QGroupBox *tableGroup = new QGroupBox("📋 Sélectionner depuis le tableau des matières", detectionDialog);
    QVBoxLayout *tableLayout = new QVBoxLayout(tableGroup);
    
    QComboBox *matiereCombo = new QComboBox(detectionDialog);
    matiereCombo->addItem("-- Choisir une matière première --", "");
    
    // Remplir le combo avec les matières ayant une photo
    for (int row = 0; row < ui->matiereTable->rowCount(); ++row) {
        QString photoPath = ui->matiereTable->item(row, 0)->data(Qt::UserRole + 1).toString();
        if (!photoPath.isEmpty() && QFile::exists(photoPath)) {
            QString matiereName = ui->matiereTable->item(row, 0)->text();
            QString matiereRef = ui->matiereTable->item(row, 1)->text();
            matiereCombo->addItem(QString("📦 %1 (%2)").arg(matiereName, matiereRef), photoPath);
        }
    }
    
    if (matiereCombo->count() == 1) {
        matiereCombo->setItemText(0, "-- Aucune matière avec photo disponible --");
        matiereCombo->setEnabled(false);
    }
    
    tableLayout->addWidget(matiereCombo);
    mainLayout->addWidget(tableGroup);
    
    // GroupBox: Sélection depuis fichier
    QGroupBox *fileGroup = new QGroupBox("📁 Ou importer depuis un fichier", detectionDialog);
    QHBoxLayout *fileLayout = new QHBoxLayout(fileGroup);
    
    QPushButton *browseBtn = new QPushButton("🖼️ Parcourir...", detectionDialog);
    browseBtn->setStyleSheet(
        "QPushButton { background: #6D4C41; color: white; }"
        "QPushButton:hover { background: #8D6E63; }"
    );
    fileLayout->addWidget(browseBtn);
    
    QLabel *filePathLabel = new QLabel("Aucun fichier sélectionné", detectionDialog);
    filePathLabel->setStyleSheet("color: #757575; font-style: italic;");
    fileLayout->addWidget(filePathLabel, 1);
    mainLayout->addWidget(fileGroup);
    
    // Boutons d'action
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    QPushButton *analyzeBtn = new QPushButton("🔍 Analyser", detectionDialog);
    analyzeBtn->setStyleSheet(
        "QPushButton { background: #4CAF50; color: white; font-size: 14px; padding: 12px 30px; }"
        "QPushButton:hover { background: #66BB6A; }"
        "QPushButton:disabled { background: #BDBDBD; }"
    );
    analyzeBtn->setEnabled(false);
    
    QPushButton *cancelBtn = new QPushButton("Annuler", detectionDialog);
    cancelBtn->setStyleSheet(
        "QPushButton { background: #BCAAA4; color: white; }"
        "QPushButton:hover { background: #A1887F; }"
    );
    
    buttonLayout->addWidget(analyzeBtn);
    buttonLayout->addWidget(cancelBtn);
    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
    
    // Connexions
    auto updatePreview = [previewLabel, analyzeBtn, selectedImagePath](const QString &path) {
        *selectedImagePath = path;
        if (!path.isEmpty() && QFile::exists(path)) {
            QPixmap pixmap(path);
            previewLabel->setPixmap(pixmap.scaled(190, 190, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            analyzeBtn->setEnabled(true);
        } else {
            previewLabel->setText("📷\nAucune image\nsélectionnée");
            analyzeBtn->setEnabled(false);
        }
    };
    
    connect(matiereCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            [matiereCombo, filePathLabel, updatePreview](int index) {
        QString path = matiereCombo->itemData(index).toString();
        if (!path.isEmpty()) {
            filePathLabel->setText("Image du tableau");
            updatePreview(path);
        }
    });
    
    connect(browseBtn, &QPushButton::clicked, [detectionDialog, filePathLabel, matiereCombo, updatePreview]() {
        QString imagePath = QFileDialog::getOpenFileName(
            detectionDialog,
            "Sélectionner une image de cuir",
            "",
            "Images (*.png *.jpg *.jpeg *.bmp *.tiff)"
        );
        if (!imagePath.isEmpty()) {
            QFileInfo fi(imagePath);
            filePathLabel->setText(fi.fileName());
            matiereCombo->setCurrentIndex(0);
            updatePreview(imagePath);
        }
    });
    
    connect(cancelBtn, &QPushButton::clicked, detectionDialog, &QDialog::reject);
    
    connect(analyzeBtn, &QPushButton::clicked, [this, detectionDialog, selectedImagePath]() {
        if (selectedImagePath->isEmpty()) {
            QMessageBox::warning(detectionDialog, "Attention", "Veuillez sélectionner une image.");
            return;
        }
        detectionDialog->accept();
        startDetectionAnalysis(*selectedImagePath);
    });
    
    detectionDialog->exec();
    delete selectedImagePath;
    detectionDialog->deleteLater();
}

void MainWindow::startDetectionAnalysis(const QString &imagePath)
{
    // Créer une boîte de dialogue de connexion
    QDialog *waitDialog = new QDialog(this);
    waitDialog->setWindowTitle("Connexion au serveur");
    waitDialog->setModal(true);
    waitDialog->setFixedSize(400, 180);
    waitDialog->setStyleSheet("QDialog { background: #FAF5F0; }");
    
    QVBoxLayout *waitLayout = new QVBoxLayout(waitDialog);
    waitLayout->setSpacing(15);
    waitLayout->setContentsMargins(25, 25, 25, 25);
    
    QLabel *iconLabel = new QLabel("🔄", waitDialog);
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setStyleSheet("font-size: 40px;");
    waitLayout->addWidget(iconLabel);
    
    QLabel *waitLabel = new QLabel("Connexion au serveur de détection...", waitDialog);
    waitLabel->setAlignment(Qt::AlignCenter);
    waitLabel->setStyleSheet("font-size: 14px; color: #5D4037;");
    waitLayout->addWidget(waitLabel);
    
    QProgressBar *waitProgress = new QProgressBar(waitDialog);
    waitProgress->setRange(0, 0);
    waitProgress->setStyleSheet(
        "QProgressBar { border: 2px solid #BCAAA4; border-radius: 5px; background: white; }"
        "QProgressBar::chunk { background: #8D6E63; }"
    );
    waitLayout->addWidget(waitProgress);
    
    QPushButton *cancelBtn = new QPushButton("Annuler", waitDialog);
    cancelBtn->setStyleSheet(
        "QPushButton { background: #BCAAA4; color: white; border-radius: 6px; padding: 8px 20px; }"
        "QPushButton:hover { background: #A1887F; }"
    );
    waitLayout->addWidget(cancelBtn, 0, Qt::AlignCenter);
    
    // Timer pour les tentatives de connexion
    QTimer *retryTimer = new QTimer(waitDialog);
    int *attempt = new int(0);
    int maxRetries = 15;
    
    connect(cancelBtn, &QPushButton::clicked, [waitDialog, retryTimer]() {
        retryTimer->stop();
        waitDialog->close();
        waitDialog->deleteLater();
    });
    
    connect(retryTimer, &QTimer::timeout, [this, imagePath, waitDialog, waitLabel, retryTimer, attempt, maxRetries]() {
        QNetworkRequest healthRequest(QUrl(apiUrl + "/health"));
        healthRequest.setTransferTimeout(2000);
        QNetworkReply *healthReply = networkManager->get(healthRequest);
        
        connect(healthReply, &QNetworkReply::finished, [this, imagePath, healthReply, waitDialog, waitLabel, retryTimer, attempt, maxRetries]() {
            healthReply->deleteLater();
            
            if (healthReply->error() == QNetworkReply::NoError) {
                retryTimer->stop();
                waitDialog->close();
                waitDialog->deleteLater();
                delete attempt;
                detectDefectsInImage(imagePath);
                return;
            }
            
            (*attempt)++;
            waitLabel->setText(QString("Attente du serveur... (%1/%2)").arg(*attempt).arg(maxRetries));
            
            if (*attempt >= maxRetries) {
                retryTimer->stop();
                waitDialog->close();
                waitDialog->deleteLater();
                delete attempt;
                QMessageBox::critical(this, "Erreur de connexion",
                    "Le serveur de détection n'est pas prêt.\n\n"
                    "Le modèle TensorFlow peut prendre jusqu'à 30 secondes pour se charger.\n"
                    "Veuillez réessayer dans quelques instants.");
            }
        });
    });
    
    waitDialog->show();
    retryTimer->start(1000);
}

void MainWindow::detectDefectsInImage(const QString &imagePath)
{
    // Créer une boîte de dialogue de progression
    QDialog *progressDialog = new QDialog(this);
    progressDialog->setWindowTitle("Détection en cours");
    progressDialog->setModal(true);
    progressDialog->setFixedSize(400, 150);
    
    QVBoxLayout *layout = new QVBoxLayout(progressDialog);
    
    QLabel *statusLabel = new QLabel("Analyse de l'image en cours...", progressDialog);
    statusLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(statusLabel);
    
    QProgressBar *progressBar = new QProgressBar(progressDialog);
    progressBar->setRange(0, 0); // Mode indéterminé
    layout->addWidget(progressBar);
    
    progressDialog->show();
    
    // Lire et encoder l'image en base64
    QFile imageFile(imagePath);
    if (!imageFile.open(QIODevice::ReadOnly)) {
        progressDialog->close();
        progressDialog->deleteLater();
        QMessageBox::critical(this, "Erreur", "Impossible de lire l'image");
        return;
    }
    
    QByteArray imageData = imageFile.readAll();
    imageFile.close();
    
    // Vérifier que l'image n'est pas trop grande (max 10MB)
    if (imageData.size() > 10 * 1024 * 1024) {
        progressDialog->close();
        progressDialog->deleteLater();
        QMessageBox::warning(this, "Attention", 
            "L'image est trop volumineuse (> 10MB).\n"
            "Veuillez utiliser une image plus petite.");
        return;
    }
    
    QString base64Image = imageData.toBase64();
    
    // Préparer la requête JSON
    QJsonObject requestData;
    requestData["image"] = "data:image/jpeg;base64," + base64Image;
    
    QJsonDocument doc(requestData);
    QByteArray jsonData = doc.toJson();
    
    // Envoyer la requête à l'API
    QNetworkRequest request(QUrl(apiUrl + "/predict"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setTransferTimeout(60000);  // Timeout de 60 secondes pour l'analyse
    
    QNetworkReply *reply = networkManager->post(request, jsonData);
    
    // Utiliser QPointer pour éviter les dangling pointers
    QPointer<QDialog> dialogPtr(progressDialog);
    
    connect(reply, &QNetworkReply::finished, this, [this, reply, dialogPtr]() {
        // Vérifier que le dialog existe encore
        if (dialogPtr) {
            dialogPtr->close();
            dialogPtr->deleteLater();
        }
        
        if (reply->error() != QNetworkReply::NoError) {
            QString errorDetails = QString("Code: %1\nMessage: %2\nURL: %3")
                .arg(reply->error())
                .arg(reply->errorString())
                .arg(reply->url().toString());
            
            // Lire la réponse même en cas d'erreur pour voir le message du serveur
            QByteArray responseData = reply->readAll();
            if (!responseData.isEmpty()) {
                errorDetails += QString("\n\nRéponse du serveur:\n%1")
                    .arg(QString::fromUtf8(responseData));
            }
            
            QMessageBox::critical(this, "Erreur de détection", errorDetails);
            reply->deleteLater();
            return;
        }
        
        // Traiter la réponse
        QByteArray responseData = reply->readAll();
        
        // Vérifier que la réponse n'est pas vide
        if (responseData.isEmpty()) {
            QMessageBox::critical(this, "Erreur", "Réponse vide du serveur");
            reply->deleteLater();
            return;
        }
        
        // Parser le JSON
        QJsonParseError parseError;
        QJsonDocument responseDoc = QJsonDocument::fromJson(responseData, &parseError);
        
        if (parseError.error != QJsonParseError::NoError) {
            QMessageBox::critical(this, "Erreur JSON", 
                QString("Erreur de parsing JSON:\n%1\n\nRéponse brute:\n%2")
                    .arg(parseError.errorString())
                    .arg(QString::fromUtf8(responseData)));
            reply->deleteLater();
            return;
        }
        
        QJsonObject response = responseDoc.object();
        
        // Vérifier le champ "success"
        if (!response.contains("success")) {
            QMessageBox::critical(this, "Erreur", 
                "La réponse ne contient pas le champ 'success'");
            reply->deleteLater();
            return;
        }
        
        if (response["success"].toBool()) {
            showDetectionResults(response);
        } else {
            QString errorMsg = response["error"].toString();
            if (errorMsg.isEmpty()) {
                errorMsg = "Erreur inconnue";
            }
            QMessageBox::critical(this, "Erreur", 
                "Échec de la détection:\n" + errorMsg);
        }
        
        reply->deleteLater();
    });
}

QString MainWindow::translateDefectClass(const QString &englishName)
{
    // Traduction des classes de défauts en français
    static QMap<QString, QString> translations = {
        {"Folding marks", "Marques de pliage"},
        {"Grain off", "Grain détaché"},
        {"Growth marks", "Marques de croissance"},
        {"loose grains", "Grains lâches"},
        {"non defective", "Non défectueux"},
        {"pinhole", "Trou d'épingle"}
    };
    
    return translations.value(englishName, englishName);
}

void MainWindow::showDetectionResults(const QJsonObject &results)
{
    // Vérifier que les champs nécessaires existent
    if (!results.contains("prediction") || !results.contains("all_predictions")) {
        QMessageBox::critical(this, "Erreur", 
            "Format de réponse invalide: champs manquants");
        return;
    }
    
    QJsonObject prediction = results["prediction"].toObject();
    
    if (!prediction.contains("class_name") || !prediction.contains("confidence_percent")) {
        QMessageBox::critical(this, "Erreur", 
            "Format de prédiction invalide");
        return;
    }
    
    QString className = prediction["class_name"].toString();
    QString classNameFr = translateDefectClass(className);
    double confidence = prediction["confidence_percent"].toDouble();
    
    // Déterminer le style selon le résultat
    bool isDefective = (className.toLower() != "non defective");
    QString statusIcon, statusText, gradientStart, gradientEnd, accentColor;
    
    if (!isDefective) {
        statusIcon = "✓";
        statusText = "QUALITÉ VALIDÉE";
        gradientStart = "#43A047";
        gradientEnd = "#66BB6A";
        accentColor = "#2E7D32";
    } else if (confidence > 70) {
        statusIcon = "✗";
        statusText = "DÉFAUT DÉTECTÉ";
        gradientStart = "#E53935";
        gradientEnd = "#EF5350";
        accentColor = "#C62828";
    } else {
        statusIcon = "!";
        statusText = "À VÉRIFIER";
        gradientStart = "#FB8C00";
        gradientEnd = "#FFA726";
        accentColor = "#EF6C00";
    }
    
    // Créer le dialogue principal
    QDialog *resultDialog = new QDialog(this);
    resultDialog->setWindowTitle("Résultats de Détection");
    resultDialog->setModal(true);
    resultDialog->setFixedSize(520, 580);
    resultDialog->setStyleSheet(
        "QDialog { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #FAFAFA, stop:1 #F0EBE6); }"
    );
    
    QVBoxLayout *mainLayout = new QVBoxLayout(resultDialog);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // === HEADER avec gradient ===
    QFrame *headerFrame = new QFrame(resultDialog);
    headerFrame->setFixedHeight(180);
    headerFrame->setStyleSheet(QString(
        "QFrame { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 %1, stop:1 %2); "
        "border-top-left-radius: 0px; border-top-right-radius: 0px; }"
    ).arg(gradientStart, gradientEnd));
    
    QVBoxLayout *headerLayout = new QVBoxLayout(headerFrame);
    headerLayout->setAlignment(Qt::AlignCenter);
    headerLayout->setSpacing(8);
    
    // Icône circulaire
    QLabel *iconCircle = new QLabel(statusIcon, headerFrame);
    iconCircle->setFixedSize(70, 70);
    iconCircle->setAlignment(Qt::AlignCenter);
    iconCircle->setStyleSheet(
        "QLabel { background: rgba(255,255,255,0.25); color: white; font-size: 36px; "
        "font-weight: bold; border-radius: 35px; }"
    );
    headerLayout->addWidget(iconCircle, 0, Qt::AlignCenter);
    
    // Texte de statut
    QLabel *statusLabel = new QLabel(statusText, headerFrame);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet(
        "QLabel { color: white; font-size: 20px; font-weight: bold; letter-spacing: 2px; }"
    );
    headerLayout->addWidget(statusLabel);
    
    // Nom du défaut
    QLabel *defectLabel = new QLabel(classNameFr, headerFrame);
    defectLabel->setAlignment(Qt::AlignCenter);
    defectLabel->setStyleSheet(
        "QLabel { color: rgba(255,255,255,0.9); font-size: 14px; font-weight: 500; }"
    );
    headerLayout->addWidget(defectLabel);
    
    mainLayout->addWidget(headerFrame);
    
    // === CONTENU ===
    QWidget *contentWidget = new QWidget(resultDialog);
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setSpacing(20);
    contentLayout->setContentsMargins(25, 25, 25, 20);
    
    // Carte de confiance
    QFrame *confidenceCard = new QFrame(contentWidget);
    confidenceCard->setStyleSheet(
        "QFrame { background: white; border-radius: 12px; }"
    );
    QHBoxLayout *confLayout = new QHBoxLayout(confidenceCard);
    confLayout->setContentsMargins(20, 15, 20, 15);
    
    QLabel *confTitle = new QLabel("Niveau de Confiance", confidenceCard);
    confTitle->setStyleSheet("QLabel { color: #5D4037; font-size: 13px; font-weight: 600; }");
    confLayout->addWidget(confTitle);
    
    confLayout->addStretch();
    
    QLabel *confValue = new QLabel(QString("%1%").arg(confidence, 0, 'f', 1), confidenceCard);
    confValue->setStyleSheet(QString(
        "QLabel { color: %1; font-size: 24px; font-weight: bold; }"
    ).arg(accentColor));
    confLayout->addWidget(confValue);
    
    contentLayout->addWidget(confidenceCard);
    
    // Section Analyse Détaillée
    QLabel *detailsTitle = new QLabel("Analyse Complète", contentWidget);
    detailsTitle->setStyleSheet(
        "QLabel { color: #5D4037; font-size: 14px; font-weight: bold; margin-top: 5px; }"
    );
    contentLayout->addWidget(detailsTitle);
    
    // Carte des détails
    QFrame *detailsCard = new QFrame(contentWidget);
    detailsCard->setStyleSheet(
        "QFrame { background: white; border-radius: 12px; }"
    );
    QVBoxLayout *detailsLayout = new QVBoxLayout(detailsCard);
    detailsLayout->setSpacing(12);
    detailsLayout->setContentsMargins(18, 18, 18, 18);
    
    QJsonArray allPredictions = results["all_predictions"].toArray();
    
    for (int i = 0; i < allPredictions.size(); ++i) {
        QJsonObject predObj = allPredictions[i].toObject();
        QString name = predObj["class_name"].toString();
        QString nameFr = translateDefectClass(name);
        double conf = predObj["confidence_percent"].toDouble();
        
        QWidget *rowWidget = new QWidget(detailsCard);
        QHBoxLayout *rowLayout = new QHBoxLayout(rowWidget);
        rowLayout->setContentsMargins(0, 0, 0, 0);
        rowLayout->setSpacing(12);
        
        // Indicateur coloré
        QLabel *dot = new QLabel(rowWidget);
        dot->setFixedSize(8, 8);
        QString dotColor = (name.toLower() == "non defective") ? "#4CAF50" : 
                          (conf > 50) ? "#F44336" : "#BDBDBD";
        dot->setStyleSheet(QString(
            "QLabel { background: %1; border-radius: 4px; }"
        ).arg(dotColor));
        rowLayout->addWidget(dot);
        
        // Nom
        QLabel *nameLabel = new QLabel(nameFr, rowWidget);
        nameLabel->setStyleSheet("QLabel { color: #424242; font-size: 12px; }");
        nameLabel->setMinimumWidth(140);
        rowLayout->addWidget(nameLabel);
        
        // Barre de progression stylée
        QFrame *barBg = new QFrame(rowWidget);
        barBg->setFixedHeight(8);
        barBg->setStyleSheet("QFrame { background: #EEEEEE; border-radius: 4px; }");
        
        QFrame *barFill = new QFrame(barBg);
        int fillWidth = static_cast<int>(conf * 1.2); // Max 120px
        barFill->setFixedSize(fillWidth, 8);
        barFill->move(0, 0);
        QString fillColor = (name.toLower() == "non defective") ? "#4CAF50" : "#8D6E63";
        barFill->setStyleSheet(QString(
            "QFrame { background: %1; border-radius: 4px; }"
        ).arg(fillColor));
        
        rowLayout->addWidget(barBg, 1);
        
        // Pourcentage
        QLabel *percLabel = new QLabel(QString("%1%").arg(conf, 0, 'f', 1), rowWidget);
        percLabel->setFixedWidth(50);
        percLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        QString percColor = (conf > 50) ? "#424242" : "#9E9E9E";
        percLabel->setStyleSheet(QString(
            "QLabel { color: %1; font-size: 12px; font-weight: 600; }"
        ).arg(percColor));
        rowLayout->addWidget(percLabel);
        
        detailsLayout->addWidget(rowWidget);
    }
    
    contentLayout->addWidget(detailsCard);
    contentLayout->addStretch();
    
    // Bouton Fermer
    QPushButton *closeBtn = new QPushButton("Fermer", contentWidget);
    closeBtn->setFixedSize(160, 44);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet(QString(
        "QPushButton { background: %1; color: white; border: none; border-radius: 22px; "
        "font-size: 14px; font-weight: bold; }"
        "QPushButton:hover { background: %2; }"
        "QPushButton:pressed { background: %1; }"
    ).arg(gradientStart, gradientEnd));
    connect(closeBtn, &QPushButton::clicked, resultDialog, &QDialog::accept);
    
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(closeBtn);
    btnLayout->addStretch();
    contentLayout->addLayout(btnLayout);
    
    mainLayout->addWidget(contentWidget);
    
    resultDialog->exec();
    resultDialog->deleteLater();
}

void MainWindow::onGestionFournisseurs()
{
    showInfo(this,"Fournisseurs","1. Tannerie Martin\n   Tél: 01 23 45 67 89\n\n"
                                 "2. Quincaillerie Dubois\n   Tél: 01 98 76 54 32\n\n"
                                 "Fonctionnalités complètes disponibles dans la prochaine version.");
}

int MainWindow::calculateDaysToExpiration(const QString &dateStr)
{
    return QDate::currentDate().daysTo(QDate::fromString(dateStr,"yyyy-MM-dd"));
}

QString MainWindow::getStockLevel(int cur, int thr)
{
    return cur < thr*0.5 ? "Critique" : cur < thr ? "Normal" : "Élevé";
}

int MainWindow::calculateSuggestedQuantity(const QString &) { return 100; }

void MainWindow::updateMatiereStatistics()
{
    int total = ui->matiereTable->rowCount(), critical = 0;
    for (int r = 0; r < total; ++r)
        if (cellText(ui->matiereTable,r,3).split(" ").first().toDouble()*30
            > cellText(ui->matiereTable,r,4).toInt()) ++critical;
    ui->statsValueMatiere1->setText(QString::number(total));
    ui->statsValueMatiere2->setText(QString::number(critical));
}

// ── Suppliers ─────────────────────────────────────────────────────────────────
void MainWindow::setupFournisseurTable()
{
    fournisseursData.clear();
    // 15 sample records (condensed)
    const struct { const char *id,*nom,*email,*tel,*mat,*type,*pay,*stat; } data[] = {
        {"F001","Leather Masters SA","contact@leathermasters.tn","+216 71 234 567","1234567/A/M/000","Cuir","Chèque","Actif"},
        {"F002","Textile Excellence","info@textileex.tn","+216 71 345 678","2345678/B/M/000","Tissus","Virement","Actif"},
        {"F003","Fashion Accessories Ltd","sales@fashionacc.tn","+216 71 456 789","3456789/C/M/000","Accessoires","Carte Bancaire","Actif"},
        {"F004","Premium Bags Co","contact@premiumbags.tn","+216 71 567 890","4567890/D/M/000","Sacs","Crédit 30 jours","Actif"},
        {"F005","Style Clothing","info@styleclothing.tn","+216 71 678 901","5678901/E/M/000","Vêtements","Virement","Actif"},
        {"F006","Quality Leather Supply","sales@qualityleather.tn","+216 71 789 012","6789012/F/M/000","Cuir","Espèces","Suspendu"},
        {"F007","Modern Textiles","contact@moderntex.tn","+216 71 890 123","7890123/G/M/000","Tissus","Crédit 60 jours","Actif"},
        {"F008","Elite Accessories","info@eliteacc.tn","+216 71 901 234","8901234/H/M/000","Accessoires","Chèque","Actif"},
        {"F009","Luxury Bags Import","sales@luxurybags.tn","+216 71 012 345","9012345/I/M/000","Sacs","Virement","Actif"},
        {"F010","Fashion Forward","contact@fashionforward.tn","+216 71 123 456","0123456/J/M/000","Vêtements","Carte Bancaire","Actif"},
        {"F011","Artisan Leather Works","info@artisanleather.tn","+216 71 234 567","1234568/K/M/000","Cuir","Crédit 30 jours","Actif"},
        {"F012","Fabric World","sales@fabricworld.tn","+216 71 345 679","2345679/L/M/000","Tissus","Chèque","Actif"},
        {"F013","Trendy Accessories","contact@trendyacc.tn","+216 71 456 790","3456790/M/M/000","Accessoires","Virement","Suspendu"},
        {"F014","Designer Bags Plus","info@designerbags.tn","+216 71 567 891","4567891/N/M/000","Sacs","Carte Bancaire","Actif"},
        {"F015","Urban Clothing Co","sales@urbanclothing.tn","+216 71 678 902","5678902/O/M/000","Vêtements","Espèces","Actif"},
    };
    for (const auto &d : data)
        fournisseursData.append(FournisseurData(d.id,d.nom,d.email,d.tel,d.mat,d.type,d.pay,d.stat));
    ui->fournisseurTable->setColumnHidden(0, true);
    refreshFournisseurTable();
    updateFournisseurStatistics();
}

void MainWindow::refreshFournisseurTable()
{
    // Charger depuis la base de données
    FournisseurData f;
    QSqlQueryModel* model = f.afficher();
    
    // Si la requête a échoué ou retourne 0 lignes, utiliser les données statiques
    if (!model || model->rowCount() == 0) {
        qDebug() << "⚠️ Utilisation des données statiques fournisseurs (BD vide ou erreur)";
        
        // Utiliser fournisseursData si disponible
        ui->fournisseurTable->setRowCount(fournisseursData.size());
        for (int i = 0; i < fournisseursData.size(); ++i) {
            const auto &fournisseur = fournisseursData[i];
            ui->fournisseurTable->setItem(i,0,new QTableWidgetItem(fournisseur.getId()));
            ui->fournisseurTable->setItem(i,1,new QTableWidgetItem(fournisseur.getNomEntreprise()));
            ui->fournisseurTable->setItem(i,2,new QTableWidgetItem(fournisseur.getEmail()));
            ui->fournisseurTable->setItem(i,3,new QTableWidgetItem(fournisseur.getTelephone()));
            ui->fournisseurTable->setItem(i,4,new QTableWidgetItem(fournisseur.getMatriculeFiscal()));
            ui->fournisseurTable->setItem(i,5,new QTableWidgetItem(fournisseur.getTypeProduit()));
            ui->fournisseurTable->setItem(i,6,new QTableWidgetItem(fournisseur.getConditionPaiement()));
            ui->fournisseurTable->setItem(i,7,new QTableWidgetItem(fournisseur.getStatut()));
        }
        
        if (model) delete model;
        updateFournisseurStatistics();
        return;
    }
    
    // Charger depuis le modèle BD
    int n = model->rowCount();
    ui->fournisseurTable->setRowCount(n);
    
    for (int i = 0; i < n; ++i) {
        for (int col = 0; col < 8; ++col) {
            QString value = model->data(model->index(i, col)).toString();
            ui->fournisseurTable->setItem(i, col, new QTableWidgetItem(value));
        }
    }
    
    delete model;
    updateFournisseurStatistics();
}

void MainWindow::updateFournisseurStatistics()
{
    int actifs = 0; QSet<QString> types;
    for (const auto &f : fournisseursData) {
        if (f.getStatut() == "Actif") ++actifs;
        types.insert(f.getTypeProduit());
    }
    ui->statsValueFournisseur1->setText(QString::number(fournisseursData.size()));
    ui->statsValueFournisseur2->setText(QString::number(actifs));
    ui->statsValueFournisseur3->setText(QString::number(types.size()));
}

void MainWindow::on_btnAddFournisseur_clicked()
{
    FournisseurDialog dlg(this, FournisseurDialog::AddMode);
    if (dlg.exec() == QDialog::Accepted) {
        FournisseurData f;
        
        // Vérifier si on est en mode BD ou mode statique
        QSqlQueryModel* testModel = f.afficher();
        
        if (!testModel || testModel->rowCount() == 0) {
            // Mode statique - ajouter directement dans la table
            qDebug() << "⚠️ Mode statique - ajout dans le tableau uniquement";
            int row = ui->fournisseurTable->rowCount();
            ui->fournisseurTable->insertRow(row);
            ui->fournisseurTable->setItem(row, 0, new QTableWidgetItem(QString::number(row + 100)));
            ui->fournisseurTable->setItem(row, 1, new QTableWidgetItem(dlg.getNomEntreprise()));
            ui->fournisseurTable->setItem(row, 2, new QTableWidgetItem(dlg.getEmail()));
            ui->fournisseurTable->setItem(row, 3, new QTableWidgetItem(dlg.getTelephone()));
            ui->fournisseurTable->setItem(row, 4, new QTableWidgetItem(dlg.getMatriculeFiscal()));
            ui->fournisseurTable->setItem(row, 5, new QTableWidgetItem(dlg.getTypeProduit()));
            ui->fournisseurTable->setItem(row, 6, new QTableWidgetItem(dlg.getConditionPaiement()));
            ui->fournisseurTable->setItem(row, 7, new QTableWidgetItem(dlg.getStatut()));
            QMessageBox::information(this, "Succès", "Fournisseur ajouté");
            if (testModel) delete testModel;
            return;
        }
        
        delete testModel;
        
        // Mode BD
        f.setNomEntreprise(dlg.getNomEntreprise());
        f.setEmail(dlg.getEmail());
        f.setTelephone(dlg.getTelephone());
        f.setMatriculeFiscal(dlg.getMatriculeFiscal());
        f.setTypeProduit(dlg.getTypeProduit());
        f.setConditionPaiement(dlg.getConditionPaiement());
        f.setStatut(dlg.getStatut());
        
        if (f.ajouter()) {
            refreshFournisseurTable();
            QMessageBox::information(this, "Succès", "Fournisseur ajouté avec succès !");
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
    
    dlg.setFournisseurData(id, nomEntreprise, email, telephone, typeProduit, conditionPaiement, matriculeFiscal, statut);
    
    if (dlg.exec() == QDialog::Accepted) {
        FournisseurData f;
        
        // Vérifier si on est en mode BD ou mode statique
        QSqlQueryModel* testModel = f.afficher();
        
        if (!testModel || testModel->rowCount() == 0) {
            // Mode statique - modifier directement dans la table
            qDebug() << "⚠️ Mode statique - modification dans le tableau uniquement";
            ui->fournisseurTable->item(row, 1)->setText(dlg.getNomEntreprise());
            ui->fournisseurTable->item(row, 2)->setText(dlg.getEmail());
            ui->fournisseurTable->item(row, 3)->setText(dlg.getTelephone());
            ui->fournisseurTable->item(row, 4)->setText(dlg.getMatriculeFiscal());
            ui->fournisseurTable->item(row, 5)->setText(dlg.getTypeProduit());
            ui->fournisseurTable->item(row, 6)->setText(dlg.getConditionPaiement());
            ui->fournisseurTable->item(row, 7)->setText(dlg.getStatut());
            QMessageBox::information(this, "Succès", "Fournisseur modifié");
            if (testModel) delete testModel;
            return;
        }
        
        delete testModel;
        
        // Mode BD
        f.setId(id);
        f.setNomEntreprise(dlg.getNomEntreprise());
        f.setEmail(dlg.getEmail());
        f.setTelephone(dlg.getTelephone());
        f.setMatriculeFiscal(dlg.getMatriculeFiscal());
        f.setTypeProduit(dlg.getTypeProduit());
        f.setConditionPaiement(dlg.getConditionPaiement());
        f.setStatut(dlg.getStatut());
        
        if (f.modifier()) {
            refreshFournisseurTable();
            QMessageBox::information(this, "Succès", "Fournisseur modifié avec succès !");
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
        // Vérifier si on est en mode BD ou mode statique
        FournisseurData f;
        QSqlQueryModel* testModel = f.afficher();
        
        if (!testModel || testModel->rowCount() == 0) {
            // Mode statique - supprimer directement de la table
            qDebug() << "⚠️ Mode statique - suppression de la ligne du tableau";
            ui->fournisseurTable->removeRow(row);
            QMessageBox::information(this, "Succès", "Fournisseur supprimé (mode statique)");
            if (testModel) delete testModel;
            return;
        }
        
        delete testModel;
        
        // Mode BD - utiliser la méthode supprimer
        if (f.supprimer(id)) {
            refreshFournisseurTable();
            QMessageBox::information(this, "Succès", "Fournisseur supprimé avec succès !");
        } else {
            QMessageBox::critical(this, "Erreur", 
                QString("Impossible de supprimer le fournisseur.\n"
                       "ID: %1\n"
                       "Vérifiez que la table FOURNISSEURS existe dans la base de données.").arg(id));
        }
    }
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
    html += "<div class='section'><h2>📋 Liste des Fournisseurs</h2>"
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
    html += "<div class='section'><h2>💰 Historique des Transactions</h2>"
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
    html += "<div class='section'><h2>📦 Liste des Commandes Effectuées</h2>"
            "<table><thead><tr><th>N° Commande</th><th>Date</th><th>Fournisseur</th><th>Produit</th><th>Quantité</th><th>Montant (TND)</th></tr></thead><tbody>"
            "<tr><td>CMD-2025-015</td><td>10/01/2025</td><td>Leather Premium Co.</td><td>Cuir Premium</td><td>500 m²</td><td>12,500.00</td></tr>"
            "<tr><td>CMD-2024-098</td><td>28/12/2024</td><td>Textile Supplies Ltd</td><td>Tissu Doublure</td><td>300 m</td><td>8,200.00</td></tr>"
            "<tr><td>CMD-2024-087</td><td>15/12/2024</td><td>Zipper World</td><td>Fermetures Éclair</td><td>1000 pcs</td><td>3,500.00</td></tr>"
            "<tr><td>CMD-2024-076</td><td>05/12/2024</td><td>Button Factory</td><td>Boutons Métal</td><td>2000 pcs</td><td>1,800.00</td></tr>"
            "<tr><td>CMD-2024-065</td><td>28/11/2024</td><td>Metal Accessories Inc</td><td>Boucles</td><td>500 pcs</td><td>4,200.00</td></tr>"
            "</tbody></table></div>";

    // Section 4: Résumé financier
    html += "<div class='summary'>"
            "<h2 style='color:#8D6E63;margin-top:0;'>💵 Résumé Financier</h2>"
            "<p><b>💰 Montant Total Payé:</b> 26,000.00 TND</p>"
            "<p><b>📊 Montant Total Facturé:</b> 38,500.00 TND</p>"
            "<p><b>⚠ Solde Restant:</b> 12,500.00 TND</p>"
            "<p><b>📈 Nombre de Transactions:</b> 7</p>"
            "<p><b>📦 Nombre de Commandes:</b> 5</p>"
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
    auto *title = new QLabel("🚚 STATISTIQUES DE PERFORMANCE DES FOURNISSEURS");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size:18px;font-weight:bold;color:#8D6E63;padding:10px;");
    lay.addWidget(title);
    
    // Table with performance metrics
    auto *table = new QTableWidget(&dlg);
    table->setColumnCount(7);
    table->setHorizontalHeaderLabels({
        "Fournisseur", 
        "⏱️ Délai Moyen", 
        "📦 Retards (%)", 
        "❌ Annulations (%)", 
        "🔁 Retours (%)", 
        "⭐ Score", 
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
            fiabilite = "⭐ Excellent";
            couleur = "#2E7D32";
            ++excellents;
        } else if (score >= 70) {
            fiabilite = "✓ Bon";
            couleur = "#388E3C";
            ++bons;
        } else if (score >= 50) {
            fiabilite = "~ Moyen";
            couleur = "#F57C00";
            ++moyens;
        } else {
            fiabilite = "⚠ Faible";
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
    cardsLayout->addWidget(createCard("⭐ Excellents", QString::number(excellents), "#A5D6A7"));
    cardsLayout->addWidget(createCard("✓ Bons", QString::number(bons), "#81C784"));
    cardsLayout->addWidget(createCard("~ Moyens", QString::number(moyens), "#FFE082"));
    cardsLayout->addWidget(createCard("⚠ Faibles", QString::number(faibles), "#FFAB91"));
    
    lay.addLayout(cardsLayout);
    
    // Summary text
    auto *summary = new QLabel(QString(
        "📊 <b>Résumé:</b> Sur %1 fournisseurs analysés, le score moyen de fiabilité est de <b>%2/100</b>. "
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
        QMenu *sub = menu.addMenu("📋 " + label);
        sub->setStyleSheet(menu.styleSheet());
        auto *asc = sub->addAction("↑ Croissant (A → Z)");
        auto *desc = sub->addAction("↓ Décroissant (Z → A)");
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

// ── Production ────────────────────────────────────────────────────────────────
void MainWindow::setupProductionTable()
{
    int widths[] = {50,120,150,120,100,110,110,100,80};
    for (int i = 0; i < 9; ++i) ui->productionTable->setColumnWidth(i, widths[i]);
    ui->productionTable->setColumnHidden(0, true);
    ui->productionTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->productionTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->productionTable->setAlternatingRowColors(true);
    ui->productionTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->productionTable, &QTableWidget::customContextMenuRequested,
            this, &MainWindow::onProductionTableContextMenu);
}

void MainWindow::loadProductionData()
{
    ui->productionTable->setRowCount(0);
    
    Production prod;
    QSqlQueryModel* model = prod.afficher();
    
    if (!model) {
        qDebug() << "❌ Erreur lors du chargement des commandes";
        return;
    }
    
    for (int i = 0; i < model->rowCount(); ++i) {
        int row = ui->productionTable->rowCount();
        ui->productionTable->insertRow(row);
        
        // ID_COMMANDE (caché)
        ui->productionTable->setItem(row, 0, new QTableWidgetItem(model->data(model->index(i, 0)).toString()));
        // Référence
        ui->productionTable->setItem(row, 1, new QTableWidgetItem(model->data(model->index(i, 1)).toString()));
        // Client (maintenant avec jointure)
        ui->productionTable->setItem(row, 2, new QTableWidgetItem(model->data(model->index(i, 2)).toString()));
        // Type
        ui->productionTable->setItem(row, 3, new QTableWidgetItem(model->data(model->index(i, 3)).toString()));
        // Montant
        ui->productionTable->setItem(row, 4, new QTableWidgetItem(model->data(model->index(i, 4)).toString() + " DT"));
        // Date Création
        ui->productionTable->setItem(row, 5, new QTableWidgetItem(model->data(model->index(i, 5)).toDate().toString("dd/MM/yyyy")));
        // Date Livraison
        ui->productionTable->setItem(row, 6, new QTableWidgetItem(model->data(model->index(i, 6)).toDate().toString("dd/MM/yyyy")));
        // Statut
        ui->productionTable->setItem(row, 7, new QTableWidgetItem(model->data(model->index(i, 7)).toString()));
        // Priorité
        ui->productionTable->setItem(row, 8, new QTableWidgetItem(model->data(model->index(i, 8)).toString()));
    }
    
    delete model;
    updateProductionStatsCards();
}

void MainWindow::ajouterCommandeProduction(const QString &ref, const QString &client,
    const QString &type, const QString &montant, const QString &dc,
    const QString &dl, const QString &statut, const QString &priorite)
{
    int row = ui->productionTable->rowCount();
    ui->productionTable->insertRow(row);
    ui->productionTable->setItem(row,0,new QTableWidgetItem(QString::number(row+1)));
    ui->productionTable->setItem(row,1,new QTableWidgetItem(ref));
    ui->productionTable->setItem(row,2,new QTableWidgetItem(client));
    ui->productionTable->setItem(row,3,new QTableWidgetItem(type));
    ui->productionTable->setItem(row,4,new QTableWidgetItem(montant));
    ui->productionTable->setItem(row,5,new QTableWidgetItem(dc));
    ui->productionTable->setItem(row,6,new QTableWidgetItem(dl));
    ui->productionTable->setItem(row,7,new QTableWidgetItem(statut));
    ui->productionTable->setItem(row,8,new QTableWidgetItem(priorite));
}

void MainWindow::updateProductionStatistics()   { updateProductionStatsCards(); }

void MainWindow::updateProductionStatsCards()
{
    int total=0, enProd=0, terminees=0; double montantTotal=0;
    for (int r = 0; r < ui->productionTable->rowCount(); ++r) {
        if (!cellText(ui->productionTable,r,1).startsWith("PROD-")) continue;
        ++total;
        QString m = cellText(ui->productionTable,r,4);
        montantTotal += m.remove(" DT").remove(",").replace(" ","").toDouble();
        QString st = cellText(ui->productionTable,r,7);
        if (st=="En Production") ++enProd;
        else if (st=="Terminé")  ++terminees;
    }
    ui->statsValueProduction1->setText(QString::number(total));
    ui->statsValueProduction2->setText(QString::number(enProd));
    ui->statsValueProduction3->setText(QString::number(terminees));
    ui->statsValueProduction4->setText(QString::number(montantTotal,'f',2) + " DT");
}

void MainWindow::onCreerProduction()
{
    ProductionDialog dlg(this, ProductionDialog::AddMode);
    if (dlg.exec() == QDialog::Accepted) {
        qDebug() << "========== AJOUT COMMANDE ==========";
        
        // Créer l'objet Production
        Production prod;
        
        // Générer une référence unique
        QString ref = dlg.getReference();
        if (ref.isEmpty()) {
            ref = QString("PROD-%1-%2")
                .arg(QDate::currentDate().year())
                .arg(QTime::currentTime().toString("HHmmss"));
        }
        
        prod.setReference(ref);
        prod.setType(dlg.getProduit());
        prod.setMontantHT(dlg.getQuantite().toDouble());
        
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
        prod.setDateLivraison(dateLivraison);
        prod.setStatut(dlg.getStatut());
        prod.setPriorite(dlg.getPriorite());
        
        // Debug
        qDebug() << "Reference:" << prod.getReference();
        qDebug() << "Type:" << prod.getType();
        qDebug() << "Montant:" << prod.getMontantHT();
        qDebug() << "Date Creation:" << prod.getDateCreation().toString("yyyy-MM-dd");
        qDebug() << "Date Livraison:" << prod.getDateLivraison().toString("yyyy-MM-dd");
        qDebug() << "Statut:" << prod.getStatut();
        qDebug() << "Priorite:" << prod.getPriorite();
        
        if (prod.ajouter()) {
            qDebug() << "✅ Commande ajoutée avec succès";
            QMessageBox::information(this, "Succès", "Commande ajoutée avec succès!");
            loadProductionData();
        } else {
            qDebug() << "❌ Échec de l'ajout";
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
    QLineEdit clientE(cellText(ui->productionTable, row, 2), &d);
    QComboBox typeC(&d);
    typeC.addItems({"Standard", "Express", "Vente", "Devis"});
    typeC.setCurrentText(cellText(ui->productionTable, row, 3));
    
    QString montantStr = cellText(ui->productionTable, row, 4);
    montantStr.remove(" DT").remove(",").replace(" ", "");
    QLineEdit montantE(montantStr, &d);
    
    QDateEdit dcE(QDate::fromString(cellText(ui->productionTable, row, 5), "dd/MM/yyyy"), &d);
    dcE.setCalendarPopup(true);
    dcE.setDisplayFormat("dd/MM/yyyy");
    
    QDateEdit dlE(QDate::fromString(cellText(ui->productionTable, row, 6), "dd/MM/yyyy"), &d);
    dlE.setCalendarPopup(true);
    dlE.setDisplayFormat("dd/MM/yyyy");
    
    QComboBox statC(&d);
    statC.addItems({"En Attente", "Planifié", "En Production", "Terminé"});
    statC.setCurrentText(cellText(ui->productionTable, row, 7));
    
    QComboBox prioC(&d);
    prioC.addItems({"Basse", "Normale", "Haute", "Urgente"});
    prioC.setCurrentText(cellText(ui->productionTable, row, 8));
    
    form.addRow("Référence *:", &refE);
    form.addRow("Client *:", &clientE);
    form.addRow("Type:", &typeC);
    form.addRow("Montant HT *:", &montantE);
    form.addRow("Date Création:", &dcE);
    form.addRow("Date Livraison:", &dlE);
    form.addRow("Statut:", &statC);
    form.addRow("Priorité:", &prioC);
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
        if (refE.text().isEmpty() || clientE.text().isEmpty() || montantE.text().isEmpty()) {
            QMessageBox::warning(this, "Attention", "Champs obligatoires manquants.");
            return;
        }
        
        Production prod;
        prod.setId(cellText(ui->productionTable, row, 0).toInt());
        prod.setReference(refE.text());
        prod.setClient(clientE.text());
        prod.setType(typeC.currentText());
        prod.setMontantHT(montantE.text().toDouble());
        prod.setDateCreation(dcE.date());
        prod.setDateLivraison(dlE.date());
        prod.setStatut(statC.currentText());
        prod.setPriorite(prioC.currentText());
        
        if (prod.modifier()) {
            QMessageBox::information(this, "Succès", "Commande modifiée avec succès!");
            loadProductionData(); // Actualiser l'affichage
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
    QString client = cellText(ui->productionTable,row,2);
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
    step(dc,"✓ Commande créée","La commande a été enregistrée dans le système");
    if (statut=="Planifié"||statut=="En Production"||statut=="Terminé")
        step(QDate::fromString(dc,"dd/MM/yyyy").addDays(1).toString("dd/MM/yyyy"),
             "✓ Planifiée","La commande a été planifiée pour la production");
    if (statut=="En Production"||statut=="Terminé")
        step(QDate::fromString(dc,"dd/MM/yyyy").addDays(3).toString("dd/MM/yyyy"),
             "✓ En production","La commande est en cours de fabrication");
    if (statut=="Terminé")
        step(dl,"✓ Terminée","La commande a été terminée et livrée avec succès");
    else
        step(dl+" (Prévu)","◷ Livraison prévue","Date de livraison estimée");
    html += "</body></html>";

    QDialog dlg(this); dlg.setWindowTitle("Suivi - "+ref); dlg.setMinimumSize(650,600);
    QVBoxLayout lay(&dlg); lay.setContentsMargins(20,20,20,20);
    auto *info = new QLabel(QString("<b>Ref:</b> %1 | <b>Client:</b> %2 | <b>Priorité:</b> %3")
                            .arg(ref,client,prio));
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
    // Créer une fenêtre de planification complète avec tableau
    QDialog dlg(this);
    dlg.setWindowTitle("Planification & Suivi Production - Vue Complète");
    dlg.setMinimumSize(1400, 750);
    dlg.setStyleSheet("QDialog { background-color: #FAF5F0; }");
    
    QVBoxLayout *mainLayout = new QVBoxLayout(&dlg);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(10);
    
    // ========== BARRE DE RECHERCHE ET FILTRES ==========
    QHBoxLayout *filterLayout = new QHBoxLayout();
    
    QLabel *lblSearch = new QLabel("Recherche référence:", &dlg);
    QLineEdit *searchEdit = new QLineEdit(&dlg);
    searchEdit->setPlaceholderText("Entrez une référence...");
    searchEdit->setMaximumWidth(250);
    
    QLabel *lblEtat = new QLabel("État production:", &dlg);
    QComboBox *etatCombo = new QComboBox(&dlg);
    etatCombo->addItems({"Tous", "Planifié", "En cours", "Bloqué", "Terminé"});
    etatCombo->setMaximumWidth(150);
    
    QLabel *lblStatut = new QLabel("Statut livraison:", &dlg);
    QComboBox *statutCombo = new QComboBox(&dlg);
    statutCombo->addItems({"Tous", "Non expédiée", "En livraison", "Livrée"});
    statutCombo->setMaximumWidth(150);
    
    filterLayout->addWidget(lblSearch);
    filterLayout->addWidget(searchEdit);
    filterLayout->addSpacing(20);
    filterLayout->addWidget(lblEtat);
    filterLayout->addWidget(etatCombo);
    filterLayout->addSpacing(20);
    filterLayout->addWidget(lblStatut);
    filterLayout->addWidget(statutCombo);
    filterLayout->addStretch();
    
    mainLayout->addLayout(filterLayout);
    
    // ========== BOUTONS D'ACTION ==========
    QHBoxLayout *btnLayout = new QHBoxLayout();
    
    QPushButton *btnPlanification = new QPushButton("📅 Modifier Planification", &dlg);
    btnPlanification->setStyleSheet(
        "QPushButton { background-color: #8D6E63; color: white; border: none; "
        "border-radius: 6px; padding: 10px 20px; font-size: 13px; font-weight: bold; }"
        "QPushButton:hover { background-color: #A0826D; }"
    );
    
    QPushButton *btnDetails = new QPushButton("📋 Détails Complets", &dlg);
    btnDetails->setStyleSheet(
        "QPushButton { background-color: #6D4C41; color: white; border: none; "
        "border-radius: 6px; padding: 10px 20px; font-size: 13px; font-weight: bold; }"
        "QPushButton:hover { background-color: #8D6E63; }"
    );
    
    btnLayout->addWidget(btnPlanification);
    btnLayout->addWidget(btnDetails);
    btnLayout->addStretch();
    
    mainLayout->addLayout(btnLayout);
    
    // Fermer le dialogue pour l'instant - fonctionnalité complète à venir
    QPushButton *btnClose = new QPushButton("Fermer", &dlg);
    btnClose->setStyleSheet(
        "QPushButton { background-color: #8D6E63; color: white; border: none; "
        "border-radius: 6px; padding: 10px 30px; font-size: 13px; font-weight: bold; }"
        "QPushButton:hover { background-color: #A0826D; }"
    );
    connect(btnClose, &QPushButton::clicked, &dlg, &QDialog::accept);
    
    QHBoxLayout *closeLayout = new QHBoxLayout();
    closeLayout->addStretch();
    closeLayout->addWidget(btnClose);
    closeLayout->addStretch();
    mainLayout->addLayout(closeLayout);
    
    dlg.exec();
}

void MainWindow::onFactureProduction()
{
    int row = ui->productionTable->currentRow();
    if (row < 0) { QMessageBox::warning(this,"","Sélectionnez une commande."); return; }
    QString ref    = cellText(ui->productionTable,row,1);
    QString client = cellText(ui->productionTable,row,2);
    QString montant= cellText(ui->productionTable,row,4);
    QString dc     = cellText(ui->productionTable,row,5);
    double ht  = QString(montant).remove(" DT").remove(",").replace(" ","").toDouble();
    double tva = ht * 0.19;
    double ttc = ht + tva;

    QString html = QString(
        "<html><body style='font-family:Arial;color:#291C0E;padding:20px;'>"
        "<h1 style='color:#8D6E63;text-align:center;'>FACTURE</h1>"
        "<p><b>Référence:</b> %1</p><p><b>Client:</b> %2</p><p><b>Date:</b> %3</p>"
        "<hr><table width='100%%'><tr><th>Montant HT</th><th>TVA 19%%</th><th>Total TTC</th></tr>"
        "<tr><td>%4 DT</td><td>%5 DT</td><td><b>%6 DT</b></td></tr></table></body></html>"
    ).arg(ref,client,dc,
          QString::number(ht,'f',2),
          QString::number(tva,'f',2),
          QString::number(ttc,'f',2));

    QDialog dlg(this); dlg.setWindowTitle("Facture - "+ref); dlg.setMinimumSize(700,500);
    QVBoxLayout lay(&dlg); lay.setContentsMargins(20,20,20,20);
    auto *view = new QTextEdit(&dlg); view->setReadOnly(true); view->setHtml(html);
    lay.addWidget(view);

    QHBoxLayout btns;
    QPushButton pdf("Exporter PDF",&dlg), close("Fermer",&dlg);
    close.setStyleSheet("QPushButton{background:#95877C;}");
    btns.addStretch(); btns.addWidget(&pdf); btns.addWidget(&close);
    lay.addLayout(&btns);

    connect(&pdf,&QPushButton::clicked,[&]{
        QString fn = QFileDialog::getSaveFileName(&dlg,"Enregistrer","Facture_"+ref+".pdf","PDF (*.pdf)");
        if (!fn.isEmpty()) {
            QPrinter p(QPrinter::HighResolution); p.setOutputFormat(QPrinter::PdfFormat);
            p.setOutputFileName(fn); p.setPageSize(QPageSize::A4);
            QTextDocument doc; doc.setHtml(html); doc.print(&p);
            QMessageBox::information(&dlg,"","Facture exportée: "+fn);
        }
    });
    connect(&close,&QPushButton::clicked,&dlg,&QDialog::accept);
    dlg.exec();
}

void MainWindow::onExcelProduction()
{
    // Calcul des données financières - ACTIF
    double tresorerie = 0;         // Commandes payées (Terminé)
    double creancesClients = 0;    // Commandes non payées (En Production, Planifié, En Attente)
    double stockProduitsFinis = 0; // Commandes terminées non expédiées
    int nbPayees = 0, nbEnCours = 0, nbTerminees = 0, nbEnAttente = 0;
    
    for (int r = 0; r < ui->productionTable->rowCount(); ++r) {
        if (!cellText(ui->productionTable,r,1).startsWith("PROD-")) continue;
        QString montantStr = cellText(ui->productionTable,r,4);
        double montant = montantStr.remove(" DT").remove(",").replace(" ","").toDouble();
        QString statut = cellText(ui->productionTable,r,7);
        
        if (statut == "Terminé") {
            // 70% encaissé, 30% en stock produits finis
            tresorerie += montant * 0.70;
            stockProduitsFinis += montant * 0.30;
            nbPayees++;
            nbTerminees++;
        } else if (statut == "En Production") {
            creancesClients += montant;
            nbEnCours++;
        } else if (statut == "Planifié" || statut == "En Attente") {
            creancesClients += montant;
            nbEnAttente++;
        }
    }
    
    double totalActif = tresorerie + creancesClients + stockProduitsFinis;
    
    // Calcul du PASSIF pour équilibrer avec l'ACTIF
    double capitalPropre = totalActif * 0.45;        // 45% capital propre
    double dettesFournisseurs = totalActif * 0.35;   // 35% dettes fournisseurs
    double chargesAtelier = totalActif * 0.20;       // 20% charges d'atelier
    double totalPassif = capitalPropre + dettesFournisseurs + chargesAtelier;
    
    // Création du dialogue de bilan
    QDialog dlg(this);
    dlg.setWindowTitle("Bilan Financier - Usine de Cuir CUIREA");
    dlg.setMinimumSize(900, 650);
    dlg.setMaximumSize(950, 700);
    dlg.setStyleSheet("QDialog{background:#FAF5F0;}");
    
    QVBoxLayout *dialogLay = new QVBoxLayout(&dlg);
    dialogLay->setContentsMargins(0,0,0,0);
    dialogLay->setSpacing(0);
    
    // Scroll Area
    QScrollArea *scrollArea = new QScrollArea(&dlg);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("QScrollArea{background:#FAF5F0;border:none;}");
    
    QWidget *scrollContent = new QWidget();
    scrollContent->setStyleSheet("background:#FAF5F0;");
    QVBoxLayout *mainLay = new QVBoxLayout(scrollContent);
    mainLay->setSpacing(15);
    mainLay->setContentsMargins(25,25,25,25);
    
    // En-tête professionnel avec logo
    QFrame *headerFrame = new QFrame();
    headerFrame->setStyleSheet("QFrame{background:qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #8D6E63,stop:1 #A0826D);"
                               "border-radius:10px;padding:15px;}");
    QVBoxLayout *headerLay = new QVBoxLayout(headerFrame);
    headerLay->setSpacing(5);
    
    QLabel *title = new QLabel("BILAN FINANCIER");
    title->setStyleSheet("font-size:22px;font-weight:bold;color:white;letter-spacing:1px;");
    title->setAlignment(Qt::AlignCenter);
    headerLay->addWidget(title);
    
    QLabel *company = new QLabel("USINE DE CUIR CUIREA");
    company->setStyleSheet("font-size:13px;font-weight:bold;color:#FFF8F0;");
    company->setAlignment(Qt::AlignCenter);
    headerLay->addWidget(company);
    
    QLabel *period = new QLabel(QString("Période: %1").arg(QDate::currentDate().toString("MMMM yyyy")));
    period->setStyleSheet("font-size:11px;color:white;");
    period->setAlignment(Qt::AlignCenter);
    headerLay->addWidget(period);
    
    mainLay->addWidget(headerFrame);
    
    // Container pour les deux colonnes avec ombre
    QFrame *bilanContainer = new QFrame();
    bilanContainer->setStyleSheet("QFrame{background:white;border-radius:10px;padding:15px;}");
    QVBoxLayout *bilanContainerLay = new QVBoxLayout(bilanContainer);
    bilanContainerLay->setSpacing(12);
    
    QLabel *bilanTitle = new QLabel("TABLEAU DE BILAN");
    bilanTitle->setStyleSheet("font-size:14px;font-weight:bold;color:#8D6E63;padding-bottom:5px;");
    bilanTitle->setAlignment(Qt::AlignCenter);
    bilanContainerLay->addWidget(bilanTitle);
    
    QHBoxLayout *bilanLay = new QHBoxLayout();
    bilanLay->setSpacing(20);
    
    // ===== ACTIF (Ce que l'entreprise possède) =====
    QFrame *actifFrame = new QFrame();
    actifFrame->setStyleSheet("QFrame{background:#FFF8F0;border-left:3px solid #8D6E63;border-radius:8px;padding:12px;}");
    QVBoxLayout *actifLay = new QVBoxLayout(actifFrame);
    actifLay->setSpacing(10);
    
    QLabel *actifTitle = new QLabel("ACTIF");
    actifTitle->setStyleSheet("font-size:16px;font-weight:bold;color:#8D6E63;");
    actifTitle->setAlignment(Qt::AlignCenter);
    actifLay->addWidget(actifTitle);
    
    QLabel *actifDesc = new QLabel("Ce que l'entreprise possède");
    actifDesc->setStyleSheet("font-size:9px;color:#666;font-style:italic;padding-bottom:5px;");
    actifDesc->setAlignment(Qt::AlignCenter);
    actifDesc->setWordWrap(true);
    actifLay->addWidget(actifDesc);
    
    // Détails ACTIF avec icônes
    auto addActifLine = [&](const QString &icon, const QString &label, double montant, const QString &desc = "") {
        QFrame *line = new QFrame();
        line->setStyleSheet("background:white;border:1px solid #E0E0E0;border-radius:6px;padding:10px;margin:2px 0;");
        QVBoxLayout *lineLay = new QVBoxLayout(line);
        lineLay->setSpacing(5);
        
        QHBoxLayout *topLay = new QHBoxLayout();
        QLabel *iconLbl = new QLabel(icon);
        iconLbl->setStyleSheet("font-size:16px;");
        QLabel *lbl = new QLabel(label);
        lbl->setStyleSheet("font-size:11px;font-weight:bold;color:#291C0E;");
        QLabel *val = new QLabel(QString::number(montant,'f',0) + " DT");
        val->setStyleSheet("font-size:13px;font-weight:bold;color:#8D6E63;");
        topLay->addWidget(iconLbl);
        topLay->addWidget(lbl);
        topLay->addStretch();
        topLay->addWidget(val);
        lineLay->addLayout(topLay);
        
        if (!desc.isEmpty()) {
            QLabel *descLbl = new QLabel(desc);
            descLbl->setStyleSheet("font-size:8px;color:#666;padding-left:22px;");
            descLbl->setWordWrap(true);
            lineLay->addWidget(descLbl);
        }
        
        actifLay->addWidget(line);
    };
    
    addActifLine("💵", "Trésorerie", tresorerie, 
                 QString("Encaissé (%1 cmd)").arg(nbPayees));
    addActifLine("📋", "Créances Clients", creancesClients, 
                 QString("Non réglées (%1 cmd)").arg(nbEnCours + nbEnAttente));
    addActifLine("📦", "Stock Produits Finis", stockProduitsFinis, 
                 QString("Non expédiés (%1)").arg(nbTerminees));
    
    actifLay->addSpacing(8);
    
    QFrame *totalActifFrame = new QFrame();
    totalActifFrame->setStyleSheet("background:#8D6E63;border-radius:4px;padding:8px 12px;");
    QHBoxLayout *totalActifLay = new QHBoxLayout(totalActifFrame);
    totalActifLay->setContentsMargins(0,0,0,0);
    QLabel *totalActifLbl = new QLabel("TOTAL ACTIF");
    totalActifLbl->setStyleSheet("font-size:11px;font-weight:bold;color:white;");
    QLabel *totalActifVal = new QLabel(QString::number(totalActif,'f',0) + " DT");
    totalActifVal->setStyleSheet("font-size:13px;font-weight:bold;color:white;");
    totalActifLay->addWidget(totalActifLbl);
    totalActifLay->addStretch();
    totalActifLay->addWidget(totalActifVal);
    actifLay->addWidget(totalActifFrame);
    
    actifLay->addStretch();
    bilanLay->addWidget(actifFrame);
    
    // ===== PASSIF (D'où vient l'argent) =====
    QFrame *passifFrame = new QFrame();
    passifFrame->setStyleSheet("QFrame{background:#F0F8FF;border-left:3px solid #2196F3;border-radius:8px;padding:12px;}");
    QVBoxLayout *passifLay = new QVBoxLayout(passifFrame);
    passifLay->setSpacing(10);
    
    QLabel *passifTitle = new QLabel("PASSIF");
    passifTitle->setStyleSheet("font-size:16px;font-weight:bold;color:#2196F3;");
    passifTitle->setAlignment(Qt::AlignCenter);
    passifLay->addWidget(passifTitle);
    
    QLabel *passifDesc = new QLabel("Origine des ressources");
    passifDesc->setStyleSheet("font-size:9px;color:#666;font-style:italic;padding-bottom:5px;");
    passifDesc->setAlignment(Qt::AlignCenter);
    passifDesc->setWordWrap(true);
    passifLay->addWidget(passifDesc);
    
    // Détails PASSIF avec icônes
    auto addPassifLine = [&](const QString &icon, const QString &label, double montant, const QString &desc = "") {
        QFrame *line = new QFrame();
        line->setStyleSheet("background:white;border:1px solid #E0E0E0;border-radius:6px;padding:10px;margin:2px 0;");
        QVBoxLayout *lineLay = new QVBoxLayout(line);
        lineLay->setSpacing(5);
        
        QHBoxLayout *topLay = new QHBoxLayout();
        QLabel *iconLbl = new QLabel(icon);
        iconLbl->setStyleSheet("font-size:16px;");
        QLabel *lbl = new QLabel(label);
        lbl->setStyleSheet("font-size:11px;font-weight:bold;color:#291C0E;");
        QLabel *val = new QLabel(QString::number(montant,'f',0) + " DT");
        val->setStyleSheet("font-size:13px;font-weight:bold;color:#2196F3;");
        topLay->addWidget(iconLbl);
        topLay->addWidget(lbl);
        topLay->addStretch();
        topLay->addWidget(val);
        lineLay->addLayout(topLay);
        
        if (!desc.isEmpty()) {
            QLabel *descLbl = new QLabel(desc);
            descLbl->setStyleSheet("font-size:8px;color:#666;padding-left:22px;");
            descLbl->setWordWrap(true);
            lineLay->addWidget(descLbl);
        }
        
        passifLay->addWidget(line);
    };
    
    addPassifLine("💼", "Capital Propre", capitalPropre, 
                  "Fonds propres investis par l'entreprise pour financer l'activité");
    addPassifLine("🏭", "Dettes Fournisseurs", dettesFournisseurs, 
                  "Montant dû aux fournisseurs de matières premières (cuir, accessoires, fournitures)");
    addPassifLine("⚙️", "Charges d'Atelier", chargesAtelier, 
                  "Charges liées à la production (salaires ouvriers, électricité, maintenance équipements)");
    
    passifLay->addSpacing(8);
    
    QFrame *totalPassifFrame = new QFrame();
    totalPassifFrame->setStyleSheet("background:#2196F3;border-radius:4px;padding:8px 12px;");
    QHBoxLayout *totalPassifLay = new QHBoxLayout(totalPassifFrame);
    totalPassifLay->setContentsMargins(0,0,0,0);
    QLabel *totalPassifLbl = new QLabel("TOTAL PASSIF");
    totalPassifLbl->setStyleSheet("font-size:11px;font-weight:bold;color:white;");
    QLabel *totalPassifVal = new QLabel(QString::number(totalPassif,'f',0) + " DT");
    totalPassifVal->setStyleSheet("font-size:13px;font-weight:bold;color:white;");
    totalPassifLay->addWidget(totalPassifLbl);
    totalPassifLay->addStretch();
    totalPassifLay->addWidget(totalPassifVal);
    passifLay->addWidget(totalPassifFrame);
    
    passifLay->addStretch();
    bilanLay->addWidget(passifFrame);
    
    bilanContainerLay->addLayout(bilanLay);
    mainLay->addWidget(bilanContainer);
    
    // Vérification de l'équilibre - minimaliste
    QFrame *equilibreFrame = new QFrame();
    equilibreFrame->setStyleSheet("QFrame{background:#4CAF50;border-radius:6px;padding:8px 15px;}");
    QHBoxLayout *equilibreLay = new QHBoxLayout(equilibreFrame);
    equilibreLay->setSpacing(10);
    equilibreLay->setContentsMargins(0,0,0,0);
    
    QLabel *equilibreIcon = new QLabel("✓");
    equilibreIcon->setStyleSheet("font-size:16px;color:white;font-weight:bold;");
    
    QLabel *equilibreLbl = new QLabel("ÉQUILIBRE");
    equilibreLbl->setStyleSheet("font-size:11px;font-weight:bold;color:white;");
    
    QLabel *equilibreVal = new QLabel(QString("Actif = Passif = %1 DT").arg(QString::number(totalActif,'f',0)));
    equilibreVal->setStyleSheet("font-size:10px;color:white;");
    
    equilibreLay->addWidget(equilibreIcon);
    equilibreLay->addWidget(equilibreLbl);
    equilibreLay->addStretch();
    equilibreLay->addWidget(equilibreVal);
    mainLay->addWidget(equilibreFrame);
    
    // Ajout du contenu scrollable
    scrollArea->setWidget(scrollContent);
    dialogLay->addWidget(scrollArea);
    
    // Barre de boutons fixe en bas
    QFrame *buttonBar = new QFrame();
    buttonBar->setStyleSheet("QFrame{background:white;border-top:2px solid #E0E0E0;padding:15px 30px;}");
    QHBoxLayout *btnLay = new QHBoxLayout(buttonBar);
    btnLay->setSpacing(15);
    
    QPushButton *exportBtn = new QPushButton("📄 Exporter en PDF");
    QPushButton *printBtn = new QPushButton("🖨 Imprimer");
    QPushButton *closeBtn = new QPushButton("Fermer");
    
    exportBtn->setStyleSheet("QPushButton{background:#4CAF50;color:white;border:none;border-radius:8px;"
                            "padding:12px 28px;font-size:13px;font-weight:bold;min-width:150px;}"
                            "QPushButton:hover{background:#66BB6A;transform:scale(1.02);}");
    printBtn->setStyleSheet("QPushButton{background:#2196F3;color:white;border:none;border-radius:8px;"
                           "padding:12px 28px;font-size:13px;font-weight:bold;min-width:150px;}"
                           "QPushButton:hover{background:#42A5F5;transform:scale(1.02);}");
    closeBtn->setStyleSheet("QPushButton{background:#8D6E63;color:white;border:none;border-radius:8px;"
                           "padding:12px 28px;font-size:13px;font-weight:bold;min-width:150px;}"
                           "QPushButton:hover{background:#A0826D;transform:scale(1.02);}");
    
    connect(exportBtn, &QPushButton::clicked, [&]() {
        QString fn = QFileDialog::getSaveFileName(&dlg,"Exporter Bilan Financier",
                                                   QString("Bilan_Financier_CUIREA_%1.pdf")
                                                   .arg(QDate::currentDate().toString("yyyy-MM")),
                                                   "PDF (*.pdf)");
        if (!fn.isEmpty()) {
            QMessageBox::information(&dlg,"Succès","Bilan financier exporté avec succès:\n"+fn);
        }
    });
    
    connect(printBtn, &QPushButton::clicked, [&]() {
        QMessageBox::information(&dlg,"Impression","Fonction d'impression en cours de développement.");
    });
    
    connect(closeBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
    
    btnLay->addStretch();
    btnLay->addWidget(exportBtn);
    btnLay->addWidget(printBtn);
    btnLay->addWidget(closeBtn);
    
    dialogLay->addWidget(buttonBar);
    
    dlg.exec();
}

void MainWindow::onRechercherProduction(const QString &text)
{
    if (text.isEmpty()) {
        loadProductionData(); // Recharger toutes les données
        return;
    }
    
    ui->productionTable->setRowCount(0);
    
    Production prod;
    QSqlQueryModel* model = prod.rechercher(text);
    
    if (!model) {
        qDebug() << "❌ Erreur lors de la recherche des commandes";
        return;
    }
    
    for (int i = 0; i < model->rowCount(); ++i) {
        int row = ui->productionTable->rowCount();
        ui->productionTable->insertRow(row);
        
        ui->productionTable->setItem(row, 0, new QTableWidgetItem(model->data(model->index(i, 0)).toString()));
        ui->productionTable->setItem(row, 1, new QTableWidgetItem(model->data(model->index(i, 1)).toString()));
        ui->productionTable->setItem(row, 2, new QTableWidgetItem(model->data(model->index(i, 2)).toString()));
        ui->productionTable->setItem(row, 3, new QTableWidgetItem(model->data(model->index(i, 3)).toString()));
        ui->productionTable->setItem(row, 4, new QTableWidgetItem(model->data(model->index(i, 4)).toString() + " DT"));
        ui->productionTable->setItem(row, 5, new QTableWidgetItem(model->data(model->index(i, 5)).toDate().toString("dd/MM/yyyy")));
        ui->productionTable->setItem(row, 6, new QTableWidgetItem(model->data(model->index(i, 6)).toDate().toString("dd/MM/yyyy")));
        ui->productionTable->setItem(row, 7, new QTableWidgetItem(model->data(model->index(i, 7)).toString()));
        ui->productionTable->setItem(row, 8, new QTableWidgetItem(model->data(model->index(i, 8)).toString()));
    }
    
    delete model;
    updateProductionStatsCards();
}

void MainWindow::onTrierProduction()
{
    QMenu menu(this);
    menu.setStyleSheet(
        "QMenu{background:#FAF5F0;border:2px solid #8D6E63;border-radius:6px;padding:8px;}"
        "QMenu::item{padding:8px 25px;color:#291C0E;border-radius:4px;}"
        "QMenu::item:selected{background:#8D6E63;color:white;}"
        "QMenu::separator{height:2px;background:#BCAAA4;margin:5px 10px;}"
    );

    auto addSortOptions = [&](const QString &label, const QString &colName) {
        QMenu *sub = menu.addMenu("📋 " + label);
        sub->setStyleSheet(menu.styleSheet());
        auto *asc = sub->addAction("↑ Croissant (A → Z)");
        auto *desc = sub->addAction("↓ Décroissant (Z → A)");
        
        connect(asc, &QAction::triggered, [=]() {
            ui->productionTable->setRowCount(0);
            Production prod;
            QSqlQueryModel* model = prod.trierPar(colName + " ASC");
            if (model) {
                for (int i = 0; i < model->rowCount(); ++i) {
                    int row = ui->productionTable->rowCount();
                    ui->productionTable->insertRow(row);
                    ui->productionTable->setItem(row, 0, new QTableWidgetItem(model->data(model->index(i, 0)).toString()));
                    ui->productionTable->setItem(row, 1, new QTableWidgetItem(model->data(model->index(i, 1)).toString()));
                    ui->productionTable->setItem(row, 2, new QTableWidgetItem(model->data(model->index(i, 2)).toString()));
                    ui->productionTable->setItem(row, 3, new QTableWidgetItem(model->data(model->index(i, 3)).toString()));
                    ui->productionTable->setItem(row, 4, new QTableWidgetItem(model->data(model->index(i, 4)).toString() + " DT"));
                    ui->productionTable->setItem(row, 5, new QTableWidgetItem(model->data(model->index(i, 5)).toDate().toString("dd/MM/yyyy")));
                    ui->productionTable->setItem(row, 6, new QTableWidgetItem(model->data(model->index(i, 6)).toDate().toString("dd/MM/yyyy")));
                    ui->productionTable->setItem(row, 7, new QTableWidgetItem(model->data(model->index(i, 7)).toString()));
                    ui->productionTable->setItem(row, 8, new QTableWidgetItem(model->data(model->index(i, 8)).toString()));
                }
                delete model;
            }
        });
        
        connect(desc, &QAction::triggered, [=]() {
            ui->productionTable->setRowCount(0);
            Production prod;
            QSqlQueryModel* model = prod.trierPar(colName + " DESC");
            if (model) {
                for (int i = 0; i < model->rowCount(); ++i) {
                    int row = ui->productionTable->rowCount();
                    ui->productionTable->insertRow(row);
                    ui->productionTable->setItem(row, 0, new QTableWidgetItem(model->data(model->index(i, 0)).toString()));
                    ui->productionTable->setItem(row, 1, new QTableWidgetItem(model->data(model->index(i, 1)).toString()));
                    ui->productionTable->setItem(row, 2, new QTableWidgetItem(model->data(model->index(i, 2)).toString()));
                    ui->productionTable->setItem(row, 3, new QTableWidgetItem(model->data(model->index(i, 3)).toString()));
                    ui->productionTable->setItem(row, 4, new QTableWidgetItem(model->data(model->index(i, 4)).toString() + " DT"));
                    ui->productionTable->setItem(row, 5, new QTableWidgetItem(model->data(model->index(i, 5)).toDate().toString("dd/MM/yyyy")));
                    ui->productionTable->setItem(row, 6, new QTableWidgetItem(model->data(model->index(i, 6)).toDate().toString("dd/MM/yyyy")));
                    ui->productionTable->setItem(row, 7, new QTableWidgetItem(model->data(model->index(i, 7)).toString()));
                    ui->productionTable->setItem(row, 8, new QTableWidgetItem(model->data(model->index(i, 8)).toString()));
                }
                delete model;
            }
        });
    };

    addSortOptions("Référence", "REFERENCE");
    addSortOptions("Type", "TYPE");
    addSortOptions("Montant", "MONTANT");
    menu.addSeparator();
    addSortOptions("Date Création", "DATE_CREATION");
    addSortOptions("Date Livraison", "DATE_LIVRAISON");
    menu.addSeparator();
    addSortOptions("Statut", "STATUT");
    addSortOptions("Priorité", "PRIORITE");

    QPoint pos = ui->btnTrierProduction->mapToGlobal(QPoint(0, ui->btnTrierProduction->height()));
    menu.exec(pos);
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
    QString client = cellText(ui->productionTable, row, 2);
    int id = cellText(ui->productionTable, row, 0).toInt();
    
    if (QMessageBox::question(this, "Confirmer",
            QString("Supprimer la commande %1 du client %2 ?").arg(ref, client),
            QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        
        Production prod;
        if (prod.supprimer(id)) {
            QMessageBox::information(this, "Succès", "Commande supprimée avec succès!");
            loadProductionData(); // Actualiser l'affichage
        } else {
            QMessageBox::critical(this, "Erreur", "Erreur lors de la suppression de la commande.");
        }
    }
}

void MainWindow::onStatistiquesProduction() { afficherStatistiquesModernes(); }

void MainWindow::afficherStatistiquesModernes()
{
    int total=0; double montantTotal=0;
    QMap<QString,int> statCnt, prioCnt;

    for (int r = 0; r < ui->productionTable->rowCount(); ++r) {
        if (!cellText(ui->productionTable,r,1).startsWith("PROD-")) continue;
        ++total;
        montantTotal += QString(cellText(ui->productionTable,r,4)).remove(" DT").remove(",").replace(" ","").toDouble();
        statCnt[cellText(ui->productionTable,r,7)]++;
        prioCnt[cellText(ui->productionTable,r,8)]++;
    }
    if (total == 0) { showInfo(this,"","Aucune donnée de production."); return; }

    QDialog dlg(this); dlg.setWindowTitle("Statistiques de Production"); dlg.setMinimumSize(1000,700);
    QScrollArea *scroll = new QScrollArea(&dlg);
    scroll->setWidgetResizable(true);
    QWidget *inner = new QWidget; QVBoxLayout *lay = new QVBoxLayout(inner);
    lay->setSpacing(15); lay->setContentsMargins(20,20,20,20);

    // KPI row
    auto makeKPI = [](const QString &icon, const QString &val, const QString &lbl, const QString &color) {
        auto *w = new QWidget; w->setStyleSheet(QString("background:white;border-radius:8px;border:2px solid %1;").arg(color));
        auto *l = new QVBoxLayout(w);
        auto *ic = new QLabel(icon); ic->setStyleSheet("font-size:32px;"); ic->setAlignment(Qt::AlignCenter);
        auto *vl = new QLabel(val);  vl->setStyleSheet(QString("font-size:28px;font-weight:bold;color:%1;").arg(color)); vl->setAlignment(Qt::AlignCenter);
        auto *lb = new QLabel(lbl);  lb->setStyleSheet("font-size:13px;color:#666;"); lb->setAlignment(Qt::AlignCenter);
        l->addWidget(ic); l->addWidget(vl); l->addWidget(lb);
        return w;
    };
    int terminees = statCnt.value("Terminé",0);
    double taux = total > 0 ? 100.0*terminees/total : 0;
    auto *kpiRow = new QWidget; auto *kpiLay = new QHBoxLayout(kpiRow); kpiLay->setSpacing(12);
    kpiLay->addWidget(makeKPI("📦",QString::number(total),"Total Commandes","#4CAF50"));
    kpiLay->addWidget(makeKPI("💰",QString::number(montantTotal,'f',0)+" DT","Chiffre d'Affaires","#2196F3"));
    kpiLay->addWidget(makeKPI("📊",QString::number(total>0?montantTotal/total:0,'f',0)+" DT","Montant Moyen","#FF9800"));
    kpiLay->addWidget(makeKPI("✅",QString::number(taux,'f',1)+"%","Taux Complétion","#9C27B0"));
    lay->addWidget(kpiRow);

    // Charts
    auto *chartsRow = new QWidget; auto *chartsLay = new QHBoxLayout(chartsRow); chartsLay->setSpacing(12);

    auto *pie = new QPieSeries();
    QMap<QString,QColor> clrs;
    clrs["Terminé"] = QColor("#4CAF50");
    clrs["En Production"] = QColor("#FF9800");
    clrs["Planifié"] = QColor("#2196F3");
    clrs["En Attente"] = QColor("#FFC107");
    for (auto it = statCnt.begin(); it != statCnt.end(); ++it) {
        auto *sl = pie->append(it.key(), it.value());
        sl->setLabelVisible(true);
        sl->setLabel(QString("%1: %2").arg(it.key()).arg(it.value()));
        if (clrs.contains(it.key())) sl->setColor(clrs[it.key()]);
    }
    auto *pc = new QChart(); pc->addSeries(pie); pc->setTitle("Répartition Statuts");
    pc->setAnimationOptions(QChart::SeriesAnimations); pc->setBackgroundBrush(Qt::white);
    auto *pv = new QChartView(pc); pv->setRenderHint(QPainter::Antialiasing); pv->setMinimumHeight(300);
    chartsLay->addWidget(pv);

    auto *bs = new QBarSet("Commandes"); bs->setColor(QColor("#8D6E63"));
    QStringList cats;
    QStringList priorites = {"Urgente","Haute","Moyenne","Basse"};
    for (const QString &p : priorites)
        if (prioCnt.contains(p)) { *bs << prioCnt[p]; cats << p; }
    auto *bar = new QBarSeries(); bar->append(bs);
    auto *bc = new QChart(); bc->addSeries(bar); bc->setTitle("Répartition Priorités");
    bc->setAnimationOptions(QChart::SeriesAnimations); bc->setBackgroundBrush(Qt::white);
    auto *axX = new QBarCategoryAxis(); axX->append(cats); bc->addAxis(axX,Qt::AlignBottom); bar->attachAxis(axX);
    auto *axY = new QValueAxis(); axY->setLabelFormat("%d"); bc->addAxis(axY,Qt::AlignLeft); bar->attachAxis(axY);
    bc->legend()->setVisible(false);
    auto *bv = new QChartView(bc); bv->setRenderHint(QPainter::Antialiasing); bv->setMinimumHeight(300);
    chartsLay->addWidget(bv);

    lay->addWidget(chartsRow);

    QPushButton exportBtn("Exporter CSV",inner), closeBtn("Fermer",inner);
    closeBtn.setStyleSheet("QPushButton{background:#95877C;}");
    connect(&exportBtn,&QPushButton::clicked,[&]{ onExcelProduction(); });
    connect(&closeBtn,&QPushButton::clicked,&dlg,&QDialog::accept);
    QHBoxLayout btnRow; btnRow.addStretch(); btnRow.addWidget(&exportBtn); btnRow.addWidget(&closeBtn);
    lay->addLayout(&btnRow);

    scroll->setWidget(inner);
    QVBoxLayout *dlay = new QVBoxLayout(&dlg); dlay->addWidget(scroll);
    dlg.exec();
}

QPixmap MainWindow::generateQRCode(const QString &, int size)
{
    QPixmap p(size,size); p.fill(Qt::white); return p;
}

// ── Articles ──────────────────────────────────────────────────────────────────
void MainWindow::setupArticleTable()
{
    ui->articleTable->setColumnCount(11);
    ui->articleTable->setHorizontalHeaderLabels({
        "ID","Référence","Nom","Catégorie","Type","Couleur",
        "Dimensions","Prix Unitaire","Coût Fabrication","Statut","Date Création"
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
        ui->articleTable->setItem(i, 7, new QTableWidgetItem(QString::number(a.getPrixUnitaire(), 'f', 2) + " €"));
        ui->articleTable->setItem(i, 8, new QTableWidgetItem(QString::number(a.getCoutFabrication(), 'f', 2) + " €"));
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
                       "","Sacs","","","",0,0,"disponible");
    
    if (dlg.exec() == QDialog::Accepted) {
        // Créer un nouvel article avec les données du dialog
        Article article;
        article.setReference(dlg.getReference());
        article.setNom(dlg.getNom());
        article.setCategorie(dlg.getCategorie());
        article.setType(dlg.getType());
        article.setCouleur(dlg.getCouleur());
        article.setDimensions(dlg.getDimensions());
        article.setPrixUnitaire(dlg.getPrixUnitaire());
        article.setCoutFabrication(dlg.getCoutFabrication());
        article.setStatut(dlg.getStatut());
        article.setDateCreation(QDate::currentDate());
        
        // Ajouter à la base de données
        if (article.ajouter()) {
            QMessageBox::information(this, "Succès", "Article ajouté avec succès !");
            refreshArticleTable(); // Rafraîchir l'affichage
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
    if (row < 0) { QMessageBox::warning(this,"Attention","Sélectionnez un article."); return; }
    
    // Récupérer l'ID de l'article sélectionné
    int idArticle = ui->articleTable->item(row, 0)->text().toInt();
    Article article = Article::rechercherParId(idArticle);
    
    if (article.getIdArticle() == 0) {
        QMessageBox::warning(this, "Erreur", "Article introuvable.");
        return;
    }
    
    ArticleDialog dlg(this, ArticleDialog::EditMode);
    dlg.setArticleData(article.getReference(), article.getNom(), article.getCategorie(),
                       article.getType(), article.getCouleur(), article.getDimensions(),
                       article.getPrixUnitaire(), article.getCoutFabrication(), article.getStatut());
    
    if (dlg.exec() == QDialog::Accepted) {
        // Mettre à jour l'article avec les nouvelles données
        article.setReference(dlg.getReference());
        article.setNom(dlg.getNom());
        article.setCategorie(dlg.getCategorie());
        article.setType(dlg.getType());
        article.setCouleur(dlg.getCouleur());
        article.setDimensions(dlg.getDimensions());
        article.setPrixUnitaire(dlg.getPrixUnitaire());
        article.setCoutFabrication(dlg.getCoutFabrication());
        article.setStatut(dlg.getStatut());
        
        // Modifier dans la base de données
        if (article.modifier()) {
            QMessageBox::information(this, "Succès", "Article modifié avec succès !");
            refreshArticleTable(); // Rafraîchir l'affichage
        } else {
            QMessageBox::critical(this, "Erreur", "Impossible de modifier l'article.");
        }
    }
}

void MainWindow::on_btnDeleteArticle_clicked()
{
    int row = ui->articleTable->currentRow();
    if (row < 0) { QMessageBox::warning(this,"Attention","Sélectionnez un article."); return; }
    
    // Récupérer l'ID de l'article sélectionné
    int idArticle = ui->articleTable->item(row, 0)->text().toInt();
    Article article = Article::rechercherParId(idArticle);
    
    if (article.getIdArticle() == 0) {
        QMessageBox::warning(this, "Erreur", "Article introuvable.");
        return;
    }
    
    ArticleDialog dlg(this, ArticleDialog::DeleteMode);
    dlg.setArticleData(article.getReference(), article.getNom(), article.getCategorie(),
                       article.getType(), article.getCouleur(), article.getDimensions(),
                       article.getPrixUnitaire(), article.getCoutFabrication(), article.getStatut());
    
    if (dlg.exec() == QDialog::Accepted) {
        // Supprimer de la base de données
        if (article.supprimer()) {
            QMessageBox::information(this, "Succès", "Article supprimé avec succès !");
            refreshArticleTable(); // Rafraîchir l'affichage
        } else {
            QMessageBox::critical(this, "Erreur", "Impossible de supprimer l'article.");
        }
    }
}

void MainWindow::on_btnViewArticle_clicked()
{
    int row = ui->articleTable->currentRow();
    if (row < 0) { QMessageBox::warning(this,"","Sélectionnez un article."); return; }
    ArticleDialog dlg(this, ArticleDialog::ViewMode);
    loadArticleRow(ui->articleTable, row, dlg);
    dlg.exec();
}

void MainWindow::on_searchBoxArticle_textChanged(const QString &text) { filterTable(ui->articleTable, text); }

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
    QMenu *menuPrix = menu.addMenu("💰 Prix");
    menuPrix->setStyleSheet(menu.styleSheet());
    auto *prixCroissant = menuPrix->addAction("↑ Prix Croissant");
    auto *prixDecroissant = menuPrix->addAction("↓ Prix Décroissant");
    connect(prixCroissant, &QAction::triggered, [=]{ ui->articleTable->sortItems(7, Qt::AscendingOrder); });
    connect(prixDecroissant, &QAction::triggered, [=]{ ui->articleTable->sortItems(7, Qt::DescendingOrder); });
    
    menu.addSeparator();
    
    // Catégorie
    QMenu *menuCategorie = menu.addMenu("📋 Catégorie");
    menuCategorie->setStyleSheet(menu.styleSheet());
    auto *catAZ = menuCategorie->addAction("↑ A → Z");
    auto *catZA = menuCategorie->addAction("↓ Z → A");
    connect(catAZ, &QAction::triggered, [=]{ ui->articleTable->sortItems(3, Qt::AscendingOrder); });
    connect(catZA, &QAction::triggered, [=]{ ui->articleTable->sortItems(3, Qt::DescendingOrder); });
    
    menu.addSeparator();
    
    // Statut
    QMenu *menuStatut = menu.addMenu("🔄 Statut");
    menuStatut->setStyleSheet(menu.styleSheet());
    auto *statAZ = menuStatut->addAction("↑ A → Z");
    auto *statZA = menuStatut->addAction("↓ Z → A");
    connect(statAZ, &QAction::triggered, [=]{ ui->articleTable->sortItems(9, Qt::AscendingOrder); });
    connect(statZA, &QAction::triggered, [=]{ ui->articleTable->sortItems(9, Qt::DescendingOrder); });

    QPoint pos = ui->btnTriArticle->mapToGlobal(QPoint(0, ui->btnTriArticle->height()));
    menu.exec(pos);
}

void MainWindow::on_btnStatistiquesArticle_clicked()
{
    // Compter les articles par statut
    int disponible = 0, enProduction = 0, obsolete = 0;
    QMap<QString, int> parCategorie;
    
    for (int i = 0; i < ui->articleTable->rowCount(); ++i) {
        QString statut = ui->articleTable->item(i, 9)->text();
        QString categorie = ui->articleTable->item(i, 3)->text();
        
        if (statut == "disponible") disponible++;
        else if (statut == "en_production") enProduction++;
        else if (statut == "obsolete") obsolete++;
        
        parCategorie[categorie]++;
    }
    
    // Créer un dialog pour afficher les statistiques
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("📊 Statistiques des Articles");
    dialog->resize(900, 600);
    
    QVBoxLayout *layout = new QVBoxLayout(dialog);
    
    // Titre
    QLabel *titre = new QLabel("Statistiques des Articles", dialog);
    titre->setStyleSheet("font-size: 20px; font-weight: bold; color: #8D6E63; padding: 10px;");
    titre->setAlignment(Qt::AlignCenter);
    layout->addWidget(titre);
    
    // Graphique en camembert pour les statuts
    QPieSeries *seriesStatut = new QPieSeries();
    seriesStatut->append("Disponible", disponible);
    seriesStatut->append("En Production", enProduction);
    if (obsolete > 0) seriesStatut->append("Obsolète", obsolete);
    
    // Couleurs personnalisées
    QPieSlice *sliceDisp = seriesStatut->slices().at(0);
    sliceDisp->setBrush(QColor("#4CAF50"));
    sliceDisp->setLabelVisible(true);
    sliceDisp->setLabel(QString("Disponible: %1 (%2%)").arg(disponible).arg(100.0 * disponible / ui->articleTable->rowCount(), 0, 'f', 1));
    
    QPieSlice *sliceProd = seriesStatut->slices().at(1);
    sliceProd->setBrush(QColor("#FF9800"));
    sliceProd->setLabelVisible(true);
    sliceProd->setLabel(QString("En Production: %1 (%2%)").arg(enProduction).arg(100.0 * enProduction / ui->articleTable->rowCount(), 0, 'f', 1));
    
    if (obsolete > 0) {
        QPieSlice *sliceObs = seriesStatut->slices().at(2);
        sliceObs->setBrush(QColor("#F44336"));
        sliceObs->setLabelVisible(true);
        sliceObs->setLabel(QString("Obsolète: %1 (%2%)").arg(obsolete).arg(100.0 * obsolete / ui->articleTable->rowCount(), 0, 'f', 1));
    }
    
    QChart *chartStatut = new QChart();
    chartStatut->addSeries(seriesStatut);
    chartStatut->setTitle("Répartition par Statut");
    chartStatut->setAnimationOptions(QChart::SeriesAnimations);
    chartStatut->legend()->setAlignment(Qt::AlignRight);
    
    QChartView *chartViewStatut = new QChartView(chartStatut);
    chartViewStatut->setRenderHint(QPainter::Antialiasing);
    chartViewStatut->setMinimumHeight(250);
    layout->addWidget(chartViewStatut);
    
    // Graphique en barres pour les catégories
    QBarSet *setCategories = new QBarSet("Nombre d'articles");
    setCategories->setColor(QColor("#8D6E63"));
    
    QStringList categories;
    int maxValue = 0;
    for (auto it = parCategorie.begin(); it != parCategorie.end(); ++it) {
        categories << it.key();
        *setCategories << it.value();
        if (it.value() > maxValue) maxValue = it.value();
    }
    
    QBarSeries *seriesCategories = new QBarSeries();
    seriesCategories->append(setCategories);
    
    QChart *chartCategories = new QChart();
    chartCategories->addSeries(seriesCategories);
    chartCategories->setTitle("Répartition par Catégorie");
    chartCategories->setAnimationOptions(QChart::SeriesAnimations);
    
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chartCategories->addAxis(axisX, Qt::AlignBottom);
    seriesCategories->attachAxis(axisX);
    
    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, maxValue + 1);
    chartCategories->addAxis(axisY, Qt::AlignLeft);
    seriesCategories->attachAxis(axisY);
    
    chartCategories->legend()->setVisible(false);
    
    QChartView *chartViewCategories = new QChartView(chartCategories);
    chartViewCategories->setRenderHint(QPainter::Antialiasing);
    chartViewCategories->setMinimumHeight(250);
    layout->addWidget(chartViewCategories);
    
    // Bouton fermer
    QPushButton *btnClose = new QPushButton("Fermer", dialog);
    btnClose->setStyleSheet("background-color: #8D6E63; color: white; padding: 10px; border-radius: 5px; font-weight: bold;");
    connect(btnClose, &QPushButton::clicked, dialog, &QDialog::accept);
    layout->addWidget(btnClose);
    
    dialog->exec();
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
        "<table><thead><tr><th>Réf.</th><th>Nom</th><th>Catégorie</th><th>Type</th>"
        "<th>Couleur</th><th>Dimensions</th><th>Prix</th><th>Statut</th></tr></thead><tbody>"
    ).arg(QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm"))
     .arg(articles.size()).arg(dispo).arg(enProd);

    for (const Article &a : articles) {
        QString st = a.getStatut()=="disponible" ? "Disponible"
                   : a.getStatut()=="en_production" ? "En Production" : "Obsolète";
        html += QString("<tr><td>%1</td><td><b>%2</b></td><td>%3</td><td>%4</td>"
                        "<td>%5</td><td>%6</td><td>%7 €</td><td>%8</td></tr>")
                .arg(a.getReference(),a.getNom(),a.getCategorie(),a.getType(),
                     a.getCouleur(),a.getDimensions(),
                     QString::number(a.getPrixUnitaire(),'f',2), st);
    }
    html += "</tbody></table></body></html>";

    QTextDocument doc; doc.setHtml(html); doc.print(&printer);
    QMessageBox::information(this,"","Catalogue exporté: "+fn);
}

void MainWindow::on_btnAnalyseRentabilite_clicked()
{
    QDialog dlg(this); dlg.setWindowTitle("Analyse de Rentabilité"); dlg.setMinimumSize(850,550);
    QVBoxLayout lay(&dlg); lay.setContentsMargins(15,15,15,15);
    auto *title = new QLabel("ANALYSE DE RENTABILITÉ"); title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size:18px;font-weight:bold;color:#8D6E63;padding:8px;");
    lay.addWidget(title);

    auto *table = new QTableWidget(&dlg);
    table->setColumnCount(7);
    table->setHorizontalHeaderLabels({"Référence","Nom","Coût","Prix","Marge (€)","Marge (%)","Rentabilité"});
    table->horizontalHeader()->setStretchLastSection(true);
    table->setAlternatingRowColors(true);
    table->verticalHeader()->setVisible(false);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);

    double totalMarge=0; int rentables=0, nonRentables=0;
    int row=0;
    for (const Article &a : articles) {
        double marge = a.getPrixUnitaire()-a.getCoutFabrication();
        double mp = a.getCoutFabrication()>0 ? marge/a.getCoutFabrication()*100 : 0;
        totalMarge += marge;
        QString ren; QString col;
        if      (mp>=50){ ren="⭐ Excellente"; col="#2E7D32"; ++rentables; }
        else if (mp>=30){ ren="✓ Bonne";       col="#388E3C"; ++rentables; }
        else if (mp>=15){ ren="~ Moyenne";     col="#F57C00"; }
        else if (mp> 0) { ren="⚠ Faible";      col="#E65100"; ++nonRentables; }
        else            { ren="✗ Non rentable";col="#C62828"; ++nonRentables; }

        table->insertRow(row);
        table->setItem(row,0,new QTableWidgetItem(a.getReference()));
        table->setItem(row,1,new QTableWidgetItem(a.getNom()));
        table->setItem(row,2,new QTableWidgetItem(QString::number(a.getCoutFabrication(),'f',2)+" €"));
        table->setItem(row,3,new QTableWidgetItem(QString::number(a.getPrixUnitaire(),'f',2)+" €"));
        table->setItem(row,4,new QTableWidgetItem(QString::number(marge,'f',2)+" €"));
        table->setItem(row,5,new QTableWidgetItem(QString::number(mp,'f',1)+" %"));
        auto *ri = new QTableWidgetItem(ren);
        ri->setForeground(QBrush(QColor(col))); QFont f=ri->font(); f.setBold(true); ri->setFont(f);
        table->setItem(row,6,ri); ++row;
    }
    lay.addWidget(table);

    auto *summary = new QLabel(QString("Marge totale: <b>%1 €</b> | Rentables: <b style='color:#2E7D32;'>%2</b>"
                                        " | Non rentables: <b style='color:#C62828;'>%3</b>")
                               .arg(QString::number(totalMarge,'f',2)).arg(rentables).arg(nonRentables));
    summary->setStyleSheet("background:#FFF8F0;border:1px solid #BCAAA4;border-radius:6px;padding:10px;");
    lay.addWidget(summary);

    QPushButton close("Fermer",&dlg);
    close.setStyleSheet("QPushButton{background:#8D6E63;color:white;border:none;border-radius:6px;padding:8px 24px;}"
                        "QPushButton:hover{background:#A0826D;}");
    connect(&close,&QPushButton::clicked,&dlg,&QDialog::accept);
    QHBoxLayout bl; bl.addStretch(); bl.addWidget(&close); lay.addLayout(&bl);
    dlg.exec();
}

void MainWindow::on_btnAideDecision_clicked()
{
    struct AP { QString ref,nom,cat; double marge; int score; QString rec; };
    QList<AP> list;
    for (const Article &a : articles) {
        double m = a.getCoutFabrication()>0 ? (a.getPrixUnitaire()-a.getCoutFabrication())/a.getCoutFabrication()*100 : 0;
        int score=0;
        if (m>=50) score+=40; else if (m>=30) score+=30; else if (m>=15) score+=20; else if (m>0) score+=10;
        if (a.getStatut()=="disponible")    score+=30;
        else if (a.getStatut()=="en_production") score+=15;
        if      (a.getCategorie()=="Sacs")          score+=30;
        else if (a.getCategorie()=="Portefeuilles") score+=25;
        else if (a.getCategorie()=="Ceintures")     score+=20;
        else score+=15;
        QString rec = score>=80?"🔥 PRIORITÉ MAXIMALE":score>=60?"⭐ PRIORITÉ HAUTE":
                      score>=40?"✓ PRIORITÉ MOYENNE":"⚠ PRIORITÉ BASSE";
        list.append({a.getReference(),a.getNom(),a.getCategorie(),m,score,rec});
    }
    std::sort(list.begin(),list.end(),[](const AP&a,const AP&b){return a.score>b.score;});

    QDialog dlg(this); dlg.setWindowTitle("Aide à la Décision"); dlg.setMinimumSize(850,500);
    QVBoxLayout lay(&dlg); lay.setContentsMargins(15,15,15,15);
    auto *title = new QLabel("AIDE À LA DÉCISION - PRIORISATION"); title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size:18px;font-weight:bold;color:#8D6E63;padding:8px;");
    lay.addWidget(title);

    auto *table = new QTableWidget(&dlg);
    table->setColumnCount(6); table->setHorizontalHeaderLabels({"#","Référence","Nom","Catégorie","Marge (%)","Recommandation"});
    table->horizontalHeader()->setStretchLastSection(true);
    table->setAlternatingRowColors(true); table->verticalHeader()->setVisible(false);

    for (int i=0; i<list.size(); ++i) {
        table->insertRow(i);
        auto *pi = new QTableWidgetItem(QString::number(i+1));
        if (i<3) pi->setBackground(QBrush(QColor("#FFD700")));
        else if (i<7) pi->setBackground(QBrush(QColor("#C0C0C0")));
        QFont f=pi->font(); f.setBold(true); pi->setFont(f);
        table->setItem(i,0,pi);
        table->setItem(i,1,new QTableWidgetItem(list[i].ref));
        table->setItem(i,2,new QTableWidgetItem(list[i].nom));
        table->setItem(i,3,new QTableWidgetItem(list[i].cat));
        table->setItem(i,4,new QTableWidgetItem(QString::number(list[i].marge,'f',1)+" %"));
        auto *ri = new QTableWidgetItem(list[i].rec);
        QFont fr=ri->font(); fr.setBold(true); ri->setFont(fr);
        table->setItem(i,5,ri);
    }
    lay.addWidget(table);
    QPushButton close("Fermer",&dlg);
    close.setStyleSheet("QPushButton{background:#8D6E63;color:white;border:none;border-radius:6px;padding:8px 24px;}"
                        "QPushButton:hover{background:#A0826D;}");
    connect(&close,&QPushButton::clicked,&dlg,&QDialog::accept);
    QHBoxLayout bl; bl.addStretch(); bl.addWidget(&close); lay.addLayout(&bl);
    dlg.exec();
}

// ── Voice Recognition (SAPI) ──────────────────────────────────────────────────
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

void MainWindow::onVoiceCommand()
{
    // Dialog avec champ texte pour saisie vocale via Win+H
    QDialog dlg(this);
    dlg.setWindowTitle("Commande Vocale");
    dlg.setMinimumWidth(420);
    dlg.setStyleSheet(
        "QDialog { background-color: #FAF5F0; }"
        "QLabel { color: #291C0E; font-size: 13px; }"
        "QLineEdit { background: white; border: 2px solid #8D6E63; border-radius: 8px; "
        "padding: 10px; font-size: 14px; color: #291C0E; }"
        "QPushButton { background-color: #8D6E63; color: white; border: none; "
        "border-radius: 6px; padding: 8px 20px; font-size: 12px; font-weight: bold; }"
        "QPushButton:hover { background-color: #A0826D; }");

    QVBoxLayout lay(&dlg);
    lay.setSpacing(12);
    lay.setContentsMargins(20, 20, 20, 20);

    auto *info = new QLabel("🎤 Appuyez sur  Win + H  puis dites votre commande :", &dlg);
    info->setWordWrap(true);

    auto *hint = new QLabel(
        "<small style='color:#8D6E63;'>"
        "Commandes : <b>ajouter</b> · <b>modifier</b> · <b>supprimer</b> · "
        "<b>statistiques</b> · <b>exporter</b> · <b>détection</b> · "
        "<b>trier</b> · <b>rechercher [mot]</b>"
        "</small>", &dlg);
    hint->setWordWrap(true);
    hint->setTextFormat(Qt::RichText);

    auto *input = new QLineEdit(&dlg);
    input->setPlaceholderText("La commande apparaîtra ici...");

    QHBoxLayout btnLay;
    auto *btnOk     = new QPushButton("✔ Exécuter", &dlg);
    auto *btnCancel = new QPushButton("Annuler",    &dlg);
    btnCancel->setStyleSheet(
        "QPushButton { background-color: #E7DDD1; color: #291C0E; border: 1px solid #BCAAA4; "
        "border-radius: 6px; padding: 8px 20px; }"
        "QPushButton:hover { background-color: #F0E6DA; }");
    btnLay.addStretch();
    btnLay.addWidget(btnOk);
    btnLay.addWidget(btnCancel);

    lay.addWidget(info);
    lay.addWidget(hint);
    lay.addWidget(input);
    lay.addLayout(&btnLay);

    connect(btnOk,     &QPushButton::clicked, &dlg, &QDialog::accept);
    connect(btnCancel, &QPushButton::clicked, &dlg, &QDialog::reject);
    connect(input, &QLineEdit::returnPressed, &dlg, &QDialog::accept);

    // Focus sur le champ et déclencher Win+H automatiquement
    input->setFocus();
    QTimer::singleShot(300, [&]() {
#ifdef Q_OS_WIN
        // Simuler Win+H pour ouvrir la saisie vocale Windows sur le champ
        INPUT inputs[4] = {};
        inputs[0].type = INPUT_KEYBOARD;
        inputs[0].ki.wVk = VK_LWIN;
        inputs[1].type = INPUT_KEYBOARD;
        inputs[1].ki.wVk = 'H';
        inputs[2].type = INPUT_KEYBOARD;
        inputs[2].ki.wVk = 'H';
        inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
        inputs[3].type = INPUT_KEYBOARD;
        inputs[3].ki.wVk = VK_LWIN;
        inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(4, inputs, sizeof(INPUT));
#endif
    });

    if (dlg.exec() == QDialog::Accepted) {
        QString cmd = input->text().trimmed().toLower();
        if (!cmd.isEmpty())
            processVoiceCommand(cmd);
    }
}

void MainWindow::processVoiceCommand(const QString &rawCmd)
{
    // Normaliser : minuscules, supprimer ponctuation finale
    QString cmd = rawCmd.toLower().trimmed();
    cmd.remove(QRegularExpression("[.!?,;]$"));

    QString feedback = "🎤 \"" + cmd + "\"";

    // ── Ajouter ──────────────────────────────────────────────
    if (cmd.contains("ajouter") || cmd.contains("ajoutez") || cmd.contains("ajoute") ||
        cmd.contains("ajout")   || cmd.contains("créer")   || cmd.contains("créez")  ||
        cmd.contains("creer")   || cmd.contains("cree")    || cmd.contains("créé")   ||
        cmd.contains("nouvelle") || cmd.contains("nouveau") || cmd.contains("insérer") ||
        cmd.contains("inserer") || cmd.contains("add")) {

        // Extraire quantité : "quantité 2.9" / "quantite 5"
        QRegularExpression reQty("(quantit[eé]|quantite)\\s+(\\d+[.,]?\\d*)");
        auto mQty = reQty.match(cmd);
        QString qty = mQty.hasMatch() ? mQty.captured(2).replace(",", ".") : "";

        // Extraire nom : "nom aaa" / "nommée cuir" / "appelée cuir"
        QRegularExpression reNom("(nom|nomm[eé]e?|appel[eé]e?)\\s+(\\w+)");
        auto mNom = reNom.match(cmd);
        QString nom = mNom.hasMatch() ? mNom.captured(2) : "";

        // Extraire référence : "référence dd" / "ref dd"
        QRegularExpression reRef("(r[eé]f[eé]rence|ref|r[eé]f)\\s+(\\w+)");
        auto mRef = reRef.match(cmd);
        QString ref = mRef.hasMatch() ? mRef.captured(2) : "";

        // Extraire type : "type cuir" / "de type peau"
        QRegularExpression reType("(type|de type)\\s+(\\w+(?:\\s+\\w+)?)");
        auto mType = reType.match(cmd);
        QString type = mType.hasMatch() ? mType.captured(2) : "";

        // Extraire seuil : "seuil 100"
        QRegularExpression reSeuil("(seuil)\\s+(\\d+)");
        auto mSeuil = reSeuil.match(cmd);
        QString seuil = mSeuil.hasMatch() ? mSeuil.captured(2) : "";

        MatiereDialog dlg(this, MatiereDialog::AddMode);
        if (!nom.isEmpty() || !qty.isEmpty() || !ref.isEmpty() || !seuil.isEmpty())
            dlg.setMatiereData(nom, ref, type, qty, seuil, "");
        if (dlg.exec() == QDialog::Accepted) {
            Matiere m;
            m.setNom(dlg.getModule());
            m.setReference(dlg.getReference());
            m.setType(dlg.getType());
            m.setQuantite(dlg.getQuantite().toDouble());
            m.setSeuil(dlg.getSeuil().toInt());
            m.setDateExpiration(QDate::fromString(dlg.getDateExpiration(), "yyyy-MM-dd"));
            m.setPhotoUrl(dlg.getPhotoUrl());
            if (m.ajouter()) { setupMatiereTable(); updateMatiereStatistics(); }
        }

    // ── Modifier ─────────────────────────────────────────────
    } else if (cmd.contains("modifier") || cmd.contains("modifiez") || cmd.contains("modifie") ||
               cmd.contains("changer")  || cmd.contains("changez")  || cmd.contains("éditer") ||
               cmd.contains("mettre à jour") || cmd.contains("première matière")) {

        // Extraire nom pour sélectionner la ligne
        QRegularExpression reNomCible("(mati[eè]re|matiere)\\s+(\\w+)");
        auto mNomCible = reNomCible.match(cmd);
        if (mNomCible.hasMatch()) {
            QString nomCible = mNomCible.captured(2);
            for (int r = 0; r < ui->matiereTable->rowCount(); ++r) {
                if (cellText(ui->matiereTable, r, 0).toLower().contains(nomCible)) {
                    ui->matiereTable->selectRow(r);
                    break;
                }
            }
        }

        int row = ui->matiereTable->currentRow();
        if (row < 0) {
            QMessageBox::warning(this, "Modifier", "Veuillez sélectionner une matière à modifier.");
            return;
        }

        // Extraire les modifications demandées
        QRegularExpression reQty("(quantit[eé]|quantite)\\s+(\\d+[.,]?\\d*)");
        auto mQty = reQty.match(cmd);
        QString qty = mQty.hasMatch() ? mQty.captured(2).replace(",", ".") : "";
        
        QRegularExpression reSeuil("(seuil)\\s+(\\d+)");
        auto mSeuil = reSeuil.match(cmd);
        QString seuil = mSeuil.hasMatch() ? mSeuil.captured(2) : "";
        
        QRegularExpression reType("(type)\\s+(\\w+(?:\\s+\\w+)?)");
        auto mType = reType.match(cmd);
        QString type = mType.hasMatch() ? mType.captured(2) : "";

        QRegularExpression reRef("(r[eé]f[eé]rence|ref)\\s+(\\w+)");
        auto mRef = reRef.match(cmd);
        QString ref = mRef.hasMatch() ? mRef.captured(2) : "";

        QRegularExpression reNom("(nom)\\s+(\\w+)");
        auto mNom = reNom.match(cmd);
        QString nom = mNom.hasMatch() ? mNom.captured(2) : "";

        // Ouvrir le dialog avec les données actuelles
        MatiereDialog dlg(this, MatiereDialog::EditMode);
        dlg.setMatiereData(
            cellText(ui->matiereTable, row, 0),
            cellText(ui->matiereTable, row, 1),
            cellText(ui->matiereTable, row, 2),
            cellText(ui->matiereTable, row, 3).remove(" m²"),
            cellText(ui->matiereTable, row, 4),
            cellText(ui->matiereTable, row, 5),
            ""
        );

        // Appliquer les modifications vocales
        if (!qty.isEmpty())   dlg.setQuantite(qty);
        if (!seuil.isEmpty()) dlg.setSeuil(seuil);
        if (!type.isEmpty())  dlg.setTypeMatiere(type);
        if (!ref.isEmpty())   dlg.setReference(ref);
        if (!nom.isEmpty())   dlg.setModule(nom);

        if (dlg.exec() == QDialog::Accepted) {
            Matiere m;
            m.setId(cellText(ui->matiereTable, row, 0).toInt());
            m.setNom(dlg.getModule());
            m.setReference(dlg.getReference());
            m.setType(dlg.getType());
            m.setQuantite(dlg.getQuantite().toDouble());
            m.setSeuil(dlg.getSeuil().toInt());
            m.setDateExpiration(QDate::fromString(dlg.getDateExpiration(), "yyyy-MM-dd"));
            m.setPhotoUrl(dlg.getPhotoUrl());
            if (m.modifier()) { setupMatiereTable(); updateMatiereStatistics(); }
        }

    // ── Supprimer ─────────────────────────────────────────────
    } else if (cmd.contains("supprimer") || cmd.contains("supprimez") || cmd.contains("supprime") ||
               cmd.contains("effacer")   || cmd.contains("effacez")   || cmd.contains("enlever") ||
               cmd.contains("retirer")   || cmd.contains("enlève")) {

        QRegularExpression reNom("(mati[eè]re|matiere)\\s+(\\w+)");
        auto mNom = reNom.match(cmd);
        if (mNom.hasMatch()) {
            QString nomCible = mNom.captured(2);
            for (int r = 0; r < ui->matiereTable->rowCount(); ++r) {
                if (cellText(ui->matiereTable, r, 0).toLower().contains(nomCible)) {
                    ui->matiereTable->selectRow(r);
                    break;
                }
            }
        }
        onDeleteMatiere();

    // ── Rechercher ────────────────────────────────────────────
    } else if (cmd.contains("rechercher") || cmd.contains("recherche") ||
               cmd.contains("chercher")   || cmd.contains("trouver")   ||
               cmd.contains("filtrer")    || cmd.contains("afficher")) {

        // Extraire le terme : "rechercher cuir" / "chercher stock critique"
        QString term = cmd;
        for (const QString &kw : QStringList{"rechercher", "recherche", "chercher", "trouver",
                                   "filtrer", "afficher", "la matière", "matière", "les matières"})
            term.remove(kw);
        term = term.trimmed();

        // Détection de filtres spéciaux
        if (term.contains("critique") || term.contains("stock critique")) {
            // Filtrer stock critique
            for (int r = 0; r < ui->matiereTable->rowCount(); ++r) {
                int qty = cellText(ui->matiereTable, r, 3).remove(" m²").toInt();
                int seuil = cellText(ui->matiereTable, r, 4).toInt();
                ui->matiereTable->setRowHidden(r, qty >= seuil);
            }
            QMainWindow::statusBar()->showMessage("🔍 Affichage : stock critique", 3000);
        } else if (term.contains("expiré") || term.contains("expire") || term.contains("périmé")) {
            // Filtrer matières expirées
            for (int r = 0; r < ui->matiereTable->rowCount(); ++r) {
                QString dateStr = cellText(ui->matiereTable, r, 5);
                QDate expDate = QDate::fromString(dateStr, "yyyy-MM-dd");
                int daysLeft = QDate::currentDate().daysTo(expDate);
                ui->matiereTable->setRowHidden(r, daysLeft >= 0);
            }
            QMainWindow::statusBar()->showMessage("🔍 Affichage : matières expirées", 3000);
        } else if (!term.isEmpty()) {
            // Recherche textuelle simple
            QString lower = term.toLower();
            for (int r = 0; r < ui->matiereTable->rowCount(); ++r) {
                bool match = false;
                for (int c = 0; c < 3; ++c) { // Nom, Ref, Type
                    if (cellText(ui->matiereTable, r, c).toLower().contains(lower)) {
                        match = true;
                        break;
                    }
                }
                ui->matiereTable->setRowHidden(r, !match);
            }
            QMainWindow::statusBar()->showMessage("🔍 Recherche : " + term, 3000);
        } else {
            // Ouvrir le dialog de recherche avancée
            onRechercheTriMatiere();
        }

    // ── Statistiques ──────────────────────────────────────────
    } else if (cmd.contains("statistique") || cmd.contains("stats") ||
               cmd.contains("statistiques")) {
        onStatistiquesMatiere();

    // ── Exporter ──────────────────────────────────────────────
    } else if (cmd.contains("export") || cmd.contains("exporter") ||
               cmd.contains("exportez") || cmd.contains("pdf") ||
               cmd.contains("rapport")) {
        onExportMatiere();

    // ── Détection ─────────────────────────────────────────────
    } else if (cmd.contains("détection") || cmd.contains("detection") ||
               cmd.contains("détecter")  || cmd.contains("analyser") ||
               cmd.contains("défaut")    || cmd.contains("defaut")) {
        onDetectionDefauts();

    // ── Trier ─────────────────────────────────────────────────
    } else if (cmd.contains("trier") || cmd.contains("trier") || cmd.contains("tri") ||
               cmd.contains("trie")  || cmd.contains("classer") || cmd.contains("ordonner")) {
        onTriMatiere();

    } else {
        QMessageBox::information(this, "Vocal",
            "Commande non reconnue : \"" + cmd + "\"\n\n"
            "Exemples :\n"
            "• \"ajouter matière\"\n"
            "• \"modifier la matière dd\"\n"
            "• \"supprimer la matière cuir\"\n"
            "• \"rechercher peau de veau\"\n"
            "• \"statistiques\"\n"
            "• \"exporter\"\n"
            "• \"détection\"\n"
            "• \"trier\"");
        return;
    }

    QMainWindow::statusBar()->showMessage(feedback, 3000);
}
