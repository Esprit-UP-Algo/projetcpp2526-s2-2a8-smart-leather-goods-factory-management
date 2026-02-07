#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "employeedialog.h"
#include "clientmanagerdialog.h"
#include "matieredialog.h"
#include "fournisseurdialog.h"
#include "productiondialog.h"
#include <QTableWidgetItem>
#include <QDebug>
#include <QMessageBox>
#include <QDate>
#include <QInputDialog>
#include <QMenu>
#include <QDialog>
#include <QFormLayout>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDateEdit>
#include <QPrinter>
#include <QPainter>
#include <QFileDialog>
#include <QTextDocument>
#include <QTextEdit>
#include <QFrame>
#include <QFile>
#include <QTextStream>
#include <QStringConverter>
#include <QMap>
#include <algorithm>
#include <cstdlib>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChart>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , isEditMode(false)
    , editingRow(-1)
{
    ui->setupUi(this);
    
    setWindowTitle("CUIREA - Management System");
    
    // Hide vertical headers (row numbers)
    ui->employeeTable->verticalHeader()->setVisible(false);
    ui->clientTable->verticalHeader()->setVisible(false);
    ui->matiereTable->verticalHeader()->setVisible(false);
    
    // Populate employee table with sample data
    populateEmployeeTable();
    
    // Set column widths for employee table
    ui->employeeTable->setColumnWidth(0, 50);   // ID
    ui->employeeTable->setColumnWidth(1, 110);  // Matricule
    ui->employeeTable->setColumnWidth(2, 100);  // Nom
    ui->employeeTable->setColumnWidth(3, 100);  // Prenom
    ui->employeeTable->setColumnWidth(4, 100);  // CIN
    ui->employeeTable->setColumnWidth(5, 120);  // Date Naissance
    ui->employeeTable->setColumnWidth(6, 130);  // Departement
    ui->employeeTable->setColumnWidth(7, 120);  // Poste
    ui->employeeTable->setColumnWidth(8, 110);  // Telephone
    ui->employeeTable->setColumnWidth(9, 180);  // Email
    
    // Add sample clients
    Client c1;
    c1.setNom("Alami");
    c1.setPrenom("Hassan");
    c1.setSexe("Homme");
    c1.setCin("AB123456");
    c1.setPays("Maroc");
    c1.setVille("Casablanca");
    c1.setAdresse("123 Rue Mohammed V");
    c1.setEmail("h.alami@email.com");
    c1.setDateInscrit(QDate::currentDate().addDays(-30));
    clients.append(c1);
    
    Client c2;
    c2.setNom("Benali");
    c2.setPrenom("Fatima");
    c2.setSexe("Femme");
    c2.setCin("CD789012");
    c2.setPays("Maroc");
    c2.setVille("Rabat");
    c2.setAdresse("456 Avenue Hassan II");
    c2.setEmail("f.benali@email.com");
    c2.setDateInscrit(QDate::currentDate().addDays(-15));
    clients.append(c2);
    
    // Populate client table
    refreshClientTable();
    
    // Setup and populate matiere table
    setupMatiereTable();
    
    // Connect raw materials buttons
    connect(ui->btnAddMatiere, &QPushButton::clicked, this, &MainWindow::onAddMatiere);
    connect(ui->btnEditMatiere, &QPushButton::clicked, this, &MainWindow::onEditMatiere);
    connect(ui->btnDeleteMatiere, &QPushButton::clicked, this, &MainWindow::onDeleteMatiere);
    connect(ui->btnSuggestion, &QPushButton::clicked, this, &MainWindow::onSuggestionCommande);
    connect(ui->btnOptimisation, &QPushButton::clicked, this, &MainWindow::onOptimisationFIFO);
    connect(ui->btnRecherche, &QPushButton::clicked, this, &MainWindow::onRechercheTriMatiere);
    connect(ui->btnFournisseurs, &QPushButton::clicked, this, &MainWindow::onGestionFournisseurs);
    connect(ui->btnExportMatiere, &QPushButton::clicked, this, &MainWindow::onExportMatiere);
    // --- New Client Section Buttons ---
    connect(ui->btnAIAgent, &QPushButton::clicked, this, &MainWindow::on_btnAIAgent_clicked);
    connect(ui->btnStatsRegion, &QPushButton::clicked, this, &MainWindow::on_btnStatsByRegion_clicked);
    connect(ui->btnFidelityClass, &QPushButton::clicked, this, &MainWindow::on_btnFidelityClassification_clicked);

    
    // Start with employee module (page 0)
    ui->stackedWidget->setCurrentIndex(0);
    ui->profilePanel->setVisible(true);
    
    // Set initial active button style
    ui->btnEmployees->setStyleSheet(
        "QPushButton { background-color: #6E473B; color: #FFFFFF; border-left: 3px solid #FFFFFF; }"
    );
    
    // Connect table selection to profile panel update
    connect(ui->employeeTable, &QTableWidget::currentCellChanged, this, &MainWindow::onEmployeeSelected);
    
    // Setup fournisseur table
    ui->fournisseurTable->verticalHeader()->setVisible(false);
    setupFournisseurTable();
    
    // Setup production table
    ui->productionTable->verticalHeader()->setVisible(false);
    setupProductionTable();
    loadProductionData();
    
    // Connect production buttons
    connect(ui->btnCreerProduction, &QPushButton::clicked, this, &MainWindow::onCreerProduction);
    connect(ui->btnModifierProduction, &QPushButton::clicked, this, &MainWindow::onModifierProduction);
    connect(ui->btnSuiviProduction, &QPushButton::clicked, this, &MainWindow::onSuiviProduction);
    connect(ui->btnPlanificationProduction, &QPushButton::clicked, this, &MainWindow::onPlanificationProduction);
    connect(ui->btnFactureProduction, &QPushButton::clicked, this, &MainWindow::onFactureProduction);
    connect(ui->btnExcelProduction, &QPushButton::clicked, this, &MainWindow::onExcelProduction);
    connect(ui->searchBoxProduction, &QLineEdit::textChanged, this, &MainWindow::onRechercherProduction);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::refreshClientTable()
{
    ui->clientTable->setRowCount(clients.size());
    
    for (int i = 0; i < clients.size(); ++i) {
        const Client &c = clients[i];
        ui->clientTable->setItem(i, 0, new QTableWidgetItem(c.getNom()));
        ui->clientTable->setItem(i, 1, new QTableWidgetItem(c.getPrenom()));
        ui->clientTable->setItem(i, 2, new QTableWidgetItem(c.getSexe()));
        ui->clientTable->setItem(i, 3, new QTableWidgetItem(c.getCin()));
        ui->clientTable->setItem(i, 4, new QTableWidgetItem(c.getPays()));
        ui->clientTable->setItem(i, 5, new QTableWidgetItem(c.getVille()));
        ui->clientTable->setItem(i, 6, new QTableWidgetItem(c.getAdresse()));
        ui->clientTable->setItem(i, 7, new QTableWidgetItem(c.getEmail()));
    }
}

// ============================================
// NAVIGATION SLOTS (Auto-generated by Qt)
// ============================================

void MainWindow::on_btnEmployees_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);  // Show employee page
    ui->profilePanel->setVisible(true);     // Show profile panel
    setWindowTitle("CUIREA - Gestion des Employés");
    
    // Reset all navigation buttons to default style
    ui->btnEmployees->setStyleSheet("");
    ui->btnClients->setStyleSheet("");
    ui->btnProducts->setStyleSheet("");
    ui->btnRawMaterials->setStyleSheet("");
    ui->btnSuppliers->setStyleSheet("");
    
    // Apply active style to current button
    ui->btnEmployees->setStyleSheet(
        "QPushButton { background-color: #6E473B; color: #FFFFFF; border-left: 3px solid #FFFFFF; }"
    );
}

void MainWindow::on_btnClients_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);  // Show client page
    ui->profilePanel->setVisible(false);    // Hide profile panel
    setWindowTitle("CUIREA - Gestion des Clients");
    
    // Reset all navigation buttons to default style
    ui->btnEmployees->setStyleSheet("");
    ui->btnClients->setStyleSheet("");
    ui->btnProducts->setStyleSheet("");
    ui->btnRawMaterials->setStyleSheet("");
    ui->btnSuppliers->setStyleSheet("");
    
    // Apply active style to current button
    ui->btnClients->setStyleSheet(
        "QPushButton { background-color: #6E473B; color: #FFFFFF; border-left: 3px solid #FFFFFF; }"
    );
}

void MainWindow::on_btnProducts_clicked()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Module en développement");
    msgBox.setText("Gestion des Produits");
    msgBox.setInformativeText("Ce module sera disponible prochainement.\n\n"
                              "Fonctionnalités prévues :\n"
                              "• Ajout et modification\n"
                              "• Recherche et filtrage\n"
                              "• Export des données\n"
                              "• Rapports et statistiques");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStyleSheet(
        "QMessageBox { background-color: #FAF5F0; }"
        "QMessageBox QLabel { color: #291C0E; font-family: Arial, sans-serif; font-size: 12px; }"
        "QPushButton { background-color: #8D6E63; color: white; border: none; border-radius: 6px; "
        "padding: 8px 20px; font-family: Arial, sans-serif; font-size: 11px; font-weight: bold; min-width: 80px; }"
        "QPushButton:hover { background-color: #A0826D; }"
    );
    msgBox.exec();
}

/* REMOVED - Commandes button deleted
void MainWindow::on_btnOrders_clicked()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Module en développement");
    msgBox.setText("Gestion des Commandes");
    msgBox.setInformativeText("Ce module sera disponible prochainement.\n\n"
                              "Fonctionnalités prévues :\n"
                              "• Ajout et modification\n"
                              "• Recherche et filtrage\n"
                              "• Export des données\n"
                              "• Rapports et statistiques");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStyleSheet(
        "QMessageBox { background-color: #FAF5F0; }"
        "QMessageBox QLabel { color: #291C0E; font-family: Arial, sans-serif; font-size: 12px; }"
        "QPushButton { background-color: #8D6E63; color: white; border: none; border-radius: 6px; "
        "padding: 8px 20px; font-family: Arial, sans-serif; font-size: 11px; font-weight: bold; min-width: 80px; }"
        "QPushButton:hover { background-color: #A0826D; }"
    );
    msgBox.exec();
}
*/

void MainWindow::on_btnRawMaterials_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);  // Show raw materials page
    ui->profilePanel->setVisible(false);    // Hide profile panel
    setWindowTitle("CUIREA - Gestion des Matières Premières");
    
    // Reset all navigation buttons to default style
    ui->btnEmployees->setStyleSheet("");
    ui->btnClients->setStyleSheet("");
    ui->btnProducts->setStyleSheet("");
    ui->btnRawMaterials->setStyleSheet("");
    ui->btnSuppliers->setStyleSheet("");
    
    // Apply active style to current button
    ui->btnRawMaterials->setStyleSheet(
        "QPushButton { background-color: #6E473B; color: #FFFFFF; border-left: 3px solid #FFFFFF; }"
    );
}

void MainWindow::on_btnSuppliers_clicked()
{
    // Switch to suppliers page (index 3)
    ui->stackedWidget->setCurrentIndex(3);
    
    // Hide profile panel for suppliers module
    ui->profilePanel->setVisible(false);
    
    // Update button styles
    ui->btnEmployees->setStyleSheet("");
    ui->btnClients->setStyleSheet("");
    ui->btnSuppliers->setStyleSheet(
        "QPushButton { background-color: #6E473B; color: #FFFFFF; border-left: 3px solid #FFFFFF; }"
    );
    ui->btnRawMaterials->setStyleSheet("");
    ui->btnProducts->setStyleSheet("");
    ui->btnProduction->setStyleSheet("");
    
    qDebug() << "Switched to Suppliers module";
}

void MainWindow::on_btnProduction_clicked()
{
    // Switch to production page (index 4)
    ui->stackedWidget->setCurrentIndex(4);
    
    // Hide profile panel for production module
    ui->profilePanel->setVisible(false);
    
    // Update button styles
    ui->btnEmployees->setStyleSheet("");
    ui->btnClients->setStyleSheet("");
    ui->btnSuppliers->setStyleSheet("");
    ui->btnRawMaterials->setStyleSheet("");
    ui->btnProducts->setStyleSheet("");
    ui->btnProduction->setStyleSheet(
        "QPushButton { background-color: #6E473B; color: #FFFFFF; border-left: 3px solid #FFFFFF; }"
    );
    
    setWindowTitle("CUIREA - Gestion de la Production");
    
    qDebug() << "Switched to Production module";
}

// ============================================
// EMPLOYEE CRUD SLOTS (Auto-generated by Qt)
// ============================================

void MainWindow::on_btnAdd_clicked()
{
    EmployeeDialog dialog(this, EmployeeDialog::AddMode);
    dialog.exec();
}

