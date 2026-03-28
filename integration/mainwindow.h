#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QMap>
#include <QPushButton>
#include <QNetworkAccessManager>
#include <QProcess>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QProgressBar>
#include <QPointer>
#include <QThread>
#include <QAudioSource>
#include <QMediaDevices>
#include <QAudioFormat>

// Windows Speech API
#ifdef Q_OS_WIN
#define COBJMACROS
#include <windows.h>
#include <sapi.h>
#include <sphelper.h>
#endif

#include "client.h"
#include "matiere.h"
#include "fournisseur.h"
#include "article.h"

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
    // Employee
    void on_btnAdd_clicked();
    void on_btnEdit_clicked();
    void on_btnDelete_clicked();
    void on_btnExport_clicked();
    void on_btnSort_clicked();
    void on_btnStatistics_clicked();

    // Client
    void on_btnAddClient_clicked();
    void on_btnEditClient_clicked();
    void on_btnDeleteClient_clicked();
    void on_btnExportClient_clicked();
    void on_btnRefreshClient_clicked();
    void on_btnTriClient_clicked();
    void on_btnStatsByRegion_clicked();
    void on_btnFidelityClassification_clicked();
    void on_btnAIAgent_clicked();


    // Navigation
    void on_btnEmployees_clicked();
    void on_btnClients_clicked();
    void on_btnProducts_clicked();
    void on_btnRawMaterials_clicked();
    void on_btnSuppliers_clicked();
    void on_btnProduction_clicked();

    // Employee table
    void onEmployeeSelected();

    // Production
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

    // Raw materials
    void onAddMatiere();
    void onEditMatiere();
    void onDeleteMatiere();
    void onSuggestionCommande();
    void onOptimisationFIFO();
    void onRechercheTriMatiere();
    void onStatistiquesMatiere();
    void onTriMatiere();
    void onDetectionDefauts();
    void onGestionFournisseurs();
    void onExportMatiere();

    // Voice
    void onVoiceCommand();
    void processVoiceCommand(const QString &cmd);

    // Suppliers
    void on_btnAddFournisseur_clicked();
    void on_btnEditFournisseur_clicked();
    void on_btnDeleteFournisseur_clicked();
    void on_btnExportFournisseur_clicked();
    void on_btnStatsFournisseur_clicked();
    void on_btnTriFournisseur_clicked();
    void on_searchBoxFournisseur_textChanged(const QString &text);

    // Articles
    void on_btnAddArticle_clicked();
    void on_btnEditArticle_clicked();
    void on_btnDeleteArticle_clicked();
    void on_btnViewArticle_clicked();
    void on_btnExportPdfArticle_clicked();
    void on_btnTriArticle_clicked();
    void on_btnStatistiquesArticle_clicked();
    void on_btnAnalyseRentabilite_clicked();
    void on_btnAideDecision_clicked();
    void on_searchBoxArticle_textChanged(const QString &text);

private:
    Ui::MainWindow *ui;
    QVector<Client> clients;
    QList<FournisseurData> fournisseursData;
    QList<Article> articles;
    bool isEditMode;
    int editingRow;
    
    // Détection de défauts - Mode API (Flask)
    QNetworkAccessManager *networkManager;
    QProcess *apiProcess;
    QString apiUrl;
    QLabel *detectionResultLabel;
    QProgressBar *detectionProgress;

    // Voice recognition
    bool voiceListening;
    QLabel *voiceFeedbackLabel;
#ifdef Q_OS_WIN
    ISpRecognizer  *spRecognizer;
    ISpRecoContext *spRecoContext;
    ISpRecoGrammar *spGrammar;
    void initSAPI();
    void stopSAPI();
#endif

    // Navigation
    void switchPage(int index, QPushButton *activeBtn, const QString &title, bool showProfile);

    // Employee
    void populateEmployeeTable();

    // Client
    void refreshClientTable();
    void afficherClients();
    
    // Raw materials
    void setupMatiereTable();
    int calculateDaysToExpiration(const QString &dateStr);
    QString getStockLevel(int current, int threshold);
    int calculateSuggestedQuantity(const QString &matiere);
    void updateMatiereStatistics();
    
    // Détection de défauts
    void startDetectionAnalysis(const QString &imagePath);
    void detectDefectsInImage(const QString &imagePath);
    void showDetectionResults(const QJsonObject &results);
    QString translateDefectClass(const QString &englishName);

    // Suppliers
    void setupFournisseurTable();
    void refreshFournisseurTable();
    void updateFournisseurStatistics();

    // Articles
    void setupArticleTable();
    void refreshArticleTable();
    void updateArticleStatistics();
    void loadArticlesFromDB();

    // Production
    void setupProductionTable();
    void loadProductionData();
    void ajouterCommandeProduction(const QString&, const QString&, const QString&,
                                   const QString&, const QString&, const QString&,
                                   const QString&, const QString&);
    void updateProductionStatistics();
    void updateProductionStatsCards();
    void afficherStatistiquesModernes();
    QPixmap generateQRCode(const QString &text, int size = 200);
};

#endif // MAINWINDOW_H
