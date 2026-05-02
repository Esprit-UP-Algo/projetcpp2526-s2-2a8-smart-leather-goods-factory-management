#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QMap>
#include <QSet>
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
#include "map.h"
#include "client.h"
#include "matiere.h"
#include "fournisseur.h"
#include "article.h"
#include "articleviewer3d.h"
#include "aichatwidget.h"
#include "notification.h"
#include "matieredetection.h"
#include "voicematieres.h"
#include "arduinomonitor.h"
#include "arduinomanager.h"
#include "pointage.h"
#include "arduino.h"
#include <QTimer>
#include <QPainter>
#include <QConicalGradient>
#include <QSerialPort>

// ── Bouton flottant style Meta AI ─────────────────────────────────────────────
class FloatingAIButton : public QWidget
{
    Q_OBJECT
public:
    explicit FloatingAIButton(QWidget *parent = nullptr);

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void enterEvent(QEnterEvent *e) override;
    void leaveEvent(QEvent *e) override;

private:
    QTimer *m_animTimer;
    float   m_angle     = 0.0f;
    float   m_logoAngle = 0.0f;
    bool    m_hovered   = false;
};

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
    // Permissions et authentification (publiques pour être appelées depuis main.cpp)
    void applyUserPermissions();
    void hideTabsBasedOnPermissions();
    void updateCRUDButtons();
    void customizeMenuBar();
    void showUserProfile();
    void changePassword();
    void logout();
    
    // Méthodes publiques pour les classes extraites
    void setupMatiereTable();
    void updateMatiereStatistics();
    void onDeleteMatiere();
    void onRechercheTriMatiere();
    void onStatistiquesMatiere();
    void onExportMatiere();
    void onDetectionDefauts();
    void onTriMatiere();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    // Employee
    void on_btnAdd_clicked();
    void on_btnEdit_clicked();
    void on_btnDelete_clicked();
    void on_btnExport_clicked();
    void on_btnSort_clicked();
    void on_btnStatistics_clicked();
    void on_btnPointage_clicked();

    // Client
    void on_btnAddClient_clicked();
    void on_btnEditClient_clicked();
    void on_btnDeleteClient_clicked();
    void on_btnExportClient_clicked();
    void on_btnRefreshClient_clicked();
    void on_btnTriClient_clicked();
    void on_btnFidelityClassification_clicked();
    void on_btnhistorique_clicked();
    void on_searchBoxClient_textChanged(const QString &text);
    void on_btntrie_clicked();
    void on_btnStatsByRegion_clicked();


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
    void checkRetards();

    // Raw materials
    void onAddMatiere();
    void onEditMatiere();

    // Suppliers
    void on_btnAddFournisseur_clicked();
    void on_btnEditFournisseur_clicked();
    void on_btnDeleteFournisseur_clicked();
    void on_btnLivraisonFournisseur_clicked();
    void on_btnExportFournisseur_clicked();
    void on_btnStatsFournisseur_clicked();
    void on_btnTriFournisseur_clicked();
    void on_btnSmsFournisseur_clicked();
    void on_btnQrFournisseur_clicked();
    void on_searchBoxFournisseur_textChanged(const QString &text);
    void on_btnmap_clicked();

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
    void on_btnView3DArticle_clicked();
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
    
    // Classes extraites pour matières premières
    MatiereDetection *matiereDetection;
    VoiceMatieres *voiceMatieres;

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
    void switchPage(int index, QPushButton *activeBtn, const QString &title);

    // Employee
    void populateEmployeeTable();
    void updateEmployeeStatistics();

    // Client
    void refreshClientTable();
    void afficherClients();
    int m_sortIndex = 0;
    
    // Suppliers
    void setupFournisseurTable();
    void refreshFournisseurTable();
    void updateFournisseurStatistics();
    void openMap(double lat, double lon);
    Map *mapService;

    // Articles
    void setupArticleTable();
    void refreshArticleTable();
    void updateArticleStatistics();
    void loadArticlesFromDB();

    // Production
    void setupProductionTable();
    void loadProductionData();
    void fillProductionRow(int row, const QAbstractItemModel *model, int i);
    void applyProductionQuery(const QString &sql);
    void ajouterCommandeProduction(const QString&, const QString&, const QString&,
                                   const QString&, const QString&, const QString&,
                                   const QString&, const QString&);
    void updateProductionStatistics();
    void updateProductionStatsCards();
    void afficherStatistiquesModernes();
    QPixmap generateQRCode(const QString &text, int size = 200);

    // AI floating button
    AIChatWidget     *m_aiWidget;
    FloatingAIButton *m_floatingBtn;

    // Production sort state
    int  m_productionSortCol;
    bool m_productionSortAsc;

    // Retard notifications
    QTimer *m_retardTimer;
    QSet<int> m_notifiedIds;

    // Notification system
    NotificationAI      *m_ai      = nullptr;
    NotificationBell    *m_bell    = nullptr;
    NotificationWatcher *m_watcher = nullptr;
    QLabel              *m_tempIndicator = nullptr;  // Indicateur température

    // === POINTAGE RFID ===
    Pointage      m_pointage;

    // === ARDUINO MULTI-CARTES ===
    ArduinoManager *m_arduinoManager       = nullptr;  // découverte automatique
    Arduino        *m_arduinoTempBalance   = nullptr;  // ROLE:TEMP_BALANCE
    Arduino        *m_arduinoLivraison     = nullptr;  // ROLE:LIVRAISON
    Arduino        *m_arduinoPointage      = nullptr;  // ROLE:POINTAGE
    ArduinoMonitor *m_arduinoMonitor       = nullptr;  // scénarios temp + balance
    QByteArray      arduinoData;
    QSerialPort    *m_serialLivraison      = nullptr;  // port série carte LIVRAISON
    QSerialPort    *m_serialPointage       = nullptr;  // port série carte POINTAGE
    QTimer         *m_timerAbsences        = nullptr;
    QLabel         *m_arduinoIndicator     = nullptr;
    QString         m_keypadBuffer;
    QLabel         *m_lcdLigne1            = nullptr;
    QLabel         *m_lcdLigne2            = nullptr;

    void setupArduinoMultiCartes();
    void onArduinoDiscoveryComplete();
    void setupArduinoPointage();
    void onArduinoDataReceived();
    void expedierActionArduino();
    void recevoir_donnee();
    void setupKeypadSimulator();
    void traiterMessageArduino(const QString &msg);
    void ouvrirCalendrierPointage(int idEmploye, const QString &nom);
};

#endif // MAINWINDOW_H