void MainWindow::on_btnEdit_clicked()
{
    int currentRow = ui->employeeTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Aucune sélection",
                           "Veuillez sélectionner un employé à modifier.");
        return;
    }
    
    EmployeeDialog dialog(this, EmployeeDialog::EditMode);
    
    // Get employee data from table
    QString id = ui->employeeTable->item(currentRow, 0)->text();
    QString matricule = ui->employeeTable->item(currentRow, 1)->text();
    QString nom = ui->employeeTable->item(currentRow, 2)->text();
    QString prenom = ui->employeeTable->item(currentRow, 3)->text();
    QString cin = ui->employeeTable->item(currentRow, 4)->text();
    QString dateNaissance = ui->employeeTable->item(currentRow, 5)->text();
    QString departement = ui->employeeTable->item(currentRow, 6)->text();
    QString poste = ui->employeeTable->item(currentRow, 7)->text();
    QString telephone = ui->employeeTable->item(currentRow, 8)->text();
    QString email = ui->employeeTable->item(currentRow, 9)->text();
    
    dialog.setEmployeeData(id, matricule, nom, prenom, cin, dateNaissance,
                          "Homme", "", telephone, email, poste, "", departement, "01/01/2024");
    dialog.exec();
}

void MainWindow::on_btnDelete_clicked()
{
    int currentRow = ui->employeeTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Aucune sélection",
                           "Veuillez sélectionner un employé à supprimer.");
        return;
    }
    
    // Get employee data
    QString id = ui->employeeTable->item(currentRow, 0)->text();
    QString matricule = ui->employeeTable->item(currentRow, 1)->text();
    QString nom = ui->employeeTable->item(currentRow, 2)->text();
    QString prenom = ui->employeeTable->item(currentRow, 3)->text();
    QString departement = ui->employeeTable->item(currentRow, 6)->text();
    QString poste = ui->employeeTable->item(currentRow, 7)->text();
    
    EmployeeDialog dialog(this, EmployeeDialog::DeleteMode);
    dialog.setEmployeeData(id, matricule, nom, prenom, "", "", "", "", "", "", poste, "", departement, "");
    dialog.exec();
}

void MainWindow::on_btnExport_clicked()
{
    EmployeeDialog dialog(this, EmployeeDialog::ExportMode);
    dialog.exec();
}

// ============================================
// CLIENT CRUD SLOTS (Auto-generated by Qt)
// ============================================

void MainWindow::on_btnAddClient_clicked()
{
    ClientManagerDialog dlg(this, ClientManagerDialog::AddMode);
    dlg.exec();
}

void MainWindow::on_btnEditClient_clicked()
{
    int currentRow = ui->clientTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Aucune sélection",
                           "Veuillez sélectionner un client à modifier.");
        return;
    }
    
    // Get current client data
    QString nom = ui->clientTable->item(currentRow, 0)->text();
    QString prenom = ui->clientTable->item(currentRow, 1)->text();
    QString sexe = ui->clientTable->item(currentRow, 2)->text();
    QString cin = ui->clientTable->item(currentRow, 3)->text();
    QString pays = ui->clientTable->item(currentRow, 4)->text();
    QString ville = ui->clientTable->item(currentRow, 5)->text();
    QString adresse = ui->clientTable->item(currentRow, 6)->text();
    QString email = ui->clientTable->item(currentRow, 7)->text();
    
    // Open edit dialog
    ClientManagerDialog dlg(this, ClientManagerDialog::EditMode);
    dlg.setClientData(nom, prenom, sexe, cin, pays, ville, adresse, email);
    dlg.exec();
}

void MainWindow::on_btnDeleteClient_clicked()
{
    int currentRow = ui->clientTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Aucune sélection",
                           "Veuillez sélectionner un client à supprimer.");
        return;
    }
    
    // Get client data
    QString nom = ui->clientTable->item(currentRow, 0)->text();
    QString prenom = ui->clientTable->item(currentRow, 1)->text();
    QString sexe = ui->clientTable->item(currentRow, 2)->text();
    QString cin = ui->clientTable->item(currentRow, 3)->text();
    QString pays = ui->clientTable->item(currentRow, 4)->text();
    QString ville = ui->clientTable->item(currentRow, 5)->text();
    
    ClientManagerDialog dlg(this, ClientManagerDialog::DeleteMode);
    dlg.setClientData(nom, prenom, sexe, cin, pays, ville, "", "");
    dlg.exec();
}

void MainWindow::on_btnExportClient_clicked()
{
    ClientManagerDialog dialog(this, ClientManagerDialog::ExportMode);
    dialog.exec();
}

void MainWindow::on_btnRefreshClient_clicked()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Actualisation");
    msgBox.setText("Liste des clients actualisée !");
    msgBox.setInformativeText("(Mode statique - données d'exemple)");
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStyleSheet(
        "QMessageBox { background-color: #FAF5F0; }"
        "QMessageBox QLabel { color: #291C0E; font-family: Arial, sans-serif; font-size: 12px; }"
        "QPushButton { background-color: #8D6E63; color: white; border: none; border-radius: 6px; "
        "padding: 8px 20px; font-family: Arial, sans-serif; font-size: 11px; font-weight: bold; min-width: 80px; }"
        "QPushButton:hover { background-color: #A0826D; }"
    );
    msgBox.exec();
}

void MainWindow::populateEmployeeTable()
{
    qDebug() << "populateEmployeeTable called";
    
    ui->employeeTable->setRowCount(15);
    
    struct Employee {
        QString id;
        QString matricule;
        QString nom;
        QString prenom;
        QString cin;
        QString dateNaissance;
        QString departement;
        QString poste;
        QString telephone;
        QString email;
    };
    
    Employee employees[] = {
        {"001", "EMP-2024-001", "Benali", "Ahmed", "AB123456", "15/03/1985", "Production", "Supervisor", "0612345678", "a.benali@cuirea.com"},
        {"002", "EMP-2024-002", "Zahra", "Fatima", "CD234567", "22/07/1990", "Quality Control", "Inspector", "0623456789", "f.zahra@cuirea.com"},
        {"003", "EMP-2024-003", "Alami", "Mohammed", "EF345678", "10/11/1988", "Cutting", "Operator", "0634567890", "m.alami@cuirea.com"},
        {"004", "EMP-2024-004", "Mansouri", "Khadija", "GH456789", "05/02/1992", "Stitching", "Team Leader", "0645678901", "k.mansouri@cuirea.com"},
        {"005", "EMP-2024-005", "Idrissi", "Youssef", "IJ567890", "18/09/1987", "Finishing", "Technician", "0656789012", "y.idrissi@cuirea.com"},
        {"006", "EMP-2024-006", "Tazi", "Amina", "KL678901", "30/04/1991", "Design", "Designer", "0667890123", "a.tazi@cuirea.com"},
        {"007", "EMP-2024-007", "Berrada", "Hassan", "MN789012", "12/06/1983", "Warehouse", "Manager", "0678901234", "h.berrada@cuirea.com"},
        {"008", "EMP-2024-008", "Chraibi", "Salma", "OP890123", "25/12/1989", "Administration", "HR Officer", "0689012345", "s.chraibi@cuirea.com"},
        {"009", "EMP-2024-009", "Fassi", "Rachid", "QR901234", "08/08/1986", "Maintenance", "Technician", "0690123456", "r.fassi@cuirea.com"},
        {"010", "EMP-2024-010", "Lahlou", "Nadia", "ST012345", "14/01/1993", "Packaging", "Operator", "0601234567", "n.lahlou@cuirea.com"},
        {"011", "EMP-2024-011", "Benjelloun", "Karim", "UV123456", "20/05/1984", "Production", "Operator", "0612345679", "k.benjelloun@cuirea.com"},
        {"012", "EMP-2024-012", "Ouazzani", "Leila", "WX234567", "03/10/1991", "Quality Control", "Analyst", "0623456780", "l.ouazzani@cuirea.com"},
        {"013", "EMP-2024-013", "Kettani", "Omar", "YZ345678", "17/03/1988", "Cutting", "Supervisor", "0634567891", "o.kettani@cuirea.com"},
        {"014", "EMP-2024-014", "Filali", "Samira", "AB456789", "28/11/1990", "Stitching", "Operator", "0645678902", "s.filali@cuirea.com"},
        {"015", "EMP-2024-015", "Tounsi", "Mehdi", "CD567890", "09/07/1989", "Finishing", "Operator", "0656789013", "m.tounsi@cuirea.com"}
    };
    
    for (int i = 0; i < 15; ++i) {
        ui->employeeTable->setItem(i, 0, new QTableWidgetItem(employees[i].id));
        ui->employeeTable->setItem(i, 1, new QTableWidgetItem(employees[i].matricule));
        ui->employeeTable->setItem(i, 2, new QTableWidgetItem(employees[i].nom));
        ui->employeeTable->setItem(i, 3, new QTableWidgetItem(employees[i].prenom));
        ui->employeeTable->setItem(i, 4, new QTableWidgetItem(employees[i].cin));
        ui->employeeTable->setItem(i, 5, new QTableWidgetItem(employees[i].dateNaissance));
        ui->employeeTable->setItem(i, 6, new QTableWidgetItem(employees[i].departement));
        ui->employeeTable->setItem(i, 7, new QTableWidgetItem(employees[i].poste));
        ui->employeeTable->setItem(i, 8, new QTableWidgetItem(employees[i].telephone));
        ui->employeeTable->setItem(i, 9, new QTableWidgetItem(employees[i].email));
    }
    
    qDebug() << "Table populated with" << ui->employeeTable->rowCount() << "rows";
}

void MainWindow::onEmployeeSelected()
{
    int currentRow = ui->employeeTable->currentRow();
    if (currentRow < 0) {
        return;
    }
    
    QString matricule = ui->employeeTable->item(currentRow, 1)->text();
    QString nom = ui->employeeTable->item(currentRow, 2)->text();
    QString prenom = ui->employeeTable->item(currentRow, 3)->text();
    QString cin = ui->employeeTable->item(currentRow, 4)->text();
    QString dateNaissance = ui->employeeTable->item(currentRow, 5)->text();
    QString departement = ui->employeeTable->item(currentRow, 6)->text();
    QString poste = ui->employeeTable->item(currentRow, 7)->text();
    QString telephone = ui->employeeTable->item(currentRow, 8)->text();
    QString email = ui->employeeTable->item(currentRow, 9)->text();
    
    ui->profileValue->setText(matricule);
    ui->profileValue_2->setText(nom);
    ui->profileValue_3->setText(prenom);
    ui->profileValue_4->setText(cin);
    ui->profileValue_5->setText(dateNaissance);
    ui->profileValue_6->setText("M");
    ui->profileValue_7->setText("Casablanca");
    ui->profileValue_8->setText(telephone);
    ui->profileValue_9->setText(email);
    ui->profileValue_10->setText(poste);
    ui->profileValue_11->setText(departement);
    ui->profileValue_12->setText(departement);
    ui->profileValue_13->setText("01/01/2024");
    
    ui->photoPlaceholder->clear();
    ui->photoPlaceholder->setText("●");
}

// ============================================
// RAW MATERIALS MANAGEMENT METHODS
// ============================================

void MainWindow::setupMatiereTable()
{
    ui->matiereTable->setRowCount(3);
    
    // Sample data
    QStringList row1 = {"Cuir Pleine Fleur", "Peau de Veau", "Peau de Veau", "2.5 m²/jour", "80", "2028-05-31"};
    QStringList row2 = {"Ficelinée", "Peau de Vachette", "Peau de Vachette", "1.8 m²/jour", "60", "2028-05-31"};
    QStringList row3 = {"LR-004", "D-oci", "Ficelinée", "15 bobines/jour", "50", "2026-05-31"};
    
    QList<QStringList> rows = {row1, row2, row3};
    
    for (int r = 0; r < rows.size(); ++r) {
        for (int c = 0; c < rows[r].size(); ++c) {
            ui->matiereTable->setItem(r, c, new QTableWidgetItem(rows[r][c]));
        }
    }
    
    ui->matiereTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->matiereTable->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void MainWindow::setupSuggestionTable()
{
    // Not needed in current implementation
}

void MainWindow::setupHistoriqueTable()
{
    // Not needed in current implementation
}

void MainWindow::showForm(bool editMode)
{
    Q_UNUSED(editMode)
    // Not needed - using dialog instead
}

void MainWindow::hideForm()
{
    // Not needed - using dialog instead
}

void MainWindow::clearForm()
{
    // Not needed - using dialog instead
}

void MainWindow::hideAllPanels()
{
    // Not needed in current implementation
}

void MainWindow::addMatiereToTable(const QString &module, const QString &reference, const QString &type,
                                   const QString &quantite, const QString &seuil, const QString &dateExp)
{
    Q_UNUSED(module)
    Q_UNUSED(reference)
    Q_UNUSED(type)
    Q_UNUSED(quantite)
    Q_UNUSED(seuil)
    Q_UNUSED(dateExp)
    // Not needed - adding directly in onAddMatiere
}

void MainWindow::onAddMatiere()
{
    MatiereDialog dialog(this, MatiereDialog::AddMode);
    if (dialog.exec() == QDialog::Accepted) {
        // Add to table
        int row = ui->matiereTable->rowCount();
        ui->matiereTable->insertRow(row);
        
        ui->matiereTable->setItem(row, 0, new QTableWidgetItem(dialog.getModule()));
        ui->matiereTable->setItem(row, 1, new QTableWidgetItem(dialog.getReference()));
        ui->matiereTable->setItem(row, 2, new QTableWidgetItem(dialog.getType()));
        ui->matiereTable->setItem(row, 3, new QTableWidgetItem(dialog.getQuantite()));
        ui->matiereTable->setItem(row, 4, new QTableWidgetItem(dialog.getSeuil()));
        ui->matiereTable->setItem(row, 5, new QTableWidgetItem(dialog.getDateExpiration()));
        
        // Update statistics
        updateMatiereStatistics();
    }
}

void MainWindow::onEditMatiere()
{
    int currentRow = ui->matiereTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Aucune sélection", 
                           "Veuillez sélectionner une matière première à modifier.");
        return;
    }
    
    MatiereDialog dialog(this, MatiereDialog::EditMode);
    
    // Load current data
    dialog.setMatiereData(
        ui->matiereTable->item(currentRow, 0)->text(),
        ui->matiereTable->item(currentRow, 1)->text(),
        ui->matiereTable->item(currentRow, 2)->text(),
        ui->matiereTable->item(currentRow, 3)->text(),
        ui->matiereTable->item(currentRow, 4)->text(),
        ui->matiereTable->item(currentRow, 5)->text()
    );
    
    if (dialog.exec() == QDialog::Accepted) {
        // Update table
        ui->matiereTable->item(currentRow, 0)->setText(dialog.getModule());
        ui->matiereTable->item(currentRow, 1)->setText(dialog.getReference());
        ui->matiereTable->item(currentRow, 2)->setText(dialog.getType());
        ui->matiereTable->item(currentRow, 3)->setText(dialog.getQuantite());
        ui->matiereTable->item(currentRow, 4)->setText(dialog.getSeuil());
        ui->matiereTable->item(currentRow, 5)->setText(dialog.getDateExpiration());
        
        // Update statistics
        updateMatiereStatistics();
    }
}

