/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    QFrame *sidebar;
    QVBoxLayout *verticalLayout_2;
    QLabel *sidebarIcon;
    QLabel *sidebarTitle;
    QFrame *separator;
    QPushButton *btnClients;
    QPushButton *btnEmployees;
    QPushButton *btnSuppliers;
    QPushButton *btnRawMaterials;
    QPushButton *btnProducts;
    QPushButton *btnProduction;
    QSpacerItem *verticalSpacer;
    QWidget *mainContent;
    QVBoxLayout *verticalLayout;
    QWidget *contentArea;
    QHBoxLayout *horizontalLayout_3;
    QStackedWidget *stackedWidget;
    QWidget *employeePage;
    QVBoxLayout *verticalLayout_3;
    QLabel *moduleTitle;
    QWidget *statsContainer;
    QHBoxLayout *horizontalLayout_4;
    QFrame *statsCard;
    QVBoxLayout *verticalLayout_4;
    QLabel *statsLabel;
    QLabel *statsValue;
    QFrame *statsCard_2;
    QVBoxLayout *verticalLayout_5;
    QLabel *statsLabel_2;
    QLabel *statsValue_2;
    QFrame *statsCard_3;
    QVBoxLayout *verticalLayout_6;
    QLabel *statsLabel_3;
    QLabel *statsValue_3;
    QSpacerItem *horizontalSpacer_2;
    QLineEdit *searchBox;
    QWidget *toolbarWidget;
    QHBoxLayout *horizontalLayout_5;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *btnAdd;
    QPushButton *btnEdit;
    QPushButton *btnDelete;
    QPushButton *btnExport;
    QPushButton *btnStatistics;
    QPushButton *btnSort;
    QWidget *contentWithProfile;
    QHBoxLayout *horizontalLayout_employeeContent;
    QFrame *employeeProfilePanel;
    QVBoxLayout *verticalLayout_profile;
    QLabel *employeeProfileTitle;
    QLabel *employeePhotoPlaceholder;
    QFrame *profileSeparator;
    QWidget *employeeInfoSection;
    QFormLayout *formLayout_employeeProfile;
    QLabel *lblMatricule;
    QLabel *valMatricule;
    QLabel *lblNom;
    QLabel *valNom;
    QLabel *lblPrenom;
    QLabel *valPrenom;
    QLabel *lblCIN;
    QLabel *valCIN;
    QLabel *lblTelephone;
    QLabel *valTelephone;
    QLabel *lblPoste;
    QLabel *valPoste;
    QLabel *lblDepartement;
    QLabel *valDepartement;
    QLabel *lblSexe;
    QLabel *valSexe;
    QLabel *lblAdresse;
    QLabel *valAdresse;
    QLabel *lblRole;
    QLabel *valRole;
    QLabel *lblDateEmbauche;
    QLabel *valDateEmbauche;
    QSpacerItem *verticalSpacer_profile;
    QTableWidget *employeeTable;
    QWidget *clientPage;
    QVBoxLayout *verticalLayout_8;
    QLabel *listTitle;
    QLineEdit *searchBoxClient;
    QWidget *clientButtonsWidget;
    QHBoxLayout *horizontalLayout_6;
    QSpacerItem *horizontalSpacer_4;
    QPushButton *btntrie;
    QPushButton *btnFidelityClassification;
    QPushButton *btnStatsByRegion;
    QPushButton *btnAddClient;
    QPushButton *btnEditClient;
    QPushButton *btnDeleteClient;
    QPushButton *btnRefreshClient;
    QPushButton *btnhistorique;
    QTableWidget *clientTable;
    QWidget *rawMaterialsPage;
    QVBoxLayout *verticalLayout_9;
    QLabel *matiereTitle;
    QWidget *statsContainerMatiere;
    QHBoxLayout *horizontalLayout_8;
    QFrame *statsCardMatiere1;
    QVBoxLayout *verticalLayout_10;
    QLabel *statsLabelMatiere1;
    QLabel *statsValueMatiere1;
    QFrame *statsCardMatiere2;
    QVBoxLayout *verticalLayout_11;
    QLabel *statsLabelMatiere2;
    QLabel *statsValueMatiere2;
    QSpacerItem *horizontalSpacer_6;
    QWidget *searchWidget;
    QHBoxLayout *searchLayout;
    QLineEdit *searchBoxMatiere;
    QPushButton *btnRecherche;
    QSpacerItem *searchSpacer;
    QWidget *matiereButtonsWidget;
    QHBoxLayout *horizontalLayout_7;
    QSpacerItem *horizontalSpacer_5;
    QPushButton *btnAddMatiere;
    QPushButton *btnEditMatiere;
    QPushButton *btnDeleteMatiere;
    QPushButton *btnStatistiques;
    QPushButton *btnTri;
    QPushButton *btnDetection;
    QPushButton *btnVoice;
    QPushButton *btnExportMatiere;
    QTableWidget *matiereTable;
    QWidget *suppliersPage;
    QVBoxLayout *verticalLayout_15;
    QLabel *fournisseursTitle;
    QWidget *statsContainerFournisseurs;
    QHBoxLayout *horizontalLayout_9;
    QFrame *statsCardFournisseur1;
    QVBoxLayout *verticalLayout_16;
    QLabel *statsLabelFournisseur1;
    QLabel *statsValueFournisseur1;
    QFrame *statsCardFournisseur2;
    QVBoxLayout *verticalLayout_17;
    QLabel *statsLabelFournisseur2;
    QLabel *statsValueFournisseur2;
    QFrame *statsCardFournisseur3;
    QVBoxLayout *verticalLayout_18;
    QLabel *statsLabelFournisseur3;
    QLabel *statsValueFournisseur3;
    QSpacerItem *horizontalSpacer_7;
    QLineEdit *searchBoxFournisseur;
    QWidget *fournisseurButtonsWidget;
    QHBoxLayout *horizontalLayout_10;
    QSpacerItem *horizontalSpacer_8;
    QPushButton *btnAddFournisseur;
    QPushButton *btnEditFournisseur;
    QPushButton *btnDeleteFournisseur;
    QPushButton *btnmap;
    QPushButton *btnExportFournisseur;
    QPushButton *btnStatsFournisseur;
    QPushButton *btnTriFournisseur;
    QPushButton *btnSmsFournisseur;
    QTableWidget *fournisseurTable;
    QWidget *productionPage;
    QVBoxLayout *verticalLayout_production;
    QLabel *productionTitle;
    QWidget *productionStatsContainer;
    QHBoxLayout *horizontalLayout_productionStats;
    QFrame *statsCardProduction1;
    QVBoxLayout *verticalLayout_statsProduction1;
    QLabel *statsLabelProduction1;
    QLabel *statsValueProduction1;
    QFrame *statsCardProduction2;
    QVBoxLayout *verticalLayout_statsProduction2;
    QLabel *statsLabelProduction2;
    QLabel *statsValueProduction2;
    QFrame *statsCardProduction3;
    QVBoxLayout *verticalLayout_statsProduction3;
    QLabel *statsLabelProduction3;
    QLabel *statsValueProduction3;
    QFrame *statsCardProduction4;
    QVBoxLayout *verticalLayout_statsProduction4;
    QLabel *statsLabelProduction4;
    QLabel *statsValueProduction4;
    QSpacerItem *horizontalSpacer_statsProduction;
    QWidget *productionToolbar;
    QHBoxLayout *horizontalLayout_production;
    QLineEdit *searchBoxProduction;
    QSpacerItem *horizontalSpacer_production;
    QPushButton *btnExportClient;
    QPushButton *btnCreerProduction;
    QPushButton *btnModifierProduction;
    QPushButton *btnSupprimerProduction;
    QPushButton *btnFactureProduction;
    QPushButton *btnExcelProduction;
    QPushButton *btnTrierProduction;
    QPushButton *btnStatistiquesProduction;
    QPushButton *btnExpedier;
    QTableWidget *productionTable;
    QWidget *articlesPage;
    QVBoxLayout *verticalLayout_articles;
    QLabel *articlesTitle;
    QWidget *statsContainerArticle;
    QHBoxLayout *horizontalLayout_statsArticle;
    QFrame *statsCardArticle1;
    QVBoxLayout *verticalLayout_statsArticle1;
    QLabel *statsLabelArticle1;
    QLabel *statsValueArticle1;
    QFrame *statsCardArticle2;
    QVBoxLayout *verticalLayout_statsArticle2;
    QLabel *statsLabelArticle2;
    QLabel *statsValueArticle2;
    QFrame *statsCardArticle3;
    QVBoxLayout *verticalLayout_statsArticle3;
    QLabel *statsLabelArticle3;
    QLabel *statsValueArticle3;
    QSpacerItem *horizontalSpacer_statsArticle;
    QWidget *articlesToolbar;
    QVBoxLayout *verticalLayout_articlesToolbar;
    QHBoxLayout *articlesRow1;
    QLineEdit *searchBoxArticle;
    QPushButton *btnAddArticle;
    QPushButton *btnEditArticle;
    QPushButton *btnDeleteArticle;
    QPushButton *btnViewArticle;
    QPushButton *btnView3DArticle;
    QPushButton *btnTriArticle;
    QPushButton *btnExportPdfArticle;
    QPushButton *btnStatistiquesArticle;
    QPushButton *btnAideDecision;
    QTableWidget *articleTable;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1965, 839);
        MainWindow->setStyleSheet(QString::fromUtf8("\n"
"/* Main Window Background */\n"
"QMainWindow {\n"
"    background-color: #FAF5F0;\n"
"}\n"
"\n"
"/* Sidebar Styling */\n"
"#sidebar {\n"
"    background-color: #8D6E63;\n"
"    border-right: none;\n"
"    border-top-right-radius: 20px;\n"
"    border-bottom-right-radius: 20px;\n"
"}\n"
"\n"
"#sidebarTitle {\n"
"    color: #FFFFFF;\n"
"    font-family: 'Times New Roman', serif;\n"
"    font-size: 18px;\n"
"    font-weight: bold;\n"
"    padding: 10px;\n"
"}\n"
"\n"
"#sidebarIcon {\n"
"    color: #FFFFFF;\n"
"    font-size: 32px;\n"
"    background-color: transparent;\n"
"    border: none;\n"
"}\n"
"\n"
"/* Main Content Area */\n"
"#contentArea {\n"
"    background-color: #FAF5F0;\n"
"}\n"
"\n"
"#moduleTitle {\n"
"    color: #291C0E;\n"
"    font-family: 'Times New Roman', serif;\n"
"    font-size: 22px;\n"
"    font-weight: bold;\n"
"}\n"
"\n"
"/* Table Styling */\n"
"#employeeTable, #clientTable, #matiereTable, #fournisseurTable, #productionTable, #articleTable {\n"
"    background-color: #FFFFFF;\n"
"   "
                        " alternate-background-color: #FFF8F0;\n"
"    border: none;\n"
"    border-radius: 12px;\n"
"    gridline-color: #F0E6DA;\n"
"    font-family: Arial, sans-serif;\n"
"    font-size: 11px;\n"
"    selection-background-color: #8D6E63;\n"
"    selection-color: white;\n"
"}\n"
"\n"
"#employeeTable::item, #clientTable::item, #matiereTable::item, #fournisseurTable::item, #productionTable::item, #articleTable::item {\n"
"    padding: 12px 8px;\n"
"    border: none;\n"
"    border-bottom: 1px solid #F0E6DA;\n"
"    color: #291C0E;\n"
"    background-color: transparent;\n"
"}\n"
"\n"
"#employeeTable::item:alternate, #clientTable::item:alternate, #matiereTable::item:alternate, #fournisseurTable::item:alternate, #productionTable::item:alternate, #articleTable::item:alternate {\n"
"    background-color: #FFF8F0;\n"
"}\n"
"\n"
"#employeeTable::item:selected, #clientTable::item:selected, #matiereTable::item:selected, #fournisseurTable::item:selected, #productionTable::item:selected, #articleTable::item:selected {\n"
"    bac"
                        "kground-color: #8D6E63;\n"
"    color: white;\n"
"}\n"
"\n"
"#employeeTable::item:hover, #clientTable::item:hover, #matiereTable::item:hover, #fournisseurTable::item:hover, #productionTable::item:hover, #articleTable::item:hover {\n"
"    background-color: #F5EDE3;\n"
"}\n"
"\n"
"QHeaderView::section {\n"
"    background-color: #8D6E63;\n"
"    color: white;\n"
"    padding: 14px 10px;\n"
"    border: none;\n"
"    border-right: 1px solid rgba(255, 255, 255, 0.1);\n"
"    font-family: Arial, sans-serif;\n"
"    font-size: 11px;\n"
"    font-weight: bold;\n"
"    text-transform: uppercase;\n"
"    letter-spacing: 0.5px;\n"
"}\n"
"\n"
"QHeaderView::section:last {\n"
"    border-right: none;\n"
"}\n"
"\n"
"QHeaderView {\n"
"    background-color: transparent;\n"
"    border-top-left-radius: 12px;\n"
"    border-top-right-radius: 12px;\n"
"}\n"
"\n"
"QHeaderView::section:horizontal:first {\n"
"    border-top-left-radius: 12px;\n"
"}\n"
"\n"
"QHeaderView::section:horizontal:last {\n"
"    border-top-right-radius: 12"
                        "px;\n"
"}\n"
"\n"
"QScrollBar:vertical {\n"
"    border: none;\n"
"    background-color: #F5F5F5;\n"
"    width: 10px;\n"
"    border-radius: 5px;\n"
"    margin: 0px;\n"
"}\n"
"\n"
"QScrollBar::handle:vertical {\n"
"    background-color: #BCAAA4;\n"
"    border-radius: 5px;\n"
"    min-height: 20px;\n"
"}\n"
"\n"
"QScrollBar::handle:vertical:hover {\n"
"    background-color: #A78D78;\n"
"}\n"
"\n"
"QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {\n"
"    height: 0px;\n"
"}\n"
"\n"
"QScrollBar:horizontal {\n"
"    border: none;\n"
"    background-color: #F5F5F5;\n"
"    height: 10px;\n"
"    border-radius: 5px;\n"
"    margin: 0px;\n"
"}\n"
"\n"
"QScrollBar::handle:horizontal {\n"
"    background-color: #BCAAA4;\n"
"    border-radius: 5px;\n"
"    min-width: 20px;\n"
"}\n"
"\n"
"QScrollBar::handle:horizontal:hover {\n"
"    background-color: #A78D78;\n"
"}\n"
"\n"
"QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {\n"
"    width: 0px;\n"
"}\n"
"\n"
"/* Profile Panel */\n"
"#profil"
                        "ePanel {\n"
"    background-color: #E7DDD1;\n"
"    border: 2px solid #BCAAA4;\n"
"    border-radius: 10px;\n"
"}\n"
"\n"
"#profileTitle {\n"
"    color: #291C0E;\n"
"    font-family: 'Times New Roman', serif;\n"
"    font-size: 16px;\n"
"    font-weight: bold;\n"
"}\n"
"\n"
"#profileLabel, #profileLabel_2, #profileLabel_3, #profileLabel_4, #profileLabel_5, \n"
"#profileLabel_6, #profileLabel_7, #profileLabel_8, #profileLabel_9, #profileLabel_10,\n"
"#profileLabel_11, #profileLabel_12, #profileLabel_13 {\n"
"    color: #291C0E;\n"
"    font-family: Arial, sans-serif;\n"
"    font-size: 11px;\n"
"    font-weight: bold;\n"
"}\n"
"\n"
"#profileValue, #profileValue_2, #profileValue_3, #profileValue_4, #profileValue_5,\n"
"#profileValue_6, #profileValue_7, #profileValue_8, #profileValue_9, #profileValue_10,\n"
"#profileValue_11, #profileValue_12, #profileValue_13 {\n"
"    color: #291C0E;\n"
"    font-family: Arial, sans-serif;\n"
"    font-size: 11px;\n"
"    font-weight: bold;\n"
"}\n"
"\n"
"/* Photo Placeholder "
                        "*/\n"
"#photoPlaceholder {\n"
"    background-color: #FFF8F0;\n"
"    border: 2px solid #BCAAA4;\n"
"    border-radius: 8px;\n"
"}\n"
"\n"
"/* Primary Action Buttons - Employee Module */\n"
"QPushButton#btnAdd, QPushButton#btnEdit, QPushButton#btnDelete, QPushButton#btnExport, QPushButton#btnRefresh, QPushButton#btnSort, QPushButton#btnStatistics {\n"
"    background-color: #8D6E63;\n"
"    color: white;\n"
"    border: none;\n"
"    border-radius: 10px;\n"
"    padding: 10px 20px;\n"
"    font-family: Arial, sans-serif;\n"
"    font-size: 12px;\n"
"    font-weight: bold;\n"
"    text-align: center;\n"
"}\n"
"\n"
"QPushButton#btnAdd:hover, QPushButton#btnEdit:hover, QPushButton#btnDelete:hover, QPushButton#btnExport:hover, QPushButton#btnRefresh:hover, QPushButton#btnSort:hover, QPushButton#btnStatistics:hover {\n"
"    background-color: #A0826D;\n"
"    border: none;\n"
"}\n"
"\n"
"QPushButton#btnAdd:pressed, QPushButton#btnEdit:pressed, QPushButton#btnDelete:pressed, QPushButton#btnExport:pressed, QPushButto"
                        "n#btnRefresh:pressed, QPushButton#btnSort:pressed, QPushButton#btnStatistics:pressed {\n"
"    background-color: #6E473B;\n"
"    border: none;\n"
"    padding: 11px 20px 9px 20px;\n"
"}\n"
"\n"
"/* Primary Action Buttons - Client Module */\n"
"QPushButton#btnAddClient, QPushButton#btnEditClient, QPushButton#btnDeleteClient, QPushButton#btnExportClient, QPushButton#btnRefreshClient, QPushButton#btnTriClient {\n"
"    background-color: #8D6E63;\n"
"    color: white;\n"
"    border: none;\n"
"    border-radius: 10px;\n"
"    padding: 10px 20px;\n"
"    font-family: Arial, sans-serif;\n"
"    font-size: 12px;\n"
"    font-weight: bold;\n"
"    text-align: center;\n"
"}\n"
"\n"
"QPushButton#btnAddClient:hover, QPushButton#btnEditClient:hover, QPushButton#btnDeleteClient:hover, QPushButton#btnExportClient:hover, QPushButton#btnRefreshClient:hover, QPushButton#btnTriClient:hover {\n"
"    background-color: #A0826D;\n"
"    border: none;\n"
"}\n"
"\n"
"QPushButton#btnAddClient:pressed, QPushButton#btnEditClient:press"
                        "ed, QPushButton#btnDeleteClient:pressed, QPushButton#btnExportClient:pressed, QPushButton#btnRefreshClient:pressed, QPushButton#btnTriClient:pressed {\n"
"    background-color: #6E473B;\n"
"    border: none;\n"
"    padding: 11px 20px 9px 20px;\n"
"}\n"
"\n"
"/* Primary Action Buttons - Raw Materials Module */\n"
"QPushButton#btnAddMatiere, QPushButton#btnEditMatiere, QPushButton#btnDeleteMatiere, QPushButton#btnExportMatiere, QPushButton#btnRecherche, QPushButton#btnStatistiques, QPushButton#btnTri, QPushButton#btnDetection, QPushButton#btnVoice {\n"
"    background-color: #8D6E63;\n"
"    color: white;\n"
"    border: none;\n"
"    border-radius: 10px;\n"
"    padding: 10px 20px;\n"
"    font-family: Arial, sans-serif;\n"
"    font-size: 12px;\n"
"    font-weight: bold;\n"
"    text-align: center;\n"
"}\n"
"\n"
"QPushButton#btnAddMatiere:hover, QPushButton#btnEditMatiere:hover, QPushButton#btnDeleteMatiere:hover, QPushButton#btnExportMatiere:hover, QPushButton#btnRecherche:hover, QPushButton#btnStatistiques:"
                        "hover, QPushButton#btnTri:hover, QPushButton#btnDetection:hover, QPushButton#btnVoice:hover {\n"
"    background-color: #A0826D;\n"
"    border: none;\n"
"}\n"
"\n"
"QPushButton#btnAddMatiere:pressed, QPushButton#btnEditMatiere:pressed, QPushButton#btnDeleteMatiere:pressed, QPushButton#btnExportMatiere:pressed, QPushButton#btnRecherche:pressed, QPushButton#btnStatistiques:pressed, QPushButton#btnTri:pressed, QPushButton#btnDetection:pressed, QPushButton#btnVoice:pressed {\n"
"    background-color: #6E473B;\n"
"    border: none;\n"
"    padding: 11px 20px 9px 20px;\n"
"}\n"
"\n"
"/* Primary Action Buttons - Suppliers Module */\n"
"QPushButton#btnAddFournisseur, QPushButton#btnEditFournisseur, QPushButton#btnDeleteFournisseur, QPushButton#btnExportFournisseur, QPushButton#btnStatsFournisseur, QPushButton#btnTriFournisseur, QPushButton#btnSmsFournisseur {\n"
"    background-color: #8D6E63;\n"
"    color: white;\n"
"    border: none;\n"
"    border-radius: 10px;\n"
"    padding: 10px 20px;\n"
"    font-family: Ari"
                        "al, sans-serif;\n"
"    font-size: 12px;\n"
"    font-weight: bold;\n"
"    text-align: center;\n"
"}\n"
"\n"
"QPushButton#btnAddFournisseur:hover, QPushButton#btnEditFournisseur:hover, QPushButton#btnDeleteFournisseur:hover, QPushButton#btnExportFournisseur:hover, QPushButton#btnStatsFournisseur:hover, QPushButton#btnTriFournisseur:hover, QPushButton#btnSmsFournisseur:hover {\n"
"    background-color: #A0826D;\n"
"    border: none;\n"
"}\n"
"\n"
"QPushButton#btnAddFournisseur:pressed, QPushButton#btnEditFournisseur:pressed, QPushButton#btnDeleteFournisseur:pressed, QPushButton#btnExportFournisseur:pressed, QPushButton#btnStatsFournisseur:pressed, QPushButton#btnTriFournisseur:pressed, QPushButton#btnSmsFournisseur:pressed {\n"
"    background-color: #6E473B;\n"
"    border: none;\n"
"    padding: 11px 20px 9px 20px;\n"
"}\n"
"\n"
"/* Primary Action Buttons - Production Module */\n"
"QPushButton#btnCreerProduction, QPushButton#btnModifierProduction, QPushButton#btnSupprimerProduction, QPushButton#btnFacturePro"
                        "duction, QPushButton#btnExcelProduction, QPushButton#btnTrierProduction, QPushButton#btnStatistiquesProduction {\n"
"    background-color: #8D6E63;\n"
"    color: white;\n"
"    border: none;\n"
"    border-radius: 10px;\n"
"    padding: 10px 20px;\n"
"    font-family: Arial, sans-serif;\n"
"    font-size: 12px;\n"
"    font-weight: bold;\n"
"    text-align: center;\n"
"}\n"
"\n"
"QPushButton#btnCreerProduction:hover, QPushButton#btnModifierProduction:hover, QPushButton#btnSupprimerProduction:hover, QPushButton#btnFactureProduction:hover, QPushButton#btnExcelProduction:hover, QPushButton#btnTrierProduction:hover, QPushButton#btnStatistiquesProduction:hover {\n"
"    background-color: #A0826D;\n"
"    border: none;\n"
"}\n"
"\n"
"QPushButton#btnCreerProduction:pressed, QPushButton#btnModifierProduction:pressed, QPushButton#btnSupprimerProduction:pressed, QPushButton#btnFactureProduction:pressed, QPushButton#btnExcelProduction:pressed, QPushButton#btnTrierProduction:pressed, QPushButton#btnStatistiquesProduction"
                        ":pressed {\n"
"    background-color: #6E473B;\n"
"    border: none;\n"
"    padding: 11px 20px 9px 20px;\n"
"}\n"
"\n"
"/* Primary Action Buttons - Articles Module */\n"
"QPushButton#btnAddArticle, QPushButton#btnEditArticle, QPushButton#btnDeleteArticle, QPushButton#btnViewArticle, QPushButton#btnView3DArticle, QPushButton#btnExportPdfArticle, QPushButton#btnAideDecision, QPushButton#btnStatistiquesArticle, QPushButton#btnTriArticle {\n"
"    background-color: #8D6E63;\n"
"    color: white;\n"
"    border: none;\n"
"    border-radius: 8px;\n"
"    padding: 6px 10px;\n"
"    font-family: Arial, sans-serif;\n"
"    font-size: 10px;\n"
"    font-weight: bold;\n"
"    text-align: center;\n"
"}\n"
"\n"
"QPushButton#btnAddArticle:hover, QPushButton#btnEditArticle:hover, QPushButton#btnDeleteArticle:hover, QPushButton#btnViewArticle:hover, QPushButton#btnView3DArticle:hover, QPushButton#btnExportPdfArticle:hover, QPushButton#btnAideDecision:hover, QPushButton#btnStatistiquesArticle:hover, QPushButton#btnTriArticle:h"
                        "over {\n"
"    background-color: #A0826D;\n"
"    border: none;\n"
"}\n"
"\n"
"QPushButton#btnAddArticle:pressed, QPushButton#btnEditArticle:pressed, QPushButton#btnDeleteArticle:pressed, QPushButton#btnViewArticle:pressed, QPushButton#btnView3DArticle:pressed, QPushButton#btnExportPdfArticle:pressed, QPushButton#btnAideDecision:pressed, QPushButton#btnStatistiquesArticle:pressed, QPushButton#btnTriArticle:pressed {\n"
"    background-color: #6E473B;\n"
"    border: none;\n"
"}\n"
"\n"
"/* Search Box */\n"
"#searchBox, #searchBoxClient, #searchBoxMatiere, #searchBoxFournisseur, #searchBoxProduction, #searchBoxArticle {\n"
"    background-color: white;\n"
"    color: #291C0E;\n"
"    border: 2px solid #A78D78;\n"
"    border-radius: 6px;\n"
"    padding: 8px;\n"
"    font-family: Arial, sans-serif;\n"
"    font-size: 11px;\n"
"}\n"
"\n"
"#searchBox::placeholder, #searchBoxClient::placeholder, #searchBoxMatiere::placeholder, #searchBoxFournisseur::placeholder, #searchBoxProduction::placeholder, #searchBoxArticle"
                        "::placeholder {\n"
"    color: #A78D78;\n"
"}\n"
"\n"
"/* Separator Lines */\n"
"QFrame#separator {\n"
"    background-color: rgba(255, 255, 255, 0.3);\n"
"    border-radius: 2px;\n"
"}\n"
"\n"
"/* Navigation Buttons */\n"
"QPushButton#btnEmployees, QPushButton#btnClients, QPushButton#btnProducts, \n"
"QPushButton#btnRawMaterials, QPushButton#btnSuppliers, QPushButton#btnProduction {\n"
"    background-color: transparent;\n"
"    color: #FFFFFF;\n"
"    border: none;\n"
"    border-left: 3px solid transparent;\n"
"    border-radius: 8px;\n"
"    text-align: left;\n"
"    padding-left: 15px;\n"
"    padding-top: 8px;\n"
"    padding-bottom: 8px;\n"
"    margin: 2px 8px;\n"
"    font-family: Arial, sans-serif;\n"
"    font-size: 12px;\n"
"    font-weight: bold;\n"
"}\n"
"\n"
"QPushButton#btnEmployees:hover, QPushButton#btnClients:hover, QPushButton#btnProducts:hover,\n"
"QPushButton#btnRawMaterials:hover, QPushButton#btnSuppliers:hover, QPushButton#btnProduction:hover {\n"
"    background-color: rgba(255, 255, 2"
                        "55, 0.2);\n"
"    color: #FFFFFF;\n"
"    border-left: 3px solid #FFFFFF;\n"
"}\n"
"\n"
"QPushButton#btnEmployees:pressed, QPushButton#btnClients:pressed, QPushButton#btnProducts:pressed,\n"
"QPushButton#btnRawMaterials:pressed, QPushButton#btnSuppliers:pressed, QPushButton#btnProduction:pressed {\n"
"    background-color: #6E473B;\n"
"    color: #FFFFFF;\n"
"}\n"
"\n"
"/* Statistics Cards */\n"
"#statsCard, #statsCard_2, #statsCard_3, #statsCardMatiere1, #statsCardMatiere2, #statsCardMatiere3, #statsCardFournisseur1, #statsCardFournisseur2, #statsCardFournisseur3, #statsCardArticle1, #statsCardArticle2, #statsCardArticle3, #statsCardProduction1, #statsCardProduction2, #statsCardProduction3, #statsCardProduction4 {\n"
"    background-color: #FFF8F0;\n"
"    border: 1px solid #BCAAA4;\n"
"    border-radius: 8px;\n"
"}\n"
"\n"
"#statsLabel, #statsLabel_2, #statsLabel_3, #statsLabelMatiere1, #statsLabelMatiere2, #statsLabelMatiere3, #statsLabelFournisseur1, #statsLabelFournisseur2, #statsLabelFournisseur3, #stats"
                        "LabelArticle1, #statsLabelArticle2, #statsLabelArticle3, #statsLabelProduction1, #statsLabelProduction2, #statsLabelProduction3, #statsLabelProduction4 {\n"
"    color: #291C0E;\n"
"    font-family: Arial, sans-serif;\n"
"    font-size: 10px;\n"
"}\n"
"\n"
"#statsValue, #statsValue_2, #statsValue_3, #statsValueMatiere1, #statsValueMatiere2, #statsValueMatiere3, #statsValueFournisseur1, #statsValueFournisseur2, #statsValueFournisseur3, #statsValueArticle1, #statsValueArticle2, #statsValueArticle3, #statsValueProduction1, #statsValueProduction2, #statsValueProduction3, #statsValueProduction4 {\n"
"    color: #8D6E63;\n"
"    font-family: 'Times New Roman', serif;\n"
"    font-size: 24px;\n"
"    font-weight: bold;\n"
"}\n"
"\n"
"/* Alert Card */\n"
"#alertCardMatiere {\n"
"    background-color: #FFF8F0;\n"
"    border: 1px solid #BCAAA4;\n"
"    border-radius: 8px;\n"
"}\n"
"\n"
"#listTitle {\n"
"    color: #291C0E;\n"
"    font-family: 'Times New Roman', serif;\n"
"    font-size: 22px;\n"
"    font-weight: bold"
                        ";\n"
"}\n"
"\n"
"#matiereTitle, #fournisseursTitle {\n"
"    color: #291C0E;\n"
"    font-family: 'Times New Roman', serif;\n"
"    font-size: 22px;\n"
"    font-weight: bold;\n"
"}\n"
"\n"
"#productionTitle {\n"
"    color: #291C0E;\n"
"    font-family: 'Times New Roman', serif;\n"
"    font-size: 22px;\n"
"    font-weight: bold;\n"
"}\n"
"\n"
"#articlesTitle {\n"
"    color: #291C0E;\n"
"    font-family: 'Times New Roman', serif;\n"
"    font-size: 22px;\n"
"    font-weight: bold;\n"
"}\n"
"   "));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        sidebar = new QFrame(centralwidget);
        sidebar->setObjectName("sidebar");
        sidebar->setMinimumSize(QSize(180, 0));
        sidebar->setMaximumSize(QSize(180, 16777215));
        sidebar->setFrameShape(QFrame::Shape::StyledPanel);
        verticalLayout_2 = new QVBoxLayout(sidebar);
        verticalLayout_2->setSpacing(20);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(15, 30, 15, 30);
        sidebarIcon = new QLabel(sidebar);
        sidebarIcon->setObjectName("sidebarIcon");
        sidebarIcon->setMinimumSize(QSize(120, 120));
        sidebarIcon->setMaximumSize(QSize(120, 120));
        sidebarIcon->setPixmap(QPixmap(QString::fromUtf8(":/logo.png")));
        sidebarIcon->setScaledContents(true);
        sidebarIcon->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_2->addWidget(sidebarIcon, 0, Qt::AlignmentFlag::AlignHCenter);

        sidebarTitle = new QLabel(sidebar);
        sidebarTitle->setObjectName("sidebarTitle");
        sidebarTitle->setAlignment(Qt::AlignmentFlag::AlignCenter);
        sidebarTitle->setWordWrap(true);

        verticalLayout_2->addWidget(sidebarTitle);

        separator = new QFrame(sidebar);
        separator->setObjectName("separator");
        separator->setFrameShadow(QFrame::Shadow::Plain);
        separator->setLineWidth(2);
        separator->setFrameShape(QFrame::Shape::HLine);

        verticalLayout_2->addWidget(separator);

        btnClients = new QPushButton(sidebar);
        btnClients->setObjectName("btnClients");
        btnClients->setMinimumSize(QSize(0, 45));

        verticalLayout_2->addWidget(btnClients);

        btnEmployees = new QPushButton(sidebar);
        btnEmployees->setObjectName("btnEmployees");
        btnEmployees->setMinimumSize(QSize(0, 45));

        verticalLayout_2->addWidget(btnEmployees);

        btnSuppliers = new QPushButton(sidebar);
        btnSuppliers->setObjectName("btnSuppliers");
        btnSuppliers->setMinimumSize(QSize(0, 45));

        verticalLayout_2->addWidget(btnSuppliers);

        btnRawMaterials = new QPushButton(sidebar);
        btnRawMaterials->setObjectName("btnRawMaterials");
        btnRawMaterials->setMinimumSize(QSize(0, 45));

        verticalLayout_2->addWidget(btnRawMaterials);

        btnProducts = new QPushButton(sidebar);
        btnProducts->setObjectName("btnProducts");
        btnProducts->setMinimumSize(QSize(0, 45));

        verticalLayout_2->addWidget(btnProducts);

        btnProduction = new QPushButton(sidebar);
        btnProduction->setObjectName("btnProduction");
        btnProduction->setMinimumSize(QSize(0, 45));

        verticalLayout_2->addWidget(btnProduction);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);


        horizontalLayout->addWidget(sidebar);

        mainContent = new QWidget(centralwidget);
        mainContent->setObjectName("mainContent");
        verticalLayout = new QVBoxLayout(mainContent);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        contentArea = new QWidget(mainContent);
        contentArea->setObjectName("contentArea");
        horizontalLayout_3 = new QHBoxLayout(contentArea);
        horizontalLayout_3->setSpacing(20);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout_3->setContentsMargins(30, 30, 30, 30);
        stackedWidget = new QStackedWidget(contentArea);
        stackedWidget->setObjectName("stackedWidget");
        employeePage = new QWidget();
        employeePage->setObjectName("employeePage");
        verticalLayout_3 = new QVBoxLayout(employeePage);
        verticalLayout_3->setSpacing(20);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        moduleTitle = new QLabel(employeePage);
        moduleTitle->setObjectName("moduleTitle");
        moduleTitle->setStyleSheet(QString::fromUtf8("color: #291C0E; font-family: 'Times New Roman', serif; font-size: 22px; font-weight: bold;"));

        verticalLayout_3->addWidget(moduleTitle, 0, Qt::AlignmentFlag::AlignHCenter);

        statsContainer = new QWidget(employeePage);
        statsContainer->setObjectName("statsContainer");
        statsContainer->setMinimumSize(QSize(0, 65));
        statsContainer->setMaximumSize(QSize(16777215, 65));
        horizontalLayout_4 = new QHBoxLayout(statsContainer);
        horizontalLayout_4->setSpacing(15);
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        statsCard = new QFrame(statsContainer);
        statsCard->setObjectName("statsCard");
        statsCard->setFrameShape(QFrame::Shape::StyledPanel);
        verticalLayout_4 = new QVBoxLayout(statsCard);
        verticalLayout_4->setSpacing(5);
        verticalLayout_4->setObjectName("verticalLayout_4");
        statsLabel = new QLabel(statsCard);
        statsLabel->setObjectName("statsLabel");
        statsLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_4->addWidget(statsLabel);

        statsValue = new QLabel(statsCard);
        statsValue->setObjectName("statsValue");
        statsValue->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_4->addWidget(statsValue);


        horizontalLayout_4->addWidget(statsCard);

        statsCard_2 = new QFrame(statsContainer);
        statsCard_2->setObjectName("statsCard_2");
        statsCard_2->setFrameShape(QFrame::Shape::StyledPanel);
        verticalLayout_5 = new QVBoxLayout(statsCard_2);
        verticalLayout_5->setSpacing(5);
        verticalLayout_5->setObjectName("verticalLayout_5");
        statsLabel_2 = new QLabel(statsCard_2);
        statsLabel_2->setObjectName("statsLabel_2");
        statsLabel_2->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_5->addWidget(statsLabel_2);

        statsValue_2 = new QLabel(statsCard_2);
        statsValue_2->setObjectName("statsValue_2");
        statsValue_2->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_5->addWidget(statsValue_2);


        horizontalLayout_4->addWidget(statsCard_2);

        statsCard_3 = new QFrame(statsContainer);
        statsCard_3->setObjectName("statsCard_3");
        statsCard_3->setFrameShape(QFrame::Shape::StyledPanel);
        verticalLayout_6 = new QVBoxLayout(statsCard_3);
        verticalLayout_6->setSpacing(5);
        verticalLayout_6->setObjectName("verticalLayout_6");
        statsLabel_3 = new QLabel(statsCard_3);
        statsLabel_3->setObjectName("statsLabel_3");
        statsLabel_3->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_6->addWidget(statsLabel_3);

        statsValue_3 = new QLabel(statsCard_3);
        statsValue_3->setObjectName("statsValue_3");
        statsValue_3->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_6->addWidget(statsValue_3);


        horizontalLayout_4->addWidget(statsCard_3);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_2);


        verticalLayout_3->addWidget(statsContainer);

        searchBox = new QLineEdit(employeePage);
        searchBox->setObjectName("searchBox");
        searchBox->setMinimumSize(QSize(400, 0));
        searchBox->setMaximumSize(QSize(400, 16777215));

        verticalLayout_3->addWidget(searchBox);

        toolbarWidget = new QWidget(employeePage);
        toolbarWidget->setObjectName("toolbarWidget");
        toolbarWidget->setMinimumSize(QSize(0, 42));
        toolbarWidget->setMaximumSize(QSize(16777215, 42));
        horizontalLayout_5 = new QHBoxLayout(toolbarWidget);
        horizontalLayout_5->setSpacing(10);
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        horizontalLayout_5->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_3);

        btnAdd = new QPushButton(toolbarWidget);
        btnAdd->setObjectName("btnAdd");
        btnAdd->setMinimumSize(QSize(85, 32));

        horizontalLayout_5->addWidget(btnAdd);

        btnEdit = new QPushButton(toolbarWidget);
        btnEdit->setObjectName("btnEdit");
        btnEdit->setMinimumSize(QSize(85, 32));

        horizontalLayout_5->addWidget(btnEdit);

        btnDelete = new QPushButton(toolbarWidget);
        btnDelete->setObjectName("btnDelete");
        btnDelete->setMinimumSize(QSize(85, 32));

        horizontalLayout_5->addWidget(btnDelete);

        btnExport = new QPushButton(toolbarWidget);
        btnExport->setObjectName("btnExport");
        btnExport->setMinimumSize(QSize(85, 32));

        horizontalLayout_5->addWidget(btnExport);

        btnStatistics = new QPushButton(toolbarWidget);
        btnStatistics->setObjectName("btnStatistics");
        btnStatistics->setMinimumSize(QSize(100, 32));

        horizontalLayout_5->addWidget(btnStatistics);

        btnSort = new QPushButton(toolbarWidget);
        btnSort->setObjectName("btnSort");
        btnSort->setMinimumSize(QSize(85, 32));

        horizontalLayout_5->addWidget(btnSort);


        verticalLayout_3->addWidget(toolbarWidget);

        contentWithProfile = new QWidget(employeePage);
        contentWithProfile->setObjectName("contentWithProfile");
        horizontalLayout_employeeContent = new QHBoxLayout(contentWithProfile);
        horizontalLayout_employeeContent->setSpacing(20);
        horizontalLayout_employeeContent->setObjectName("horizontalLayout_employeeContent");
        horizontalLayout_employeeContent->setContentsMargins(0, 0, 0, 0);
        employeeProfilePanel = new QFrame(contentWithProfile);
        employeeProfilePanel->setObjectName("employeeProfilePanel");
        employeeProfilePanel->setMinimumSize(QSize(280, 0));
        employeeProfilePanel->setMaximumSize(QSize(280, 16777215));
        employeeProfilePanel->setStyleSheet(QString::fromUtf8("QFrame#employeeProfilePanel {\n"
"    background-color: #E7DDD1;\n"
"    border: 2px solid #BCAAA4;\n"
"    border-radius: 10px;\n"
"}"));
        employeeProfilePanel->setFrameShape(QFrame::Shape::StyledPanel);
        verticalLayout_profile = new QVBoxLayout(employeeProfilePanel);
        verticalLayout_profile->setSpacing(15);
        verticalLayout_profile->setObjectName("verticalLayout_profile");
        verticalLayout_profile->setContentsMargins(20, 20, 20, 20);
        employeeProfileTitle = new QLabel(employeeProfilePanel);
        employeeProfileTitle->setObjectName("employeeProfileTitle");
        employeeProfileTitle->setStyleSheet(QString::fromUtf8("color: #291C0E; font-family: 'Times New Roman', serif; font-size: 16px; font-weight: bold;"));
        employeeProfileTitle->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_profile->addWidget(employeeProfileTitle);

        employeePhotoPlaceholder = new QLabel(employeeProfilePanel);
        employeePhotoPlaceholder->setObjectName("employeePhotoPlaceholder");
        employeePhotoPlaceholder->setMinimumSize(QSize(120, 120));
        employeePhotoPlaceholder->setMaximumSize(QSize(120, 120));
        employeePhotoPlaceholder->setStyleSheet(QString::fromUtf8("background-color: #FFF8F0; border: 2px solid #BCAAA4; border-radius: 8px; font-size: 48px; color: #8D6E63;"));
        employeePhotoPlaceholder->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_profile->addWidget(employeePhotoPlaceholder, 0, Qt::AlignmentFlag::AlignHCenter);

        profileSeparator = new QFrame(employeeProfilePanel);
        profileSeparator->setObjectName("profileSeparator");
        profileSeparator->setFrameShadow(QFrame::Shadow::Plain);
        profileSeparator->setLineWidth(2);
        profileSeparator->setFrameShape(QFrame::Shape::HLine);

        verticalLayout_profile->addWidget(profileSeparator);

        employeeInfoSection = new QWidget(employeeProfilePanel);
        employeeInfoSection->setObjectName("employeeInfoSection");
        formLayout_employeeProfile = new QFormLayout(employeeInfoSection);
        formLayout_employeeProfile->setObjectName("formLayout_employeeProfile");
        formLayout_employeeProfile->setHorizontalSpacing(10);
        formLayout_employeeProfile->setVerticalSpacing(10);
        lblMatricule = new QLabel(employeeInfoSection);
        lblMatricule->setObjectName("lblMatricule");
        lblMatricule->setStyleSheet(QString::fromUtf8("color: #291C0E; font-size: 11px; font-weight: bold;"));

        formLayout_employeeProfile->setWidget(0, QFormLayout::LabelRole, lblMatricule);

        valMatricule = new QLabel(employeeInfoSection);
        valMatricule->setObjectName("valMatricule");
        valMatricule->setStyleSheet(QString::fromUtf8("color: #291C0E; font-size: 11px;"));

        formLayout_employeeProfile->setWidget(0, QFormLayout::FieldRole, valMatricule);

        lblNom = new QLabel(employeeInfoSection);
        lblNom->setObjectName("lblNom");
        lblNom->setStyleSheet(QString::fromUtf8("color: #291C0E; font-size: 11px; font-weight: bold;"));

        formLayout_employeeProfile->setWidget(1, QFormLayout::LabelRole, lblNom);

        valNom = new QLabel(employeeInfoSection);
        valNom->setObjectName("valNom");
        valNom->setStyleSheet(QString::fromUtf8("color: #291C0E; font-size: 11px;"));

        formLayout_employeeProfile->setWidget(1, QFormLayout::FieldRole, valNom);

        lblPrenom = new QLabel(employeeInfoSection);
        lblPrenom->setObjectName("lblPrenom");
        lblPrenom->setStyleSheet(QString::fromUtf8("color: #291C0E; font-size: 11px; font-weight: bold;"));

        formLayout_employeeProfile->setWidget(2, QFormLayout::LabelRole, lblPrenom);

        valPrenom = new QLabel(employeeInfoSection);
        valPrenom->setObjectName("valPrenom");
        valPrenom->setStyleSheet(QString::fromUtf8("color: #291C0E; font-size: 11px;"));

        formLayout_employeeProfile->setWidget(2, QFormLayout::FieldRole, valPrenom);

        lblCIN = new QLabel(employeeInfoSection);
        lblCIN->setObjectName("lblCIN");
        lblCIN->setStyleSheet(QString::fromUtf8("color: #291C0E; font-size: 11px; font-weight: bold;"));

        formLayout_employeeProfile->setWidget(3, QFormLayout::LabelRole, lblCIN);

        valCIN = new QLabel(employeeInfoSection);
        valCIN->setObjectName("valCIN");
        valCIN->setStyleSheet(QString::fromUtf8("color: #291C0E; font-size: 11px;"));

        formLayout_employeeProfile->setWidget(3, QFormLayout::FieldRole, valCIN);

        lblTelephone = new QLabel(employeeInfoSection);
        lblTelephone->setObjectName("lblTelephone");
        lblTelephone->setStyleSheet(QString::fromUtf8("color: #291C0E; font-size: 11px; font-weight: bold;"));

        formLayout_employeeProfile->setWidget(4, QFormLayout::LabelRole, lblTelephone);

        valTelephone = new QLabel(employeeInfoSection);
        valTelephone->setObjectName("valTelephone");
        valTelephone->setStyleSheet(QString::fromUtf8("color: #291C0E; font-size: 11px;"));

        formLayout_employeeProfile->setWidget(4, QFormLayout::FieldRole, valTelephone);

        lblPoste = new QLabel(employeeInfoSection);
        lblPoste->setObjectName("lblPoste");
        lblPoste->setStyleSheet(QString::fromUtf8("color: #291C0E; font-size: 11px; font-weight: bold;"));

        formLayout_employeeProfile->setWidget(5, QFormLayout::LabelRole, lblPoste);

        valPoste = new QLabel(employeeInfoSection);
        valPoste->setObjectName("valPoste");
        valPoste->setStyleSheet(QString::fromUtf8("color: #291C0E; font-size: 11px;"));

        formLayout_employeeProfile->setWidget(5, QFormLayout::FieldRole, valPoste);

        lblDepartement = new QLabel(employeeInfoSection);
        lblDepartement->setObjectName("lblDepartement");
        lblDepartement->setStyleSheet(QString::fromUtf8("color: #291C0E; font-size: 11px; font-weight: bold;"));

        formLayout_employeeProfile->setWidget(6, QFormLayout::LabelRole, lblDepartement);

        valDepartement = new QLabel(employeeInfoSection);
        valDepartement->setObjectName("valDepartement");
        valDepartement->setStyleSheet(QString::fromUtf8("color: #291C0E; font-size: 11px;"));

        formLayout_employeeProfile->setWidget(6, QFormLayout::FieldRole, valDepartement);

        lblSexe = new QLabel(employeeInfoSection);
        lblSexe->setObjectName("lblSexe");
        lblSexe->setStyleSheet(QString::fromUtf8("color: #291C0E; font-size: 11px; font-weight: bold;"));

        formLayout_employeeProfile->setWidget(7, QFormLayout::LabelRole, lblSexe);

        valSexe = new QLabel(employeeInfoSection);
        valSexe->setObjectName("valSexe");
        valSexe->setStyleSheet(QString::fromUtf8("color: #291C0E; font-size: 11px;"));

        formLayout_employeeProfile->setWidget(7, QFormLayout::FieldRole, valSexe);

        lblAdresse = new QLabel(employeeInfoSection);
        lblAdresse->setObjectName("lblAdresse");
        lblAdresse->setStyleSheet(QString::fromUtf8("color: #291C0E; font-size: 11px; font-weight: bold;"));

        formLayout_employeeProfile->setWidget(8, QFormLayout::LabelRole, lblAdresse);

        valAdresse = new QLabel(employeeInfoSection);
        valAdresse->setObjectName("valAdresse");
        valAdresse->setStyleSheet(QString::fromUtf8("color: #291C0E; font-size: 11px;"));
        valAdresse->setWordWrap(true);

        formLayout_employeeProfile->setWidget(8, QFormLayout::FieldRole, valAdresse);

        lblRole = new QLabel(employeeInfoSection);
        lblRole->setObjectName("lblRole");
        lblRole->setStyleSheet(QString::fromUtf8("color: #291C0E; font-size: 11px; font-weight: bold;"));

        formLayout_employeeProfile->setWidget(9, QFormLayout::LabelRole, lblRole);

        valRole = new QLabel(employeeInfoSection);
        valRole->setObjectName("valRole");
        valRole->setStyleSheet(QString::fromUtf8("color: #291C0E; font-size: 11px;"));

        formLayout_employeeProfile->setWidget(9, QFormLayout::FieldRole, valRole);

        lblDateEmbauche = new QLabel(employeeInfoSection);
        lblDateEmbauche->setObjectName("lblDateEmbauche");
        lblDateEmbauche->setStyleSheet(QString::fromUtf8("color: #291C0E; font-size: 11px; font-weight: bold;"));

        formLayout_employeeProfile->setWidget(10, QFormLayout::LabelRole, lblDateEmbauche);

        valDateEmbauche = new QLabel(employeeInfoSection);
        valDateEmbauche->setObjectName("valDateEmbauche");
        valDateEmbauche->setStyleSheet(QString::fromUtf8("color: #291C0E; font-size: 11px;"));

        formLayout_employeeProfile->setWidget(10, QFormLayout::FieldRole, valDateEmbauche);


        verticalLayout_profile->addWidget(employeeInfoSection);

        verticalSpacer_profile = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_profile->addItem(verticalSpacer_profile);


        horizontalLayout_employeeContent->addWidget(employeeProfilePanel);

        employeeTable = new QTableWidget(contentWithProfile);
        if (employeeTable->columnCount() < 10)
            employeeTable->setColumnCount(10);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        employeeTable->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        employeeTable->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        employeeTable->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        employeeTable->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        employeeTable->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        employeeTable->setHorizontalHeaderItem(5, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        employeeTable->setHorizontalHeaderItem(6, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        employeeTable->setHorizontalHeaderItem(7, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        employeeTable->setHorizontalHeaderItem(8, __qtablewidgetitem8);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        employeeTable->setHorizontalHeaderItem(9, __qtablewidgetitem9);
        employeeTable->setObjectName("employeeTable");
        employeeTable->setEnabled(true);
        employeeTable->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
        employeeTable->setAlternatingRowColors(true);
        employeeTable->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
        employeeTable->setShowGrid(true);
        employeeTable->setSortingEnabled(true);
        employeeTable->horizontalHeader()->setStretchLastSection(true);

        horizontalLayout_employeeContent->addWidget(employeeTable);


        verticalLayout_3->addWidget(contentWithProfile);

        stackedWidget->addWidget(employeePage);
        clientPage = new QWidget();
        clientPage->setObjectName("clientPage");
        verticalLayout_8 = new QVBoxLayout(clientPage);
        verticalLayout_8->setSpacing(16);
        verticalLayout_8->setObjectName("verticalLayout_8");
        verticalLayout_8->setContentsMargins(0, 0, 0, 0);
        listTitle = new QLabel(clientPage);
        listTitle->setObjectName("listTitle");

        verticalLayout_8->addWidget(listTitle, 0, Qt::AlignmentFlag::AlignHCenter);

        searchBoxClient = new QLineEdit(clientPage);
        searchBoxClient->setObjectName("searchBoxClient");
        searchBoxClient->setMinimumSize(QSize(400, 0));
        searchBoxClient->setMaximumSize(QSize(400, 16777215));

        verticalLayout_8->addWidget(searchBoxClient);

        clientButtonsWidget = new QWidget(clientPage);
        clientButtonsWidget->setObjectName("clientButtonsWidget");
        clientButtonsWidget->setMinimumSize(QSize(0, 42));
        clientButtonsWidget->setMaximumSize(QSize(16777215, 42));
        horizontalLayout_6 = new QHBoxLayout(clientButtonsWidget);
        horizontalLayout_6->setSpacing(10);
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        horizontalLayout_6->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_4);

        btntrie = new QPushButton(clientButtonsWidget);
        btntrie->setObjectName("btntrie");
        btntrie->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"  background-color: #8D6E63;\n"
"  color: white;\n"
"  border: none;\n"
"  border-radius: 10px;\n"
"  padding: 10px 20px;\n"
"  font-family: Arial, sans-serif;\n"
"  font-size: 12px;\n"
"  font-weight: bold;\n"
"  text-align: center;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"  background-color: #A98273;\n"
"}\n"
""));

        horizontalLayout_6->addWidget(btntrie);

        btnFidelityClassification = new QPushButton(clientButtonsWidget);
        btnFidelityClassification->setObjectName("btnFidelityClassification");
        btnFidelityClassification->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"  background-color: #8D6E63;\n"
