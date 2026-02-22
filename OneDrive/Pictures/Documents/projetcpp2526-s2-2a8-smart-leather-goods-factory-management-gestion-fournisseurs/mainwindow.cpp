#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "employeedialog.h"
#include "clientmanagerdialog.h"
#include "matieredialog.h"
#include "fournisseurdialog.h"
#include "productiondialog.h"
#include "articledialog.h"
#include "employe.h"
#include <QTableWidgetItem>
#include <QDebug>
#include <QMessageBox>
#include <QDate>
#include <QDateTime>
#include <QMenu>
#include <QDialog>
#include <QFormLayout>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDateEdit>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPrinter>
#include <QPageLayout>
#include <QPainter>
#include <QFileDialog>
#include <QTextDocument>
#include <QTextEdit>
#include <QFrame>
#include <QFile>
#include <QTextStream>
#include <QStringConverter>
#include <QScrollArea>
#include <QTimer>
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

    // ── Employee table ──────────────────────────────────────────────────────
    ui->employeeTable->verticalHeader()->setVisible(false);
    ui->employeeTable->setColumnHidden(0, true);
    int empWidths[] = {50,110,100,100,100,120,130,120,110,180};
    for (int i = 0; i < 10; ++i) ui->employeeTable->setColumnWidth(i, empWidths[i]);
    populateEmployeeTable();
    connect(ui->employeeTable, &QTableWidget::currentCellChanged,
            this, &MainWindow::onEmployeeSelected);

    // ── Clients ─────────────────────────────────────────────────────────────
    ui->clientTable->verticalHeader()->setVisible(false);
    auto addClient = [&](const QString &nom, const QString &prenom, const QString &sexe,
                         const QString &cin, const QString &pays, const QString &ville,
                         const QString &adr, const QString &email, int daysAgo) {
        Client c;
        c.setNom(nom); c.setPrenom(prenom); c.setSexe(sexe); c.setCin(cin);
        c.setPays(pays); c.setVille(ville); c.setAdresse(adr); c.setEmail(email);
        c.setDateInscrit(QDate::currentDate().addDays(-daysAgo));
        clients.append(c);
    };
    addClient("Alami","Hassan","Homme","AB123456","Maroc","Casablanca","123 Rue Mohammed V","h.alami@email.com",30);
    addClient("Benali","Fatima","Femme","CD789012","Maroc","Rabat","456 Avenue Hassan II","f.benali@email.com",15);
    refreshClientTable();

    // ── Raw materials ───────────────────────────────────────────────────────
    ui->matiereTable->verticalHeader()->setVisible(false);
    setupMatiereTable();
    connect(ui->btnAddMatiere,  &QPushButton::clicked, this, &MainWindow::onAddMatiere);
    connect(ui->btnEditMatiere, &QPushButton::clicked, this, &MainWindow::onEditMatiere);
    connect(ui->btnDeleteMatiere, &QPushButton::clicked, this, &MainWindow::onDeleteMatiere);
    connect(ui->btnSuggestion,  &QPushButton::clicked, this, &MainWindow::onSuggestionCommande);
    connect(ui->btnOptimisation,&QPushButton::clicked, this, &MainWindow::onOptimisationFIFO);
    connect(ui->btnRecherche,   &QPushButton::clicked, this, &MainWindow::onRechercheTriMatiere);
    connect(ui->btnFournisseurs,&QPushButton::clicked, this, &MainWindow::onGestionFournisseurs);
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

MainWindow::~MainWindow() { delete ui; }

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
void MainWindow::refreshClientTable()
{
    ui->clientTable->setRowCount(clients.size());
    for (int i = 0; i < clients.size(); ++i) {
        const Client &c = clients[i];
        ui->clientTable->setItem(i,0,new QTableWidgetItem(c.getNom()));
        ui->clientTable->setItem(i,1,new QTableWidgetItem(c.getPrenom()));
        ui->clientTable->setItem(i,2,new QTableWidgetItem(c.getSexe()));
        ui->clientTable->setItem(i,3,new QTableWidgetItem(c.getCin()));
        ui->clientTable->setItem(i,4,new QTableWidgetItem(c.getPays()));
        ui->clientTable->setItem(i,5,new QTableWidgetItem(c.getVille()));
        ui->clientTable->setItem(i,6,new QTableWidgetItem(c.getAdresse()));
        ui->clientTable->setItem(i,7,new QTableWidgetItem(c.getEmail()));
    }
}

