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
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout_main;
    QFrame *sidebarFrame;
    QVBoxLayout *verticalLayout_sidebar;
    QWidget *adminWidget;
    QVBoxLayout *verticalLayout_admin;
    QLabel *adminIcon;
    QLabel *lblAdminName;
    QLabel *lblRole;
    QFrame *line;
    QWidget *menuWidget;
    QVBoxLayout *verticalLayout_menu;
    QPushButton *btnMenuTableau;
    QPushButton *btnMenuProductions;
    QPushButton *btnMenuClients;
    QPushButton *btnMenuStatistiques;
    QPushButton *btnMenuRapports;
    QPushButton *btnMenuParametres;
    QSpacerItem *verticalSpacer;
    QWidget *docWidget;
    QVBoxLayout *verticalLayout_doc;
    QPushButton *btnDocumentation;
    QWidget *mainContent;
    QVBoxLayout *verticalLayout_main;
    QFrame *headerFrame;
    QVBoxLayout *verticalLayout_header;
    QHBoxLayout *horizontalLayout_title;
    QVBoxLayout *verticalLayout_title;
    QLabel *lblTitle;
    QSpacerItem *horizontalSpacer_title;
    QHBoxLayout *horizontalLayout_actions;
    QPushButton *btnCreer;
    QPushButton *btnModifier;
    QPushButton *btnSuivi;
    QPushButton *btnPlanification;
    QPushButton *btnPDF;
    QPushButton *btnExcel;
    QSpacerItem *horizontalSpacer_actions;
    QPushButton *btnRetour;
    QFrame *contentFrame;
    QVBoxLayout *verticalLayout_content;
    QHBoxLayout *horizontalLayout_contentHeader;
    QLabel *lblHistorique;
    QSpacerItem *horizontalSpacer_contentHeader;
    QPushButton *btnTousStatuts;
    QLineEdit *searchBox;
    QFrame *separatorLine;
    QTableWidget *tableWidget;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1400, 900);
        MainWindow->setStyleSheet(QString::fromUtf8("\n"
"QMainWindow { background-color: #F5F5F0; }\n"
"QWidget#centralwidget { background-color: #F5F5F0; }\n"
"QFrame#sidebarFrame { background-color: #6B5444; border: none; }\n"
"QLabel#lblAdminName { color: white; font-size: 14px; font-weight: bold; padding: 10px; }\n"
"QLabel#lblRole { color: white; font-size: 16px; font-weight: bold; padding: 5px; }\n"
"QPushButton#menuBtn { background-color: transparent; color: white; border: none; border-radius: 8px; padding: 12px 20px; font-size: 14px; text-align: left; margin: 3px 10px; }\n"
"QPushButton#menuBtn:hover { background-color: rgba(255, 255, 255, 0.1); }\n"
"QPushButton#menuBtn:pressed { background-color: rgba(255, 255, 255, 0.2); }\n"
"QPushButton#menuBtnActive { background-color: rgba(255, 255, 255, 0.15); color: white; border: none; border-left: 4px solid #D4A574; border-radius: 8px; padding: 12px 20px; font-size: 14px; text-align: left; margin: 3px 10px; font-weight: bold; }\n"
"QFrame#topFrame { background-color: white; border-radius: 15px; }\n"
"QLineEdi"
                        "t#searchBox { \n"
"    background-color: #F8F8F8; \n"
"    border: 1px solid #E0E0E0; \n"
"    border-radius: 8px; \n"
"    padding: 10px 15px; \n"
"    font-size: 13px;\n"
"    color: #2C2416;\n"
"}\n"
"QLineEdit#searchBox:focus { \n"
"    border: 2px solid #6B5444;\n"
"    background-color: white;\n"
"}\n"
"QLabel#lblTitle { color: #2C2416; font-size: 26px; font-weight: bold; padding: 15px; }\n"
"QPushButton { background-color: #6B5444; color: white; border: none; border-radius: 8px; padding: 10px 20px; font-size: 14px; font-weight: 500; }\n"
"QPushButton:hover { background-color: #7D6555; }\n"
"QPushButton:pressed { background-color: #5A4635; }\n"
"QPushButton#btnRetour { background-color: #8B7355; color: white; border: none; border-radius: 8px; padding: 10px 20px; font-size: 13px; font-weight: 500; }\n"
"QPushButton#btnRetour:hover { background-color: #9B8365; }\n"
"QFrame#statsCard { background-color: white; border: 1px solid #E0E0E0; border-radius: 12px; padding: 20px; }\n"
"QLabel#statsTitle { color: #6"
                        "66666; font-size: 13px; font-weight: normal; }\n"
"QLabel#statsValue { color: #5D4E37; font-size: 36px; font-weight: bold; }\n"
"QFrame#contentFrame { background-color: white; border-radius: 15px; border: 1px solid #D4C4B0; padding: 20px; }\n"
"QLabel#lblHistorique { \n"
"    color: #2C2416; \n"
"    font-size: 18px; \n"
"    font-weight: bold;\n"
"    padding: 8px 0px;\n"
"    letter-spacing: 0.5px;\n"
"}\n"
"QPushButton#btnTousStatuts { \n"
"    background-color: #F5F5F0; \n"
"    color: #6B5444; \n"
"    border: 1px solid #D4C4B0; \n"
"    border-radius: 6px; \n"
"    padding: 8px 16px; \n"
"    font-size: 12px;\n"
"    font-weight: 500;\n"
"}\n"
"QPushButton#btnTousStatuts:hover { \n"
"    background-color: #6B5444; \n"
"    color: white;\n"
"    border: 1px solid #6B5444;\n"
"}\n"
"QTableWidget { \n"
"    background-color: #FFFFFF; \n"
"    border: none;\n"
"    border-radius: 12px; \n"
"    gridline-color: #F5F5F5; \n"
"    selection-background-color: rgba(107, 84, 68, 0.1);\n"
"    selection-color: #2C2"
                        "416; \n"
"    outline: none;\n"
"    alternate-background-color: #FAFAFA;\n"
"}\n"
"QTableWidget::item { \n"
"    padding: 16px 12px; \n"
"    border-bottom: 1px solid #F5F5F5; \n"
"    border-left: none;\n"
"    border-right: none;\n"
"    color: #2C2416; \n"
"    font-size: 13px;\n"
"}\n"
"QTableWidget::item:selected { \n"
"    background-color: rgba(107, 84, 68, 0.15);\n"
"    color: #2C2416;\n"
"    font-weight: 500;\n"
"}\n"
"QTableWidget::item:hover { \n"
"    background-color: #F8F8F8;\n"
"}\n"
"QTableWidget::item:alternate { \n"
"    background-color: #FAFAFA; \n"
"}\n"
"QHeaderView::section { \n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #7D6555, stop:1 #6B5444);\n"
"    color: white; \n"
"    padding: 18px 12px; \n"
"    border: none; \n"
"    border-right: 1px solid rgba(255, 255, 255, 0.1); \n"
"    font-weight: bold; \n"
"    font-size: 10px; \n"
"    text-transform: uppercase; \n"
"    letter-spacing: 1.5px;\n"
"}\n"
"QHeaderView::section:first {\n"
"    border-top-left-radi"
                        "us: 12px;\n"
"}\n"
"QHeaderView::section:last { \n"
"    border-right: none;\n"
"    border-top-right-radius: 12px;\n"
"}\n"
"QHeaderView::section:hover { \n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #8B7355, stop:1 #7D6555);\n"
"}\n"
"QTableWidget QTableCornerButton::section { \n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #7D6555, stop:1 #6B5444);\n"
"    border: none;\n"
"    border-top-left-radius: 12px;\n"
"}\n"
"QScrollBar:horizontal { \n"
"    background-color: #F8F8F8; \n"
"    height: 8px; \n"
"    border-radius: 4px;\n"
"    margin: 0px 2px;\n"
"}\n"
"QScrollBar::handle:horizontal { \n"
"    background-color: #D4C4B0; \n"
"    border-radius: 4px; \n"
"    min-width: 30px;\n"
"}\n"
"QScrollBar::handle:horizontal:hover { \n"
"    background-color: #8B7355; \n"
"}\n"
"QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { \n"
"    width: 0px; \n"
"}\n"
"QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal { \n"
"    background: none; \n"
""
                        "}\n"
"QScrollBar:vertical { \n"
"    background-color: #F8F8F8; \n"
"    width: 8px; \n"
"    border-radius: 4px;\n"
"    margin: 2px 0px;\n"
"}\n"
"QScrollBar::handle:vertical { \n"
"    background-color: #C0B5A8; \n"
"    border-radius: 5px; \n"
"    min-height: 30px;\n"
"}\n"
"QScrollBar::handle:vertical:hover { \n"
"    background-color: #8B7355; \n"
"}\n"
"QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { \n"
"    height: 0px; \n"
"}\n"
"QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { \n"
"    background: none; \n"
"}\n"
"   "));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        horizontalLayout_main = new QHBoxLayout(centralwidget);
        horizontalLayout_main->setSpacing(0);
        horizontalLayout_main->setObjectName("horizontalLayout_main");
        horizontalLayout_main->setContentsMargins(0, 0, 0, 0);
        sidebarFrame = new QFrame(centralwidget);
        sidebarFrame->setObjectName("sidebarFrame");
        sidebarFrame->setMinimumSize(QSize(180, 0));
        sidebarFrame->setMaximumSize(QSize(180, 16777215));
        verticalLayout_sidebar = new QVBoxLayout(sidebarFrame);
        verticalLayout_sidebar->setSpacing(0);
        verticalLayout_sidebar->setObjectName("verticalLayout_sidebar");
        verticalLayout_sidebar->setContentsMargins(0, 0, 0, 0);
        adminWidget = new QWidget(sidebarFrame);
        adminWidget->setObjectName("adminWidget");
        adminWidget->setMinimumSize(QSize(0, 120));
        verticalLayout_admin = new QVBoxLayout(adminWidget);
        verticalLayout_admin->setSpacing(5);
        verticalLayout_admin->setObjectName("verticalLayout_admin");
        verticalLayout_admin->setContentsMargins(20, 30, 0, 0);
        adminIcon = new QLabel(adminWidget);
        adminIcon->setObjectName("adminIcon");
        adminIcon->setMinimumSize(QSize(50, 50));
        adminIcon->setMaximumSize(QSize(50, 50));
        adminIcon->setStyleSheet(QString::fromUtf8("background-color: #5D4E37; border-radius: 25px;"));
        adminIcon->setAlignment(Qt::AlignCenter);

        verticalLayout_admin->addWidget(adminIcon);

        lblAdminName = new QLabel(adminWidget);
        lblAdminName->setObjectName("lblAdminName");

        verticalLayout_admin->addWidget(lblAdminName);

        lblRole = new QLabel(adminWidget);
        lblRole->setObjectName("lblRole");

        verticalLayout_admin->addWidget(lblRole);


        verticalLayout_sidebar->addWidget(adminWidget);

        line = new QFrame(sidebarFrame);
        line->setObjectName("line");
        line->setStyleSheet(QString::fromUtf8("background-color: #3D3228;"));
        line->setFrameShape(QFrame::Shape::HLine);
        line->setFrameShadow(QFrame::Shadow::Sunken);

        verticalLayout_sidebar->addWidget(line);

        menuWidget = new QWidget(sidebarFrame);
        menuWidget->setObjectName("menuWidget");
        verticalLayout_menu = new QVBoxLayout(menuWidget);
        verticalLayout_menu->setSpacing(2);
        verticalLayout_menu->setObjectName("verticalLayout_menu");
        verticalLayout_menu->setContentsMargins(0, 20, 0, 0);
        btnMenuTableau = new QPushButton(menuWidget);
        btnMenuTableau->setObjectName("btnMenuTableau");

        verticalLayout_menu->addWidget(btnMenuTableau);

        btnMenuProductions = new QPushButton(menuWidget);
        btnMenuProductions->setObjectName("btnMenuProductions");

        verticalLayout_menu->addWidget(btnMenuProductions);

        btnMenuClients = new QPushButton(menuWidget);
        btnMenuClients->setObjectName("btnMenuClients");

        verticalLayout_menu->addWidget(btnMenuClients);

        btnMenuStatistiques = new QPushButton(menuWidget);
        btnMenuStatistiques->setObjectName("btnMenuStatistiques");

        verticalLayout_menu->addWidget(btnMenuStatistiques);

        btnMenuRapports = new QPushButton(menuWidget);
        btnMenuRapports->setObjectName("btnMenuRapports");

        verticalLayout_menu->addWidget(btnMenuRapports);

        btnMenuParametres = new QPushButton(menuWidget);
        btnMenuParametres->setObjectName("btnMenuParametres");

        verticalLayout_menu->addWidget(btnMenuParametres);


        verticalLayout_sidebar->addWidget(menuWidget);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_sidebar->addItem(verticalSpacer);

        docWidget = new QWidget(sidebarFrame);
        docWidget->setObjectName("docWidget");
        docWidget->setMinimumSize(QSize(0, 80));
        verticalLayout_doc = new QVBoxLayout(docWidget);
        verticalLayout_doc->setObjectName("verticalLayout_doc");
        verticalLayout_doc->setContentsMargins(0, 0, 0, 0);
        btnDocumentation = new QPushButton(docWidget);
        btnDocumentation->setObjectName("btnDocumentation");

        verticalLayout_doc->addWidget(btnDocumentation);


        verticalLayout_sidebar->addWidget(docWidget);


        horizontalLayout_main->addWidget(sidebarFrame);

        mainContent = new QWidget(centralwidget);
        mainContent->setObjectName("mainContent");
        verticalLayout_main = new QVBoxLayout(mainContent);
        verticalLayout_main->setSpacing(20);
        verticalLayout_main->setObjectName("verticalLayout_main");
        verticalLayout_main->setContentsMargins(30, 20, 30, 20);
        headerFrame = new QFrame(mainContent);
        headerFrame->setObjectName("headerFrame");
        verticalLayout_header = new QVBoxLayout(headerFrame);
        verticalLayout_header->setSpacing(10);
        verticalLayout_header->setObjectName("verticalLayout_header");
        horizontalLayout_title = new QHBoxLayout();
        horizontalLayout_title->setObjectName("horizontalLayout_title");
        verticalLayout_title = new QVBoxLayout();
        verticalLayout_title->setObjectName("verticalLayout_title");
        lblTitle = new QLabel(headerFrame);
        lblTitle->setObjectName("lblTitle");

        verticalLayout_title->addWidget(lblTitle);


        horizontalLayout_title->addLayout(verticalLayout_title);

        horizontalSpacer_title = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_title->addItem(horizontalSpacer_title);


        verticalLayout_header->addLayout(horizontalLayout_title);

        horizontalLayout_actions = new QHBoxLayout();
        horizontalLayout_actions->setSpacing(10);
        horizontalLayout_actions->setObjectName("horizontalLayout_actions");
        btnCreer = new QPushButton(headerFrame);
        btnCreer->setObjectName("btnCreer");
        btnCreer->setMinimumSize(QSize(100, 35));

        horizontalLayout_actions->addWidget(btnCreer);

        btnModifier = new QPushButton(headerFrame);
        btnModifier->setObjectName("btnModifier");
        btnModifier->setMinimumSize(QSize(100, 35));

        horizontalLayout_actions->addWidget(btnModifier);

        btnSuivi = new QPushButton(headerFrame);
        btnSuivi->setObjectName("btnSuivi");
        btnSuivi->setMinimumSize(QSize(100, 35));

        horizontalLayout_actions->addWidget(btnSuivi);

        btnPlanification = new QPushButton(headerFrame);
        btnPlanification->setObjectName("btnPlanification");
        btnPlanification->setMinimumSize(QSize(140, 35));

        horizontalLayout_actions->addWidget(btnPlanification);

        btnPDF = new QPushButton(headerFrame);
        btnPDF->setObjectName("btnPDF");
        btnPDF->setMinimumSize(QSize(100, 35));

        horizontalLayout_actions->addWidget(btnPDF);

        btnExcel = new QPushButton(headerFrame);
        btnExcel->setObjectName("btnExcel");
        btnExcel->setMinimumSize(QSize(80, 35));

        horizontalLayout_actions->addWidget(btnExcel);

        horizontalSpacer_actions = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_actions->addItem(horizontalSpacer_actions);

        btnRetour = new QPushButton(headerFrame);
        btnRetour->setObjectName("btnRetour");
        btnRetour->setMinimumSize(QSize(100, 35));

        horizontalLayout_actions->addWidget(btnRetour);


        verticalLayout_header->addLayout(horizontalLayout_actions);


        verticalLayout_main->addWidget(headerFrame);

        contentFrame = new QFrame(mainContent);
        contentFrame->setObjectName("contentFrame");
        verticalLayout_content = new QVBoxLayout(contentFrame);
        verticalLayout_content->setSpacing(15);
        verticalLayout_content->setObjectName("verticalLayout_content");
        verticalLayout_content->setContentsMargins(20, 20, 20, 20);
        horizontalLayout_contentHeader = new QHBoxLayout();
        horizontalLayout_contentHeader->setObjectName("horizontalLayout_contentHeader");
        lblHistorique = new QLabel(contentFrame);
        lblHistorique->setObjectName("lblHistorique");

        horizontalLayout_contentHeader->addWidget(lblHistorique);

        horizontalSpacer_contentHeader = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_contentHeader->addItem(horizontalSpacer_contentHeader);

        btnTousStatuts = new QPushButton(contentFrame);
        btnTousStatuts->setObjectName("btnTousStatuts");
        btnTousStatuts->setMinimumSize(QSize(120, 35));

        horizontalLayout_contentHeader->addWidget(btnTousStatuts);

        searchBox = new QLineEdit(contentFrame);
        searchBox->setObjectName("searchBox");
        searchBox->setMinimumSize(QSize(300, 35));

        horizontalLayout_contentHeader->addWidget(searchBox);


        verticalLayout_content->addLayout(horizontalLayout_contentHeader);

        separatorLine = new QFrame(contentFrame);
        separatorLine->setObjectName("separatorLine");
        separatorLine->setMinimumSize(QSize(0, 1));
        separatorLine->setMaximumSize(QSize(16777215, 1));
        separatorLine->setStyleSheet(QString::fromUtf8("background-color: #E0E0E0;"));
        separatorLine->setFrameShape(QFrame::HLine);
        separatorLine->setFrameShadow(QFrame::Plain);

        verticalLayout_content->addWidget(separatorLine);

        tableWidget = new QTableWidget(contentFrame);
        if (tableWidget->columnCount() < 9)
            tableWidget->setColumnCount(9);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(5, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(6, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(7, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(8, __qtablewidgetitem8);
        tableWidget->setObjectName("tableWidget");
        tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
        tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableWidget->setShowGrid(true);

        verticalLayout_content->addWidget(tableWidget);


        verticalLayout_main->addWidget(contentFrame);


        horizontalLayout_main->addWidget(mainContent);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Gestion de la Production", nullptr));
        adminIcon->setText(QString());
        lblAdminName->setText(QCoreApplication::translate("MainWindow", "ADMINISTRATEUR", nullptr));
        lblRole->setText(QCoreApplication::translate("MainWindow", "Manager Production", nullptr));
        btnMenuTableau->setText(QCoreApplication::translate("MainWindow", "\360\237\223\212 Tableau de bord", nullptr));
        btnMenuTableau->setObjectName(QCoreApplication::translate("MainWindow", "menuBtn", nullptr));
        btnMenuProductions->setText(QCoreApplication::translate("MainWindow", "\342\232\231\357\270\217 Productions \342\226\274", nullptr));
        btnMenuProductions->setObjectName(QCoreApplication::translate("MainWindow", "menuBtnActive", nullptr));
        btnMenuClients->setText(QCoreApplication::translate("MainWindow", "\360\237\221\245 Clients", nullptr));
        btnMenuClients->setObjectName(QCoreApplication::translate("MainWindow", "menuBtn", nullptr));
        btnMenuStatistiques->setText(QCoreApplication::translate("MainWindow", "\360\237\223\210 Statistiques", nullptr));
        btnMenuStatistiques->setObjectName(QCoreApplication::translate("MainWindow", "menuBtn", nullptr));
        btnMenuRapports->setText(QCoreApplication::translate("MainWindow", "\360\237\223\204 Rapports", nullptr));
        btnMenuRapports->setObjectName(QCoreApplication::translate("MainWindow", "menuBtn", nullptr));
        btnMenuParametres->setText(QCoreApplication::translate("MainWindow", "\342\232\231\357\270\217 Param\303\250tres", nullptr));
        btnMenuParametres->setObjectName(QCoreApplication::translate("MainWindow", "menuBtn", nullptr));
        btnDocumentation->setText(QCoreApplication::translate("MainWindow", "\360\237\223\232 Documentation", nullptr));
        btnDocumentation->setObjectName(QCoreApplication::translate("MainWindow", "menuBtn", nullptr));
        lblTitle->setText(QCoreApplication::translate("MainWindow", "Gestion de la Production", nullptr));
        btnCreer->setText(QCoreApplication::translate("MainWindow", "\342\236\225 CR\303\211ER", nullptr));
        btnModifier->setText(QCoreApplication::translate("MainWindow", "\342\234\217\357\270\217 MODIFIER", nullptr));
        btnSuivi->setText(QCoreApplication::translate("MainWindow", "\360\237\223\246 SUIVI", nullptr));
        btnPlanification->setText(QCoreApplication::translate("MainWindow", "\360\237\223\205 PLANIFICATION", nullptr));
        btnPDF->setText(QCoreApplication::translate("MainWindow", "\360\237\223\204 FACTURE", nullptr));
        btnExcel->setText(QCoreApplication::translate("MainWindow", "\360\237\223\212 EXCEL", nullptr));
        btnRetour->setText(QCoreApplication::translate("MainWindow", "\342\206\220 RETOUR", nullptr));
        lblHistorique->setText(QCoreApplication::translate("MainWindow", "\360\237\223\213 HISTORIQUE DES COMMANDES", nullptr));
        btnTousStatuts->setText(QCoreApplication::translate("MainWindow", "\360\237\224\215 Tous statuts", nullptr));
        searchBox->setPlaceholderText(QCoreApplication::translate("MainWindow", "\360\237\224\215 Rechercher par r\303\251f\303\251rence, client, statut...", nullptr));
        QTableWidgetItem *___qtablewidgetitem = tableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("MainWindow", "ID", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("MainWindow", "R\303\211F\303\211RENCE", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tableWidget->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("MainWindow", "CLIENT", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = tableWidget->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("MainWindow", "TYPE", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = tableWidget->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("MainWindow", "MONTANT HT", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = tableWidget->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("MainWindow", "DATE CR\303\211ATION", nullptr));
        QTableWidgetItem *___qtablewidgetitem6 = tableWidget->horizontalHeaderItem(6);
        ___qtablewidgetitem6->setText(QCoreApplication::translate("MainWindow", "DATE LIVRAISON", nullptr));
        QTableWidgetItem *___qtablewidgetitem7 = tableWidget->horizontalHeaderItem(7);
        ___qtablewidgetitem7->setText(QCoreApplication::translate("MainWindow", "STATUT", nullptr));
        QTableWidgetItem *___qtablewidgetitem8 = tableWidget->horizontalHeaderItem(8);
        ___qtablewidgetitem8->setText(QCoreApplication::translate("MainWindow", "PRIORIT\303\211", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