"  color: white;\n"
"  border: none;\n"
"  border-radius: 10px;\n"
"  padding: 10px 20px;\n"
"  font-family: Arial, sans-serif;\n"
"  font-size: 12px;\n"
"  font-weight: bold;\n"
"  text-align: center;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"  background-color: #A98273;\n"
"}\n"
""));

        horizontalLayout_6->addWidget(btnFidelityClassification);

        btnStatsByRegion = new QPushButton(clientButtonsWidget);
        btnStatsByRegion->setObjectName("btnStatsByRegion");
        btnStatsByRegion->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"  background-color: #8D6E63;\n"
"  color: white;\n"
"  border: none;\n"
"  border-radius: 10px;\n"
"  padding: 10px 20px;\n"
"  font-family: Arial, sans-serif;\n"
"  font-size: 12px;\n"
"  font-weight: bold;\n"
"  text-align: center;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"  background-color: #A98273;\n"
"}\n"
""));

        horizontalLayout_6->addWidget(btnStatsByRegion);

        btnAddClient = new QPushButton(clientButtonsWidget);
        btnAddClient->setObjectName("btnAddClient");
        btnAddClient->setMinimumSize(QSize(85, 32));

        horizontalLayout_6->addWidget(btnAddClient);

        btnEditClient = new QPushButton(clientButtonsWidget);
        btnEditClient->setObjectName("btnEditClient");
        btnEditClient->setMinimumSize(QSize(85, 32));

        horizontalLayout_6->addWidget(btnEditClient);

        btnDeleteClient = new QPushButton(clientButtonsWidget);
        btnDeleteClient->setObjectName("btnDeleteClient");
        btnDeleteClient->setMinimumSize(QSize(85, 32));

        horizontalLayout_6->addWidget(btnDeleteClient);

        btnRefreshClient = new QPushButton(clientButtonsWidget);
        btnRefreshClient->setObjectName("btnRefreshClient");
        btnRefreshClient->setMinimumSize(QSize(85, 34));

        horizontalLayout_6->addWidget(btnRefreshClient);

        btnhistorique = new QPushButton(clientButtonsWidget);
        btnhistorique->setObjectName("btnhistorique");
        btnhistorique->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"  background-color: #8D6E63;\n"