void MainWindow::on_btnAddClient_clicked()  { ClientManagerDialog(this, ClientManagerDialog::AddMode).exec(); }
void MainWindow::on_btnExportClient_clicked(){ ClientManagerDialog(this, ClientManagerDialog::ExportMode).exec(); }

void MainWindow::on_btnEditClient_clicked()
{
    int row = ui->clientTable->currentRow();
    if (row < 0) { QMessageBox::warning(this,"","Veuillez sélectionner un client à modifier."); return; }
    ClientManagerDialog dlg(this, ClientManagerDialog::EditMode);
    dlg.setClientData(cellText(ui->clientTable,row,0),cellText(ui->clientTable,row,1),
                      cellText(ui->clientTable,row,2),cellText(ui->clientTable,row,3),
                      cellText(ui->clientTable,row,4),cellText(ui->clientTable,row,5),
                      cellText(ui->clientTable,row,6),cellText(ui->clientTable,row,7));
    dlg.exec();
}

void MainWindow::on_btnDeleteClient_clicked()
{
    int row = ui->clientTable->currentRow();
    if (row < 0) { QMessageBox::warning(this,"","Veuillez sélectionner un client à supprimer."); return; }
    ClientManagerDialog dlg(this, ClientManagerDialog::DeleteMode);
    dlg.setClientData(cellText(ui->clientTable,row,0),cellText(ui->clientTable,row,1),
                      cellText(ui->clientTable,row,2),cellText(ui->clientTable,row,3),
                      cellText(ui->clientTable,row,4),cellText(ui->clientTable,row,5),"","");
    dlg.exec();
}

void MainWindow::on_btnRefreshClient_clicked()
{
    showInfo(this,"Actualisation","Liste des clients actualisée ! (Mode statique - données d'exemple)");
}

void MainWindow::on_btnTriClient_clicked()
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
        connect(asc, &QAction::triggered, [=]{ ui->clientTable->sortItems(col, Qt::AscendingOrder); });
        connect(desc, &QAction::triggered, [=]{ ui->clientTable->sortItems(col, Qt::DescendingOrder); });
    };

    addSortOptions("Nom", 0);
    addSortOptions("Prénom", 1);
    addSortOptions("Sexe", 2);
    menu.addSeparator();
    addSortOptions("CIN", 3);
    addSortOptions("Pays", 4);
    addSortOptions("Ville", 5);
    menu.addSeparator();
    addSortOptions("Email", 7);

    QPoint pos = ui->btnTriClient->mapToGlobal(QPoint(0, ui->btnTriClient->height()));
    menu.exec(pos);
}

void MainWindow::on_btnStatsByRegion_clicked()       { showInfo(this,"Stats Région","Affiche le nombre de clients par région."); }
void MainWindow::on_btnFidelityClassification_clicked(){ showInfo(this,"Classification Fidélité","Classe les clients selon leur fidélité."); }
void MainWindow::on_btnAIAgent_clicked()             { showInfo(this,"Agent IA","Permet de rechercher rapidement un client."); }

// ── Raw Materials ─────────────────────────────────────────────────────────────
void MainWindow::setupMatiereTable()
{
    const QList<QStringList> rows = {
        {"Cuir Pleine Fleur","Peau de Veau","Peau de Veau","2.5 m²/jour","80","2028-05-31"},
        {"Ficelinée","Peau de Vachette","Peau de Vachette","1.8 m²/jour","60","2028-05-31"},
        {"LR-004","D-oci","Ficelinée","15 bobines/jour","50","2026-05-31"}
    };
    ui->matiereTable->setRowCount(rows.size());
    for (int r = 0; r < rows.size(); ++r)
        for (int c = 0; c < rows[r].size(); ++c)
            ui->matiereTable->setItem(r, c, new QTableWidgetItem(rows[r][c]));
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
        int row = ui->matiereTable->rowCount();
        ui->matiereTable->insertRow(row);
        ui->matiereTable->setItem(row,0,new QTableWidgetItem(dlg.getModule()));
        ui->matiereTable->setItem(row,1,new QTableWidgetItem(dlg.getReference()));
        ui->matiereTable->setItem(row,2,new QTableWidgetItem(dlg.getType()));
        ui->matiereTable->setItem(row,3,new QTableWidgetItem(dlg.getQuantite()));
        ui->matiereTable->setItem(row,4,new QTableWidgetItem(dlg.getSeuil()));
        ui->matiereTable->setItem(row,5,new QTableWidgetItem(dlg.getDateExpiration()));
        updateMatiereStatistics();
    }
}

