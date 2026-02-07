#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "employeedialog.h"
#include "clientmanagerdialog.h"
#include "productdialog.h"
#include <QTableWidgetItem>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("CUIREA - Management System");
    
    ui->tblEmploye->verticalHeader()->setVisible(false);
    ui->tblClient->verticalHeader()->setVisible(false);
    ui->tblProduit->verticalHeader()->setVisible(false);
    
    populateEmployeeTable();
    
    ui->tblEmploye->setColumnWidth(0, 50);
    ui->tblEmploye->setColumnWidth(1, 110);
    ui->tblEmploye->setColumnWidth(2, 100);
    ui->tblEmploye->setColumnWidth(3, 100);
    ui->tblEmploye->setColumnWidth(4, 100);
    ui->tblEmploye->setColumnWidth(5, 120);
    ui->tblEmploye->setColumnWidth(6, 130);
    ui->tblEmploye->setColumnWidth(7, 120);
    ui->tblEmploye->setColumnWidth(8, 110);
    ui->tblEmploye->setColumnWidth(9, 180);
    
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
    
    // Add sample products
    Produit p1;
    p1.setReference("SAC-001");
    p1.setNom("Sac à main cuir classique");
    p1.setCategorie("Sacs");
    p1.setType("Cuir véritable");
    p1.setCouleur("Marron");
    p1.setDimensions("30x25x10 cm");
    p1.setPrixUnitaire(189.99);
    p1.setCoutFabrication(113.99);
    p1.setStock(25);
    p1.setStatut("Disponible");
    p1.setDescription("Sac à main élégant en cuir véritable");
    produits.append(p1);
    
    Produit p2;
    p2.setReference("CHU-002");
    p2.setNom("Bottines en cuir marron");
    p2.setCategorie("Chaussures");
    p2.setType("Cuir véritable");
    p2.setCouleur("Marron");
    p2.setDimensions("Pointures 36-44");
    p2.setPrixUnitaire(299.50);
    p2.setCoutFabrication(179.70);
    p2.setStock(15);
    p2.setStatut("Disponible");
    p2.setDescription("Bottines confortables en cuir de qualité");
    produits.append(p2);
    
    Produit p3;
    p3.setReference("CEI-003");
    p3.setNom("Ceinture cuir noir");
    p3.setCategorie("Ceintures");
    p3.setType("Cuir véritable");
    p3.setCouleur("Noir");
    p3.setDimensions("Ajustable");
    p3.setPrixUnitaire(75.00);
    p3.setCoutFabrication(45.00);
    p3.setStock(40);
    p3.setStatut("Disponible");
    p3.setDescription("Ceinture classique en cuir noir");
    produits.append(p3);
    
    // Populate product table
    refreshProductTable();
    
    ui->stackedWidget->setCurrentIndex(0);
    ui->stackedWidgetEmpl->setCurrentIndex(0);
    ui->stackedWidgetClient->setCurrentIndex(0);
    ui->stackedWidgetProduit->setCurrentIndex(0);
    
    ui->profilePanel->setVisible(true);
    
    ui->btnEmployees->setStyleSheet(
        "QPushButton { background-color: #6E473B; color: #FFFFFF; border-left: 3px solid #FFFFFF; }"
    );
    
    connect(ui->tblEmploye, &QTableWidget::currentCellChanged, this, &MainWindow::onEmployeeSelected);
    connect(ui->tblEmploye, &QTableWidget::itemSelectionChanged, this, &MainWindow::onEmployeeSelected);
    connect(ui->tblClient, &QTableWidget::currentCellChanged, this, &MainWindow::onClientSelected);
    connect(ui->tblClient, &QTableWidget::itemSelectionChanged, this, &MainWindow::onClientSelected);
    connect(ui->tblProduit, &QTableWidget::currentCellChanged, this, &MainWindow::onProductSelected);
    connect(ui->tblProduit, &QTableWidget::itemSelectionChanged, this, &MainWindow::onProductSelected);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::refreshClientTable()
{
    ui->tblClient->setRowCount(clients.size());
    
    for (int i = 0; i < clients.size(); ++i) {
        const Client &c = clients[i];
        ui->tblClient->setItem(i, 0, new QTableWidgetItem(c.getNom()));
        ui->tblClient->setItem(i, 1, new QTableWidgetItem(c.getPrenom()));
        ui->tblClient->setItem(i, 2, new QTableWidgetItem(c.getSexe()));
        ui->tblClient->setItem(i, 3, new QTableWidgetItem(c.getCin()));
        ui->tblClient->setItem(i, 4, new QTableWidgetItem(c.getPays()));
        ui->tblClient->setItem(i, 5, new QTableWidgetItem(c.getVille()));
        ui->tblClient->setItem(i, 6, new QTableWidgetItem(c.getAdresse()));
        ui->tblClient->setItem(i, 7, new QTableWidgetItem(c.getEmail()));
    }
}