void MainWindow::onDeleteMatiere()
{
    int currentRow = ui->matiereTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Aucune sélection", 
                           "Veuillez sélectionner une matière première à supprimer.");
        return;
    }
    
    MatiereDialog dialog(this, MatiereDialog::DeleteMode);
    
    // Load current data (read-only)
    dialog.setMatiereData(
        ui->matiereTable->item(currentRow, 0)->text(),
        ui->matiereTable->item(currentRow, 1)->text(),
        ui->matiereTable->item(currentRow, 2)->text(),
        ui->matiereTable->item(currentRow, 3)->text(),
        ui->matiereTable->item(currentRow, 4)->text(),
        ui->matiereTable->item(currentRow, 5)->text()
    );
    
    if (dialog.exec() == QDialog::Accepted) {
        ui->matiereTable->removeRow(currentRow);
        
        // Update statistics
        updateMatiereStatistics();
    }
}

void MainWindow::onSaveMatiere()
{
    // Not needed - using dialog instead
}

void MainWindow::onCancelForm()
{
    // Not needed - using dialog instead
}

void MainWindow::onSuggestionCommande()
{
    QString suggestions;
    suggestions += "📊 SUGGESTIONS DE COMMANDE AUTOMATIQUES\n\n";
    
    for (int row = 0; row < ui->matiereTable->rowCount(); ++row) {
        QString matiere = ui->matiereTable->item(row, 0)->text();
        QString consommationStr = ui->matiereTable->item(row, 3)->text();
        QString seuilStr = ui->matiereTable->item(row, 4)->text();
        
        double consommation = consommationStr.split(" ").first().toDouble();
        int seuil = seuilStr.toInt();
        double stockNecessaire30j = consommation * 30;
        
        if (stockNecessaire30j > seuil * 0.5) {
            QString priorite = (stockNecessaire30j > seuil) ? "🔴 URGENT" : "🟡 NORMAL";
            int quantiteSuggeree = static_cast<int>(stockNecessaire30j * 1.5);
            
            suggestions += QString("%1 - %2\n").arg(priorite, matiere);
            suggestions += QString("   Consommation: %1/jour\n").arg(consommation);
            suggestions += QString("   Seuil actuel: %2\n").arg(seuil);
            suggestions += QString("   Quantité suggérée: %3\n").arg(quantiteSuggeree);
            suggestions += QString("   Délai recommandé: %4\n\n")
                .arg((stockNecessaire30j > seuil) ? "3-5 jours" : "7-14 jours");
        }
    }
    
    if (suggestions.count('\n') <= 2) {
        suggestions += "✅ Aucune commande urgente nécessaire.\n";
        suggestions += "Tous les stocks sont à des niveaux acceptables.";
    }
    
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Suggestions de Commande");
    msgBox.setText(suggestions);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStyleSheet(
        "QMessageBox { background-color: #FAF5F0; min-width: 500px; }"
        "QMessageBox QLabel { color: #291C0E; font-family: 'Courier New', monospace; font-size: 11px; }"
        "QPushButton { background-color: #8D6E63; color: white; border: none; border-radius: 6px; "
        "padding: 8px 20px; font-family: Arial, sans-serif; font-size: 11px; font-weight: bold; min-width: 80px; }"
        "QPushButton:hover { background-color: #A0826D; }"
    );
    msgBox.exec();
}

void MainWindow::onOptimisationFIFO()
{
    QString fifoAnalysis;
    fifoAnalysis += "🔄 ANALYSE FIFO (First In, First Out)\n\n";
    
    QList<QPair<QString, int>> urgentItems;
    QList<QPair<QString, int>> normalItems;
    QList<QPair<QString, int>> safeItems;
    
    for (int row = 0; row < ui->matiereTable->rowCount(); ++row) {
        QString matiere = ui->matiereTable->item(row, 0)->text();
        QString dateExp = ui->matiereTable->item(row, 5)->text();
        
        int daysToExpiration = calculateDaysToExpiration(dateExp);
        
        if (daysToExpiration <= 30) {
            urgentItems.append(qMakePair(matiere, daysToExpiration));
        } else if (daysToExpiration <= 90) {
            normalItems.append(qMakePair(matiere, daysToExpiration));
        } else {
            safeItems.append(qMakePair(matiere, daysToExpiration));
        }
    }
    
    fifoAnalysis += "🔴 À CONSOMMER EN URGENCE (≤ 30 jours):\n";
    if (urgentItems.isEmpty()) {
        fifoAnalysis += "   Aucun\n";
    } else {
        for (const auto &item : urgentItems) {
            fifoAnalysis += QString("   • %1 - Expire dans %2 jours\n")
                .arg(item.first).arg(item.second);
        }
    }
    
    fifoAnalysis += "\n🟡 À CONSOMMER NORMALEMENT (31-90 jours):\n";
    if (normalItems.isEmpty()) {
        fifoAnalysis += "   Aucun\n";
    } else {
        for (const auto &item : normalItems) {
            fifoAnalysis += QString("   • %1 - Expire dans %2 jours\n")
                .arg(item.first).arg(item.second);
        }
    }
    
    fifoAnalysis += "\n🟢 STOCK SÛR (> 90 jours):\n";
    if (safeItems.isEmpty()) {
        fifoAnalysis += "   Aucun\n";
    } else {
        for (const auto &item : safeItems) {
            fifoAnalysis += QString("   • %1 - Expire dans %2 jours\n")
                .arg(item.first).arg(item.second);
        }
    }
    
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Optimisation FIFO");
    msgBox.setText(fifoAnalysis);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStyleSheet(
        "QMessageBox { background-color: #FAF5F0; min-width: 500px; }"
        "QMessageBox QLabel { color: #291C0E; font-family: 'Courier New', monospace; font-size: 11px; }"
        "QPushButton { background-color: #8D6E63; color: white; border: none; border-radius: 6px; "
        "padding: 8px 20px; font-family: Arial, sans-serif; font-size: 11px; font-weight: bold; min-width: 80px; }"
        "QPushButton:hover { background-color: #A0826D; }"
    );
    msgBox.exec();
}

void MainWindow::onExportMatiere()
{
    QString exportData;
    exportData += "═══════════════════════════════════════════════════════\n";
    exportData += "        CUIREA - RAPPORT DES MATIÈRES PREMIÈRES\n";
    exportData += "═══════════════════════════════════════════════════════\n\n";
    exportData += QString("Date d'export: %1\n\n").arg(QDate::currentDate().toString("dd/MM/yyyy"));
    
    exportData += "STATISTIQUES GÉNÉRALES:\n";
    exportData += QString("  • Total matières: %1\n").arg(ui->matiereTable->rowCount());
    exportData += QString("  • Stock critique: %1\n").arg(ui->statsValueMatiere2->text());
    exportData += QString("  • Fournisseurs: %1\n\n").arg(ui->statsValueMatiere3->text());
    
    exportData += "DÉTAIL DES MATIÈRES:\n";
    exportData += "───────────────────────────────────────────────────────\n";
    
    for (int row = 0; row < ui->matiereTable->rowCount(); ++row) {
        exportData += QString("\n%1. %2\n").arg(row + 1).arg(ui->matiereTable->item(row, 0)->text());
        exportData += QString("   Référence: %1\n").arg(ui->matiereTable->item(row, 1)->text());
        exportData += QString("   Type: %1\n").arg(ui->matiereTable->item(row, 2)->text());
        exportData += QString("   Consommation: %1\n").arg(ui->matiereTable->item(row, 3)->text());
        exportData += QString("   Seuil: %1\n").arg(ui->matiereTable->item(row, 4)->text());
        exportData += QString("   Expiration: %1\n").arg(ui->matiereTable->item(row, 5)->text());
    }
    
    exportData += "\n═══════════════════════════════════════════════════════\n";
    exportData += "              Fin du rapport\n";
    exportData += "═══════════════════════════════════════════════════════\n";
    
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Export PDF - Aperçu");
    msgBox.setText("Aperçu du rapport à exporter:");
    msgBox.setDetailedText(exportData);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStyleSheet(
        "QMessageBox { background-color: #FAF5F0; min-width: 600px; }"
        "QMessageBox QLabel { color: #291C0E; font-family: Arial, sans-serif; font-size: 12px; }"
        "QPushButton { background-color: #8D6E63; color: white; border: none; border-radius: 6px; "
        "padding: 8px 20px; font-family: Arial, sans-serif; font-size: 11px; font-weight: bold; min-width: 80px; }"
        "QPushButton:hover { background-color: #A0826D; }"
    );
    msgBox.exec();
    
    QMessageBox::information(this, "Export", 
        "📄 Le rapport a été généré avec succès!\n\n"
        "Note: L'export PDF complet sera disponible dans la version finale.");
}

void MainWindow::onRechercheTriMatiere()
{
    QMessageBox::information(this, "Recherche & Tri", 
        "🔍 Fonctionnalité de recherche avancée\n\n"
        "Cette fonctionnalité permettra de:\n"
        "• Rechercher par nom de matière\n"
        "• Filtrer par type\n"
        "• Trier par date d'expiration\n"
        "• Filtrer par niveau de stock\n\n"
        "Disponible dans la prochaine version.");
}

void MainWindow::onGestionFournisseurs()
{
    QString fournisseurs;
    fournisseurs += "👥 GESTION DES FOURNISSEURS\n\n";
    fournisseurs += "📋 Liste des fournisseurs:\n\n";
    fournisseurs += "1. Tannerie Martin\n";
    fournisseurs += "   Contact: Jean Martin\n";
    fournisseurs += "   Tél: 01 23 45 67 89\n";
    fournisseurs += "   Email: contact@tannerie-martin.fr\n";
    fournisseurs += "   Matières: Cuir Pleine Fleur, Peau de Veau\n\n";
    fournisseurs += "2. Quincaillerie Dubois\n";
    fournisseurs += "   Contact: Marie Dubois\n";
    fournisseurs += "   Tél: 01 98 76 54 32\n";
    fournisseurs += "   Email: marie@quincaillerie-dubois.fr\n";
    fournisseurs += "   Matières: Ficelinée, Accessoires\n\n";
    fournisseurs += "───────────────────────────────────────\n";
    fournisseurs += "Fonctionnalités complètes disponibles\n";
    fournisseurs += "dans la prochaine version.";
    
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Gestion des Fournisseurs");
    msgBox.setText(fournisseurs);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStyleSheet(
        "QMessageBox { background-color: #FAF5F0; min-width: 450px; }"
        "QMessageBox QLabel { color: #291C0E; font-family: 'Courier New', monospace; font-size: 11px; }"
        "QPushButton { background-color: #8D6E63; color: white; border: none; border-radius: 6px; "
        "padding: 8px 20px; font-family: Arial, sans-serif; font-size: 11px; font-weight: bold; min-width: 80px; }"
        "QPushButton:hover { background-color: #A0826D; }"
    );
    msgBox.exec();
}

void MainWindow::onGenerateSuggestion()
{
    // Redirect to main suggestion method
    onSuggestionCommande();
}

void MainWindow::onCloseSuggestion()
{
    // Close suggestion panel
}

void MainWindow::onAnalyzeFIFO()
{
    // Redirect to main FIFO method
    onOptimisationFIFO();
}

void MainWindow::onCloseOptimisation()
{
    // Close optimisation panel
}

void MainWindow::onAppliquerRecherche()
{
    // Redirect to main search method
    onRechercheTriMatiere();
}

void MainWindow::onResetRecherche()
{
    // Reset search filters
}

void MainWindow::onCloseRecherche()
{
    // Close recherche panel
}

void MainWindow::onAddFournisseur()
{
    QMessageBox::information(this, "Ajouter Fournisseur", "Fonctionnalité en développement");
}

