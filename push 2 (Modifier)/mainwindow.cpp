#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "employeedialog.h"
#include "clientmanagerdialog.h"
#include "matieredialog.h"
#include "fournisseurdialog.h"
#include <QTableWidgetItem>
#include <QDebug>
#include <QMessageBox>
#include <QDate>
#include <QInputDialog>
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
    ui->btnOrders->setStyleSheet("");
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
    ui->btnOrders->setStyleSheet("");
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

void MainWindow::on_btnRawMaterials_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);  // Show raw materials page
    ui->profilePanel->setVisible(false);    // Hide profile panel
    setWindowTitle("CUIREA - Gestion des Matières Premières");
    
    // Reset all navigation buttons to default style
    ui->btnEmployees->setStyleSheet("");
    ui->btnClients->setStyleSheet("");
    ui->btnProducts->setStyleSheet("");
    ui->btnOrders->setStyleSheet("");
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
    ui->btnOrders->setStyleSheet("");
    
    qDebug() << "Switched to Suppliers module";
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