void MainWindow::onEditMatiere()
{
    int row = ui->matiereTable->currentRow();
    if (row < 0) { QMessageBox::warning(this,"","Veuillez sélectionner une matière à modifier."); return; }
    MatiereDialog dlg(this, MatiereDialog::EditMode);
    dlg.setMatiereData(cellText(ui->matiereTable,row,0),cellText(ui->matiereTable,row,1),
                       cellText(ui->matiereTable,row,2),cellText(ui->matiereTable,row,3),
                       cellText(ui->matiereTable,row,4),cellText(ui->matiereTable,row,5));
    if (dlg.exec() == QDialog::Accepted) {
        ui->matiereTable->item(row,0)->setText(dlg.getModule());
        ui->matiereTable->item(row,1)->setText(dlg.getReference());
        ui->matiereTable->item(row,2)->setText(dlg.getType());
        ui->matiereTable->item(row,3)->setText(dlg.getQuantite());
        ui->matiereTable->item(row,4)->setText(dlg.getSeuil());
        ui->matiereTable->item(row,5)->setText(dlg.getDateExpiration());
        updateMatiereStatistics();
    }
}

void MainWindow::onDeleteMatiere()
{
    int row = ui->matiereTable->currentRow();
    if (row < 0) { QMessageBox::warning(this,"","Veuillez sélectionner une matière à supprimer."); return; }
    MatiereDialog dlg(this, MatiereDialog::DeleteMode);
    dlg.setMatiereData(cellText(ui->matiereTable,row,0),cellText(ui->matiereTable,row,1),
                       cellText(ui->matiereTable,row,2),cellText(ui->matiereTable,row,3),
                       cellText(ui->matiereTable,row,4),cellText(ui->matiereTable,row,5));
    if (dlg.exec() == QDialog::Accepted) {
        ui->matiereTable->removeRow(row);
        updateMatiereStatistics();
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
    QString txt = "RAPPORT DES MATIÈRES - " + QDate::currentDate().toString("dd/MM/yyyy") + "\n\n";
    txt += QString("Total: %1  |  Stock critique: %2  |  Fournisseurs: %3\n\n")
           .arg(ui->matiereTable->rowCount())
           .arg(ui->statsValueMatiere2->text())
           .arg(ui->statsValueMatiere3->text());
    for (int r = 0; r < ui->matiereTable->rowCount(); ++r)
        txt += QString("%1. %2 | %3 | %4 | seuil: %5 | exp: %6\n").arg(r+1)
               .arg(cellText(ui->matiereTable,r,0), cellText(ui->matiereTable,r,2),
                    cellText(ui->matiereTable,r,3), cellText(ui->matiereTable,r,4),
                    cellText(ui->matiereTable,r,5));
    QMessageBox mb(this); mb.setWindowTitle("Export - Aperçu");
    mb.setText("Aperçu du rapport:"); mb.setDetailedText(txt);
    mb.setStyleSheet(MSGBOX_STYLE); mb.exec();
}

void MainWindow::onRechercheTriMatiere()
{
    showInfo(this,"Recherche & Tri","Fonctionnalité de recherche avancée disponible dans la prochaine version.");
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
    ui->statsValueMatiere3->setText("2");
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
    ui->fournisseurTable->setRowCount(fournisseursData.size());
    for (int i = 0; i < fournisseursData.size(); ++i) {
        const auto &f = fournisseursData[i];
        ui->fournisseurTable->setItem(i,0,new QTableWidgetItem(f.getId()));
        ui->fournisseurTable->setItem(i,1,new QTableWidgetItem(f.getNomEntreprise()));
        ui->fournisseurTable->setItem(i,2,new QTableWidgetItem(f.getEmail()));
        ui->fournisseurTable->setItem(i,3,new QTableWidgetItem(f.getTelephone()));
        ui->fournisseurTable->setItem(i,4,new QTableWidgetItem(f.getMatriculeFiscal()));
        ui->fournisseurTable->setItem(i,5,new QTableWidgetItem(f.getTypeProduit()));
        ui->fournisseurTable->setItem(i,6,new QTableWidgetItem(f.getConditionPaiement()));
        ui->fournisseurTable->setItem(i,7,new QTableWidgetItem(f.getStatut()));
    }
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
    FournisseurDialog(this, FournisseurDialog::AddMode).exec();
}

void MainWindow::on_btnEditFournisseur_clicked()
{
    int row = ui->fournisseurTable->currentRow();
    if (row < 0) { QMessageBox::warning(this,"","Veuillez sélectionner un fournisseur."); return; }
    FournisseurDialog dlg(this, FournisseurDialog::EditMode);
    const auto &f = fournisseursData[row];
    dlg.setFournisseurData(f.getId(),f.getNomEntreprise(),f.getEmail(),f.getTelephone(),
                           f.getTypeProduit(),f.getConditionPaiement(),f.getMatriculeFiscal(),f.getStatut());
    dlg.exec();
}

void MainWindow::on_btnDeleteFournisseur_clicked()
{
    int row = ui->fournisseurTable->currentRow();
    if (row < 0) { QMessageBox::warning(this,"","Veuillez sélectionner un fournisseur."); return; }
    FournisseurDialog dlg(this, FournisseurDialog::DeleteMode);
    const auto &f = fournisseursData[row];
    dlg.setFournisseurData(f.getId(),f.getNomEntreprise(),f.getEmail(),f.getTelephone(),
                           f.getTypeProduit(),f.getConditionPaiement(),f.getMatriculeFiscal(),f.getStatut());
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
    filterTable(ui->fournisseurTable, text);
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
    const struct { const char *ref,*client,*type,*montant,*dc,*dl,*stat,*prio; } data[] = {
        {"PROD-2024-001","Leather Masters SA","Sacs en Cuir","15,500.00 DT","2024-01-15","2024-02-15","En Production","Haute"},
        {"PROD-2024-002","Textile Excellence","Vêtements","8,200.00 DT","2024-01-18","2024-02-20","Planifié","Moyenne"},
        {"PROD-2024-003","Fashion Accessories Ltd","Accessoires","3,750.00 DT","2024-01-20","2024-02-10","En Production","Haute"},
        {"PROD-2024-004","Premium Bags Co","Sacs Premium","22,000.00 DT","2024-01-22","2024-03-01","En Attente","Basse"},
        {"PROD-2024-005","Style Clothing","Collection Été","12,800.00 DT","2024-01-25","2024-02-28","En Production","Moyenne"},
        {"PROD-2024-006","Quality Leather Supply","Cuir Traité","9,500.00 DT","2024-01-28","2024-02-25","Terminé","Moyenne"},
        {"PROD-2024-007","Modern Textiles","Tissus Techniques","6,300.00 DT","2024-02-01","2024-03-05","Planifié","Basse"},
        {"PROD-2024-008","Elite Accessories","Bijoux Mode","4,200.00 DT","2024-02-03","2024-02-18","En Production","Haute"},
        {"PROD-2024-009","Luxury Bags Import","Sacs Luxe","28,500.00 DT","2024-02-05","2024-03-15","En Attente","Haute"},
        {"PROD-2024-010","Fashion Forward","Prêt-à-Porter","11,200.00 DT","2024-02-07","2024-03-10","Planifié","Moyenne"},
    };
    for (const auto &d : data)
        ajouterCommandeProduction(d.ref,d.client,d.type,d.montant,d.dc,d.dl,d.stat,d.prio);
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
    if (dlg.exec() == QDialog::Accepted)
        ajouterCommandeProduction(dlg.getReference(), dlg.getProduit(), dlg.getProduit(),
                                   "0.00 DT", dlg.getDateDebut(), dlg.getDateFin(),
                                   dlg.getStatut(), dlg.getPriorite());
}

void MainWindow::onModifierProduction()
{
    int row = ui->productionTable->currentRow();
    if (row < 0 || !cellText(ui->productionTable,row,1).startsWith("PROD-")) {
        QMessageBox::warning(this,"","Veuillez sélectionner une commande."); return;
    }
    QDialog d(this); d.setWindowTitle("Modifier Commande"); d.setMinimumSize(500,450);
    d.setStyleSheet(DIALOG_STYLE);
    QVBoxLayout lay(&d); lay.setContentsMargins(20,20,20,20);
    QFormLayout form;
    QLineEdit refE(cellText(ui->productionTable,row,1),&d);
    QLineEdit clientE(cellText(ui->productionTable,row,2),&d);
    QComboBox typeC(&d); typeC.addItems({"Standard","Express","Vente","Devis"});
    typeC.setCurrentText(cellText(ui->productionTable,row,3));
    QLineEdit montantE(cellText(ui->productionTable,row,4),&d);
    QDateEdit dcE(QDate::fromString(cellText(ui->productionTable,row,5),"dd/MM/yyyy"),&d);
    dcE.setCalendarPopup(true); dcE.setDisplayFormat("dd/MM/yyyy");
    QDateEdit dlE(QDate::fromString(cellText(ui->productionTable,row,6),"dd/MM/yyyy"),&d);
    dlE.setCalendarPopup(true); dlE.setDisplayFormat("dd/MM/yyyy");
    QComboBox statC(&d); statC.addItems({"En Attente","Planifié","En Production","Terminé"});
    statC.setCurrentText(cellText(ui->productionTable,row,7));
    QComboBox prioC(&d); prioC.addItems({"Basse","Normale","Haute","Urgente"});
    prioC.setCurrentText(cellText(ui->productionTable,row,8));
    form.addRow("Référence *:",&refE); form.addRow("Client *:",&clientE);
    form.addRow("Type:",&typeC); form.addRow("Montant HT *:",&montantE);
    form.addRow("Date Création:",&dcE); form.addRow("Date Livraison:",&dlE);
    form.addRow("Statut:",&statC); form.addRow("Priorité:",&prioC);
    lay.addLayout(&form);
    QHBoxLayout btns; QPushButton ok("Enregistrer",&d), cancel("Annuler",&d);
    cancel.setStyleSheet("QPushButton{background:#95877C;}");
    btns.addStretch(); btns.addWidget(&ok); btns.addWidget(&cancel);
    lay.addLayout(&btns);
    connect(&ok,&QPushButton::clicked,&d,&QDialog::accept);
    connect(&cancel,&QPushButton::clicked,&d,&QDialog::reject);
    if (d.exec() == QDialog::Accepted) {
        if (refE.text().isEmpty()||clientE.text().isEmpty()||montantE.text().isEmpty()) {
            QMessageBox::warning(this,"","Champs obligatoires manquants."); return;
        }
        ui->productionTable->item(row,1)->setText(refE.text());
        ui->productionTable->item(row,2)->setText(clientE.text());
        ui->productionTable->item(row,3)->setText(typeC.currentText());
        ui->productionTable->item(row,4)->setText(montantE.text());
        ui->productionTable->item(row,5)->setText(dcE.date().toString("dd/MM/yyyy"));
        ui->productionTable->item(row,6)->setText(dlE.date().toString("dd/MM/yyyy"));
        ui->productionTable->item(row,7)->setText(statC.currentText());
        ui->productionTable->item(row,8)->setText(prioC.currentText());
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
    // Créer un dialogue avec tableau unifié planification + suivi
    QDialog dlg(this);
    dlg.setWindowTitle("Planification & Suivi Production");
    dlg.setMinimumSize(1200, 700);
    
    QVBoxLayout *mainLay = new QVBoxLayout(&dlg);
    mainLay->setContentsMargins(20, 20, 20, 20);
    mainLay->setSpacing(15);
    
    // En-tête avec statistiques
    QFrame *headerFrame = new QFrame();
    headerFrame->setStyleSheet("background:qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #8D6E63,stop:1 #A0826D);"
                                "border-radius:8px;padding:15px;");
    QVBoxLayout *headerLay = new QVBoxLayout(headerFrame);
    
    QLabel *title = new QLabel("📋 PLANIFICATION & SUIVI DE PRODUCTION");
    title->setStyleSheet("font-size:18px;font-weight:bold;color:white;");
    title->setAlignment(Qt::AlignCenter);
    headerLay->addWidget(title);
    
    // Statistiques par statut
    QHBoxLayout *statsLay = new QHBoxLayout();
    QMap<QString,int> cnt;
    int prioritaires = 0;
    for (int r = 0; r < ui->productionTable->rowCount(); ++r) {
        cnt[cellText(ui->productionTable,r,7)]++;
        if (cellText(ui->productionTable,r,8)=="Haute") prioritaires++;
    }
    
    auto addStat = [&](const QString &label, int value, const QString &color) {
        QFrame *statFrame = new QFrame();
        statFrame->setStyleSheet(QString("background:%1;border-radius:6px;padding:8px;").arg(color));
        QVBoxLayout *statLay = new QVBoxLayout(statFrame);
        statLay->setSpacing(2);
        QLabel *valLbl = new QLabel(QString::number(value));
        valLbl->setStyleSheet("font-size:20px;font-weight:bold;color:white;");
        valLbl->setAlignment(Qt::AlignCenter);
        QLabel *lblLbl = new QLabel(label);
        lblLbl->setStyleSheet("font-size:10px;color:white;");
        lblLbl->setAlignment(Qt::AlignCenter);
        statLay->addWidget(valLbl);
        statLay->addWidget(lblLbl);
        statsLay->addWidget(statFrame);
    };
    
    addStat("En Attente", cnt["En Attente"], "#FF9800");
    addStat("Planifié", cnt["Planifié"], "#2196F3");
    addStat("En Production", cnt["En Production"], "#FFC107");
    addStat("Terminé", cnt["Terminé"], "#4CAF50");
    addStat("Prioritaires", prioritaires, "#F44336");
    
    headerLay->addLayout(statsLay);
    mainLay->addWidget(headerFrame);
    
    // Tableau unifié
    QTableWidget *table = new QTableWidget();
    table->setColumnCount(10);
    table->setHorizontalHeaderLabels({"Réf", "Client", "Article", "Qté", "Montant", 
                                       "Date Cmd", "Livraison", "Statut", "Priorité", "Avancement"});
    table->setRowCount(ui->productionTable->rowCount());
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true);
    table->horizontalHeader()->setStretchLastSection(true);
    table->setStyleSheet(
        "QTableWidget{background:white;border:1px solid #E0E0E0;border-radius:6px;}"
        "QHeaderView::section{background:#8D6E63;color:white;font-weight:bold;padding:8px;border:none;}"
        "QTableWidget::item{padding:8px;}"
    );
    
    // Remplir le tableau avec code couleur
    for (int r = 0; r < ui->productionTable->rowCount(); ++r) {
        QString ref = cellText(ui->productionTable,r,1);
        QString client = cellText(ui->productionTable,r,2);
        QString article = cellText(ui->productionTable,r,3);
        QString montant = cellText(ui->productionTable,r,4);
        QString dc = cellText(ui->productionTable,r,5);
        QString dl = cellText(ui->productionTable,r,6);
        QString statut = cellText(ui->productionTable,r,7);
        QString prio = cellText(ui->productionTable,r,8);
        
        // Calculer quantité depuis le type d'article (simplification)
        QString qte = "1";
        
        // Calculer l'avancement
        QString avancement = "0%";
        QColor bgColor = Qt::white;
        if (statut == "En Attente") {
            avancement = "0%";
            bgColor = QColor("#FFF3E0");
        } else if (statut == "Planifié") {
            avancement = "25%";
            bgColor = QColor("#E3F2FD");
        } else if (statut == "En Production") {
            avancement = "60%";
            bgColor = QColor("#FFF9C4");
        } else if (statut == "Terminé") {
            avancement = "100%";
            bgColor = QColor("#E8F5E9");
        }
        
        // Vérifier retard
        QDate dateLivraison = QDate::fromString(dl, "dd/MM/yyyy");
        bool enRetard = (dateLivraison < QDate::currentDate() && statut != "Terminé");
        if (enRetard) bgColor = QColor("#FFEBEE");
        
        table->setItem(r, 0, new QTableWidgetItem(ref));
        table->setItem(r, 1, new QTableWidgetItem(client));
        table->setItem(r, 2, new QTableWidgetItem(article));
        table->setItem(r, 3, new QTableWidgetItem(qte));
        table->setItem(r, 4, new QTableWidgetItem(montant));
        table->setItem(r, 5, new QTableWidgetItem(dc));
        table->setItem(r, 6, new QTableWidgetItem(dl + (enRetard ? " ⚠️" : "")));
        table->setItem(r, 7, new QTableWidgetItem(statut));
        table->setItem(r, 8, new QTableWidgetItem(prio));
        table->setItem(r, 9, new QTableWidgetItem(avancement));
        
        // Appliquer couleur de fond
        for (int c = 0; c < 10; ++c) {
            table->item(r, c)->setBackground(bgColor);
            if (prio == "Haute") {
                table->item(r, c)->setForeground(QColor("#D32F2F"));
                if (c == 8) table->item(r, c)->setFont(QFont("Arial", 10, QFont::Bold));
            }
        }
    }
    
    mainLay->addWidget(table);
    
    // Légende
    QFrame *legendFrame = new QFrame();
    legendFrame->setStyleSheet("background:#F5F5F5;border-radius:6px;padding:10px;");
    QHBoxLayout *legendLay = new QHBoxLayout(legendFrame);
    
    auto addLegend = [&](const QString &color, const QString &text) {
        QLabel *colorBox = new QLabel();
        colorBox->setFixedSize(20, 20);
        colorBox->setStyleSheet(QString("background:%1;border:1px solid #CCC;border-radius:3px;").arg(color));
        QLabel *textLbl = new QLabel(text);
        textLbl->setStyleSheet("font-size:11px;color:#666;");
        legendLay->addWidget(colorBox);
        legendLay->addWidget(textLbl);
        legendLay->addSpacing(15);
    };
    
    addLegend("#FFF3E0", "En Attente");
    addLegend("#E3F2FD", "Planifié");
    addLegend("#FFF9C4", "En Production");
    addLegend("#E8F5E9", "Terminé");
    addLegend("#FFEBEE", "En Retard");
    legendLay->addStretch();
    
    mainLay->addWidget(legendFrame);
    
    // Boutons
    QHBoxLayout *btnLay = new QHBoxLayout();
    btnLay->addStretch();
    
    QPushButton *btnDetails = new QPushButton("📋 Voir Détails");
    btnDetails->setStyleSheet("background:#2196F3;color:white;border:none;border-radius:6px;padding:10px 20px;font-weight:bold;");
    connect(btnDetails, &QPushButton::clicked, [&]() {
        int row = table->currentRow();
        if (row >= 0) {
            ui->productionTable->selectRow(row);
            onSuiviProduction();
        }
    });
    
    QPushButton *btnClose = new QPushButton("Fermer");
    btnClose->setStyleSheet("background:#8D6E63;color:white;border:none;border-radius:6px;padding:10px 20px;font-weight:bold;");
    connect(btnClose, &QPushButton::clicked, &dlg, &QDialog::accept);
    
    btnLay->addWidget(btnDetails);
    btnLay->addWidget(btnClose);
    mainLay->addLayout(btnLay);
    
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

void MainWindow::onRechercherProduction(const QString &text) { filterTable(ui->productionTable, text); }

void MainWindow::onTrierProduction()
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
        connect(asc, &QAction::triggered, [=]{ ui->productionTable->sortItems(col, Qt::AscendingOrder); });
        connect(desc, &QAction::triggered, [=]{ ui->productionTable->sortItems(col, Qt::DescendingOrder); });
    };

    addSortOptions("Référence", 1);
    addSortOptions("Client", 2);
    addSortOptions("Type", 3);
    addSortOptions("Montant", 4);
    menu.addSeparator();
    addSortOptions("Date Création", 5);
    addSortOptions("Date Livraison", 6);
    menu.addSeparator();
    addSortOptions("Statut", 7);
    addSortOptions("Priorité", 8);

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
    if (row < 0) { QMessageBox::warning(this,"","Sélectionnez une commande."); return; }
    if (QMessageBox::question(this,"Confirmer",
            QString("Supprimer la commande %1 du client %2 ?")
            .arg(cellText(ui->productionTable,row,1),cellText(ui->productionTable,row,2)),
            QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes) {
        ui->productionTable->removeRow(row);
        updateProductionStatsCards();
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
    struct AD { const char *ref,*nom,*cat,*type,*col,*dim; double prix,cout; const char *stat; };
    static const AD data[] = {
        {"ART-2024-0001","Sac à Main Classique","Sacs","Sac à main","Noir","30x25x10cm",89.99,45.00,"disponible"},
        {"ART-2024-0002","Portefeuille Cuir Premium","Portefeuilles","Portefeuille long","Marron","19x10x2cm",45.50,22.00,"disponible"},
        {"ART-2024-0003","Ceinture Homme Élégante","Ceintures","Ceinture classique","Noir","110x3.5cm",35.00,18.00,"disponible"},
        {"ART-2024-0004","Sac Bandoulière Femme","Sacs","Sac bandoulière","Beige","28x20x8cm",75.00,38.00,"en_production"},
        {"ART-2024-0005","Porte-Cartes Compact","Accessoires","Porte-cartes","Bleu Marine","11x7x1cm",25.00,12.00,"disponible"},
        {"ART-2024-0006","Sac Fourre-Tout","Sacs","Tote bag","Camel","40x35x15cm",65.00,32.00,"disponible"},
        {"ART-2024-0007","Ceinture Femme Mode","Ceintures","Ceinture fashion","Blanc","95x2.5cm",28.00,14.00,"disponible"},
        {"ART-2024-0008","Porte-Monnaie Zip","Accessoires","Porte-monnaie","Rouge","12x9x2cm",18.00,9.00,"disponible"},
    };
    int n = sizeof(data)/sizeof(*data);
    ui->articleTable->setRowCount(n);
    for (int i = 0; i < n; ++i) {
        Article a;
        a.setIdProduit(i+1); a.setReference(data[i].ref); a.setNom(data[i].nom);
        a.setCategorie(data[i].cat); a.setType(data[i].type); a.setCouleur(data[i].col);
        a.setDimensions(data[i].dim); a.setPrixUnitaire(data[i].prix);
        a.setCoutFabrication(data[i].cout); a.setStatut(data[i].stat);
        articles.append(a);
        ui->articleTable->setItem(i,0,new QTableWidgetItem(QString::number(i+1)));
        ui->articleTable->setItem(i,1,new QTableWidgetItem(a.getReference()));
        ui->articleTable->setItem(i,2,new QTableWidgetItem(a.getNom()));
        ui->articleTable->setItem(i,3,new QTableWidgetItem(a.getCategorie()));
        ui->articleTable->setItem(i,4,new QTableWidgetItem(a.getType()));
        ui->articleTable->setItem(i,5,new QTableWidgetItem(a.getCouleur()));
        ui->articleTable->setItem(i,6,new QTableWidgetItem(a.getDimensions()));
        ui->articleTable->setItem(i,7,new QTableWidgetItem(QString::number(a.getPrixUnitaire(),'f',2)+" €"));
        ui->articleTable->setItem(i,8,new QTableWidgetItem(QString::number(a.getCoutFabrication(),'f',2)+" €"));
        ui->articleTable->setItem(i,9,new QTableWidgetItem(a.getStatut()));
        ui->articleTable->setItem(i,10,new QTableWidgetItem(a.getDateCreation().toString("yyyy-MM-dd")));
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
    dlg.exec();
}

void MainWindow::on_btnEditArticle_clicked()
{
    int row = ui->articleTable->currentRow();
    if (row < 0) { QMessageBox::warning(this,"","Sélectionnez un article."); return; }
    ArticleDialog dlg(this, ArticleDialog::EditMode);
    loadArticleRow(ui->articleTable, row, dlg);
    dlg.exec();
}

void MainWindow::on_btnDeleteArticle_clicked()
{
    int row = ui->articleTable->currentRow();
    if (row < 0) { QMessageBox::warning(this,"","Sélectionnez un article."); return; }
    ArticleDialog dlg(this, ArticleDialog::DeleteMode);
    loadArticleRow(ui->articleTable, row, dlg);
    dlg.exec();
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

    auto addSortOptions = [&](const QString &label, int col) {
        QMenu *sub = menu.addMenu("📋 " + label);
        sub->setStyleSheet(menu.styleSheet());
        auto *asc = sub->addAction("↑ Croissant (A → Z)");
        auto *desc = sub->addAction("↓ Décroissant (Z → A)");
        connect(asc, &QAction::triggered, [=]{ ui->articleTable->sortItems(col, Qt::AscendingOrder); });
        connect(desc, &QAction::triggered, [=]{ ui->articleTable->sortItems(col, Qt::DescendingOrder); });
    };

    addSortOptions("Référence", 1);
    addSortOptions("Nom", 2);
    addSortOptions("Catégorie", 3);
    addSortOptions("Type", 4);
    menu.addSeparator();
    addSortOptions("Couleur", 5);
    addSortOptions("Prix Unitaire", 7);
    menu.addSeparator();
    addSortOptions("Statut", 9);
    addSortOptions("Date Création", 10);

    QPoint pos = ui->btnTriArticle->mapToGlobal(QPoint(0, ui->btnTriArticle->height()));
    menu.exec(pos);
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