void MainWindow::onEditFournisseur()
{
    QMessageBox::information(this, "Modifier Fournisseur", "Fonctionnalité en développement");
}

void MainWindow::onDeleteFournisseur()
{
    QMessageBox::information(this, "Supprimer Fournisseur", "Fonctionnalité en développement");
}

void MainWindow::onCloseFournisseurs()
{
    // Close fournisseurs panel
}

void MainWindow::loadFournisseurs()
{
    // Load fournisseurs data
}

void MainWindow::saveFournisseurs()
{
    // Save fournisseurs data
}

void MainWindow::updateFournisseursList()
{
    // Update fournisseurs list
}

void MainWindow::updateHistoriqueTable()
{
    // Update historique table
}

void MainWindow::updateConsommationChart()
{
    // Update consumption chart
}

int MainWindow::calculateDaysToExpiration(const QString &dateStr)
{
    QDate expirationDate = QDate::fromString(dateStr, "yyyy-MM-dd");
    QDate currentDate = QDate::currentDate();
    return currentDate.daysTo(expirationDate);
}

QString MainWindow::getStockLevel(int currentStock, int threshold)
{
    if (currentStock < threshold * 0.5) {
        return "Critique";
    } else if (currentStock < threshold) {
        return "Normal";
    } else {
        return "Élevé";
    }
}

int MainWindow::calculateSuggestedQuantity(const QString &matiere)
{
    Q_UNUSED(matiere)
    return 100; // Default value
}

void MainWindow::updateMatiereStatistics()
{
    // Update total count
    int totalMatieres = ui->matiereTable->rowCount();
    ui->statsValueMatiere1->setText(QString::number(totalMatieres));
    
    // Calculate critical stock count
    int criticalCount = 0;
    for (int row = 0; row < ui->matiereTable->rowCount(); ++row) {
        QString consommationStr = ui->matiereTable->item(row, 3)->text();
        QString seuilStr = ui->matiereTable->item(row, 4)->text();
        
        // Extract numeric value
        double consommation = consommationStr.split(" ").first().toDouble();
        int seuil = seuilStr.toInt();
        
        // Check if critical (consumption * 30 days > threshold)
        if (consommation * 30 > seuil) {
            criticalCount++;
        }
    }
    ui->statsValueMatiere2->setText(QString::number(criticalCount));
    
    // Fournisseurs count (static for now)
    ui->statsValueMatiere3->setText("2");
}


// ============================================
// SUPPLIERS MANAGEMENT
// ============================================

void MainWindow::setupFournisseurTable()
{
    // Add sample suppliers data
    fournisseursData.clear();
    
    fournisseursData.append(FournisseurData("F001", "Leather Masters SA", "contact@leathermasters.tn", "+216 71 234 567", "1234567/A/M/000", "Cuir", "Chèque", "Actif"));
    fournisseursData.append(FournisseurData("F002", "Textile Excellence", "info@textileex.tn", "+216 71 345 678", "2345678/B/M/000", "Tissus", "Virement", "Actif"));
    fournisseursData.append(FournisseurData("F003", "Fashion Accessories Ltd", "sales@fashionacc.tn", "+216 71 456 789", "3456789/C/M/000", "Accessoires", "Carte Bancaire", "Actif"));
    fournisseursData.append(FournisseurData("F004", "Premium Bags Co", "contact@premiumbags.tn", "+216 71 567 890", "4567890/D/M/000", "Sacs", "Crédit 30 jours", "Actif"));
    fournisseursData.append(FournisseurData("F005", "Style Clothing", "info@styleclothing.tn", "+216 71 678 901", "5678901/E/M/000", "Vêtements", "Virement", "Actif"));
    fournisseursData.append(FournisseurData("F006", "Quality Leather Supply", "sales@qualityleather.tn", "+216 71 789 012", "6789012/F/M/000", "Cuir", "Espèces", "Suspendu"));
    fournisseursData.append(FournisseurData("F007", "Modern Textiles", "contact@moderntex.tn", "+216 71 890 123", "7890123/G/M/000", "Tissus", "Crédit 60 jours", "Actif"));
    fournisseursData.append(FournisseurData("F008", "Elite Accessories", "info@eliteacc.tn", "+216 71 901 234", "8901234/H/M/000", "Accessoires", "Chèque", "Actif"));
    fournisseursData.append(FournisseurData("F009", "Luxury Bags Import", "sales@luxurybags.tn", "+216 71 012 345", "9012345/I/M/000", "Sacs", "Virement", "Actif"));
    fournisseursData.append(FournisseurData("F010", "Fashion Forward", "contact@fashionforward.tn", "+216 71 123 456", "0123456/J/M/000", "Vêtements", "Carte Bancaire", "Actif"));
    fournisseursData.append(FournisseurData("F011", "Artisan Leather Works", "info@artisanleather.tn", "+216 71 234 567", "1234568/K/M/000", "Cuir", "Crédit 30 jours", "Actif"));
    fournisseursData.append(FournisseurData("F012", "Fabric World", "sales@fabricworld.tn", "+216 71 345 679", "2345679/L/M/000", "Tissus", "Chèque", "Actif"));
    fournisseursData.append(FournisseurData("F013", "Trendy Accessories", "contact@trendyacc.tn", "+216 71 456 790", "3456790/M/M/000", "Accessoires", "Virement", "Suspendu"));
    fournisseursData.append(FournisseurData("F014", "Designer Bags Plus", "info@designerbags.tn", "+216 71 567 891", "4567891/N/M/000", "Sacs", "Carte Bancaire", "Actif"));
    fournisseursData.append(FournisseurData("F015", "Urban Clothing Co", "sales@urbanclothing.tn", "+216 71 678 902", "5678902/O/M/000", "Vêtements", "Espèces", "Actif"));
    
    refreshFournisseurTable();
    updateFournisseurStatistics();
}

void MainWindow::refreshFournisseurTable()
{
    ui->fournisseurTable->setRowCount(fournisseursData.size());
    
    for (int i = 0; i < fournisseursData.size(); ++i) {
        const FournisseurData &f = fournisseursData[i];
        ui->fournisseurTable->setItem(i, 0, new QTableWidgetItem(f.getId()));
        ui->fournisseurTable->setItem(i, 1, new QTableWidgetItem(f.getNomEntreprise()));
        ui->fournisseurTable->setItem(i, 2, new QTableWidgetItem(f.getEmail()));
        ui->fournisseurTable->setItem(i, 3, new QTableWidgetItem(f.getTelephone()));
        ui->fournisseurTable->setItem(i, 4, new QTableWidgetItem(f.getMatriculeFiscal()));
        ui->fournisseurTable->setItem(i, 5, new QTableWidgetItem(f.getTypeProduit()));
        ui->fournisseurTable->setItem(i, 6, new QTableWidgetItem(f.getConditionPaiement()));
        ui->fournisseurTable->setItem(i, 7, new QTableWidgetItem(f.getStatut()));
    }
}

void MainWindow::updateFournisseurStatistics()
{
    // Update total count
    int total = fournisseursData.size();
    ui->statsValueFournisseur1->setText(QString::number(total));
    
    // Count active suppliers
    int actifs = 0;
    QSet<QString> typesSet;
    
    for (const FournisseurData &f : fournisseursData) {
        if (f.getStatut() == "Actif") {
            actifs++;
        }
        typesSet.insert(f.getTypeProduit());
    }
    
    ui->statsValueFournisseur2->setText(QString::number(actifs));
    ui->statsValueFournisseur3->setText(QString::number(typesSet.size()));
}


// ============================================
// SUPPLIERS CRUD OPERATIONS
// ============================================

void MainWindow::on_btnAddFournisseur_clicked()
{
    FournisseurDialog dialog(this, FournisseurDialog::AddMode);
    if (dialog.exec() == QDialog::Accepted) {
        // Non fonctionnel - juste l'interface
        QMessageBox::information(this, "Information", 
                                 "Interface de démonstration - Fonctionnalité non implémentée");
    }
}

void MainWindow::on_btnEditFournisseur_clicked()
{
    // Check if a row is selected
    int currentRow = ui->fournisseurTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Aucune sélection", 
                            "Veuillez sélectionner un fournisseur à modifier.");
        return;
    }
    
    FournisseurDialog dialog(this, FournisseurDialog::EditMode);
    
    // Load selected supplier data
    FournisseurData fournisseur = fournisseursData[currentRow];
    dialog.setFournisseurData(
        fournisseur.getId(),
        fournisseur.getNomEntreprise(),
        fournisseur.getEmail(),
        fournisseur.getTelephone(),
        fournisseur.getTypeProduit(),
        fournisseur.getConditionPaiement(),
        fournisseur.getMatriculeFiscal(),
        fournisseur.getStatut()
    );
    
    if (dialog.exec() == QDialog::Accepted) {
        // Non fonctionnel - juste l'interface
        QMessageBox::information(this, "Information", 
                                 "Interface de démonstration - Fonctionnalité non implémentée");
    }
}

void MainWindow::on_btnDeleteFournisseur_clicked()
{
    // Check if a row is selected
    int currentRow = ui->fournisseurTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Aucune sélection", 
                            "Veuillez sélectionner un fournisseur à supprimer.");
        return;
    }
    
    FournisseurDialog dialog(this, FournisseurDialog::DeleteMode);
    
    // Load selected supplier data
    FournisseurData fournisseur = fournisseursData[currentRow];
    dialog.setFournisseurData(
        fournisseur.getId(),
        fournisseur.getNomEntreprise(),
        fournisseur.getEmail(),
        fournisseur.getTelephone(),
        fournisseur.getTypeProduit(),
        fournisseur.getConditionPaiement(),
        fournisseur.getMatriculeFiscal(),
        fournisseur.getStatut()
    );
    
    if (dialog.exec() == QDialog::Accepted) {
        // Non fonctionnel - juste l'interface
        QMessageBox::information(this, "Information", 
                                 "Interface de démonstration - Fonctionnalité non implémentée");
    }
}

void MainWindow::on_btnExportFournisseur_clicked()
{
    QMessageBox::information(this, "Exporter", 
                             "Fonctionnalité d'export des fournisseurs en cours de développement.\n\n"
                             "Formats prévus:\n"
                             "• PDF\n"
                             "• Excel (CSV)\n"
                             "• JSON");
}

void MainWindow::on_searchBoxFournisseur_textChanged(const QString &text)
{
    QString searchText = text.trimmed().toLower();
    
    // If search is empty, show all rows
    if (searchText.isEmpty()) {
        for (int row = 0; row < ui->fournisseurTable->rowCount(); ++row) {
            ui->fournisseurTable->setRowHidden(row, false);
        }
        return;
    }
    
    // Search in all columns
    for (int row = 0; row < ui->fournisseurTable->rowCount(); ++row) {
        bool match = false;
        
        for (int col = 0; col < ui->fournisseurTable->columnCount(); ++col) {
            QTableWidgetItem* item = ui->fournisseurTable->item(row, col);
            if (item && item->text().toLower().contains(searchText)) {
                match = true;
                break;
            }
        }
        
        ui->fournisseurTable->setRowHidden(row, !match);
    }
}
void MainWindow::on_btnAIAgent_clicked()
{
    QMessageBox::information(this, "Agent IA",
                             "🔍 Permet de rechercher rapidement un client dans la base de données.");

    QString buttonStyle =
        "QPushButton {"
        "  background-color: #8D6E63;"
        "  color: white;"
        "  border: none;"
        "  border-radius: 10px;"
        "  padding: 10px 20px;"
        "  font-family: Arial, sans-serif;"
        "  font-size: 12px;"
        "  font-weight: bold;"
        "  text-align: center;"
        "}"
        "QPushButton:hover {"
        "  background-color: #A98273;" // slightly lighter on hover
        "}";

    ui->btnAIAgent->setStyleSheet(buttonStyle);
}

void MainWindow::on_btnStatsByRegion_clicked()
{
    QMessageBox::information(this, "Statistiques par Région",
                             "📊 Affiche le nombre de clients par région (statistiques simples).");

    QString buttonStyle =
        "QPushButton {"
        "  background-color: #8D6E63;"
        "  color: white;"
        "  border: none;"
        "  border-radius: 10px;"
        "  padding: 10px 20px;"
        "  font-family: Arial, sans-serif;"
        "  font-size: 12px;"
        "  font-weight: bold;"
        "  text-align: center;"
        "}"
        "QPushButton:hover {"
        "  background-color: #A98273;" // slightly lighter on hover
        "}";

    ui->btnStatsRegion->setStyleSheet(buttonStyle);
}

void MainWindow::on_btnFidelityClassification_clicked()
{
    QMessageBox::information(this, "Classification Fidélité",
                             "🏆 Classe les clients selon leur fidélité (nombre de commandes).");

    QString buttonStyle =
        "QPushButton {"
        "  background-color: #8D6E63;"
        "  color: white;"
        "  border: none;"
        "  border-radius: 10px;"
        "  padding: 10px 20px;"
        "  font-family: Arial, sans-serif;"
        "  font-size: 12px;"
        "  font-weight: bold;"
        "  text-align: center;"
        "}"
        "QPushButton:hover {"
        "  background-color: #A98273;" // slightly lighter on hover
        "}";

    ui->btnFidelityClass->setStyleSheet(buttonStyle);
}




// ============================================
// PRODUCTION MANAGEMENT METHODS
// ============================================

