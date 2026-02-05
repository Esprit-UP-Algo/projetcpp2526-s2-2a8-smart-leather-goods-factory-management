#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include "client.h"

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
    void on_btnAdd_clicked();
    void on_btnEdit_clicked();
    void on_btnDelete_clicked();
    void on_btnExport_clicked();
    
    // Auto-generated slots for Client buttons
    void on_btnAddClient_clicked();
    void on_btnEditClient_clicked();
    void on_btnDeleteClient_clicked();
    void on_btnExportClient_clicked();
    void on_btnRefreshClient_clicked();
    
    // Auto-generated slots for Navigation buttons
    void on_btnEmployees_clicked();
    void on_btnClients_clicked();
    void on_btnProducts_clicked();
    void on_btnOrders_clicked();
    void on_btnRawMaterials_clicked();
    void on_btnSuppliers_clicked();
    
    // Table selection
    void onEmployeeSelected();

private:
    Ui::MainWindow *ui;
    QVector<Client> clients;
    
    void populateEmployeeTable();
    void refreshClientTable();
};
#endif // MAINWINDOW_H
