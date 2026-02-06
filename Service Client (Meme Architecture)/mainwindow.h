#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QPushButton>
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
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Client CRUD buttons
    void on_btnAddClient_clicked();
    void on_btnEditClient_clicked();
    void on_btnDeleteClient_clicked();
    void on_btnExportClient_clicked();
    void on_btnRefreshClient_clicked();

    // Client advanced features
    void on_btnStatsByRegion_clicked();
    void on_btnFidelityClassification_clicked();
    void on_btnAIAgent_clicked();

    // Navigation
    void on_btnClients_clicked();

private:
    Ui::MainWindow *ui;

    // Client data
    QVector<Client> clients;
    bool isEditMode = false;
    int editingRow = -1;

    // Client advanced buttons
    QPushButton* btnStatsByRegion;
    QPushButton* btnFidelityClassification;
    QPushButton* btnAIAgent;

    // Client helpers
    void refreshClientTable();
};

#endif // MAINWINDOW_H