void MainWindow::setupProductionTable()
{
    // Configure table columns
    ui->productionTable->setColumnWidth(0, 50);   // ID
    ui->productionTable->setColumnWidth(1, 120);  // Référence
    ui->productionTable->setColumnWidth(2, 150);  // Client
    ui->productionTable->setColumnWidth(3, 120);  // Type
    ui->productionTable->setColumnWidth(4, 100);  // Montant HT
    ui->productionTable->setColumnWidth(5, 110);  // Date Création
    ui->productionTable->setColumnWidth(6, 110);  // Date Livraison
    ui->productionTable->setColumnWidth(7, 100);  // Statut
    ui->productionTable->setColumnWidth(8, 80);   // Priorité
    
    ui->productionTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->productionTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->productionTable->setAlternatingRowColors(true);
    
    // Enable context menu
    ui->productionTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->productionTable, &QTableWidget::customContextMenuRequested, 
            this, &MainWindow::onProductionTableContextMenu);
}

void MainWindow::loadProductionData()
{
    // Clear existing data
    ui->productionTable->setRowCount(0);
    
    // Sample production data
    ajouterCommandeProduction("PROD-2024-001", "Leather Masters SA", "Sacs en Cuir", "15,500.00 DT", 
                             "2024-01-15", "2024-02-15", "En Production", "Haute");
    ajouterCommandeProduction("PROD-2024-002", "Textile Excellence", "Vêtements", "8,200.00 DT", 
                             "2024-01-18", "2024-02-20", "Planifié", "Moyenne");
    ajouterCommandeProduction("PROD-2024-003", "Fashion Accessories Ltd", "Accessoires", "3,750.00 DT", 
                             "2024-01-20", "2024-02-10", "En Production", "Haute");
    ajouterCommandeProduction("PROD-2024-004", "Premium Bags Co", "Sacs Premium", "22,000.00 DT", 
                             "2024-01-22", "2024-03-01", "En Attente", "Basse");
    ajouterCommandeProduction("PROD-2024-005", "Style Clothing", "Collection Été", "12,800.00 DT", 
                             "2024-01-25", "2024-02-28", "En Production", "Moyenne");
    ajouterCommandeProduction("PROD-2024-006", "Quality Leather Supply", "Cuir Traité", "9,500.00 DT", 
                             "2024-01-28", "2024-02-25", "Terminé", "Moyenne");
    ajouterCommandeProduction("PROD-2024-007", "Modern Textiles", "Tissus Techniques", "6,300.00 DT", 
                             "2024-02-01", "2024-03-05", "Planifié", "Basse");
    ajouterCommandeProduction("PROD-2024-008", "Elite Accessories", "Bijoux Mode", "4,200.00 DT", 
                             "2024-02-03", "2024-02-18", "En Production", "Haute");
    ajouterCommandeProduction("PROD-2024-009", "Luxury Bags Import", "Sacs Luxe", "28,500.00 DT", 
                             "2024-02-05", "2024-03-15", "En Attente", "Haute");
    ajouterCommandeProduction("PROD-2024-010", "Fashion Forward", "Prêt-à-Porter", "11,200.00 DT", 
                             "2024-02-07", "2024-03-10", "Planifié", "Moyenne");
}

void MainWindow::ajouterCommandeProduction(const QString &ref, const QString &client, const QString &type,
                                          const QString &montant, const QString &dateCreation,
                                          const QString &dateLivraison, const QString &statut, const QString &priorite)
{
    int row = ui->productionTable->rowCount();
    ui->productionTable->insertRow(row);
    
    ui->productionTable->setItem(row, 0, new QTableWidgetItem(QString::number(row + 1)));
    ui->productionTable->setItem(row, 1, new QTableWidgetItem(ref));
    ui->productionTable->setItem(row, 2, new QTableWidgetItem(client));
    ui->productionTable->setItem(row, 3, new QTableWidgetItem(type));
    ui->productionTable->setItem(row, 4, new QTableWidgetItem(montant));
    ui->productionTable->setItem(row, 5, new QTableWidgetItem(dateCreation));
    ui->productionTable->setItem(row, 6, new QTableWidgetItem(dateLivraison));
    ui->productionTable->setItem(row, 7, new QTableWidgetItem(statut));
    ui->productionTable->setItem(row, 8, new QTableWidgetItem(priorite));
}

void MainWindow::onCreerProduction()
{
    // Créer un dialogue personnalisé
    QDialog dialog(this);
    dialog.setWindowTitle("Créer une nouvelle commande");
    dialog.setMinimumWidth(550);
    dialog.setStyleSheet(
        "QDialog { background-color: #FFFFFF; }"
        "QLabel { color: #2C2416; font-size: 13px; font-weight: 500; }"
        "QLineEdit { "
        "   background-color: #F8F8F8; "
        "   border: 2px solid #E0E0E0; "
        "   border-radius: 6px; "
        "   padding: 10px; "
        "   font-size: 13px; "
        "   color: #2C2416; "
        "}"
        "QLineEdit:focus { "
        "   border: 2px solid #8D6E63; "
        "   background-color: #FFFFFF; "
        "}"
        "QComboBox { "
        "   background-color: #F8F8F8; "
        "   border: 2px solid #E0E0E0; "
        "   border-radius: 6px; "
        "   padding: 10px; "
        "   font-size: 13px; "
        "   color: #2C2416; "
        "}"
        "QComboBox:focus { border: 2px solid #8D6E63; }"
        "QDateEdit { "
        "   background-color: #F8F8F8; "
        "   border: 2px solid #E0E0E0; "
        "   border-radius: 6px; "
        "   padding: 10px; "
        "   font-size: 13px; "
        "   color: #2C2416; "
        "}"
        "QDateEdit:focus { border: 2px solid #8D6E63; }"
        "QPushButton { "
        "   background-color: #8D6E63; "
        "   color: white; "
        "   border: none; "
        "   border-radius: 6px; "
        "   padding: 10px 20px; "
        "   font-size: 13px; "
        "   font-weight: 500; "
        "}"
        "QPushButton:hover { background-color: #A0826D; }"
    );
    
    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    
    QLabel *titre = new QLabel("➕ Créer une nouvelle commande");
    titre->setStyleSheet("font-size: 20px; font-weight: bold; color: #8D6E63; margin-bottom: 10px;");
    mainLayout->addWidget(titre);
    
    QFormLayout *layout = new QFormLayout();
    layout->setSpacing(15);
    layout->setLabelAlignment(Qt::AlignRight);
    
    QLineEdit *refEdit = new QLineEdit(&dialog);
    refEdit->setPlaceholderText("Ex: CMD-2026-009");
    
    QLineEdit *clientEdit = new QLineEdit(&dialog);
    clientEdit->setPlaceholderText("Nom du client");
    
    QComboBox *typeCombo = new QComboBox(&dialog);
    typeCombo->addItems({"Standard", "Express", "Vente", "Devis"});
    
    QLineEdit *montantEdit = new QLineEdit(&dialog);
    montantEdit->setPlaceholderText("Ex: 15000.00");
    
    QDateEdit *dateCreationEdit = new QDateEdit(QDate::currentDate(), &dialog);
    dateCreationEdit->setCalendarPopup(true);
    dateCreationEdit->setDisplayFormat("dd/MM/yyyy");
    
    QDateEdit *dateLivraisonEdit = new QDateEdit(QDate::currentDate().addDays(30), &dialog);
    dateLivraisonEdit->setCalendarPopup(true);
    dateLivraisonEdit->setDisplayFormat("dd/MM/yyyy");
    
    QComboBox *statutCombo = new QComboBox(&dialog);
    statutCombo->addItems({"En Attente", "Planifié", "En Production", "Terminé"});
    
    QComboBox *prioriteCombo = new QComboBox(&dialog);
    prioriteCombo->addItems({"Basse", "Normale", "Haute", "Urgente"});
    prioriteCombo->setCurrentIndex(1);
    
    layout->addRow("Référence *:", refEdit);
    layout->addRow("Client *:", clientEdit);
    layout->addRow("Type:", typeCombo);
    layout->addRow("Montant HT *:", montantEdit);
    layout->addRow("Date Création:", dateCreationEdit);
    layout->addRow("Date Livraison:", dateLivraisonEdit);
    layout->addRow("Statut:", statutCombo);
    layout->addRow("Priorité:", prioriteCombo);
    
    mainLayout->addLayout(layout);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    QPushButton *btnOk = new QPushButton("✓ Créer");
    QPushButton *btnCancel = new QPushButton("✗ Annuler");
    btnCancel->setStyleSheet("QPushButton { background-color: #95877C; } QPushButton:hover { background-color: #A5978C; }");
    buttonLayout->addWidget(btnOk);
    buttonLayout->addWidget(btnCancel);
    mainLayout->addLayout(buttonLayout);
    
    connect(btnOk, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(btnCancel, &QPushButton::clicked, &dialog, &QDialog::reject);
    
    if (dialog.exec() == QDialog::Accepted) {
        if (refEdit->text().isEmpty() || clientEdit->text().isEmpty() || montantEdit->text().isEmpty()) {
            QMessageBox::warning(this, "Champs manquants", "Veuillez remplir tous les champs obligatoires (*)");
            return;
        }
        
        QString montantFormatted = montantEdit->text() + " DT";
        ajouterCommandeProduction(refEdit->text(), clientEdit->text(), typeCombo->currentText(),
                       montantFormatted, dateCreationEdit->date().toString("dd/MM/yyyy"),
                       dateLivraisonEdit->date().toString("dd/MM/yyyy"),
                       statutCombo->currentText(), prioriteCombo->currentText());
        
        QMessageBox::information(this, "Succès", "Commande créée avec succès !");
    }
}

void MainWindow::onModifierProduction()
{
    int currentRow = ui->productionTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Aucune sélection",
                           "Veuillez sélectionner une commande à modifier.");
        return;
    }
    
    QDialog dialog(this);
    dialog.setWindowTitle("Modifier la commande");
    dialog.setMinimumWidth(550);
    dialog.setStyleSheet(
        "QDialog { background-color: #FFFFFF; }"
        "QLabel { color: #2C2416; font-size: 13px; font-weight: 500; }"
        "QLineEdit { "
        "   background-color: #F8F8F8; "
        "   border: 2px solid #E0E0E0; "
        "   border-radius: 6px; "
        "   padding: 10px; "
        "   font-size: 13px; "
        "   color: #2C2416; "
        "}"
        "QLineEdit:focus { border: 2px solid #8D6E63; background-color: #FFFFFF; }"
        "QComboBox { "
        "   background-color: #F8F8F8; "
        "   border: 2px solid #E0E0E0; "
        "   border-radius: 6px; "
        "   padding: 10px; "
        "   font-size: 13px; "
        "   color: #2C2416; "
        "}"
        "QComboBox:focus { border: 2px solid #8D6E63; }"
        "QDateEdit { "
        "   background-color: #F8F8F8; "
        "   border: 2px solid #E0E0E0; "
        "   border-radius: 6px; "
        "   padding: 10px; "
        "   font-size: 13px; "
        "   color: #2C2416; "
        "}"
        "QDateEdit:focus { border: 2px solid #8D6E63; }"
        "QPushButton { "
        "   background-color: #8D6E63; "
        "   color: white; "
        "   border: none; "
        "   border-radius: 6px; "
        "   padding: 10px 20px; "
        "   font-size: 13px; "
        "   font-weight: 500; "
        "}"
        "QPushButton:hover { background-color: #A0826D; }"
    );
    
    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    
    QLabel *titre = new QLabel("✏️ Modifier la commande");
    titre->setStyleSheet("font-size: 20px; font-weight: bold; color: #8D6E63; margin-bottom: 10px;");
    mainLayout->addWidget(titre);
    
    QFormLayout *layout = new QFormLayout();
    layout->setSpacing(15);
    layout->setLabelAlignment(Qt::AlignRight);
    
    QLineEdit *refEdit = new QLineEdit(ui->productionTable->item(currentRow, 1)->text(), &dialog);
    QLineEdit *clientEdit = new QLineEdit(ui->productionTable->item(currentRow, 2)->text(), &dialog);
    
    QComboBox *typeCombo = new QComboBox(&dialog);
    typeCombo->addItems({"Standard", "Express", "Vente", "Devis"});
    typeCombo->setCurrentText(ui->productionTable->item(currentRow, 3)->text());
    
    QLineEdit *montantEdit = new QLineEdit(ui->productionTable->item(currentRow, 4)->text(), &dialog);
    
    QDateEdit *dateCreationEdit = new QDateEdit(QDate::fromString(ui->productionTable->item(currentRow, 5)->text(), "dd/MM/yyyy"), &dialog);
    dateCreationEdit->setCalendarPopup(true);
    dateCreationEdit->setDisplayFormat("dd/MM/yyyy");
    
    QDateEdit *dateLivraisonEdit = new QDateEdit(QDate::fromString(ui->productionTable->item(currentRow, 6)->text(), "dd/MM/yyyy"), &dialog);
    dateLivraisonEdit->setCalendarPopup(true);
    dateLivraisonEdit->setDisplayFormat("dd/MM/yyyy");
    
    QComboBox *statutCombo = new QComboBox(&dialog);
    statutCombo->addItems({"En Attente", "Planifié", "En Production", "Terminé"});
    statutCombo->setCurrentText(ui->productionTable->item(currentRow, 7)->text());
    
    QComboBox *prioriteCombo = new QComboBox(&dialog);
    prioriteCombo->addItems({"Basse", "Normale", "Haute", "Urgente"});
    prioriteCombo->setCurrentText(ui->productionTable->item(currentRow, 8)->text());
    
    layout->addRow("Référence *:", refEdit);
    layout->addRow("Client *:", clientEdit);
    layout->addRow("Type:", typeCombo);
    layout->addRow("Montant HT *:", montantEdit);
    layout->addRow("Date Création:", dateCreationEdit);
    layout->addRow("Date Livraison:", dateLivraisonEdit);
    layout->addRow("Statut:", statutCombo);
    layout->addRow("Priorité:", prioriteCombo);
    
    mainLayout->addLayout(layout);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    QPushButton *btnOk = new QPushButton("✓ Enregistrer");
    QPushButton *btnCancel = new QPushButton("✗ Annuler");
    btnCancel->setStyleSheet("QPushButton { background-color: #95877C; } QPushButton:hover { background-color: #A5978C; }");
    buttonLayout->addWidget(btnOk);
    buttonLayout->addWidget(btnCancel);
    mainLayout->addLayout(buttonLayout);
    
    connect(btnOk, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(btnCancel, &QPushButton::clicked, &dialog, &QDialog::reject);
    
    if (dialog.exec() == QDialog::Accepted) {
        if (refEdit->text().isEmpty() || clientEdit->text().isEmpty() || montantEdit->text().isEmpty()) {
            QMessageBox::warning(this, "Champs manquants", "Veuillez remplir tous les champs obligatoires (*)");
            return;
        }
        
        ui->productionTable->item(currentRow, 1)->setText(refEdit->text());
        ui->productionTable->item(currentRow, 2)->setText(clientEdit->text());
        ui->productionTable->item(currentRow, 3)->setText(typeCombo->currentText());
        ui->productionTable->item(currentRow, 4)->setText(montantEdit->text());
        ui->productionTable->item(currentRow, 5)->setText(dateCreationEdit->date().toString("dd/MM/yyyy"));
        ui->productionTable->item(currentRow, 6)->setText(dateLivraisonEdit->date().toString("dd/MM/yyyy"));
        ui->productionTable->item(currentRow, 7)->setText(statutCombo->currentText());
        ui->productionTable->item(currentRow, 8)->setText(prioriteCombo->currentText());
        
        QMessageBox::information(this, "Succès", "Commande modifiée avec succès !");
    }
}

