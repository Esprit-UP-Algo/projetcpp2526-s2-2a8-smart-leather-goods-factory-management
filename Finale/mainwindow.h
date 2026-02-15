#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QPushButton>  // Needed for QPushButton
#include <QSqlDatabase>
#include "client.h"
#include "matiere.h"
#include "fournisseur.h"
#include "article.h"

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

    // New Client section buttons
    void on_btnStatsByRegion_clicked();
    void on_btnFidelityClassification_clicked();
    void on_btnAIAgent_clicked();

    // Auto-generated slots for Navigation buttons
    void on_btnEmployees_clicked();
    void on_btnClients_clicked();
    void on_btnProducts_clicked();
    // void on_btnOrders_clicked(); // REMOVED - Commandes button deleted
    void on_btnRawMaterials_clicked();
    void on_btnSuppliers_clicked();
    void on_btnProduction_clicked();

    // Table selection
    void onEmployeeSelected();

    // Production Management slots
    void onCreerProduction();
    void onModifierProduction();
    void onSuiviProduction();
    void onPlanificationProduction();
    void onFactureProduction();
    void onExcelProduction();
    void onRechercherProduction(const QString &text);
    void onProductionTableContextMenu(const QPoint &pos);
    void onSupprimerProduction();
    void onStatistiquesProduction();
    void onTrierProduction();

    // Raw Materials Management slots
    void onAddMatiere();
    void onEditMatiere();
    void onDeleteMatiere();
    void onSaveMatiere();
    void onCancelForm();
    void onSuggestionCommande();
    void onOptimisationFIFO();
    void onRechercheTriMatiere();
    void onStatistiquesMatiere();
    void onTriMatiere();
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
    void on_btnStatsFournisseur_clicked();
    void on_btnTriFournisseur_clicked();
    void on_searchBoxFournisseur_textChanged(const QString &text);

    // Articles Management slots
    void on_btnAddArticle_clicked();
    void on_btnEditArticle_clicked();
    void on_btnDeleteArticle_clicked();
    void on_btnViewArticle_clicked();
    void on_btnExportPdfArticle_clicked();
    void on_btnAnalyseRentabilite_clicked();
    void on_btnAideDecision_clicked();
    void on_searchBoxArticle_textChanged(const QString &text);

private:
    Ui::MainWindow *ui;
    QVector<Client> clients;
    QList<Fournisseur> fournisseurs;
    QList<FournisseurData> fournisseursData;  // Liste des fournisseurs pour la page
    QList<MouvementStock> historiqueMouvements;
    QSqlDatabase articleDB;  // Base de données pour les articles
    bool isEditMode;
    int editingRow;

    // New Client section buttons
    QPushButton* btnStatsByRegion;
    QPushButton* btnFidelityClassification;
    QPushButton* btnAIAgent;

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

    // Articles Management methods
    void setupArticleTable();
    void refreshArticleTable();
    void updateArticleStatistics();
    void loadArticlesFromDB();
    bool connectArticleDatabase();

    // Production Management methods
    void setupProductionTable();
    void loadProductionData();
    void ajouterCommandeProduction(const QString &ref, const QString &client, const QString &type,
                                   const QString &montant, const QString &dateCreation,
                                   const QString &dateLivraison, const QString &statut, const QString &priorite);
    void updateProductionStatistics();
    void updateProductionStatsCards();
    void afficherStatistiquesModernes();
    QPixmap generateQRCode(const QString &text, int size = 200);
};

#endif // MAINWINDOW_H