"  color: white;\n"
"  border: none;\n"
"  border-radius: 10px;\n"
"  padding: 10px 20px;\n"
"  font-family: Arial, sans-serif;\n"
"  font-size: 12px;\n"
"  font-weight: bold;\n"
"  text-align: center;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"  background-color: #A98273;\n"
"}\n"
""));

        horizontalLayout_6->addWidget(btnhistorique);


        verticalLayout_8->addWidget(clientButtonsWidget);

        clientTable = new QTableWidget(clientPage);
        if (clientTable->columnCount() < 8)
            clientTable->setColumnCount(8);
        QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
        clientTable->setHorizontalHeaderItem(0, __qtablewidgetitem10);
        QTableWidgetItem *__qtablewidgetitem11 = new QTableWidgetItem();
        clientTable->setHorizontalHeaderItem(1, __qtablewidgetitem11);
        QTableWidgetItem *__qtablewidgetitem12 = new QTableWidgetItem();
        clientTable->setHorizontalHeaderItem(2, __qtablewidgetitem12);
        QTableWidgetItem *__qtablewidgetitem13 = new QTableWidgetItem();
        clientTable->setHorizontalHeaderItem(3, __qtablewidgetitem13);
        QTableWidgetItem *__qtablewidgetitem14 = new QTableWidgetItem();
        clientTable->setHorizontalHeaderItem(4, __qtablewidgetitem14);
        QTableWidgetItem *__qtablewidgetitem15 = new QTableWidgetItem();
        clientTable->setHorizontalHeaderItem(5, __qtablewidgetitem15);
        QTableWidgetItem *__qtablewidgetitem16 = new QTableWidgetItem();
        clientTable->setHorizontalHeaderItem(6, __qtablewidgetitem16);
        QTableWidgetItem *__qtablewidgetitem17 = new QTableWidgetItem();
        clientTable->setHorizontalHeaderItem(7, __qtablewidgetitem17);
        clientTable->setObjectName("clientTable");
        clientTable->setEnabled(true);
        clientTable->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
        clientTable->setAlternatingRowColors(true);
        clientTable->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
        clientTable->setShowGrid(true);
        clientTable->setSortingEnabled(true);
        clientTable->horizontalHeader()->setStretchLastSection(true);

        verticalLayout_8->addWidget(clientTable);

        stackedWidget->addWidget(clientPage);
        rawMaterialsPage = new QWidget();
        rawMaterialsPage->setObjectName("rawMaterialsPage");
        verticalLayout_9 = new QVBoxLayout(rawMaterialsPage);
        verticalLayout_9->setSpacing(16);
        verticalLayout_9->setObjectName("verticalLayout_9");
        verticalLayout_9->setContentsMargins(0, 0, 0, 0);
        matiereTitle = new QLabel(rawMaterialsPage);
        matiereTitle->setObjectName("matiereTitle");

        verticalLayout_9->addWidget(matiereTitle, 0, Qt::AlignmentFlag::AlignHCenter);

        statsContainerMatiere = new QWidget(rawMaterialsPage);
        statsContainerMatiere->setObjectName("statsContainerMatiere");
        statsContainerMatiere->setMinimumSize(QSize(0, 65));
        statsContainerMatiere->setMaximumSize(QSize(16777215, 65));
        horizontalLayout_8 = new QHBoxLayout(statsContainerMatiere);
        horizontalLayout_8->setSpacing(15);
        horizontalLayout_8->setObjectName("horizontalLayout_8");
        horizontalLayout_8->setContentsMargins(0, 0, 0, 0);
        statsCardMatiere1 = new QFrame(statsContainerMatiere);
        statsCardMatiere1->setObjectName("statsCardMatiere1");
        statsCardMatiere1->setFrameShape(QFrame::Shape::StyledPanel);
        verticalLayout_10 = new QVBoxLayout(statsCardMatiere1);
        verticalLayout_10->setObjectName("verticalLayout_10");
        statsLabelMatiere1 = new QLabel(statsCardMatiere1);
        statsLabelMatiere1->setObjectName("statsLabelMatiere1");
        statsLabelMatiere1->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_10->addWidget(statsLabelMatiere1);

        statsValueMatiere1 = new QLabel(statsCardMatiere1);
        statsValueMatiere1->setObjectName("statsValueMatiere1");
        statsValueMatiere1->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_10->addWidget(statsValueMatiere1);


        horizontalLayout_8->addWidget(statsCardMatiere1);

        statsCardMatiere2 = new QFrame(statsContainerMatiere);
        statsCardMatiere2->setObjectName("statsCardMatiere2");
        statsCardMatiere2->setFrameShape(QFrame::Shape::StyledPanel);
        verticalLayout_11 = new QVBoxLayout(statsCardMatiere2);
        verticalLayout_11->setObjectName("verticalLayout_11");
        statsLabelMatiere2 = new QLabel(statsCardMatiere2);
        statsLabelMatiere2->setObjectName("statsLabelMatiere2");
        statsLabelMatiere2->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_11->addWidget(statsLabelMatiere2);

        statsValueMatiere2 = new QLabel(statsCardMatiere2);
        statsValueMatiere2->setObjectName("statsValueMatiere2");
        statsValueMatiere2->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_11->addWidget(statsValueMatiere2);


        horizontalLayout_8->addWidget(statsCardMatiere2);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_8->addItem(horizontalSpacer_6);


        verticalLayout_9->addWidget(statsContainerMatiere);

        searchWidget = new QWidget(rawMaterialsPage);
        searchWidget->setObjectName("searchWidget");
        searchWidget->setMinimumSize(QSize(0, 42));
        searchLayout = new QHBoxLayout(searchWidget);
        searchLayout->setSpacing(10);
        searchLayout->setObjectName("searchLayout");
        searchLayout->setContentsMargins(0, 0, 0, 0);
        searchBoxMatiere = new QLineEdit(searchWidget);
        searchBoxMatiere->setObjectName("searchBoxMatiere");
        searchBoxMatiere->setMinimumSize(QSize(400, 32));
        searchBoxMatiere->setMaximumSize(QSize(400, 32));

        searchLayout->addWidget(searchBoxMatiere);

        btnRecherche = new QPushButton(searchWidget);
        btnRecherche->setObjectName("btnRecherche");
        btnRecherche->setMinimumSize(QSize(100, 32));

        searchLayout->addWidget(btnRecherche);

        searchSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        searchLayout->addItem(searchSpacer);


        verticalLayout_9->addWidget(searchWidget);

        matiereButtonsWidget = new QWidget(rawMaterialsPage);
        matiereButtonsWidget->setObjectName("matiereButtonsWidget");
        matiereButtonsWidget->setMinimumSize(QSize(0, 42));
        matiereButtonsWidget->setMaximumSize(QSize(16777215, 42));
        horizontalLayout_7 = new QHBoxLayout(matiereButtonsWidget);
        horizontalLayout_7->setSpacing(10);
        horizontalLayout_7->setObjectName("horizontalLayout_7");
        horizontalLayout_7->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer_5);

        btnAddMatiere = new QPushButton(matiereButtonsWidget);
        btnAddMatiere->setObjectName("btnAddMatiere");
        btnAddMatiere->setMinimumSize(QSize(120, 32));

        horizontalLayout_7->addWidget(btnAddMatiere);

        btnEditMatiere = new QPushButton(matiereButtonsWidget);
        btnEditMatiere->setObjectName("btnEditMatiere");
        btnEditMatiere->setMinimumSize(QSize(120, 32));

        horizontalLayout_7->addWidget(btnEditMatiere);

        btnDeleteMatiere = new QPushButton(matiereButtonsWidget);
        btnDeleteMatiere->setObjectName("btnDeleteMatiere");
        btnDeleteMatiere->setMinimumSize(QSize(120, 32));

        horizontalLayout_7->addWidget(btnDeleteMatiere);

        btnStatistiques = new QPushButton(matiereButtonsWidget);
        btnStatistiques->setObjectName("btnStatistiques");
        btnStatistiques->setMinimumSize(QSize(120, 32));

        horizontalLayout_7->addWidget(btnStatistiques);

        btnTri = new QPushButton(matiereButtonsWidget);
        btnTri->setObjectName("btnTri");
        btnTri->setMinimumSize(QSize(100, 32));

        horizontalLayout_7->addWidget(btnTri);

        btnDetection = new QPushButton(matiereButtonsWidget);
        btnDetection->setObjectName("btnDetection");
        btnDetection->setMinimumSize(QSize(120, 32));

        horizontalLayout_7->addWidget(btnDetection);

        btnVoice = new QPushButton(matiereButtonsWidget);
        btnVoice->setObjectName("btnVoice");
        btnVoice->setMinimumSize(QSize(100, 32));

        horizontalLayout_7->addWidget(btnVoice);

        btnExportMatiere = new QPushButton(matiereButtonsWidget);
        btnExportMatiere->setObjectName("btnExportMatiere");
        btnExportMatiere->setMinimumSize(QSize(100, 32));

        horizontalLayout_7->addWidget(btnExportMatiere);


        verticalLayout_9->addWidget(matiereButtonsWidget);

        matiereTable = new QTableWidget(rawMaterialsPage);
        if (matiereTable->columnCount() < 6)
            matiereTable->setColumnCount(6);
        QTableWidgetItem *__qtablewidgetitem18 = new QTableWidgetItem();
        matiereTable->setHorizontalHeaderItem(0, __qtablewidgetitem18);
        QTableWidgetItem *__qtablewidgetitem19 = new QTableWidgetItem();
        matiereTable->setHorizontalHeaderItem(1, __qtablewidgetitem19);
        QTableWidgetItem *__qtablewidgetitem20 = new QTableWidgetItem();
        matiereTable->setHorizontalHeaderItem(2, __qtablewidgetitem20);
        QTableWidgetItem *__qtablewidgetitem21 = new QTableWidgetItem();
        matiereTable->setHorizontalHeaderItem(3, __qtablewidgetitem21);
        QTableWidgetItem *__qtablewidgetitem22 = new QTableWidgetItem();
        matiereTable->setHorizontalHeaderItem(4, __qtablewidgetitem22);
        QTableWidgetItem *__qtablewidgetitem23 = new QTableWidgetItem();
        matiereTable->setHorizontalHeaderItem(5, __qtablewidgetitem23);
        matiereTable->setObjectName("matiereTable");
        matiereTable->setEnabled(true);
        matiereTable->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
        matiereTable->setAlternatingRowColors(true);
        matiereTable->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
        matiereTable->setShowGrid(true);
        matiereTable->setSortingEnabled(true);
        matiereTable->horizontalHeader()->setStretchLastSection(true);

        verticalLayout_9->addWidget(matiereTable);

        stackedWidget->addWidget(rawMaterialsPage);
        suppliersPage = new QWidget();
        suppliersPage->setObjectName("suppliersPage");
        verticalLayout_15 = new QVBoxLayout(suppliersPage);
        verticalLayout_15->setSpacing(16);
        verticalLayout_15->setObjectName("verticalLayout_15");
        verticalLayout_15->setContentsMargins(0, 0, 0, 0);
        fournisseursTitle = new QLabel(suppliersPage);
        fournisseursTitle->setObjectName("fournisseursTitle");
        fournisseursTitle->setStyleSheet(QString::fromUtf8("color: #291C0E; font-family: 'Times New Roman', serif; font-size: 22px; font-weight: bold;"));

        verticalLayout_15->addWidget(fournisseursTitle, 0, Qt::AlignmentFlag::AlignHCenter);

        statsContainerFournisseurs = new QWidget(suppliersPage);
        statsContainerFournisseurs->setObjectName("statsContainerFournisseurs");
        statsContainerFournisseurs->setMinimumSize(QSize(0, 65));
        statsContainerFournisseurs->setMaximumSize(QSize(16777215, 65));
        horizontalLayout_9 = new QHBoxLayout(statsContainerFournisseurs);
        horizontalLayout_9->setSpacing(15);
        horizontalLayout_9->setObjectName("horizontalLayout_9");
        horizontalLayout_9->setContentsMargins(0, 0, 0, 0);
        statsCardFournisseur1 = new QFrame(statsContainerFournisseurs);
        statsCardFournisseur1->setObjectName("statsCardFournisseur1");
        statsCardFournisseur1->setFrameShape(QFrame::Shape::StyledPanel);
        verticalLayout_16 = new QVBoxLayout(statsCardFournisseur1);
        verticalLayout_16->setSpacing(5);
        verticalLayout_16->setObjectName("verticalLayout_16");
        statsLabelFournisseur1 = new QLabel(statsCardFournisseur1);
        statsLabelFournisseur1->setObjectName("statsLabelFournisseur1");
        statsLabelFournisseur1->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_16->addWidget(statsLabelFournisseur1);

        statsValueFournisseur1 = new QLabel(statsCardFournisseur1);
        statsValueFournisseur1->setObjectName("statsValueFournisseur1");
        statsValueFournisseur1->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_16->addWidget(statsValueFournisseur1);


        horizontalLayout_9->addWidget(statsCardFournisseur1);

        statsCardFournisseur2 = new QFrame(statsContainerFournisseurs);
        statsCardFournisseur2->setObjectName("statsCardFournisseur2");
        statsCardFournisseur2->setFrameShape(QFrame::Shape::StyledPanel);
        verticalLayout_17 = new QVBoxLayout(statsCardFournisseur2);
        verticalLayout_17->setSpacing(5);
        verticalLayout_17->setObjectName("verticalLayout_17");
        statsLabelFournisseur2 = new QLabel(statsCardFournisseur2);
        statsLabelFournisseur2->setObjectName("statsLabelFournisseur2");
        statsLabelFournisseur2->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_17->addWidget(statsLabelFournisseur2);

        statsValueFournisseur2 = new QLabel(statsCardFournisseur2);
        statsValueFournisseur2->setObjectName("statsValueFournisseur2");
        statsValueFournisseur2->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_17->addWidget(statsValueFournisseur2);


        horizontalLayout_9->addWidget(statsCardFournisseur2);

        statsCardFournisseur3 = new QFrame(statsContainerFournisseurs);
        statsCardFournisseur3->setObjectName("statsCardFournisseur3");
        statsCardFournisseur3->setFrameShape(QFrame::Shape::StyledPanel);
        verticalLayout_18 = new QVBoxLayout(statsCardFournisseur3);
        verticalLayout_18->setSpacing(5);
        verticalLayout_18->setObjectName("verticalLayout_18");
        statsLabelFournisseur3 = new QLabel(statsCardFournisseur3);
        statsLabelFournisseur3->setObjectName("statsLabelFournisseur3");
        statsLabelFournisseur3->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_18->addWidget(statsLabelFournisseur3);

        statsValueFournisseur3 = new QLabel(statsCardFournisseur3);
        statsValueFournisseur3->setObjectName("statsValueFournisseur3");
        statsValueFournisseur3->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_18->addWidget(statsValueFournisseur3);


        horizontalLayout_9->addWidget(statsCardFournisseur3);

        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_9->addItem(horizontalSpacer_7);


        verticalLayout_15->addWidget(statsContainerFournisseurs);

        searchBoxFournisseur = new QLineEdit(suppliersPage);
        searchBoxFournisseur->setObjectName("searchBoxFournisseur");
        searchBoxFournisseur->setMinimumSize(QSize(400, 0));
        searchBoxFournisseur->setMaximumSize(QSize(400, 16777215));

        verticalLayout_15->addWidget(searchBoxFournisseur);

        fournisseurButtonsWidget = new QWidget(suppliersPage);
        fournisseurButtonsWidget->setObjectName("fournisseurButtonsWidget");
        fournisseurButtonsWidget->setMinimumSize(QSize(0, 42));
        fournisseurButtonsWidget->setMaximumSize(QSize(16777215, 42));
        horizontalLayout_10 = new QHBoxLayout(fournisseurButtonsWidget);
        horizontalLayout_10->setSpacing(10);
        horizontalLayout_10->setObjectName("horizontalLayout_10");
        horizontalLayout_10->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_10->addItem(horizontalSpacer_8);

        btnAddFournisseur = new QPushButton(fournisseurButtonsWidget);
        btnAddFournisseur->setObjectName("btnAddFournisseur");
        btnAddFournisseur->setMinimumSize(QSize(85, 32));

        horizontalLayout_10->addWidget(btnAddFournisseur);

        btnEditFournisseur = new QPushButton(fournisseurButtonsWidget);
        btnEditFournisseur->setObjectName("btnEditFournisseur");
        btnEditFournisseur->setMinimumSize(QSize(85, 32));

        horizontalLayout_10->addWidget(btnEditFournisseur);

        btnDeleteFournisseur = new QPushButton(fournisseurButtonsWidget);
        btnDeleteFournisseur->setObjectName("btnDeleteFournisseur");
        btnDeleteFournisseur->setMinimumSize(QSize(85, 32));

        horizontalLayout_10->addWidget(btnDeleteFournisseur);

        btnmap = new QPushButton(fournisseurButtonsWidget);
        btnmap->setObjectName("btnmap");
        btnmap->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"  background-color: #8D6E63;\n"