void MainWindow::onSuiviProduction()
{
    int currentRow = ui->productionTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Aucune sélection",
                           "Veuillez sélectionner une commande pour voir le suivi.");
        return;
    }
    
    QString ref = ui->productionTable->item(currentRow, 1)->text();
    QString client = ui->productionTable->item(currentRow, 2)->text();
    QString type = ui->productionTable->item(currentRow, 3)->text();
    QString montantHT = ui->productionTable->item(currentRow, 4)->text();
    QString dateCreation = ui->productionTable->item(currentRow, 5)->text();
    QString dateLivraison = ui->productionTable->item(currentRow, 6)->text();
    QString statut = ui->productionTable->item(currentRow, 7)->text();
    QString priorite = ui->productionTable->item(currentRow, 8)->text();
    
    // Créer le dialogue de suivi
    QDialog suiviDialog(this);
    suiviDialog.setWindowTitle("Suivi de commande - " + ref);
    suiviDialog.setMinimumSize(750, 850);
    suiviDialog.setStyleSheet(
        "QDialog { background-color: #FFFFFF; }"
        "QLabel { color: #2C2416; font-size: 14px; }"
        "QTextEdit { background-color: #FAFAFA; border: 2px solid #E0E0E0; border-radius: 10px; padding: 15px; }"
        "QPushButton { background-color: #8D6E63; color: white; border: none; border-radius: 8px; "
        "padding: 12px 30px; font-size: 14px; font-weight: 500; }"
        "QPushButton:hover { background-color: #A0826D; }"
    );
    
    QVBoxLayout *mainLayout = new QVBoxLayout(&suiviDialog);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    
    QLabel *mainTitle = new QLabel("📦 SUIVI DE COMMANDE");
    mainTitle->setStyleSheet("font-size: 24px; font-weight: bold; color: #8D6E63; padding: 10px;");
    mainTitle->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(mainTitle);
    
    // En-tête avec QR Code
    QHBoxLayout *headerLayout = new QHBoxLayout();
    
    // Informations de la commande
    QFrame *infoFrame = new QFrame();
    infoFrame->setStyleSheet("background-color: #F8F8F8; border-radius: 10px; padding: 20px; border: 2px solid #E0E0E0;");
    QVBoxLayout *infoFrameLayout = new QVBoxLayout(infoFrame);
    
    QLabel *lblRef = new QLabel("<b style='color: #8D6E63;'>Référence:</b> " + ref);
    QLabel *lblClient = new QLabel("<b style='color: #8D6E63;'>Client:</b> " + client);
    QLabel *lblType = new QLabel("<b style='color: #8D6E63;'>Type:</b> " + type);
    QLabel *lblMontant = new QLabel("<b style='color: #8D6E63;'>Montant HT:</b> " + montantHT);
    QLabel *lblPriorite = new QLabel("<b style='color: #8D6E63;'>Priorité:</b> <span style='font-weight: bold;'>" + priorite + "</span>");
    
    lblRef->setStyleSheet("font-size: 14px; padding: 5px; color: #2C2416;");
    lblClient->setStyleSheet("font-size: 14px; padding: 5px; color: #2C2416;");
    lblType->setStyleSheet("font-size: 14px; padding: 5px; color: #2C2416;");
    lblMontant->setStyleSheet("font-size: 14px; padding: 5px; color: #2C2416;");
    
    if (priorite == "Urgente") {
        lblPriorite->setStyleSheet("font-size: 14px; padding: 5px; color: #F44336; font-weight: bold;");
    } else if (priorite == "Haute") {
        lblPriorite->setStyleSheet("font-size: 14px; padding: 5px; color: #FF9800; font-weight: bold;");
    } else if (priorite == "Normale") {
        lblPriorite->setStyleSheet("font-size: 14px; padding: 5px; color: #FBC02D; font-weight: bold;");
    } else {
        lblPriorite->setStyleSheet("font-size: 14px; padding: 5px; color: #4CAF50; font-weight: bold;");
    }
    
    infoFrameLayout->addWidget(lblRef);
    infoFrameLayout->addWidget(lblClient);
    infoFrameLayout->addWidget(lblType);
    infoFrameLayout->addWidget(lblMontant);
    infoFrameLayout->addWidget(lblPriorite);
    
    // QR Code avec cadre
    QFrame *qrFrame = new QFrame();
    qrFrame->setStyleSheet("background-color: #F8F8F8; border-radius: 10px; padding: 15px; border: 2px solid #E0E0E0;");
    QVBoxLayout *qrFrameLayout = new QVBoxLayout(qrFrame);
    
    QLabel *qrLabel = new QLabel();
    QPixmap qrCode = generateQRCode(ref, 180);
    qrLabel->setPixmap(qrCode);
    qrLabel->setAlignment(Qt::AlignCenter);
    
    QLabel *qrText = new QLabel("Scannez pour le suivi");
    qrText->setStyleSheet("color: #8D6E63; font-size: 12px; font-weight: 500;");
    qrText->setAlignment(Qt::AlignCenter);
    
    qrFrameLayout->addWidget(qrLabel);
    qrFrameLayout->addWidget(qrText);
    
    headerLayout->addWidget(infoFrame, 2);
    headerLayout->addWidget(qrFrame, 1);
    mainLayout->addLayout(headerLayout);
    
    // Timeline de suivi
    QLabel *lblTimeline = new QLabel("📍 Historique de Suivi");
    lblTimeline->setStyleSheet("font-size: 18px; font-weight: bold; color: #8D6E63; margin-top: 15px; padding: 10px 0;");
    mainLayout->addWidget(lblTimeline);
    
    // Zone de texte pour le suivi
    QTextEdit *timelineView = new QTextEdit(&suiviDialog);
    timelineView->setReadOnly(true);
    
    QString timelineHTML = QString(
        "<html><head><style>"
        "body { font-family: 'Segoe UI', Arial, sans-serif; padding: 15px; background-color: #FAFAFA; }"
        ".timeline-item { margin: 18px 0; padding: 18px; background-color: #FFFFFF; border-left: 5px solid #8D6E63; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.08); }"
        ".timeline-date { font-weight: 600; color: #8D6E63; font-size: 13px; margin-bottom: 8px; }"
        ".timeline-status { font-size: 16px; font-weight: bold; margin: 8px 0; }"
        ".timeline-desc { color: #666; font-size: 13px; line-height: 1.5; }"
        ".status-attente { color: #FF9800; }"
        ".status-planifie { color: #2196F3; }"
        ".status-production { color: #9C27B0; }"
        ".status-termine { color: #4CAF50; }"
        "</style></head><body>"
    );
    
    // Générer l'historique basé sur le statut actuel
    timelineHTML += "<div class='timeline-item'>";
    timelineHTML += "<div class='timeline-date'>📅 " + dateCreation + "</div>";
    timelineHTML += "<div class='timeline-status status-attente'>✓ Commande créée</div>";
    timelineHTML += "<div class='timeline-desc'>La commande a été enregistrée dans le système</div>";
    timelineHTML += "</div>";
    
    if (statut == "Planifié" || statut == "En Production" || statut == "Terminé") {
        QDate dateCreationParsed = QDate::fromString(dateCreation, "dd/MM/yyyy");
        QString dateTraitement = dateCreationParsed.addDays(1).toString("dd/MM/yyyy");
        
        timelineHTML += "<div class='timeline-item'>";
        timelineHTML += "<div class='timeline-date'>📅 " + dateTraitement + "</div>";
        timelineHTML += "<div class='timeline-status status-planifie'>✓ Planifiée</div>";
        timelineHTML += "<div class='timeline-desc'>La commande a été planifiée pour la production</div>";
        timelineHTML += "</div>";
    }
    
    if (statut == "En Production" || statut == "Terminé") {
        QDate dateCreationParsed = QDate::fromString(dateCreation, "dd/MM/yyyy");
        QString dateProduction = dateCreationParsed.addDays(3).toString("dd/MM/yyyy");
        
        timelineHTML += "<div class='timeline-item'>";
        timelineHTML += "<div class='timeline-date'>📅 " + dateProduction + "</div>";
        timelineHTML += "<div class='timeline-status status-production'>✓ En production</div>";
        timelineHTML += "<div class='timeline-desc'>La commande est en cours de fabrication</div>";
        timelineHTML += "</div>";
    }
    
    if (statut == "Terminé") {
        timelineHTML += "<div class='timeline-item'>";
        timelineHTML += "<div class='timeline-date'>📅 " + dateLivraison + "</div>";
        timelineHTML += "<div class='timeline-status status-termine'>✓ Terminée</div>";
        timelineHTML += "<div class='timeline-desc'>La commande a été terminée et livrée avec succès</div>";
        timelineHTML += "</div>";
    } else {
        timelineHTML += "<div class='timeline-item' style='opacity: 0.5;'>";
        timelineHTML += "<div class='timeline-date'>📅 " + dateLivraison + " (Prévu)</div>";
        timelineHTML += "<div class='timeline-status'>⏳ Livraison prévue</div>";
        timelineHTML += "<div class='timeline-desc'>Date de livraison estimée</div>";
        timelineHTML += "</div>";
    }
    
    timelineHTML += "</body></html>";
    
    timelineView->setHtml(timelineHTML);
    mainLayout->addWidget(timelineView);
    
    // Bouton fermer
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 15, 0, 0);
    QPushButton *btnFermer = new QPushButton("Fermer", &suiviDialog);
    buttonLayout->addStretch();
    buttonLayout->addWidget(btnFermer);
    mainLayout->addLayout(buttonLayout);
    
    connect(btnFermer, &QPushButton::clicked, &suiviDialog, &QDialog::accept);
    
    suiviDialog.exec();
}

void MainWindow::onPlanificationProduction()
{
    QString planification;
    planification += "📅 PLANIFICATION DE LA PRODUCTION\n\n";
    planification += "Vue d'ensemble des commandes:\n\n";
    
    // Count by status
    int enAttente = 0, planifie = 0, enProduction = 0, termine = 0;
    
    for (int row = 0; row < ui->productionTable->rowCount(); ++row) {
        QString statut = ui->productionTable->item(row, 7)->text();
        if (statut == "En Attente") enAttente++;
        else if (statut == "Planifié") planifie++;
        else if (statut == "En Production") enProduction++;
        else if (statut == "Terminé") termine++;
    }
    
    planification += QString("⏳ En Attente: %1 commandes\n").arg(enAttente);
    planification += QString("📋 Planifié: %1 commandes\n").arg(planifie);
    planification += QString("🔄 En Production: %1 commandes\n").arg(enProduction);
    planification += QString("✅ Terminé: %1 commandes\n\n").arg(termine);
    
    planification += "COMMANDES PRIORITAIRES:\n";
    planification += "───────────────────────────────────────\n";
    
    for (int row = 0; row < ui->productionTable->rowCount(); ++row) {
        QString priorite = ui->productionTable->item(row, 8)->text();
        if (priorite == "Haute") {
            QString ref = ui->productionTable->item(row, 1)->text();
            QString client = ui->productionTable->item(row, 2)->text();
            QString dateLivraison = ui->productionTable->item(row, 6)->text();
            planification += QString("🔴 %1 - %2\n   Livraison: %3\n\n")
                .arg(ref, client, dateLivraison);
        }
    }
    
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Planification Production");
    msgBox.setText(planification);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStyleSheet(
        "QMessageBox { background-color: #FAF5F0; min-width: 500px; }"
        "QMessageBox QLabel { color: #291C0E; font-family: 'Courier New', monospace; font-size: 11px; }"
        "QPushButton { background-color: #8D6E63; color: white; border: none; border-radius: 6px; "
        "padding: 8px 20px; font-family: Arial, sans-serif; font-size: 11px; font-weight: bold; min-width: 80px; }"
        "QPushButton:hover { background-color: #A0826D; }"
    );
    msgBox.exec();
}

