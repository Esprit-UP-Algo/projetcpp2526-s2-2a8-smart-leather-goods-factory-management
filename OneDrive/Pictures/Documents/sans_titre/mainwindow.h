#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include "client.h"
#include "matiere.h"
#include "fournisseur.h"

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
    
    // Raw Materials Management slots
    void onAddMatiere();
    void onEditMatiere();
    void onDeleteMatiere();
    void onSaveMatiere();
    void onCancelForm();
    void onSuggestionCommande();
    void onOptimisationFIFO();
    void onRechercheTriMatiere();
    void onGestionFournisseurs();
    void onGenerateSuggestion();
    void onCloseSuggestion();
    void onAnalyzeFIFO();
    void onCloseOptimisation();
    void onAppliquerRecherche();
    void onResetRecherche();
    void onCloseRecherche();
    void onAddFournisseur();
    void onEditFournisseur();
    void onDeleteFournisseur();
    void onCloseFournisseurs();
    void onExportMatiere();
    
    // Suppliers Management slots
    void on_btnAddFournisseur_clicked();
    void on_btnEditFournisseur_clicked();
    void on_btnDeleteFournisseur_clicked();
    void on_btnExportFournisseur_clicked();
    void on_searchBoxFournisseur_textChanged(const QString &text);

private:
    Ui::MainWindow *ui;
    QVector<Client> clients;
    QList<Fournisseur> fournisseurs;
    QList<FournisseurData> fournisseursData;  // Liste des fournisseurs pour la page
    QList<MouvementStock> historiqueMouvements;
    bool isEditMode;
    int editingRow;
    
    void populateEmployeeTable();
    void refreshClientTable();
    
    // Raw Materials Management methods
    void setupMatiereTable();
    void setupSuggestionTable();
    void setupHistoriqueTable();
    void showForm(bool editMode = false);
    void hideForm();
    void clearForm();
    void hideAllPanels();
    void addMatiereToTable(const QString &module, const QString &reference, const QString &type, 
                          const QString &quantite, const QString &seuil, const QString &dateExp);
    void loadFournisseurs();
    void saveFournisseurs();
    void updateFournisseursList();
    void updateHistoriqueTable();
    void updateConsommationChart();
    int calculateDaysToExpiration(const QString &dateStr);
    QString getStockLevel(int currentStock, int threshold);
    int calculateSuggestedQuantity(const QString &matiere);
    void updateMatiereStatistics();
    
    // Suppliers Management methods
    void setupFournisseurTable();
    void refreshFournisseurTable();
    void updateFournisseurStatistics();
};
#endif // MAINWINDOW_H