void MainWindow::on_btnEmployees_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->profilePanel->setVisible(true);
    setWindowTitle("CUIREA - Gestion des Employés");
    
    ui->btnEmployees->setStyleSheet("");
    ui->btnClients->setStyleSheet("");
    ui->btnProducts->setStyleSheet("");
    ui->btnOrders->setStyleSheet("");
    ui->btnRawMaterials->setStyleSheet("");
    ui->btnSuppliers->setStyleSheet("");
    
    ui->btnEmployees->setStyleSheet(
        "QPushButton { background-color: #6E473B; color: #FFFFFF; border-left: 3px solid #FFFFFF; }"
    );
}

void MainWindow::on_btnClients_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->profilePanel->setVisible(false);
    setWindowTitle("CUIREA - Gestion des Clients");
    
    ui->btnEmployees->setStyleSheet("");
    ui->btnClients->setStyleSheet("");
    ui->btnProducts->setStyleSheet("");
    ui->btnOrders->setStyleSheet("");
    ui->btnRawMaterials->setStyleSheet("");
    ui->btnSuppliers->setStyleSheet("");
    
    ui->btnClients->setStyleSheet(
        "QPushButton { background-color: #6E473B; color: #FFFFFF; border-left: 3px solid #FFFFFF; }"
    );
}

void MainWindow::on_btnProducts_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    ui->profilePanel->setVisible(false);
    setWindowTitle("CUIREA - Gestion des Produits");
    
    ui->btnEmployees->setStyleSheet("");
    ui->btnClients->setStyleSheet("");
    ui->btnProducts->setStyleSheet("");
    ui->btnOrders->setStyleSheet("");
    ui->btnRawMaterials->setStyleSheet("");
    ui->btnSuppliers->setStyleSheet("");
    
    ui->btnProducts->setStyleSheet(
        "QPushButton { background-color: #6E473B; color: #FFFFFF; border-left: 3px solid #FFFFFF; }"
    );
}

void MainWindow::on_btnOrders_clicked()
{
    QMessageBox::information(this, "Module en développement",
        "Gestion des Commandes\n\n"
        "Ce module sera disponible prochainement.\n\n"
        "Fonctionnalités prévues :\n"
        "• Ajout et modification\n"
        "• Recherche et filtrage\n"
        "• Export des données\n"
        "• Rapports et statistiques");
}

void MainWindow::on_btnRawMaterials_clicked()
{
    QMessageBox::information(this, "Module en développement",
        "Gestion des Matières Premières\n\n"
        "Ce module sera disponible prochainement.\n\n"
        "Fonctionnalités prévues :\n"
        "• Ajout et modification\n"
        "• Recherche et filtrage\n"
        "• Export des données\n"
        "• Rapports et statistiques");
}

void MainWindow::on_btnSuppliers_clicked()
{
    QMessageBox::information(this, "Module en développement",
        "Gestion des Fournisseurs\n\n"
        "Ce module sera disponible prochainement.\n\n"
        "Fonctionnalités prévues :\n"
        "• Ajout et modification\n"
        "• Recherche et filtrage\n"
        "• Export des données\n"
        "• Rapports et statistiques");
}

void MainWindow::on_btnAjouterEmploye_clicked()
{
    EmployeeDialog dialog(this, EmployeeDialog::AddMode);
    dialog.exec();
}