void MainWindow::onFactureProduction()
{
    int currentRow = ui->productionTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Aucune sélection",
                           "Veuillez sélectionner une commande pour générer la facture.");
        return;
    }
    
    QString ref = ui->productionTable->item(currentRow, 1)->text();
    QString client = ui->productionTable->item(currentRow, 2)->text();
    QString type = ui->productionTable->item(currentRow, 3)->text();
    QString montantHT = ui->productionTable->item(currentRow, 4)->text();
    QString dateCreation = ui->productionTable->item(currentRow, 5)->text();
    QString dateLivraison = ui->productionTable->item(currentRow, 6)->text();
    QString statut = ui->productionTable->item(currentRow, 7)->text();
    
    // Créer le dialogue de facture
    QDialog factureDialog(this);
    factureDialog.setWindowTitle("Facture - " + ref);
    factureDialog.setMinimumSize(850, 900);
    factureDialog.setStyleSheet(
        "QDialog { background-color: #FFFFFF; }"
        "QTextEdit { background-color: #FAFAFA; border: 2px solid #E0E0E0; border-radius: 10px; padding: 15px; }"
        "QPushButton { background-color: #8D6E63; color: white; border: none; border-radius: 8px; "
        "padding: 12px 24px; font-size: 14px; font-weight: 500; }"
        "QPushButton:hover { background-color: #A0826D; }"
    );
    
    QVBoxLayout *mainLayout = new QVBoxLayout(&factureDialog);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(25, 25, 25, 25);
    
    QLabel *dialogTitle = new QLabel("📄 FACTURE");
    dialogTitle->setStyleSheet("font-size: 24px; font-weight: bold; color: #8D6E63; padding: 10px;");
    dialogTitle->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(dialogTitle);
    
    QTextEdit *factureView = new QTextEdit(&factureDialog);
    factureView->setReadOnly(true);
    
    QString factureHTML = QString(
        "<html><head><style>"
        "body { font-family: 'Segoe UI', Arial, sans-serif; margin: 30px; background-color: #FFFFFF; color: #2C2416; }"
        ".header { text-align: center; border-bottom: 4px solid #8D6E63; padding-bottom: 25px; margin-bottom: 35px; }"
        ".company { font-size: 28px; font-weight: bold; color: #8D6E63; letter-spacing: 1px; }"
        ".company-info { font-size: 13px; color: #666; margin-top: 8px; }"
        ".facture-title { font-size: 36px; font-weight: bold; color: #2C2416; margin: 25px 0; text-align: center; }"
        ".info-section { margin: 25px 0; padding: 20px; background-color: #F8F8F8; border-radius: 8px; }"
        ".info-label { font-weight: 600; color: #8D6E63; font-size: 14px; }"
        ".info-value { color: #2C2416; font-size: 14px; margin-left: 10px; }"
        ".client-box { background-color: #F5F5F0; padding: 25px; border-radius: 10px; margin: 25px 0; border-left: 5px solid #8D6E63; }"
        ".client-title { font-weight: bold; color: #8D6E63; font-size: 15px; margin-bottom: 12px; }"
        ".client-name { font-size: 18px; font-weight: bold; color: #2C2416; margin: 8px 0; }"
        "table { width: 100%%; border-collapse: collapse; margin: 30px 0; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }"
        "th { background-color: #8D6E63; color: white; padding: 16px; text-align: left; font-size: 13px; font-weight: 600; }"
        "td { padding: 14px; border-bottom: 1px solid #E8E8E8; font-size: 13px; color: #2C2416; }"
        "tr:hover { background-color: #F9F9F9; }"
        ".total-section { text-align: right; margin-top: 35px; padding: 20px; background-color: #F8F8F8; border-radius: 8px; }"
        ".total-line { margin: 12px 0; font-size: 16px; color: #2C2416; }"
        ".total-final { font-size: 26px; font-weight: bold; color: #8D6E63; border-top: 3px solid #8D6E63; padding-top: 18px; margin-top: 15px; }"
        ".footer { text-align: center; margin-top: 50px; padding-top: 25px; border-top: 2px solid #E0E0E0; color: #888; font-size: 11px; line-height: 1.6; }"
        "</style></head><body>"
        "<div class='header'>"
        "<div class='company'>CUIREA</div>"
        "<div class='company-info'>123 Avenue Mohammed V, Casablanca</div>"
        "<div class='company-info'>Tél: +216 71 234 567 | Email: contact@cuirea.tn</div>"
        "</div>"
        "<div class='facture-title'>FACTURE</div>"
        "<div class='info-section'>"
        "<span class='info-label'>Numéro de facture:</span><span class='info-value'>%1</span><br>"
        "<span class='info-label'>Date d'émission:</span><span class='info-value'>%2</span><br>"
        "<span class='info-label'>Date de livraison:</span><span class='info-value'>%3</span><br>"
        "<span class='info-label'>Statut:</span><span class='info-value'>%4</span>"
        "</div>"
        "<div class='client-box'>"
        "<div class='client-title'>FACTURÉ À:</div>"
        "<div class='client-name'>%5</div>"
        "</div>"
        "<table>"
        "<tr><th>Description</th><th>Type</th><th>Quantité</th><th>Prix Unitaire HT</th><th>Total HT</th></tr>"
        "<tr><td>Commande %1</td><td>%6</td><td>1</td><td>%7</td><td>%7</td></tr>"
        "</table>"
        "<div class='total-section'>"
        "<div class='total-line'><strong>Sous-total HT:</strong> %7</div>"
        "<div class='total-line'><strong>TVA (19%%):</strong> %8</div>"
        "<div class='total-final'><strong>TOTAL TTC:</strong> %9</div>"
        "</div>"
        "<div class='footer'>"
        "<p><strong>Merci pour votre confiance !</strong></p>"
        "<p>Conditions de paiement: 30 jours</p>"
        "</div>"
        "</body></html>"
    ).arg(ref).arg(dateCreation).arg(dateLivraison).arg(statut).arg(client).arg(type).arg(montantHT);
    
    // Calculer TVA et TTC
    QString montantStr = montantHT;
    montantStr.remove(" DT").remove(" €").remove(",").remove(" ");
    bool ok;
    double montant = montantStr.toDouble(&ok);
    if (ok) {
        double tva = montant * 0.19;
        double ttc = montant + tva;
        QString tvaStr = QString::number(tva, 'f', 2) + " DT";
        QString ttcStr = QString::number(ttc, 'f', 2) + " DT";
        factureHTML = factureHTML.arg(tvaStr).arg(ttcStr);
    } else {
        factureHTML = factureHTML.arg("N/A").arg("N/A");
    }
    
    factureView->setHtml(factureHTML);
    mainLayout->addWidget(factureView);
    
    // QR Code
    QLabel *qrLabel = new QLabel();
    QPixmap qrCode = generateQRCode(ref, 150);
    qrLabel->setPixmap(qrCode);
    qrLabel->setAlignment(Qt::AlignCenter);
    QLabel *qrText = new QLabel("<center><b>Scannez pour le suivi</b></center>");
    qrText->setStyleSheet("color: #8D6E63; font-size: 12px;");
    mainLayout->addWidget(qrLabel);
    mainLayout->addWidget(qrText);
    
    // Boutons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *btnTelecharger = new QPushButton("📥 Télécharger PDF", &factureDialog);
    QPushButton *btnFermer = new QPushButton("Fermer", &factureDialog);
    btnFermer->setStyleSheet("QPushButton { background-color: #95877C; } QPushButton:hover { background-color: #A5978C; }");
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(btnTelecharger);
    buttonLayout->addWidget(btnFermer);
    mainLayout->addLayout(buttonLayout);
    
    connect(btnTelecharger, &QPushButton::clicked, [&, factureHTML, ref]() {
        QString fileName = QFileDialog::getSaveFileName(&factureDialog, "Enregistrer la facture", 
                                                        "Facture_" + ref + ".pdf", 
                                                        "PDF Files (*.pdf)");
        if (!fileName.isEmpty()) {
            QPrinter printer(QPrinter::HighResolution);
            printer.setOutputFormat(QPrinter::PdfFormat);
            printer.setOutputFileName(fileName);
            printer.setPageSize(QPageSize::A4);
            
            QTextDocument document;
            document.setHtml(factureHTML);
            document.print(&printer);
            
            QMessageBox::information(&factureDialog, "Succès", "Facture enregistrée avec succès !");
        }
    });
    
    connect(btnFermer, &QPushButton::clicked, &factureDialog, &QDialog::accept);
    
    factureDialog.exec();
}