"  color: white;\n"
"  border: none;\n"
"  border-radius: 10px;\n"
"  padding: 10px 20px;\n"
"  font-family: Arial, sans-serif;\n"
"  font-size: 12px;\n"
"  font-weight: bold;\n"
"  text-align: center;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"  background-color: #A98273;\n"
"}\n"
""));

        horizontalLayout_10->addWidget(btnmap);

        btnExportFournisseur = new QPushButton(fournisseurButtonsWidget);
        btnExportFournisseur->setObjectName("btnExportFournisseur");
        btnExportFournisseur->setMinimumSize(QSize(85, 32));

        horizontalLayout_10->addWidget(btnExportFournisseur);

        btnStatsFournisseur = new QPushButton(fournisseurButtonsWidget);
        btnStatsFournisseur->setObjectName("btnStatsFournisseur");
        btnStatsFournisseur->setMinimumSize(QSize(100, 32));

        horizontalLayout_10->addWidget(btnStatsFournisseur);

        btnTriFournisseur = new QPushButton(fournisseurButtonsWidget);
        btnTriFournisseur->setObjectName("btnTriFournisseur");
        btnTriFournisseur->setMinimumSize(QSize(85, 32));

        horizontalLayout_10->addWidget(btnTriFournisseur);

        btnSmsFournisseur = new QPushButton(fournisseurButtonsWidget);
        btnSmsFournisseur->setObjectName("btnSmsFournisseur");
        btnSmsFournisseur->setMinimumSize(QSize(100, 32));

        horizontalLayout_10->addWidget(btnSmsFournisseur);


        verticalLayout_15->addWidget(fournisseurButtonsWidget);

        fournisseurTable = new QTableWidget(suppliersPage);
        if (fournisseurTable->columnCount() < 9)
            fournisseurTable->setColumnCount(9);
        QTableWidgetItem *__qtablewidgetitem24 = new QTableWidgetItem();
        fournisseurTable->setHorizontalHeaderItem(0, __qtablewidgetitem24);
        QTableWidgetItem *__qtablewidgetitem25 = new QTableWidgetItem();
        fournisseurTable->setHorizontalHeaderItem(1, __qtablewidgetitem25);
        QTableWidgetItem *__qtablewidgetitem26 = new QTableWidgetItem();
        fournisseurTable->setHorizontalHeaderItem(2, __qtablewidgetitem26);
        QTableWidgetItem *__qtablewidgetitem27 = new QTableWidgetItem();
        fournisseurTable->setHorizontalHeaderItem(3, __qtablewidgetitem27);
        QTableWidgetItem *__qtablewidgetitem28 = new QTableWidgetItem();
        fournisseurTable->setHorizontalHeaderItem(4, __qtablewidgetitem28);
        QTableWidgetItem *__qtablewidgetitem29 = new QTableWidgetItem();
        fournisseurTable->setHorizontalHeaderItem(5, __qtablewidgetitem29);
        QTableWidgetItem *__qtablewidgetitem30 = new QTableWidgetItem();
        fournisseurTable->setHorizontalHeaderItem(6, __qtablewidgetitem30);
        QTableWidgetItem *__qtablewidgetitem31 = new QTableWidgetItem();
        fournisseurTable->setHorizontalHeaderItem(7, __qtablewidgetitem31);
        QTableWidgetItem *__qtablewidgetitem32 = new QTableWidgetItem();
        fournisseurTable->setHorizontalHeaderItem(8, __qtablewidgetitem32);
        fournisseurTable->setObjectName("fournisseurTable");
        fournisseurTable->setEnabled(true);
        fournisseurTable->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
        fournisseurTable->setAlternatingRowColors(true);
        fournisseurTable->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
        fournisseurTable->setShowGrid(true);
        fournisseurTable->setSortingEnabled(true);
        fournisseurTable->horizontalHeader()->setStretchLastSection(true);

        verticalLayout_15->addWidget(fournisseurTable);

        stackedWidget->addWidget(suppliersPage);
        productionPage = new QWidget();
        productionPage->setObjectName("productionPage");
        verticalLayout_production = new QVBoxLayout(productionPage);
        verticalLayout_production->setSpacing(16);
        verticalLayout_production->setObjectName("verticalLayout_production");
        verticalLayout_production->setContentsMargins(0, 0, 0, 0);
        productionTitle = new QLabel(productionPage);
        productionTitle->setObjectName("productionTitle");
        productionTitle->setStyleSheet(QString::fromUtf8("color: #291C0E; font-family: 'Times New Roman', serif; font-size: 22px; font-weight: bold;"));

        verticalLayout_production->addWidget(productionTitle, 0, Qt::AlignmentFlag::AlignHCenter);

        productionStatsContainer = new QWidget(productionPage);
        productionStatsContainer->setObjectName("productionStatsContainer");
        productionStatsContainer->setMinimumSize(QSize(0, 65));
        productionStatsContainer->setMaximumSize(QSize(16777215, 65));
        horizontalLayout_productionStats = new QHBoxLayout(productionStatsContainer);
        horizontalLayout_productionStats->setSpacing(15);
        horizontalLayout_productionStats->setObjectName("horizontalLayout_productionStats");
        horizontalLayout_productionStats->setContentsMargins(0, 0, 0, 0);
        statsCardProduction1 = new QFrame(productionStatsContainer);
        statsCardProduction1->setObjectName("statsCardProduction1");
        statsCardProduction1->setFrameShape(QFrame::Shape::StyledPanel);
        verticalLayout_statsProduction1 = new QVBoxLayout(statsCardProduction1);
        verticalLayout_statsProduction1->setObjectName("verticalLayout_statsProduction1");
        statsLabelProduction1 = new QLabel(statsCardProduction1);
        statsLabelProduction1->setObjectName("statsLabelProduction1");
        statsLabelProduction1->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_statsProduction1->addWidget(statsLabelProduction1);

        statsValueProduction1 = new QLabel(statsCardProduction1);
        statsValueProduction1->setObjectName("statsValueProduction1");
        statsValueProduction1->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_statsProduction1->addWidget(statsValueProduction1);


        horizontalLayout_productionStats->addWidget(statsCardProduction1);

        statsCardProduction2 = new QFrame(productionStatsContainer);
        statsCardProduction2->setObjectName("statsCardProduction2");
        statsCardProduction2->setFrameShape(QFrame::Shape::StyledPanel);
        verticalLayout_statsProduction2 = new QVBoxLayout(statsCardProduction2);
        verticalLayout_statsProduction2->setObjectName("verticalLayout_statsProduction2");
        statsLabelProduction2 = new QLabel(statsCardProduction2);
        statsLabelProduction2->setObjectName("statsLabelProduction2");
        statsLabelProduction2->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_statsProduction2->addWidget(statsLabelProduction2);

        statsValueProduction2 = new QLabel(statsCardProduction2);
        statsValueProduction2->setObjectName("statsValueProduction2");
        statsValueProduction2->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_statsProduction2->addWidget(statsValueProduction2);


        horizontalLayout_productionStats->addWidget(statsCardProduction2);

        statsCardProduction3 = new QFrame(productionStatsContainer);
        statsCardProduction3->setObjectName("statsCardProduction3");
        statsCardProduction3->setFrameShape(QFrame::Shape::StyledPanel);
        verticalLayout_statsProduction3 = new QVBoxLayout(statsCardProduction3);
        verticalLayout_statsProduction3->setObjectName("verticalLayout_statsProduction3");
        statsLabelProduction3 = new QLabel(statsCardProduction3);
        statsLabelProduction3->setObjectName("statsLabelProduction3");
        statsLabelProduction3->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_statsProduction3->addWidget(statsLabelProduction3);

        statsValueProduction3 = new QLabel(statsCardProduction3);
        statsValueProduction3->setObjectName("statsValueProduction3");
        statsValueProduction3->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_statsProduction3->addWidget(statsValueProduction3);


        horizontalLayout_productionStats->addWidget(statsCardProduction3);

        statsCardProduction4 = new QFrame(productionStatsContainer);
        statsCardProduction4->setObjectName("statsCardProduction4");
        statsCardProduction4->setFrameShape(QFrame::Shape::StyledPanel);
        verticalLayout_statsProduction4 = new QVBoxLayout(statsCardProduction4);
        verticalLayout_statsProduction4->setObjectName("verticalLayout_statsProduction4");
        statsLabelProduction4 = new QLabel(statsCardProduction4);
        statsLabelProduction4->setObjectName("statsLabelProduction4");
        statsLabelProduction4->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_statsProduction4->addWidget(statsLabelProduction4);

        statsValueProduction4 = new QLabel(statsCardProduction4);
        statsValueProduction4->setObjectName("statsValueProduction4");
        statsValueProduction4->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_statsProduction4->addWidget(statsValueProduction4);


        horizontalLayout_productionStats->addWidget(statsCardProduction4);

        horizontalSpacer_statsProduction = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_productionStats->addItem(horizontalSpacer_statsProduction);


        verticalLayout_production->addWidget(productionStatsContainer);

        productionToolbar = new QWidget(productionPage);
        productionToolbar->setObjectName("productionToolbar");
        productionToolbar->setMinimumSize(QSize(0, 42));
        productionToolbar->setMaximumSize(QSize(16777215, 42));
        horizontalLayout_production = new QHBoxLayout(productionToolbar);
        horizontalLayout_production->setSpacing(10);
        horizontalLayout_production->setObjectName("horizontalLayout_production");
        horizontalLayout_production->setContentsMargins(0, 0, 0, 0);
        searchBoxProduction = new QLineEdit(productionToolbar);
        searchBoxProduction->setObjectName("searchBoxProduction");
        searchBoxProduction->setMinimumSize(QSize(300, 0));

        horizontalLayout_production->addWidget(searchBoxProduction);

        horizontalSpacer_production = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_production->addItem(horizontalSpacer_production);

        btnExportClient = new QPushButton(productionToolbar);
        btnExportClient->setObjectName("btnExportClient");

        horizontalLayout_production->addWidget(btnExportClient);

        btnCreerProduction = new QPushButton(productionToolbar);
        btnCreerProduction->setObjectName("btnCreerProduction");
        btnCreerProduction->setMinimumSize(QSize(100, 32));

        horizontalLayout_production->addWidget(btnCreerProduction);

        btnModifierProduction = new QPushButton(productionToolbar);
        btnModifierProduction->setObjectName("btnModifierProduction");
        btnModifierProduction->setMinimumSize(QSize(100, 32));

        horizontalLayout_production->addWidget(btnModifierProduction);

        btnSupprimerProduction = new QPushButton(productionToolbar);
        btnSupprimerProduction->setObjectName("btnSupprimerProduction");
        btnSupprimerProduction->setMinimumSize(QSize(110, 32));

        horizontalLayout_production->addWidget(btnSupprimerProduction);

        btnFactureProduction = new QPushButton(productionToolbar);
        btnFactureProduction->setObjectName("btnFactureProduction");
        btnFactureProduction->setMinimumSize(QSize(100, 32));

        horizontalLayout_production->addWidget(btnFactureProduction);

        btnExcelProduction = new QPushButton(productionToolbar);
        btnExcelProduction->setObjectName("btnExcelProduction");
        btnExcelProduction->setMinimumSize(QSize(80, 32));

        horizontalLayout_production->addWidget(btnExcelProduction);

        btnTrierProduction = new QPushButton(productionToolbar);
        btnTrierProduction->setObjectName("btnTrierProduction");
        btnTrierProduction->setMinimumSize(QSize(80, 32));

        horizontalLayout_production->addWidget(btnTrierProduction);

        btnStatistiquesProduction = new QPushButton(productionToolbar);
        btnStatistiquesProduction->setObjectName("btnStatistiquesProduction");
        btnStatistiquesProduction->setMinimumSize(QSize(120, 32));

        horizontalLayout_production->addWidget(btnStatistiquesProduction);

        btnExpedier = new QPushButton(productionToolbar);
        btnExpedier->setObjectName("btnExpedier");
        btnExpedier->setMinimumSize(QSize(120, 32));

        horizontalLayout_production->addWidget(btnExpedier);


        verticalLayout_production->addWidget(productionToolbar);

        productionTable = new QTableWidget(productionPage);
        if (productionTable->columnCount() < 11)
            productionTable->setColumnCount(11);
        QTableWidgetItem *__qtablewidgetitem33 = new QTableWidgetItem();
        productionTable->setHorizontalHeaderItem(0, __qtablewidgetitem33);
        QTableWidgetItem *__qtablewidgetitem34 = new QTableWidgetItem();
        productionTable->setHorizontalHeaderItem(1, __qtablewidgetitem34);
        QTableWidgetItem *__qtablewidgetitem35 = new QTableWidgetItem();
        productionTable->setHorizontalHeaderItem(2, __qtablewidgetitem35);
        QTableWidgetItem *__qtablewidgetitem36 = new QTableWidgetItem();
        productionTable->setHorizontalHeaderItem(3, __qtablewidgetitem36);
        QTableWidgetItem *__qtablewidgetitem37 = new QTableWidgetItem();
        productionTable->setHorizontalHeaderItem(4, __qtablewidgetitem37);
        QTableWidgetItem *__qtablewidgetitem38 = new QTableWidgetItem();
        productionTable->setHorizontalHeaderItem(5, __qtablewidgetitem38);
        QTableWidgetItem *__qtablewidgetitem39 = new QTableWidgetItem();
        productionTable->setHorizontalHeaderItem(6, __qtablewidgetitem39);
        QTableWidgetItem *__qtablewidgetitem40 = new QTableWidgetItem();
        productionTable->setHorizontalHeaderItem(7, __qtablewidgetitem40);
        QTableWidgetItem *__qtablewidgetitem41 = new QTableWidgetItem();
        productionTable->setHorizontalHeaderItem(8, __qtablewidgetitem41);
        QTableWidgetItem *__qtablewidgetitem42 = new QTableWidgetItem();
        productionTable->setHorizontalHeaderItem(9, __qtablewidgetitem42);
        QTableWidgetItem *__qtablewidgetitem43 = new QTableWidgetItem();
        productionTable->setHorizontalHeaderItem(10, __qtablewidgetitem43);
        productionTable->setObjectName("productionTable");
        productionTable->setEnabled(true);
        productionTable->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
        productionTable->setAlternatingRowColors(true);
        productionTable->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
        productionTable->setShowGrid(true);
        productionTable->setSortingEnabled(false);
        productionTable->horizontalHeader()->setStretchLastSection(true);

        verticalLayout_production->addWidget(productionTable);

        stackedWidget->addWidget(productionPage);
        articlesPage = new QWidget();
        articlesPage->setObjectName("articlesPage");
        verticalLayout_articles = new QVBoxLayout(articlesPage);
        verticalLayout_articles->setSpacing(16);
        verticalLayout_articles->setObjectName("verticalLayout_articles");
        verticalLayout_articles->setContentsMargins(0, 0, 0, 0);
        articlesTitle = new QLabel(articlesPage);
        articlesTitle->setObjectName("articlesTitle");
        articlesTitle->setStyleSheet(QString::fromUtf8("color: #291C0E; font-family: 'Times New Roman', serif; font-size: 22px; font-weight: bold;"));

        verticalLayout_articles->addWidget(articlesTitle, 0, Qt::AlignmentFlag::AlignHCenter);

        statsContainerArticle = new QWidget(articlesPage);
        statsContainerArticle->setObjectName("statsContainerArticle");
        statsContainerArticle->setMinimumSize(QSize(0, 65));
        statsContainerArticle->setMaximumSize(QSize(16777215, 65));
        horizontalLayout_statsArticle = new QHBoxLayout(statsContainerArticle);
        horizontalLayout_statsArticle->setSpacing(15);
        horizontalLayout_statsArticle->setObjectName("horizontalLayout_statsArticle");
        horizontalLayout_statsArticle->setContentsMargins(0, 0, 0, 0);
        statsCardArticle1 = new QFrame(statsContainerArticle);
        statsCardArticle1->setObjectName("statsCardArticle1");
        statsCardArticle1->setFrameShape(QFrame::Shape::StyledPanel);
        verticalLayout_statsArticle1 = new QVBoxLayout(statsCardArticle1);
        verticalLayout_statsArticle1->setObjectName("verticalLayout_statsArticle1");
        statsLabelArticle1 = new QLabel(statsCardArticle1);
        statsLabelArticle1->setObjectName("statsLabelArticle1");
        statsLabelArticle1->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_statsArticle1->addWidget(statsLabelArticle1);

        statsValueArticle1 = new QLabel(statsCardArticle1);
        statsValueArticle1->setObjectName("statsValueArticle1");
        statsValueArticle1->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_statsArticle1->addWidget(statsValueArticle1);


        horizontalLayout_statsArticle->addWidget(statsCardArticle1);

        statsCardArticle2 = new QFrame(statsContainerArticle);
        statsCardArticle2->setObjectName("statsCardArticle2");
        statsCardArticle2->setFrameShape(QFrame::Shape::StyledPanel);
        verticalLayout_statsArticle2 = new QVBoxLayout(statsCardArticle2);
        verticalLayout_statsArticle2->setObjectName("verticalLayout_statsArticle2");
        statsLabelArticle2 = new QLabel(statsCardArticle2);
        statsLabelArticle2->setObjectName("statsLabelArticle2");
        statsLabelArticle2->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_statsArticle2->addWidget(statsLabelArticle2);

        statsValueArticle2 = new QLabel(statsCardArticle2);
        statsValueArticle2->setObjectName("statsValueArticle2");
        statsValueArticle2->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_statsArticle2->addWidget(statsValueArticle2);


        horizontalLayout_statsArticle->addWidget(statsCardArticle2);

        statsCardArticle3 = new QFrame(statsContainerArticle);
        statsCardArticle3->setObjectName("statsCardArticle3");
        statsCardArticle3->setFrameShape(QFrame::Shape::StyledPanel);
        verticalLayout_statsArticle3 = new QVBoxLayout(statsCardArticle3);
        verticalLayout_statsArticle3->setObjectName("verticalLayout_statsArticle3");
        statsLabelArticle3 = new QLabel(statsCardArticle3);
        statsLabelArticle3->setObjectName("statsLabelArticle3");
        statsLabelArticle3->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_statsArticle3->addWidget(statsLabelArticle3);

        statsValueArticle3 = new QLabel(statsCardArticle3);
        statsValueArticle3->setObjectName("statsValueArticle3");
        statsValueArticle3->setAlignment(Qt::AlignmentFlag::AlignCenter);

        verticalLayout_statsArticle3->addWidget(statsValueArticle3);


        horizontalLayout_statsArticle->addWidget(statsCardArticle3);

        horizontalSpacer_statsArticle = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_statsArticle->addItem(horizontalSpacer_statsArticle);


        verticalLayout_articles->addWidget(statsContainerArticle);

        articlesToolbar = new QWidget(articlesPage);
        articlesToolbar->setObjectName("articlesToolbar");
        articlesToolbar->setMinimumSize(QSize(0, 80));
        articlesToolbar->setMaximumSize(QSize(16777215, 90));
        verticalLayout_articlesToolbar = new QVBoxLayout(articlesToolbar);
        verticalLayout_articlesToolbar->setSpacing(4);
        verticalLayout_articlesToolbar->setObjectName("verticalLayout_articlesToolbar");
        verticalLayout_articlesToolbar->setContentsMargins(0, 0, 0, 0);
        articlesRow1 = new QHBoxLayout();
        articlesRow1->setSpacing(6);
        articlesRow1->setObjectName("articlesRow1");
        searchBoxArticle = new QLineEdit(articlesToolbar);
        searchBoxArticle->setObjectName("searchBoxArticle");
        searchBoxArticle->setMinimumSize(QSize(150, 0));
        searchBoxArticle->setMaximumSize(QSize(250, 16777215));

        articlesRow1->addWidget(searchBoxArticle);

        btnAddArticle = new QPushButton(articlesToolbar);
        btnAddArticle->setObjectName("btnAddArticle");
        btnAddArticle->setMinimumSize(QSize(0, 28));

        articlesRow1->addWidget(btnAddArticle);

        btnEditArticle = new QPushButton(articlesToolbar);
        btnEditArticle->setObjectName("btnEditArticle");
        btnEditArticle->setMinimumSize(QSize(0, 28));

        articlesRow1->addWidget(btnEditArticle);

        btnDeleteArticle = new QPushButton(articlesToolbar);
        btnDeleteArticle->setObjectName("btnDeleteArticle");
        btnDeleteArticle->setMinimumSize(QSize(0, 28));

        articlesRow1->addWidget(btnDeleteArticle);

        btnViewArticle = new QPushButton(articlesToolbar);
        btnViewArticle->setObjectName("btnViewArticle");
        btnViewArticle->setMinimumSize(QSize(0, 28));

        articlesRow1->addWidget(btnViewArticle);

        btnView3DArticle = new QPushButton(articlesToolbar);
        btnView3DArticle->setObjectName("btnView3DArticle");
        btnView3DArticle->setMinimumSize(QSize(0, 28));

        articlesRow1->addWidget(btnView3DArticle);

        btnTriArticle = new QPushButton(articlesToolbar);
        btnTriArticle->setObjectName("btnTriArticle");
        btnTriArticle->setMinimumSize(QSize(0, 28));

        articlesRow1->addWidget(btnTriArticle);

        btnExportPdfArticle = new QPushButton(articlesToolbar);
        btnExportPdfArticle->setObjectName("btnExportPdfArticle");
        btnExportPdfArticle->setMinimumSize(QSize(0, 28));

        articlesRow1->addWidget(btnExportPdfArticle);

        btnStatistiquesArticle = new QPushButton(articlesToolbar);
        btnStatistiquesArticle->setObjectName("btnStatistiquesArticle");
        btnStatistiquesArticle->setMinimumSize(QSize(0, 28));

        articlesRow1->addWidget(btnStatistiquesArticle);

        btnAideDecision = new QPushButton(articlesToolbar);
        btnAideDecision->setObjectName("btnAideDecision");
        btnAideDecision->setMinimumSize(QSize(0, 28));

        articlesRow1->addWidget(btnAideDecision);


        verticalLayout_articlesToolbar->addLayout(articlesRow1);


        verticalLayout_articles->addWidget(articlesToolbar);

        articleTable = new QTableWidget(articlesPage);
        if (articleTable->columnCount() < 11)
            articleTable->setColumnCount(11);
        QTableWidgetItem *__qtablewidgetitem44 = new QTableWidgetItem();
        articleTable->setHorizontalHeaderItem(0, __qtablewidgetitem44);
        QTableWidgetItem *__qtablewidgetitem45 = new QTableWidgetItem();
        articleTable->setHorizontalHeaderItem(1, __qtablewidgetitem45);
        QTableWidgetItem *__qtablewidgetitem46 = new QTableWidgetItem();
        articleTable->setHorizontalHeaderItem(2, __qtablewidgetitem46);
        QTableWidgetItem *__qtablewidgetitem47 = new QTableWidgetItem();
        articleTable->setHorizontalHeaderItem(3, __qtablewidgetitem47);
        QTableWidgetItem *__qtablewidgetitem48 = new QTableWidgetItem();
        articleTable->setHorizontalHeaderItem(4, __qtablewidgetitem48);
        QTableWidgetItem *__qtablewidgetitem49 = new QTableWidgetItem();
        articleTable->setHorizontalHeaderItem(5, __qtablewidgetitem49);
        QTableWidgetItem *__qtablewidgetitem50 = new QTableWidgetItem();
        articleTable->setHorizontalHeaderItem(6, __qtablewidgetitem50);
        QTableWidgetItem *__qtablewidgetitem51 = new QTableWidgetItem();
        articleTable->setHorizontalHeaderItem(7, __qtablewidgetitem51);
        QTableWidgetItem *__qtablewidgetitem52 = new QTableWidgetItem();
        articleTable->setHorizontalHeaderItem(8, __qtablewidgetitem52);
        QTableWidgetItem *__qtablewidgetitem53 = new QTableWidgetItem();
        articleTable->setHorizontalHeaderItem(9, __qtablewidgetitem53);
        QTableWidgetItem *__qtablewidgetitem54 = new QTableWidgetItem();
        articleTable->setHorizontalHeaderItem(10, __qtablewidgetitem54);
        articleTable->setObjectName("articleTable");
        articleTable->setEnabled(true);
        articleTable->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
        articleTable->setAlternatingRowColors(true);
        articleTable->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
        articleTable->setShowGrid(true);
        articleTable->setSortingEnabled(true);
        articleTable->horizontalHeader()->setStretchLastSection(true);

        verticalLayout_articles->addWidget(articleTable);

        stackedWidget->addWidget(articlesPage);

        horizontalLayout_3->addWidget(stackedWidget);

        horizontalLayout_3->setStretch(0, 1);

        verticalLayout->addWidget(contentArea);


        horizontalLayout->addWidget(mainContent);

        horizontalLayout->setStretch(1, 1);
        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        stackedWidget->setCurrentIndex(4);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "CUIREA ", nullptr));
        sidebarIcon->setText(QString());
        sidebarTitle->setText(QCoreApplication::translate("MainWindow", "CUIREA\n"
"Management", nullptr));
        btnClients->setText(QCoreApplication::translate("MainWindow", " \342\254\241 Clients  ", nullptr));
        btnEmployees->setText(QCoreApplication::translate("MainWindow", " \342\254\241 Employ\303\251s  ", nullptr));
        btnSuppliers->setText(QCoreApplication::translate("MainWindow", " \342\254\241 Fournisseurs  ", nullptr));
        btnRawMaterials->setText(QCoreApplication::translate("MainWindow", " \342\254\241 Mati\303\250res Premi\303\250res  ", nullptr));
        btnProducts->setText(QCoreApplication::translate("MainWindow", " \342\254\241 Articles  ", nullptr));
        btnProduction->setText(QCoreApplication::translate("MainWindow", " \342\254\241 Production  ", nullptr));
        moduleTitle->setText(QCoreApplication::translate("MainWindow", "GESTION DES EMPLOY\303\211S", nullptr));
        statsLabel->setText(QCoreApplication::translate("MainWindow", "Total Employ\303\251s", nullptr));
        statsValue->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        statsLabel_2->setText(QCoreApplication::translate("MainWindow", "Actifs", nullptr));
        statsValue_2->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        statsLabel_3->setText(QCoreApplication::translate("MainWindow", "D\303\251partements", nullptr));
        statsValue_3->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        searchBox->setPlaceholderText(QCoreApplication::translate("MainWindow", "\342\214\225 Rechercher des employ\303\251s...", nullptr));
        btnAdd->setText(QCoreApplication::translate("MainWindow", "+ Ajouter", nullptr));
        btnEdit->setText(QCoreApplication::translate("MainWindow", "\342\234\216 Modifier", nullptr));
        btnDelete->setText(QCoreApplication::translate("MainWindow", "\342\234\225 Supprimer", nullptr));
        btnExport->setText(QCoreApplication::translate("MainWindow", "\342\207\251 Exporter", nullptr));
        btnStatistics->setText(QCoreApplication::translate("MainWindow", "\342\230\206 Statistiques", nullptr));
        btnSort->setText(QCoreApplication::translate("MainWindow", "\342\207\205 Trier", nullptr));
        employeeProfileTitle->setText(QCoreApplication::translate("MainWindow", "Profil Employ\303\251", nullptr));
        employeePhotoPlaceholder->setText(QCoreApplication::translate("MainWindow", "\360\237\221\244", nullptr));
        lblMatricule->setText(QCoreApplication::translate("MainWindow", "Matricule:", nullptr));
        valMatricule->setText(QCoreApplication::translate("MainWindow", "-", nullptr));
        lblNom->setText(QCoreApplication::translate("MainWindow", "Nom:", nullptr));
        valNom->setText(QCoreApplication::translate("MainWindow", "-", nullptr));
        lblPrenom->setText(QCoreApplication::translate("MainWindow", "Pr\303\251nom:", nullptr));
        valPrenom->setText(QCoreApplication::translate("MainWindow", "-", nullptr));
        lblCIN->setText(QCoreApplication::translate("MainWindow", "CIN:", nullptr));
        valCIN->setText(QCoreApplication::translate("MainWindow", "-", nullptr));
        lblTelephone->setText(QCoreApplication::translate("MainWindow", "T\303\251l\303\251phone:", nullptr));
        valTelephone->setText(QCoreApplication::translate("MainWindow", "-", nullptr));
        lblPoste->setText(QCoreApplication::translate("MainWindow", "Poste:", nullptr));
        valPoste->setText(QCoreApplication::translate("MainWindow", "-", nullptr));
        lblDepartement->setText(QCoreApplication::translate("MainWindow", "D\303\251partement:", nullptr));
        valDepartement->setText(QCoreApplication::translate("MainWindow", "-", nullptr));
        lblSexe->setText(QCoreApplication::translate("MainWindow", "Sexe:", nullptr));
        valSexe->setText(QCoreApplication::translate("MainWindow", "-", nullptr));
        lblAdresse->setText(QCoreApplication::translate("MainWindow", "Adresse:", nullptr));
        valAdresse->setText(QCoreApplication::translate("MainWindow", "-", nullptr));
        lblRole->setText(QCoreApplication::translate("MainWindow", "R\303\264le:", nullptr));
        valRole->setText(QCoreApplication::translate("MainWindow", "-", nullptr));
        lblDateEmbauche->setText(QCoreApplication::translate("MainWindow", "Date Embauche:", nullptr));
        valDateEmbauche->setText(QCoreApplication::translate("MainWindow", "-", nullptr));
        QTableWidgetItem *___qtablewidgetitem = employeeTable->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("MainWindow", "ID", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = employeeTable->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("MainWindow", "Matricule", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = employeeTable->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("MainWindow", "Nom", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = employeeTable->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("MainWindow", "Prenom", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = employeeTable->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("MainWindow", "CIN", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = employeeTable->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("MainWindow", "Date Naissance", nullptr));
        QTableWidgetItem *___qtablewidgetitem6 = employeeTable->horizontalHeaderItem(6);
        ___qtablewidgetitem6->setText(QCoreApplication::translate("MainWindow", "Departement", nullptr));
        QTableWidgetItem *___qtablewidgetitem7 = employeeTable->horizontalHeaderItem(7);
        ___qtablewidgetitem7->setText(QCoreApplication::translate("MainWindow", "Poste", nullptr));
        QTableWidgetItem *___qtablewidgetitem8 = employeeTable->horizontalHeaderItem(8);
        ___qtablewidgetitem8->setText(QCoreApplication::translate("MainWindow", "Telephone", nullptr));
        QTableWidgetItem *___qtablewidgetitem9 = employeeTable->horizontalHeaderItem(9);
        ___qtablewidgetitem9->setText(QCoreApplication::translate("MainWindow", "Email", nullptr));
        listTitle->setText(QCoreApplication::translate("MainWindow", "HISTORIQUE DES CLIENTS", nullptr));
        searchBoxClient->setPlaceholderText(QCoreApplication::translate("MainWindow", "\342\214\225 Rechercher un client...", nullptr));
        btntrie->setText(QCoreApplication::translate("MainWindow", "Trier", nullptr));
        btnFidelityClassification->setText(QCoreApplication::translate("MainWindow", "Fidelit\303\251", nullptr));
        btnStatsByRegion->setText(QCoreApplication::translate("MainWindow", "StatsRegion", nullptr));
        btnAddClient->setText(QCoreApplication::translate("MainWindow", "+ Ajouter", nullptr));
        btnEditClient->setText(QCoreApplication::translate("MainWindow", "\342\234\216 Modifier", nullptr));
        btnDeleteClient->setText(QCoreApplication::translate("MainWindow", "\342\234\225 Supprimer", nullptr));
        btnRefreshClient->setText(QCoreApplication::translate("MainWindow", "\342\206\273 Actualiser", nullptr));
        btnhistorique->setText(QCoreApplication::translate("MainWindow", "\342\237\201 Agent IA", nullptr));
        QTableWidgetItem *___qtablewidgetitem10 = clientTable->horizontalHeaderItem(0);
        ___qtablewidgetitem10->setText(QCoreApplication::translate("MainWindow", "Nom", nullptr));
        QTableWidgetItem *___qtablewidgetitem11 = clientTable->horizontalHeaderItem(1);
        ___qtablewidgetitem11->setText(QCoreApplication::translate("MainWindow", "Pr\303\251nom", nullptr));
        QTableWidgetItem *___qtablewidgetitem12 = clientTable->horizontalHeaderItem(2);
        ___qtablewidgetitem12->setText(QCoreApplication::translate("MainWindow", "Sexe", nullptr));
        QTableWidgetItem *___qtablewidgetitem13 = clientTable->horizontalHeaderItem(3);
        ___qtablewidgetitem13->setText(QCoreApplication::translate("MainWindow", "CIN", nullptr));
        QTableWidgetItem *___qtablewidgetitem14 = clientTable->horizontalHeaderItem(4);
        ___qtablewidgetitem14->setText(QCoreApplication::translate("MainWindow", "Pays", nullptr));
        QTableWidgetItem *___qtablewidgetitem15 = clientTable->horizontalHeaderItem(5);
        ___qtablewidgetitem15->setText(QCoreApplication::translate("MainWindow", "Ville", nullptr));
        QTableWidgetItem *___qtablewidgetitem16 = clientTable->horizontalHeaderItem(6);
        ___qtablewidgetitem16->setText(QCoreApplication::translate("MainWindow", "Adresse", nullptr));
        QTableWidgetItem *___qtablewidgetitem17 = clientTable->horizontalHeaderItem(7);
        ___qtablewidgetitem17->setText(QCoreApplication::translate("MainWindow", "Email", nullptr));
        matiereTitle->setText(QCoreApplication::translate("MainWindow", "GESTION DES MATI\303\210RES PREMI\303\210RES", nullptr));
        statsLabelMatiere1->setText(QCoreApplication::translate("MainWindow", "Total Mati\303\250res", nullptr));
        statsValueMatiere1->setText(QCoreApplication::translate("MainWindow", "3", nullptr));
        statsLabelMatiere2->setText(QCoreApplication::translate("MainWindow", "Stock Critique", nullptr));
        statsValueMatiere2->setText(QCoreApplication::translate("MainWindow", "1", nullptr));
        searchBoxMatiere->setPlaceholderText(QCoreApplication::translate("MainWindow", "\342\214\225 Rechercher une mati\303\250re premi\303\250re...", nullptr));
        btnRecherche->setText(QCoreApplication::translate("MainWindow", "\342\214\225 Recherche", nullptr));
        btnAddMatiere->setText(QCoreApplication::translate("MainWindow", "+ Ajouter Mati\303\250re", nullptr));
        btnEditMatiere->setText(QCoreApplication::translate("MainWindow", "\342\234\216 Modifier", nullptr));
        btnDeleteMatiere->setText(QCoreApplication::translate("MainWindow", "\342\234\225 Supprimer", nullptr));
        btnStatistiques->setText(QCoreApplication::translate("MainWindow", "\360\237\223\212 Statistiques", nullptr));
        btnTri->setText(QCoreApplication::translate("MainWindow", "\342\207\205 Tri", nullptr));
        btnDetection->setText(QCoreApplication::translate("MainWindow", "\360\237\224\215 D\303\251tection", nullptr));
        btnVoice->setText(QCoreApplication::translate("MainWindow", "\360\237\216\244 Vocal", nullptr));
        btnExportMatiere->setText(QCoreApplication::translate("MainWindow", "\342\207\251 Export PDF", nullptr));
        QTableWidgetItem *___qtablewidgetitem18 = matiereTable->horizontalHeaderItem(0);
        ___qtablewidgetitem18->setText(QCoreApplication::translate("MainWindow", "Module", nullptr));
        QTableWidgetItem *___qtablewidgetitem19 = matiereTable->horizontalHeaderItem(1);
        ___qtablewidgetitem19->setText(QCoreApplication::translate("MainWindow", "R\303\251f\303\251rence", nullptr));
        QTableWidgetItem *___qtablewidgetitem20 = matiereTable->horizontalHeaderItem(2);
        ___qtablewidgetitem20->setText(QCoreApplication::translate("MainWindow", "Type", nullptr));
        QTableWidgetItem *___qtablewidgetitem21 = matiereTable->horizontalHeaderItem(3);
        ___qtablewidgetitem21->setText(QCoreApplication::translate("MainWindow", "Quantit\303\251 Actuelle", nullptr));
        QTableWidgetItem *___qtablewidgetitem22 = matiereTable->horizontalHeaderItem(4);
        ___qtablewidgetitem22->setText(QCoreApplication::translate("MainWindow", "Seuil", nullptr));
        QTableWidgetItem *___qtablewidgetitem23 = matiereTable->horizontalHeaderItem(5);
        ___qtablewidgetitem23->setText(QCoreApplication::translate("MainWindow", "Date D'Expiration", nullptr));
        fournisseursTitle->setText(QCoreApplication::translate("MainWindow", "GESTION DES FOURNISSEURS", nullptr));
        statsLabelFournisseur1->setText(QCoreApplication::translate("MainWindow", "Total Fournisseurs", nullptr));
        statsValueFournisseur1->setText(QCoreApplication::translate("MainWindow", "15", nullptr));
        statsLabelFournisseur2->setText(QCoreApplication::translate("MainWindow", "Actifs", nullptr));
        statsValueFournisseur2->setText(QCoreApplication::translate("MainWindow", "13", nullptr));
        statsLabelFournisseur3->setText(QCoreApplication::translate("MainWindow", "Types de Produits", nullptr));
        statsValueFournisseur3->setText(QCoreApplication::translate("MainWindow", "5", nullptr));
        searchBoxFournisseur->setPlaceholderText(QCoreApplication::translate("MainWindow", "\342\214\225 Rechercher fournisseurs...", nullptr));
        btnAddFournisseur->setText(QCoreApplication::translate("MainWindow", "+ Ajouter", nullptr));
        btnEditFournisseur->setText(QCoreApplication::translate("MainWindow", "\342\234\216 Modifier", nullptr));
        btnDeleteFournisseur->setText(QCoreApplication::translate("MainWindow", "\342\234\225 Supprimer", nullptr));
        btnmap->setText(QCoreApplication::translate("MainWindow", "Localisation", nullptr));
        btnExportFournisseur->setText(QCoreApplication::translate("MainWindow", "\342\207\251 Exporter", nullptr));
        btnStatsFournisseur->setText(QCoreApplication::translate("MainWindow", "\342\230\206 Statistiques", nullptr));
        btnTriFournisseur->setText(QCoreApplication::translate("MainWindow", "\342\207\205 Trier", nullptr));
        btnSmsFournisseur->setText(QCoreApplication::translate("MainWindow", "\342\234\211 Envoyer SMS", nullptr));
        QTableWidgetItem *___qtablewidgetitem24 = fournisseurTable->horizontalHeaderItem(0);
        ___qtablewidgetitem24->setText(QCoreApplication::translate("MainWindow", "ID", nullptr));
        QTableWidgetItem *___qtablewidgetitem25 = fournisseurTable->horizontalHeaderItem(1);
        ___qtablewidgetitem25->setText(QCoreApplication::translate("MainWindow", "NOM ENTREPRISE", nullptr));
        QTableWidgetItem *___qtablewidgetitem26 = fournisseurTable->horizontalHeaderItem(2);
        ___qtablewidgetitem26->setText(QCoreApplication::translate("MainWindow", "EMAIL", nullptr));
        QTableWidgetItem *___qtablewidgetitem27 = fournisseurTable->horizontalHeaderItem(3);
        ___qtablewidgetitem27->setText(QCoreApplication::translate("MainWindow", "T\303\211L\303\211PHONE", nullptr));
        QTableWidgetItem *___qtablewidgetitem28 = fournisseurTable->horizontalHeaderItem(4);
        ___qtablewidgetitem28->setText(QCoreApplication::translate("MainWindow", "MATRICULE FISCAL", nullptr));
        QTableWidgetItem *___qtablewidgetitem29 = fournisseurTable->horizontalHeaderItem(5);
        ___qtablewidgetitem29->setText(QCoreApplication::translate("MainWindow", "TYPE PRODUIT", nullptr));
        QTableWidgetItem *___qtablewidgetitem30 = fournisseurTable->horizontalHeaderItem(6);
        ___qtablewidgetitem30->setText(QCoreApplication::translate("MainWindow", "CONDITION PAIEMENT", nullptr));
        QTableWidgetItem *___qtablewidgetitem31 = fournisseurTable->horizontalHeaderItem(7);
        ___qtablewidgetitem31->setText(QCoreApplication::translate("MainWindow", "STATUT", nullptr));
        QTableWidgetItem *___qtablewidgetitem32 = fournisseurTable->horizontalHeaderItem(8);
        ___qtablewidgetitem32->setText(QCoreApplication::translate("MainWindow", "ADRESSE", nullptr));
        productionTitle->setText(QCoreApplication::translate("MainWindow", "GESTION DE LA PRODUCTION", nullptr));
        statsLabelProduction1->setText(QCoreApplication::translate("MainWindow", "Total Commandes", nullptr));
        statsValueProduction1->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        statsLabelProduction2->setText(QCoreApplication::translate("MainWindow", "En Production", nullptr));
        statsValueProduction2->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        statsLabelProduction3->setText(QCoreApplication::translate("MainWindow", "Termin\303\251es", nullptr));
        statsValueProduction3->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        statsLabelProduction4->setText(QCoreApplication::translate("MainWindow", "Montant Total", nullptr));
        statsValueProduction4->setText(QCoreApplication::translate("MainWindow", "0 DT", nullptr));
        searchBoxProduction->setPlaceholderText(QCoreApplication::translate("MainWindow", "\342\214\225 Rechercher par r\303\251f\303\251rence, client, statut...", nullptr));
        btnExportClient->setText(QCoreApplication::translate("MainWindow", "Export", nullptr));
        btnCreerProduction->setText(QCoreApplication::translate("MainWindow", "\342\234\232 Ajouter", nullptr));
        btnModifierProduction->setText(QCoreApplication::translate("MainWindow", "\342\234\216 Modifier", nullptr));
        btnSupprimerProduction->setText(QCoreApplication::translate("MainWindow", "\342\234\225 Supprimer", nullptr));
        btnFactureProduction->setText(QCoreApplication::translate("MainWindow", "\342\202\254 Facture", nullptr));
        btnExcelProduction->setText(QCoreApplication::translate("MainWindow", "\342\232\226 Bilan", nullptr));
        btnTrierProduction->setText(QCoreApplication::translate("MainWindow", "\342\207\205 Trier", nullptr));
        btnStatistiquesProduction->setText(QCoreApplication::translate("MainWindow", "\342\230\206 Statistiques", nullptr));
        btnExpedier->setText(QCoreApplication::translate("MainWindow", "\360\237\232\232 Exp\303\251dier", nullptr));
        QTableWidgetItem *___qtablewidgetitem33 = productionTable->horizontalHeaderItem(0);
        ___qtablewidgetitem33->setText(QCoreApplication::translate("MainWindow", "ID", nullptr));
        QTableWidgetItem *___qtablewidgetitem34 = productionTable->horizontalHeaderItem(1);
        ___qtablewidgetitem34->setText(QCoreApplication::translate("MainWindow", "R\303\211F\303\211RENCE", nullptr));
        QTableWidgetItem *___qtablewidgetitem35 = productionTable->horizontalHeaderItem(2);
        ___qtablewidgetitem35->setText(QCoreApplication::translate("MainWindow", "EMPLOY\303\211", nullptr));
        QTableWidgetItem *___qtablewidgetitem36 = productionTable->horizontalHeaderItem(3);
        ___qtablewidgetitem36->setText(QCoreApplication::translate("MainWindow", "PRODUIT", nullptr));
        QTableWidgetItem *___qtablewidgetitem37 = productionTable->horizontalHeaderItem(4);
        ___qtablewidgetitem37->setText(QCoreApplication::translate("MainWindow", "MONTANT HT", nullptr));
        QTableWidgetItem *___qtablewidgetitem38 = productionTable->horizontalHeaderItem(5);
        ___qtablewidgetitem38->setText(QCoreApplication::translate("MainWindow", "\303\211TAT PAIEMENT", nullptr));
        QTableWidgetItem *___qtablewidgetitem39 = productionTable->horizontalHeaderItem(6);
        ___qtablewidgetitem39->setText(QCoreApplication::translate("MainWindow", "DATE CR\303\211ATION", nullptr));
        QTableWidgetItem *___qtablewidgetitem40 = productionTable->horizontalHeaderItem(7);
        ___qtablewidgetitem40->setText(QCoreApplication::translate("MainWindow", "DATE LIVRAISON", nullptr));
        QTableWidgetItem *___qtablewidgetitem41 = productionTable->horizontalHeaderItem(8);
        ___qtablewidgetitem41->setText(QCoreApplication::translate("MainWindow", "STATUT", nullptr));
        QTableWidgetItem *___qtablewidgetitem42 = productionTable->horizontalHeaderItem(9);
        ___qtablewidgetitem42->setText(QCoreApplication::translate("MainWindow", "PRIORIT\303\211", nullptr));
        QTableWidgetItem *___qtablewidgetitem43 = productionTable->horizontalHeaderItem(10);
        ___qtablewidgetitem43->setText(QCoreApplication::translate("MainWindow", "MAIL CLIENT", nullptr));
        articlesTitle->setText(QCoreApplication::translate("MainWindow", "GESTION DES ARTICLES", nullptr));
        statsLabelArticle1->setText(QCoreApplication::translate("MainWindow", "Total Articles", nullptr));
        statsValueArticle1->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        statsLabelArticle2->setText(QCoreApplication::translate("MainWindow", "Disponibles", nullptr));
        statsValueArticle2->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        statsLabelArticle3->setText(QCoreApplication::translate("MainWindow", "En Production", nullptr));
        statsValueArticle3->setText(QCoreApplication::translate("MainWindow", "0", nullptr));
        searchBoxArticle->setPlaceholderText(QCoreApplication::translate("MainWindow", "\342\214\225 Rechercher...", nullptr));
        btnAddArticle->setText(QCoreApplication::translate("MainWindow", "+ Ajouter", nullptr));
        btnEditArticle->setText(QCoreApplication::translate("MainWindow", "\342\234\216 Modifier", nullptr));
        btnDeleteArticle->setText(QCoreApplication::translate("MainWindow", "\342\234\225 Supprimer", nullptr));
        btnViewArticle->setText(QCoreApplication::translate("MainWindow", "\342\227\216 Consulter", nullptr));
        btnView3DArticle->setText(QCoreApplication::translate("MainWindow", "\360\237\216\256 Vue 3D", nullptr));
        btnTriArticle->setText(QCoreApplication::translate("MainWindow", "\342\207\205 Trier", nullptr));
        btnExportPdfArticle->setText(QCoreApplication::translate("MainWindow", "\342\207\251 PDF", nullptr));
        btnStatistiquesArticle->setText(QCoreApplication::translate("MainWindow", "\342\230\206 Stats", nullptr));
        btnAideDecision->setText(QCoreApplication::translate("MainWindow", "\342\232\241 Aide IA", nullptr));
        QTableWidgetItem *___qtablewidgetitem44 = articleTable->horizontalHeaderItem(0);
        ___qtablewidgetitem44->setText(QCoreApplication::translate("MainWindow", "ID", nullptr));
        QTableWidgetItem *___qtablewidgetitem45 = articleTable->horizontalHeaderItem(1);
        ___qtablewidgetitem45->setText(QCoreApplication::translate("MainWindow", "R\303\211F\303\211RENCE", nullptr));
        QTableWidgetItem *___qtablewidgetitem46 = articleTable->horizontalHeaderItem(2);
        ___qtablewidgetitem46->setText(QCoreApplication::translate("MainWindow", "NOM", nullptr));
        QTableWidgetItem *___qtablewidgetitem47 = articleTable->horizontalHeaderItem(3);
        ___qtablewidgetitem47->setText(QCoreApplication::translate("MainWindow", "CAT\303\211GORIE", nullptr));
        QTableWidgetItem *___qtablewidgetitem48 = articleTable->horizontalHeaderItem(4);
        ___qtablewidgetitem48->setText(QCoreApplication::translate("MainWindow", "TYPE", nullptr));
        QTableWidgetItem *___qtablewidgetitem49 = articleTable->horizontalHeaderItem(5);
        ___qtablewidgetitem49->setText(QCoreApplication::translate("MainWindow", "COULEUR", nullptr));
        QTableWidgetItem *___qtablewidgetitem50 = articleTable->horizontalHeaderItem(6);
        ___qtablewidgetitem50->setText(QCoreApplication::translate("MainWindow", "DIMENSIONS", nullptr));
        QTableWidgetItem *___qtablewidgetitem51 = articleTable->horizontalHeaderItem(7);
        ___qtablewidgetitem51->setText(QCoreApplication::translate("MainWindow", "PRIX UNITAIRE", nullptr));
        QTableWidgetItem *___qtablewidgetitem52 = articleTable->horizontalHeaderItem(8);
        ___qtablewidgetitem52->setText(QCoreApplication::translate("MainWindow", "CO\303\233T FABRICATION", nullptr));
        QTableWidgetItem *___qtablewidgetitem53 = articleTable->horizontalHeaderItem(9);
        ___qtablewidgetitem53->setText(QCoreApplication::translate("MainWindow", "STATUT", nullptr));
        QTableWidgetItem *___qtablewidgetitem54 = articleTable->horizontalHeaderItem(10);
        ___qtablewidgetitem54->setText(QCoreApplication::translate("MainWindow", "DATE CR\303\211ATION", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