void MainWindow::on_btnModifierEmploye_clicked()
{
    int currentRow = ui->tblEmploye->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Aucune sélection", "Veuillez sélectionner un employé à modifier.");
        return;
    }
    
    EmployeeDialog dialog(this, EmployeeDialog::EditMode);
    dialog.setEmployeeData(
        ui->tblEmploye->item(currentRow, 0)->text(),
        ui->tblEmploye->item(currentRow, 1)->text(),
        ui->tblEmploye->item(currentRow, 2)->text(),
        ui->tblEmploye->item(currentRow, 3)->text(),
        ui->tblEmploye->item(currentRow, 4)->text(),
        ui->tblEmploye->item(currentRow, 5)->text(),
        "Homme", "",
        ui->tblEmploye->item(currentRow, 8)->text(),
        ui->tblEmploye->item(currentRow, 9)->text(),
        ui->tblEmploye->item(currentRow, 7)->text(),
        "",
        ui->tblEmploye->item(currentRow, 6)->text(),
        "01/01/2024"
    );
    dialog.exec();
}

void MainWindow::on_btnSupprimerEmploye_clicked()
{
    int currentRow = ui->tblEmploye->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Aucune sélection", "Veuillez sélectionner un employé à supprimer.");
        return;
    }
    
    EmployeeDialog dialog(this, EmployeeDialog::DeleteMode);
    dialog.setEmployeeData(
        ui->tblEmploye->item(currentRow, 0)->text(),
        ui->tblEmploye->item(currentRow, 1)->text(),
        ui->tblEmploye->item(currentRow, 2)->text(),
        ui->tblEmploye->item(currentRow, 3)->text(),
        "", "", "", "", "", "",
        ui->tblEmploye->item(currentRow, 7)->text(),
        "",
        ui->tblEmploye->item(currentRow, 6)->text(),
        ""
    );
    dialog.exec();
}

void MainWindow::on_btnExporterEmploye_clicked()
{
    EmployeeDialog dialog(this, EmployeeDialog::ExportMode);
    dialog.exec();
}

void MainWindow::on_btnAjouterClient_clicked()
{
    ClientManagerDialog dlg(this, ClientManagerDialog::AddMode);
    dlg.exec();
}

void MainWindow::on_btnModifierClient_clicked()
{
    int currentRow = ui->tblClient->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Aucune sélection", "Veuillez sélectionner un client à modifier.");
        return;
    }
    
    ClientManagerDialog dlg(this, ClientManagerDialog::EditMode);
    dlg.setClientData(
        ui->tblClient->item(currentRow, 0)->text(),
        ui->tblClient->item(currentRow, 1)->text(),
        ui->tblClient->item(currentRow, 2)->text(),
        ui->tblClient->item(currentRow, 3)->text(),
        ui->tblClient->item(currentRow, 4)->text(),
        ui->tblClient->item(currentRow, 5)->text(),
        ui->tblClient->item(currentRow, 6)->text(),
        ui->tblClient->item(currentRow, 7)->text()
    );
    dlg.exec();
}

void MainWindow::on_btnSupprimerClient_clicked()
{
    int currentRow = ui->tblClient->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Aucune sélection", "Veuillez sélectionner un client à supprimer.");
        return;
    }
    
    ClientManagerDialog dlg(this, ClientManagerDialog::DeleteMode);
    dlg.setClientData(
        ui->tblClient->item(currentRow, 0)->text(),
        ui->tblClient->item(currentRow, 1)->text(),
        ui->tblClient->item(currentRow, 2)->text(),
        ui->tblClient->item(currentRow, 3)->text(),
        ui->tblClient->item(currentRow, 4)->text(),
        ui->tblClient->item(currentRow, 5)->text(),
        "", ""
    );
    dlg.exec();
}

void MainWindow::on_btnExporterClient_clicked()
{
    ClientManagerDialog dialog(this, ClientManagerDialog::ExportMode);
    dialog.exec();
}

void MainWindow::on_btnActualiserClient_clicked()
{
    QMessageBox::information(this, "Actualisation",
        "Liste des clients actualisée !\n(Mode statique - données d'exemple)");
}