void MainWindow::onExcelProduction()
{
    // Créer le dialogue de bilan financier
    QDialog bilanDialog(this);
    bilanDialog.setWindowTitle("📊 Bilan Financier");
    bilanDialog.setMinimumSize(950, 750);
    bilanDialog.setStyleSheet(
        "QDialog { background-color: #FFFFFF; }"
        "QLabel { color: #2C2416; font-weight: bold; }"
        "QTextEdit { background-color: #FFFFFF; border: 2px solid #E0E0E0; border-radius: 10px; padding: 15px; }"
        "QPushButton { background-color: #8D6E63; color: white; border: none; border-radius: 8px; "
        "padding: 12px 24px; font-size: 14px; font-weight: 500; }"
        "QPushButton:hover { background-color: #A0826D; }"
    );
    
    QVBoxLayout *mainLayout = new QVBoxLayout(&bilanDialog);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    
    // En-tête
    QLabel *lblTitre = new QLabel("📊 BILAN FINANCIER");
    lblTitre->setStyleSheet("font-size: 28px; font-weight: bold; color: #2C2416; padding: 10px;");
    lblTitre->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(lblTitre);
    
    QLabel *lblPeriode = new QLabel("Période: " + QDate::currentDate().toString("MMMM yyyy"));
    lblPeriode->setStyleSheet("font-size: 15px; color: #2C2416; padding: 5px;");
    lblPeriode->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(lblPeriode);
    
    // Calculer les statistiques financières
    double totalHT = 0;
    double totalEnCours = 0;
    double totalTermine = 0;
    int nbCommandes = ui->productionTable->rowCount();
    int nbEnAttente = 0;
    int nbPlanifie = 0;
    int nbEnProduction = 0;
    int nbTermine = 0;
    
    QMap<QString, double> caParClient;
    QMap<QString, int> commandesParType;
    
    for (int row = 0; row < ui->productionTable->rowCount(); ++row) {
        QString montantStr = ui->productionTable->item(row, 4)->text();
        QString statut = ui->productionTable->item(row, 7)->text();
        QString client = ui->productionTable->item(row, 2)->text();
        QString type = ui->productionTable->item(row, 3)->text();
        
        // Extraire le montant
        montantStr.remove(" DT").remove(" €").remove(",").replace(".", "");
        bool ok;
        double montant = montantStr.toDouble(&ok);
        if (ok) {
            totalHT += montant;
            caParClient[client] += montant;
            
            if (statut == "En Production") {
                totalEnCours += montant;
                nbEnProduction++;
            } else if (statut == "Terminé") {
                totalTermine += montant;
                nbTermine++;
            } else if (statut == "En Attente") {
                nbEnAttente++;
            } else if (statut == "Planifié") {
                nbPlanifie++;
            }
        }
        
        commandesParType[type]++;
    }
    
    double totalTVA = totalHT * 0.19;
    double totalTTC = totalHT + totalTVA;
    double moyenneCommande = nbCommandes > 0 ? totalHT / nbCommandes : 0;
    
    // Zone de texte pour afficher le bilan
    QTextEdit *bilanView = new QTextEdit(&bilanDialog);
    bilanView->setReadOnly(true);
    
    QString bilanHTML = QString(
        "<html><head><style>"
        "body { font-family: Arial, sans-serif; padding: 20px; background-color: #FFFFFF; color: #000000; }"
        ".section { background-color: #F8F8F8; margin: 15px 0; padding: 20px; border-radius: 10px; border-left: 4px solid #8D6E63; }"
        ".section-title { font-size: 18px; font-weight: bold; color: #000000; margin-bottom: 15px; border-bottom: 2px solid #8D6E63; padding-bottom: 10px; }"
        ".stat-row { display: flex; justify-content: space-between; padding: 10px 0; border-bottom: 1px solid #D0D0D0; }"
        ".stat-label { font-weight: 600; color: #000000; font-size: 14px; }"
        ".stat-value { font-weight: bold; color: #8D6E63; font-size: 16px; }"
        ".stat-value-big { font-weight: bold; color: #8D6E63; font-size: 24px; }"
        ".highlight { background-color: #FFF8F0; padding: 15px; border-radius: 8px; margin: 10px 0; border: 2px solid #8D6E63; }"
        ".success { color: #2E7D32; }"
        ".warning { color: #F57C00; }"
        ".info { color: #1976D2; }"
        "table { width: 100%%; border-collapse: collapse; margin: 10px 0; background-color: white; }"
        "th { background-color: #8D6E63; color: white; padding: 12px; text-align: left; font-weight: bold; font-size: 14px; }"
        "td { padding: 10px; border-bottom: 1px solid #E0E0E0; color: #000000; font-size: 14px; font-weight: normal; }"
        "tr:hover { background-color: #FFF8F0; }"
        "</style></head><body>"
    );
    
    // Section 1: Résumé Global
    bilanHTML += "<div class='section'>";
    bilanHTML += "<div class='section-title'>💰 RÉSUMÉ GLOBAL</div>";
    bilanHTML += "<div class='highlight'>";
    bilanHTML += QString("<div class='stat-row'><span class='stat-label'>Chiffre d'Affaires Total HT:</span><span class='stat-value-big'>%1 DT</span></div>")
                 .arg(QString::number(totalHT, 'f', 2));
    bilanHTML += QString("<div class='stat-row'><span class='stat-label'>TVA (19%):</span><span class='stat-value'>%1 DT</span></div>")
                 .arg(QString::number(totalTVA, 'f', 2));
    bilanHTML += QString("<div class='stat-row'><span class='stat-label'>Total TTC:</span><span class='stat-value-big success'>%1 DT</span></div>")
                 .arg(QString::number(totalTTC, 'f', 2));
    bilanHTML += "</div>";
    bilanHTML += QString("<div class='stat-row'><span class='stat-label'>Nombre de commandes:</span><span class='stat-value'>%1</span></div>").arg(nbCommandes);
    bilanHTML += QString("<div class='stat-row'><span class='stat-label'>Montant moyen par commande:</span><span class='stat-value'>%1 DT</span></div>")
                 .arg(QString::number(moyenneCommande, 'f', 2));
    bilanHTML += "</div>";
    
    // Section 2: Répartition par Statut
    bilanHTML += "<div class='section'>";
    bilanHTML += "<div class='section-title'>📈 RÉPARTITION PAR STATUT</div>";
    bilanHTML += QString("<div class='stat-row'><span class='stat-label'>✓ Terminées:</span><span class='stat-value success'>%1 commandes (%2 DT)</span></div>")
                 .arg(nbTermine).arg(QString::number(totalTermine, 'f', 2));
    bilanHTML += QString("<div class='stat-row'><span class='stat-label'>⏳ En production:</span><span class='stat-value info'>%1 commandes (%2 DT)</span></div>")
                 .arg(nbEnProduction).arg(QString::number(totalEnCours, 'f', 2));
    bilanHTML += QString("<div class='stat-row'><span class='stat-label'>📋 Planifiées:</span><span class='stat-value warning'>%1 commandes</span></div>").arg(nbPlanifie);
    bilanHTML += QString("<div class='stat-row'><span class='stat-label'>📝 En attente:</span><span class='stat-value'>%1 commandes</span></div>").arg(nbEnAttente);
    
    double tauxCompletion = nbCommandes > 0 ? (double)nbTermine / nbCommandes * 100 : 0;
    bilanHTML += QString("<div class='highlight'><b>Taux de complétion:</b> <span class='stat-value success'>%1%</span></div>")
                 .arg(QString::number(tauxCompletion, 'f', 1));
    bilanHTML += "</div>";
    
    // Section 3: Top Clients
    bilanHTML += "<div class='section'>";
    bilanHTML += "<div class='section-title'>👥 TOP CLIENTS</div>";
    bilanHTML += "<table>";
    bilanHTML += "<tr><th>Client</th><th>CA HT</th><th>% du Total</th></tr>";
    
    QList<QPair<QString, double>> clientsList;
    for (auto it = caParClient.begin(); it != caParClient.end(); ++it) {
        clientsList.append(qMakePair(it.key(), it.value()));
    }
    std::sort(clientsList.begin(), clientsList.end(), 
              [](const QPair<QString, double> &a, const QPair<QString, double> &b) {
                  return a.second > b.second;
              });
    
    for (const auto &client : clientsList) {
        double pourcentage = totalHT > 0 ? (client.second / totalHT * 100) : 0;
        bilanHTML += QString("<tr><td>%1</td><td>%2 DT</td><td>%3%</td></tr>")
                     .arg(client.first)
                     .arg(QString::number(client.second, 'f', 2))
                     .arg(QString::number(pourcentage, 'f', 1));
    }
    bilanHTML += "</table>";
    bilanHTML += "</div>";
    
    // Section 4: Répartition par Type
    bilanHTML += "<div class='section'>";
    bilanHTML += "<div class='section-title'>📊 RÉPARTITION PAR TYPE</div>";
    bilanHTML += "<table>";
    bilanHTML += "<tr><th>Type</th><th>Nombre</th><th>% du Total</th></tr>";
    
    for (auto it = commandesParType.begin(); it != commandesParType.end(); ++it) {
        double pourcentage = nbCommandes > 0 ? ((double)it.value() / nbCommandes * 100) : 0;
        bilanHTML += QString("<tr><td>%1</td><td>%2</td><td>%3%</td></tr>")
                     .arg(it.key())
                     .arg(it.value())
                     .arg(QString::number(pourcentage, 'f', 1));
    }
    bilanHTML += "</table>";
    bilanHTML += "</div>";
    
    bilanHTML += "</body></html>";
    
    bilanView->setHtml(bilanHTML);
    mainLayout->addWidget(bilanView);
    
    // Boutons d'action
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *btnExporter = new QPushButton("📥 Exporter en CSV", &bilanDialog);
    QPushButton *btnImprimer = new QPushButton("🖨️ Imprimer PDF", &bilanDialog);
    QPushButton *btnFermer = new QPushButton("Fermer", &bilanDialog);
    
    btnExporter->setStyleSheet("QPushButton { background-color: #4CAF50; } QPushButton:hover { background-color: #5CBF60; }");
    btnFermer->setStyleSheet("QPushButton { background-color: #95877C; } QPushButton:hover { background-color: #A5978C; }");
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(btnExporter);
    buttonLayout->addWidget(btnImprimer);
    buttonLayout->addWidget(btnFermer);
    mainLayout->addLayout(buttonLayout);
    
    // Connexion du bouton exporter
    connect(btnExporter, &QPushButton::clicked, [&, totalHT, totalTVA, totalTTC, moyenneCommande, nbCommandes, 
                                                   nbTermine, totalTermine, nbEnProduction, totalEnCours, 
                                                   nbPlanifie, nbEnAttente, clientsList]() {
        QString fileName = QFileDialog::getSaveFileName(&bilanDialog, "Exporter le bilan", 
                                                        "Bilan_Financier_" + QDate::currentDate().toString("yyyy-MM-dd") + ".csv", 
                                                        "CSV Files (*.csv)");
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file);
                out.setEncoding(QStringConverter::Utf8);
                
                out << "BILAN FINANCIER - " << QDate::currentDate().toString("dd/MM/yyyy") << "\n\n";
                out << "RÉSUMÉ GLOBAL\n";
                out << "CA Total HT;" << QString::number(totalHT, 'f', 2) << " DT\n";
                out << "TVA (19%);" << QString::number(totalTVA, 'f', 2) << " DT\n";
                out << "Total TTC;" << QString::number(totalTTC, 'f', 2) << " DT\n";
                out << "Nombre de commandes;" << nbCommandes << "\n";
                out << "Montant moyen;" << QString::number(moyenneCommande, 'f', 2) << " DT\n\n";
                
                out << "RÉPARTITION PAR STATUT\n";
                out << "Terminées;" << nbTermine << ";" << QString::number(totalTermine, 'f', 2) << " DT\n";
                out << "En production;" << nbEnProduction << ";" << QString::number(totalEnCours, 'f', 2) << " DT\n";
                out << "Planifiées;" << nbPlanifie << "\n";
                out << "En attente;" << nbEnAttente << "\n\n";
                
                out << "TOP CLIENTS\n";
                out << "Client;CA HT;% du Total\n";
                for (const auto &client : clientsList) {
                    double pourcentage = totalHT > 0 ? (client.second / totalHT * 100) : 0;
                    out << client.first << ";" << QString::number(client.second, 'f', 2) << " DT;" 
                        << QString::number(pourcentage, 'f', 1) << "%\n";
                }
                
                file.close();
                QMessageBox::information(&bilanDialog, "Succès", "Bilan exporté avec succès !");
            }
        }
    });
    
    // Connexion du bouton imprimer
    connect(btnImprimer, &QPushButton::clicked, [&, bilanHTML]() {
        QString fileName = QFileDialog::getSaveFileName(&bilanDialog, "Imprimer le bilan", 
                                                        "Bilan_Financier_" + QDate::currentDate().toString("yyyy-MM-dd") + ".pdf", 
                                                        "PDF Files (*.pdf)");
        if (!fileName.isEmpty()) {
            QPrinter printer(QPrinter::HighResolution);
            printer.setOutputFormat(QPrinter::PdfFormat);
            printer.setOutputFileName(fileName);
            printer.setPageSize(QPageSize::A4);
            
            QTextDocument document;
            document.setHtml(bilanHTML);
            document.print(&printer);
            
            QMessageBox::information(&bilanDialog, "Succès", "Bilan imprimé avec succès !");
        }
    });
    
    connect(btnFermer, &QPushButton::clicked, &bilanDialog, &QDialog::accept);
    
    bilanDialog.exec();
}

void MainWindow::onRechercherProduction(const QString &text)
{
    QString searchText = text.trimmed().toLower();
    
    // If search is empty, show all rows
    if (searchText.isEmpty()) {
        for (int row = 0; row < ui->productionTable->rowCount(); ++row) {
            ui->productionTable->setRowHidden(row, false);
        }
        return;
    }
    
    // Search in all columns
    for (int row = 0; row < ui->productionTable->rowCount(); ++row) {
        bool match = false;
        
        for (int col = 0; col < ui->productionTable->columnCount(); ++col) {
            QTableWidgetItem* item = ui->productionTable->item(row, col);
            if (item && item->text().toLower().contains(searchText)) {
                match = true;
                break;
            }
        }
        
        ui->productionTable->setRowHidden(row, !match);
    }
}

void MainWindow::onProductionTableContextMenu(const QPoint &pos)
{
    QMenu contextMenu(tr("Menu Contextuel"), this);
    
    QAction *actionVoir = contextMenu.addAction("👁️ Voir détails");
    QAction *actionModifier = contextMenu.addAction("✏️ Modifier");
    contextMenu.addSeparator();
    QAction *actionSupprimer = contextMenu.addAction("🗑️ Supprimer");
    contextMenu.addSeparator();
    QAction *actionFacture = contextMenu.addAction("📄 Générer facture");
    QAction *actionSuivi = contextMenu.addAction("📦 Voir suivi");
    
    // Style the context menu
    contextMenu.setStyleSheet(
        "QMenu { background-color: #FAF5F0; border: 1px solid #BCAAA4; }"
        "QMenu::item { padding: 8px 25px; color: #291C0E; }"
        "QMenu::item:selected { background-color: #8D6E63; color: white; }"
        "QMenu::separator { height: 1px; background: #BCAAA4; margin: 5px 0px; }"
    );
    
    QAction *selectedAction = contextMenu.exec(ui->productionTable->viewport()->mapToGlobal(pos));
    
    if (selectedAction == actionVoir) {
        onSuiviProduction();
    } else if (selectedAction == actionModifier) {
        onModifierProduction();
    } else if (selectedAction == actionSupprimer) {
        onSupprimerProduction();
    } else if (selectedAction == actionFacture) {
        onFactureProduction();
    } else if (selectedAction == actionSuivi) {
        onSuiviProduction();
    }
}

void MainWindow::onSupprimerProduction()
{
    int currentRow = ui->productionTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Aucune sélection",
                           "Veuillez sélectionner une commande à supprimer.");
        return;
    }
    
    QString ref = ui->productionTable->item(currentRow, 1)->text();
    QString client = ui->productionTable->item(currentRow, 2)->text();
    
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirmer la suppression",
                                 QString("Êtes-vous sûr de vouloir supprimer cette commande?\n\n"
                                        "Référence: %1\nClient: %2\n\n"
                                        "Cette action est irréversible.")
                                 .arg(ref, client),
                                 QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        ui->productionTable->removeRow(currentRow);
        QMessageBox::information(this, "Suppression réussie",
                               "La commande a été supprimée avec succès.");
    }
}

QPixmap MainWindow::generateQRCode(const QString &text, int size)
{
    Q_UNUSED(text)
    Q_UNUSED(size)
    // QR Code generation would require an external library like qrencode
    // For now, return a placeholder
    QPixmap placeholder(size, size);
    placeholder.fill(Qt::white);
    return placeholder;
}
