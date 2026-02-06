#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "clientmanagerdialog.h"
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QDate>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("CUIREA - Client Management");

    // Hide vertical headers
    ui->clientTable->verticalHeader()->setVisible(false);

    // Sample clients
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

    // Connect buttons
    connect(ui->btnAddClient, &QPushButton::clicked, this, &MainWindow::on_btnAddClient_clicked);
    connect(ui->btnEditClient, &QPushButton::clicked, this, &MainWindow::on_btnEditClient_clicked);
    connect(ui->btnDeleteClient, &QPushButton::clicked, this, &MainWindow::on_btnDeleteClient_clicked);
    connect(ui->btnExportClient, &QPushButton::clicked, this, &MainWindow::on_btnExportClient_clicked);
    connect(ui->btnRefreshClient, &QPushButton::clicked, this, &MainWindow::on_btnRefreshClient_clicked);
}

MainWindow::~MainWindow()
{
    delete ui;
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


// ------------------- Client Table -------------------
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

// ------------------- Client CRUD -------------------
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

    QString nom = ui->clientTable->item(currentRow, 0)->text();
    QString prenom = ui->clientTable->item(currentRow, 1)->text();
    QString sexe = ui->clientTable->item(currentRow, 2)->text();
    QString cin = ui->clientTable->item(currentRow, 3)->text();
    QString pays = ui->clientTable->item(currentRow, 4)->text();
    QString ville = ui->clientTable->item(currentRow, 5)->text();
    QString adresse = ui->clientTable->item(currentRow, 6)->text();
    QString email = ui->clientTable->item(currentRow, 7)->text();

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
    msgBox.exec();
}
void MainWindow::on_btnAIAgent_clicked()
{
    QMessageBox::information(this, "Agent IA", "🔍 Permet de rechercher rapidement un client dans la base de données.");
    QMessageBox msgBox(this);

}

void MainWindow::on_btnStatsByRegion_clicked()
{
    QMessageBox::information(this, "Statistiques par Région", "📊 Affiche le nombre de clients par région (statistiques simples).");

}

void MainWindow::on_btnFidelityClassification_clicked()
{
    QMessageBox::information(this, "Classification Fidélité","🏆 Classe les clients selon leur fidélité (nombre de commandes).");




}