void MainWindow::populateEmployeeTable()
{
    ui->tblEmploye->setRowCount(15);
    
    struct Employee {
        QString id, matricule, nom, prenom, cin, dateNaissance, departement, poste, telephone, email;
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
        ui->tblEmploye->setItem(i, 0, new QTableWidgetItem(employees[i].id));
        ui->tblEmploye->setItem(i, 1, new QTableWidgetItem(employees[i].matricule));
        ui->tblEmploye->setItem(i, 2, new QTableWidgetItem(employees[i].nom));
        ui->tblEmploye->setItem(i, 3, new QTableWidgetItem(employees[i].prenom));
        ui->tblEmploye->setItem(i, 4, new QTableWidgetItem(employees[i].cin));
        ui->tblEmploye->setItem(i, 5, new QTableWidgetItem(employees[i].dateNaissance));
        ui->tblEmploye->setItem(i, 6, new QTableWidgetItem(employees[i].departement));
        ui->tblEmploye->setItem(i, 7, new QTableWidgetItem(employees[i].poste));
        ui->tblEmploye->setItem(i, 8, new QTableWidgetItem(employees[i].telephone));
        ui->tblEmploye->setItem(i, 9, new QTableWidgetItem(employees[i].email));
    }
}

void MainWindow::onEmployeeSelected()
{
    int currentRow = ui->tblEmploye->currentRow();
    QList<QTableWidgetItem*> selectedItems = ui->tblEmploye->selectedItems();
    
    if (currentRow >= 0 && !selectedItems.isEmpty()) {
        ui->btnModifierEmploye->setEnabled(true);
        ui->btnSupprimerEmploye->setEnabled(true);
        
        ui->profileValue->setText(ui->tblEmploye->item(currentRow, 1)->text());
        ui->profileValue_2->setText(ui->tblEmploye->item(currentRow, 2)->text());
        ui->profileValue_3->setText(ui->tblEmploye->item(currentRow, 3)->text());
        ui->profileValue_4->setText(ui->tblEmploye->item(currentRow, 4)->text());
        ui->profileValue_5->setText(ui->tblEmploye->item(currentRow, 5)->text());
        ui->profileValue_6->setText("M");
        ui->profileValue_7->setText("Casablanca");
        ui->profileValue_8->setText(ui->tblEmploye->item(currentRow, 8)->text());
        ui->profileValue_9->setText(ui->tblEmploye->item(currentRow, 9)->text());
        ui->profileValue_10->setText(ui->tblEmploye->item(currentRow, 7)->text());
        ui->profileValue_11->setText(ui->tblEmploye->item(currentRow, 6)->text());
        ui->profileValue_12->setText(ui->tblEmploye->item(currentRow, 6)->text());
        ui->profileValue_13->setText("01/01/2024");
        ui->photoPlaceholder->clear();
        ui->photoPlaceholder->setText("●");
    } else {
        ui->btnModifierEmploye->setEnabled(false);
        ui->btnSupprimerEmploye->setEnabled(false);
    }
}

void MainWindow::onClientSelected()
{
    int currentRow = ui->tblClient->currentRow();
    QList<QTableWidgetItem*> selectedItems = ui->tblClient->selectedItems();
    
    if (currentRow >= 0 && !selectedItems.isEmpty()) {
        ui->btnModifierClient->setEnabled(true);
        ui->btnSupprimerClient->setEnabled(true);
    } else {
        ui->btnModifierClient->setEnabled(false);
        ui->btnSupprimerClient->setEnabled(false);
    }
}


void MainWindow::refreshProductTable()
{
    ui->tblProduit->setRowCount(produits.size());
    
    for (int i = 0; i < produits.size(); ++i) {
        const Produit &p = produits[i];
        ui->tblProduit->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
        ui->tblProduit->setItem(i, 1, new QTableWidgetItem(p.getReference()));
        ui->tblProduit->setItem(i, 2, new QTableWidgetItem(p.getNom()));
        ui->tblProduit->setItem(i, 3, new QTableWidgetItem(p.getCategorie()));
        ui->tblProduit->setItem(i, 4, new QTableWidgetItem(QString::number(p.getPrixUnitaire(), 'f', 2)));
        ui->tblProduit->setItem(i, 5, new QTableWidgetItem(QString::number(p.getStock())));
        ui->tblProduit->setItem(i, 6, new QTableWidgetItem(p.getStatut()));
    }
}

