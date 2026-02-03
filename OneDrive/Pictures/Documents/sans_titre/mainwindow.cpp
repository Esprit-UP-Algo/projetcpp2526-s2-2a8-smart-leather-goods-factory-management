#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "addemployeedialog.h"
#include "editemployeedialog.h"
#include "deleteemployeedialog.h"
#include "exportdialog.h"
#include <QTableWidgetItem>
#include <QIcon>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    // Customize window title and icon for title bar only
    setWindowTitle("CUIREA - Employés Management");
    setWindowIcon(QIcon(":/logo.png"));
    
    // Hide vertical header (row numbers)
    ui->employeeTable->verticalHeader()->setVisible(false);
    
    // Populate employee table with sample data
    populateEmployeeTable();
    
    // Set column widths for better layout
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
    
    // Connect table selection to profile panel update
    connect(ui->employeeTable, &QTableWidget::currentCellChanged, this, &MainWindow::onEmployeeSelected);
    
    // Connect navigation buttons
    connect(ui->btnEmployees, &QPushButton::clicked, this, &MainWindow::onNavigationButtonClicked);
    connect(ui->btnClients, &QPushButton::clicked, this, &MainWindow::onNavigationButtonClicked);
    connect(ui->btnProducts, &QPushButton::clicked, this, &MainWindow::onNavigationButtonClicked);
    connect(ui->btnOrders, &QPushButton::clicked, this, &MainWindow::onNavigationButtonClicked);
    connect(ui->btnRawMaterials, &QPushButton::clicked, this, &MainWindow::onNavigationButtonClicked);
    connect(ui->btnSuppliers, &QPushButton::clicked, this, &MainWindow::onNavigationButtonClicked);
    
    // Connect CRUD buttons (static mode - dialogs open but don't save)
    connect(ui->btnAdd, &QPushButton::clicked, this, &MainWindow::onAddButtonClicked);
    connect(ui->btnEdit, &QPushButton::clicked, this, &MainWindow::onEditButtonClicked);
    connect(ui->btnDelete, &QPushButton::clicked, this, &MainWindow::onDeleteButtonClicked);
    connect(ui->btnExport, &QPushButton::clicked, this, &MainWindow::onExportButtonClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::populateEmployeeTable()
{
    ui->employeeTable->setRowCount(15);
    
    // Sample employee data with all attributes
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
}


void MainWindow::onAddButtonClicked()
{
    AddEmployeeDialog dialog(this);
    dialog.exec();
}

void MainWindow::onEditButtonClicked()
{
    EditEmployeeDialog dialog(this);
    dialog.exec();
}

void MainWindow::onDeleteButtonClicked()
{
    DeleteEmployeeDialog dialog(this);
    dialog.exec();
}

void MainWindow::onExportButtonClicked()
{
    ExportDialog dialog(this);
    dialog.exec();
}

void MainWindow::onEmployeeSelected()
{
    int currentRow = ui->employeeTable->currentRow();
    if (currentRow < 0) {
        return;
    }
    
    // Get employee data from selected row
    QString matricule = ui->employeeTable->item(currentRow, 1)->text();
    QString nom = ui->employeeTable->item(currentRow, 2)->text();
    QString prenom = ui->employeeTable->item(currentRow, 3)->text();
    QString cin = ui->employeeTable->item(currentRow, 4)->text();
    QString dateNaissance = ui->employeeTable->item(currentRow, 5)->text();
    QString departement = ui->employeeTable->item(currentRow, 6)->text();
    QString poste = ui->employeeTable->item(currentRow, 7)->text();
    QString telephone = ui->employeeTable->item(currentRow, 8)->text();
    QString email = ui->employeeTable->item(currentRow, 9)->text();
    
    // Update profile panel
    ui->profileValue->setText(matricule);
    ui->profileValue_2->setText(nom);
    ui->profileValue_3->setText(prenom);
    ui->profileValue_4->setText(cin);
    ui->profileValue_5->setText(dateNaissance);
    ui->profileValue_6->setText("M"); // Default
    ui->profileValue_7->setText("Casablanca"); // Default
    ui->profileValue_8->setText(telephone);
    ui->profileValue_9->setText(email);
    ui->profileValue_10->setText(poste);
    ui->profileValue_11->setText(departement);
    ui->profileValue_12->setText(departement);
    ui->profileValue_13->setText("01/01/2024"); // Default
}

void MainWindow::onNavigationButtonClicked()
{
    // Navigation buttons clicked - modules to be implemented later
    // No action for now
}
