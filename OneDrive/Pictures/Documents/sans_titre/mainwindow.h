#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include "client.h"
#include "produit.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Auto-generated slots for Employee buttons
    void on_btnAjouterEmploye_clicked();
    void on_btnModifierEmploye_clicked();
    void on_btnSupprimerEmploye_clicked();
    void on_btnExporterEmploye_clicked();
    
    // Auto-generated slots for Client buttons
    void on_btnAjouterClient_clicked();
    void on_btnModifierClient_clicked();
    void on_btnSupprimerClient_clicked();
    void on_btnExporterClient_clicked();
    void on_btnActualiserClient_clicked();
    
    // Auto-generated slots for Product buttons
    void on_btnAjouterProduit_clicked();
    void on_btnModifierProduit_clicked();
    void on_btnSupprimerProduit_clicked();
    void on_btnConsulterProduit_clicked();
    void on_btnStatistiquesProduit_clicked();
    
    // Auto-generated slots for Navigation buttons
    void on_btnEmployees_clicked();
    void on_btnClients_clicked();
    void on_btnProducts_clicked();
    void on_btnOrders_clicked();
    void on_btnRawMaterials_clicked();
    void on_btnSuppliers_clicked();
    
    // Table selection
    void onEmployeeSelected();
    void onClientSelected();

private:
    Ui::MainWindow *ui;
    QVector<Client> clients;
    QVector<Produit> produits;
    
    void populateEmployeeTable();
    void refreshClientTable();
    void refreshProductTable();
    void onProductSelected();
};
#endif // MAINWINDOW_H