void MainWindow::onProductSelected()
{
    int currentRow = ui->tblProduit->currentRow();
    QList<QTableWidgetItem*> selectedItems = ui->tblProduit->selectedItems();
    
    if (currentRow >= 0 && !selectedItems.isEmpty()) {
        ui->btnModifierProduit->setEnabled(true);
        ui->btnSupprimerProduit->setEnabled(true);
        ui->btnConsulterProduit->setEnabled(true);
    } else {
        ui->btnModifierProduit->setEnabled(false);
        ui->btnSupprimerProduit->setEnabled(false);
        ui->btnConsulterProduit->setEnabled(false);
    }
}

void MainWindow::on_btnAjouterProduit_clicked()
{
    ProductDialog dialog(this, ProductDialog::AddMode);
    dialog.exec();
}

void MainWindow::on_btnModifierProduit_clicked()
{
    int currentRow = ui->tblProduit->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Aucune sélection", "Veuillez sélectionner un produit à modifier.");
        return;
    }
    
    ProductDialog dialog(this, ProductDialog::EditMode);
    dialog.setProductData(
        ui->tblProduit->item(currentRow, 0)->text(),
        ui->tblProduit->item(currentRow, 1)->text(),
        ui->tblProduit->item(currentRow, 2)->text(),
        ui->tblProduit->item(currentRow, 3)->text(),
        "Cuir véritable", "Naturel", "Standard",
        ui->tblProduit->item(currentRow, 4)->text().toDouble(),
        ui->tblProduit->item(currentRow, 4)->text().toDouble() * 0.6,
        ui->tblProduit->item(currentRow, 5)->text().toInt(),
        ui->tblProduit->item(currentRow, 6)->text(),
        "Description exemple", "05/02/2026"
    );
    dialog.exec();
}

void MainWindow::on_btnSupprimerProduit_clicked()
{
    int currentRow = ui->tblProduit->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Aucune sélection", "Veuillez sélectionner un produit à supprimer.");
        return;
    }
    
    ProductDialog dialog(this, ProductDialog::DeleteMode);
    dialog.setProductData(
        ui->tblProduit->item(currentRow, 0)->text(),
        ui->tblProduit->item(currentRow, 1)->text(),
        ui->tblProduit->item(currentRow, 2)->text(),
        "", "", "", "",
        ui->tblProduit->item(currentRow, 4)->text().toDouble(),
        0, 0, "", "", ""
    );
    dialog.exec();
}

void MainWindow::on_btnConsulterProduit_clicked()
{
    int currentRow = ui->tblProduit->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "Aucune sélection", "Veuillez sélectionner un produit à consulter.");
        return;
    }
    
    ProductDialog dialog(this, ProductDialog::ViewMode);
    dialog.setProductData(
        ui->tblProduit->item(currentRow, 0)->text(),
        ui->tblProduit->item(currentRow, 1)->text(),
        ui->tblProduit->item(currentRow, 2)->text(),
        ui->tblProduit->item(currentRow, 3)->text(),
        "Cuir véritable", "Naturel", "Standard",
        ui->tblProduit->item(currentRow, 4)->text().toDouble(),
        ui->tblProduit->item(currentRow, 4)->text().toDouble() * 0.6,
        ui->tblProduit->item(currentRow, 5)->text().toInt(),
        ui->tblProduit->item(currentRow, 6)->text(),
        "Description exemple du produit " + ui->tblProduit->item(currentRow, 2)->text(),
        "05/02/2026"
    );
    dialog.exec();
}

void MainWindow::on_btnStatistiquesProduit_clicked()
{
    int total = produits.size();
    int disponible = 0, enProduction = 0, obsolete = 0;
    double valeurStock = 0.0;
    QString plusDemande = "N/A";
    int maxStock = 0;
    
    for (const Produit &p : produits) {
        if (p.getStatut() == "Disponible") {
            disponible++;
            valeurStock += p.getPrixUnitaire() * p.getStock();
        } else if (p.getStatut() == "En Production") {
            enProduction++;
        } else if (p.getStatut() == QString::fromUtf8("Obsolète")) {
            obsolete++;
        }
        
        if (p.getStock() > maxStock) {
            maxStock = p.getStock();
            plusDemande = p.getNom();
        }
    }
    
    ProductDialog dialog(this, ProductDialog::StatisticsMode);
    dialog.setStatisticsData(total, disponible, enProduction, obsolete, valeurStock, plusDemande);
    dialog.exec();
}
