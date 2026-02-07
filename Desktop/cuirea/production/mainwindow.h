#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onCreerClicked();
    void onModifierClicked();
    void onPDFClicked();
    void onExcelClicked();
    void onRetourClicked();
    void onRechercherTextChanged(const QString &text);
    void onTousStatutsClicked();
    void onSuiviClicked();
    void onPlanificationClicked();
    void showContextMenu(const QPoint &pos);
    void supprimerLigneSelectionnee();

private:
    Ui::MainWindow *ui;
    void loadSampleData();
    void updateStatistics();
    void ajouterCommande(const QString &ref, const QString &client, const QString &type, 
                        const QString &montant, const QString &dateCreation, 
                        const QString &dateLivraison, const QString &statut, const QString &priorite);
    QPixmap generateQRCode(const QString &text, int size = 200);
};
#endif // MAINWINDOW_H
