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
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout_main;
    QScrollArea *scrollAreaSidebar;
    QWidget *scrollAreaWidgetContentsSidebar;
    QVBoxLayout *verticalLayout_sidebarScroll;
    QFrame *sidebarFrame;
    QVBoxLayout *verticalLayout_sidebar;
    QFrame *profileFrame;
    QVBoxLayout *verticalLayout_profile;
    QLabel *labelPhoto;
    QLabel *labelUser;
    QLabel *labelRole;
    QFrame *line;
    QPushButton *btnTableauBord;
    QPushButton *btnProductions;
    QFrame *productionsSubMenu;
    QVBoxLayout *verticalLayout_submenu;
    QPushButton *btnSubCommandes;
    QPushButton *btnSubPlanification;
    QPushButton *btnSubStock;
    QPushButton *btnClients;
    QPushButton *btnStatistiques;
    QPushButton *btnRapports;
    QPushButton *btnParametres;
    QSpacerItem *verticalSpacer;
    QFrame *line_2;
    QPushButton *btnDeconnexion;
    QScrollArea *scrollAreaMain;
    QWidget *scrollAreaWidgetContentsMain;
    QVBoxLayout *verticalLayout_mainScroll;
    QFrame *contentFrame;
    QVBoxLayout *verticalLayout_content;
    QFrame *headerFrame;
    QVBoxLayout *verticalLayout_header;
    QHBoxLayout *horizontalLayout_title;
    QLabel *labelTitre;
    QSpacerItem *horizontalSpacer_title;
    QLabel *labelDate;
    QHBoxLayout *horizontalLayout_buttons;
    QPushButton *btnCreer;
    QPushButton *btnModifier;
    QPushButton *btnSupprimer;
    QPushButton *btnDupliquer;
    QPushButton *btnExporterPDF;
    QPushButton *btnExporterExcel;
    QPushButton *btnActualiser;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnRetourListe;
    QStackedWidget *stackedWidget;
    QWidget *pageListe;
    QVBoxLayout *verticalLayout_pageListe;
    QFrame *frameStatistiques;
    QHBoxLayout *horizontalLayout_stats;
    QFrame *statFrame1;
    QVBoxLayout *verticalLayout_stat1;
    QLabel *labelStatTitre1;
    QLabel *labelStatValeur1;
    QFrame *statFrame2;
    QVBoxLayout *verticalLayout_stat2;
    QLabel *labelStatTitre2;
    QLabel *labelStatValeur2;
    QFrame *statFrame3;
    QVBoxLayout *verticalLayout_stat3;
    QLabel *labelStatTitre3;
    QLabel *labelStatValeur3;
    QFrame *statFrame4;
    QVBoxLayout *verticalLayout_stat4;
    QLabel *labelStatTitre4;
    QLabel *labelStatValeur4;
    QFrame *frameHistorique;
    QVBoxLayout *verticalLayout_historique;
    QHBoxLayout *horizontalLayout_search;
    QLabel *labelHistorique;
    QSpacerItem *horizontalSpacer_search;
    QComboBox *comboBoxFiltreStatut;
    QLineEdit *lineEditRecherche;
    QTableWidget *tableCommandes;
    QWidget *pageCreer;
    QVBoxLayout *verticalLayout_creer;
    QLabel *labelCreerTitre;
    QGroupBox *groupBoxCreer;
    QFormLayout *formLayout_creer;
    QLabel *labelCreerRef;
    QLineEdit *lineEditCreerRef;
    QLabel *labelCreerClient;
    QComboBox *comboBoxCreerClient;
    QLabel *labelCreerMontant;
    QDoubleSpinBox *spinBoxCreerMontant;
    QHBoxLayout *horizontalLayout_creer_btn;
    QSpacerItem *horizontalSpacer_creer;
    QPushButton *btnCreerAnnuler;
    QPushButton *btnCreerEnregistrer;
    QSpacerItem *verticalSpacer_creer;
    QWidget *pageModifier;
    QVBoxLayout *verticalLayout_modifier;
    QLabel *labelModifierTitre;
    QGroupBox *groupBoxModifier;
    QFormLayout *formLayout_modifier;
    QLabel *labelModifierId;
    QLineEdit *lineEditModifierId;
    QLabel *labelModifierRef;
    QLineEdit *lineEditModifierRef;
    QLabel *labelModifierMontant;
    QDoubleSpinBox *spinBoxModifierMontant;
    QLabel *labelModifierStatut;
    QComboBox *comboBoxModifierStatut;
    QHBoxLayout *horizontalLayout_modifier_btn;
    QSpacerItem *horizontalSpacer_modifier;
    QPushButton *btnModifierAnnuler;
    QPushButton *btnModifierEnregistrer;
    QSpacerItem *verticalSpacer_modifier;
    QWidget *pageSupprimer;
    QVBoxLayout *verticalLayout_supprimer;
    QLabel *labelSupprimerTitre;
    QGroupBox *groupBoxSupprimer;
    QFormLayout *formLayout_supprimer;
    QLabel *labelSupprimerId;
    QLabel *labelSupprimerIdValue;
    QLabel *labelSupprimerRef;
    QLabel *labelSupprimerRefValue;
    QLabel *labelSupprimerMontant;
    QLabel *labelSupprimerMontantValue;
    QHBoxLayout *horizontalLayout_supprimer_btn;
    QSpacerItem *horizontalSpacer_supprimer;
    QPushButton *btnSupprimerAnnuler;
    QPushButton *btnSupprimerConfirmer;
    QSpacerItem *verticalSpacer_supprimer;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1366, 768);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        horizontalLayout_main = new QHBoxLayout(centralwidget);
        horizontalLayout_main->setSpacing(0);
        horizontalLayout_main->setObjectName("horizontalLayout_main");
        horizontalLayout_main->setContentsMargins(0, 0, 0, 0);
        scrollAreaSidebar = new QScrollArea(centralwidget);
        scrollAreaSidebar->setObjectName("scrollAreaSidebar");
        scrollAreaSidebar->setMinimumSize(QSize(220, 0));
        scrollAreaSidebar->setMaximumSize(QSize(220, 16777215));
        scrollAreaSidebar->setWidgetResizable(true);
        scrollAreaWidgetContentsSidebar = new QWidget();
        scrollAreaWidgetContentsSidebar->setObjectName("scrollAreaWidgetContentsSidebar");
        scrollAreaWidgetContentsSidebar->setGeometry(QRect(0, 0, 220, 768));
        verticalLayout_sidebarScroll = new QVBoxLayout(scrollAreaWidgetContentsSidebar);
        verticalLayout_sidebarScroll->setSpacing(0);
        verticalLayout_sidebarScroll->setObjectName("verticalLayout_sidebarScroll");
        verticalLayout_sidebarScroll->setContentsMargins(0, 0, 0, 0);
        sidebarFrame = new QFrame(scrollAreaWidgetContentsSidebar);
        sidebarFrame->setObjectName("sidebarFrame");
        verticalLayout_sidebar = new QVBoxLayout(sidebarFrame);
        verticalLayout_sidebar->setSpacing(0);
        verticalLayout_sidebar->setObjectName("verticalLayout_sidebar");
        profileFrame = new QFrame(sidebarFrame);
        profileFrame->setObjectName("profileFrame");
        verticalLayout_profile = new QVBoxLayout(profileFrame);
        verticalLayout_profile->setObjectName("verticalLayout_profile");
        labelPhoto = new QLabel(profileFrame);
        labelPhoto->setObjectName("labelPhoto");
        labelPhoto->setMinimumSize(QSize(60, 60));
        labelPhoto->setMaximumSize(QSize(60, 60));
        labelPhoto->setAlignment(Qt::AlignCenter);

        verticalLayout_profile->addWidget(labelPhoto);

        labelUser = new QLabel(profileFrame);
        labelUser->setObjectName("labelUser");
        labelUser->setAlignment(Qt::AlignCenter);

        verticalLayout_profile->addWidget(labelUser);

        labelRole = new QLabel(profileFrame);
        labelRole->setObjectName("labelRole");
        labelRole->setAlignment(Qt::AlignCenter);

        verticalLayout_profile->addWidget(labelRole);


        verticalLayout_sidebar->addWidget(profileFrame);

        line = new QFrame(sidebarFrame);
        line->setObjectName("line");
        line->setFrameShape(QFrame::Shape::HLine);
        line->setFrameShadow(QFrame::Shadow::Sunken);

        verticalLayout_sidebar->addWidget(line);

        btnTableauBord = new QPushButton(sidebarFrame);
        btnTableauBord->setObjectName("btnTableauBord");

        verticalLayout_sidebar->addWidget(btnTableauBord);

        btnProductions = new QPushButton(sidebarFrame);
        btnProductions->setObjectName("btnProductions");
        btnProductions->setCheckable(true);

        verticalLayout_sidebar->addWidget(btnProductions);

        productionsSubMenu = new QFrame(sidebarFrame);
        productionsSubMenu->setObjectName("productionsSubMenu");
        productionsSubMenu->setVisible(false);
        verticalLayout_submenu = new QVBoxLayout(productionsSubMenu);
        verticalLayout_submenu->setObjectName("verticalLayout_submenu");
        btnSubCommandes = new QPushButton(productionsSubMenu);
        btnSubCommandes->setObjectName("btnSubCommandes");

        verticalLayout_submenu->addWidget(btnSubCommandes);

        btnSubPlanification = new QPushButton(productionsSubMenu);
        btnSubPlanification->setObjectName("btnSubPlanification");

        verticalLayout_submenu->addWidget(btnSubPlanification);

        btnSubStock = new QPushButton(productionsSubMenu);
        btnSubStock->setObjectName("btnSubStock");

        verticalLayout_submenu->addWidget(btnSubStock);


        verticalLayout_sidebar->addWidget(productionsSubMenu);

        btnClients = new QPushButton(sidebarFrame);
        btnClients->setObjectName("btnClients");

        verticalLayout_sidebar->addWidget(btnClients);

        btnStatistiques = new QPushButton(sidebarFrame);
        btnStatistiques->setObjectName("btnStatistiques");

        verticalLayout_sidebar->addWidget(btnStatistiques);

        btnRapports = new QPushButton(sidebarFrame);
        btnRapports->setObjectName("btnRapports");

        verticalLayout_sidebar->addWidget(btnRapports);

        btnParametres = new QPushButton(sidebarFrame);
        btnParametres->setObjectName("btnParametres");

        verticalLayout_sidebar->addWidget(btnParametres);

        verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_sidebar->addItem(verticalSpacer);

        line_2 = new QFrame(sidebarFrame);
        line_2->setObjectName("line_2");
        line_2->setFrameShape(QFrame::Shape::HLine);
        line_2->setFrameShadow(QFrame::Shadow::Sunken);

        verticalLayout_sidebar->addWidget(line_2);

        btnDeconnexion = new QPushButton(sidebarFrame);
        btnDeconnexion->setObjectName("btnDeconnexion");

        verticalLayout_sidebar->addWidget(btnDeconnexion);


        verticalLayout_sidebarScroll->addWidget(sidebarFrame);

        scrollAreaSidebar->setWidget(scrollAreaWidgetContentsSidebar);

        horizontalLayout_main->addWidget(scrollAreaSidebar);

        scrollAreaMain = new QScrollArea(centralwidget);
        scrollAreaMain->setObjectName("scrollAreaMain");
        scrollAreaMain->setWidgetResizable(true);
        scrollAreaWidgetContentsMain = new QWidget();
        scrollAreaWidgetContentsMain->setObjectName("scrollAreaWidgetContentsMain");
        scrollAreaWidgetContentsMain->setGeometry(QRect(0, 0, 1146, 1200));
        verticalLayout_mainScroll = new QVBoxLayout(scrollAreaWidgetContentsMain);
        verticalLayout_mainScroll->setObjectName("verticalLayout_mainScroll");
        contentFrame = new QFrame(scrollAreaWidgetContentsMain);
        contentFrame->setObjectName("contentFrame");
        verticalLayout_content = new QVBoxLayout(contentFrame);
        verticalLayout_content->setSpacing(10);
        verticalLayout_content->setObjectName("verticalLayout_content");
        verticalLayout_content->setContentsMargins(15, 15, 15, 15);
        headerFrame = new QFrame(contentFrame);
        headerFrame->setObjectName("headerFrame");
        headerFrame->setMinimumSize(QSize(0, 100));
        headerFrame->setMaximumSize(QSize(16777215, 100));
        verticalLayout_header = new QVBoxLayout(headerFrame);
        verticalLayout_header->setObjectName("verticalLayout_header");
        horizontalLayout_title = new QHBoxLayout();
        horizontalLayout_title->setObjectName("horizontalLayout_title");
        labelTitre = new QLabel(headerFrame);
        labelTitre->setObjectName("labelTitre");

        horizontalLayout_title->addWidget(labelTitre);

        horizontalSpacer_title = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_title->addItem(horizontalSpacer_title);

        labelDate = new QLabel(headerFrame);
        labelDate->setObjectName("labelDate");

        horizontalLayout_title->addWidget(labelDate);


        verticalLayout_header->addLayout(horizontalLayout_title);

        horizontalLayout_buttons = new QHBoxLayout();
        horizontalLayout_buttons->setObjectName("horizontalLayout_buttons");
        btnCreer = new QPushButton(headerFrame);
        btnCreer->setObjectName("btnCreer");
        btnCreer->setMinimumSize(QSize(0, 35));

        horizontalLayout_buttons->addWidget(btnCreer);

        btnModifier = new QPushButton(headerFrame);
        btnModifier->setObjectName("btnModifier");
        btnModifier->setMinimumSize(QSize(0, 35));

        horizontalLayout_buttons->addWidget(btnModifier);

        btnSupprimer = new QPushButton(headerFrame);
        btnSupprimer->setObjectName("btnSupprimer");
        btnSupprimer->setMinimumSize(QSize(0, 35));

        horizontalLayout_buttons->addWidget(btnSupprimer);

        btnDupliquer = new QPushButton(headerFrame);
        btnDupliquer->setObjectName("btnDupliquer");
        btnDupliquer->setMinimumSize(QSize(0, 35));

        horizontalLayout_buttons->addWidget(btnDupliquer);

        btnExporterPDF = new QPushButton(headerFrame);
        btnExporterPDF->setObjectName("btnExporterPDF");
        btnExporterPDF->setMinimumSize(QSize(0, 35));

        horizontalLayout_buttons->addWidget(btnExporterPDF);

        btnExporterExcel = new QPushButton(headerFrame);
        btnExporterExcel->setObjectName("btnExporterExcel");
        btnExporterExcel->setMinimumSize(QSize(0, 35));

        horizontalLayout_buttons->addWidget(btnExporterExcel);

        btnActualiser = new QPushButton(headerFrame);
        btnActualiser->setObjectName("btnActualiser");
        btnActualiser->setMinimumSize(QSize(0, 35));

        horizontalLayout_buttons->addWidget(btnActualiser);

        horizontalSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_buttons->addItem(horizontalSpacer);

        btnRetourListe = new QPushButton(headerFrame);
        btnRetourListe->setObjectName("btnRetourListe");
        btnRetourListe->setMinimumSize(QSize(0, 35));

        horizontalLayout_buttons->addWidget(btnRetourListe);


        verticalLayout_header->addLayout(horizontalLayout_buttons);


        verticalLayout_content->addWidget(headerFrame);

        stackedWidget = new QStackedWidget(contentFrame);
        stackedWidget->setObjectName("stackedWidget");
        pageListe = new QWidget();
        pageListe->setObjectName("pageListe");
        verticalLayout_pageListe = new QVBoxLayout(pageListe);
        verticalLayout_pageListe->setSpacing(10);
        verticalLayout_pageListe->setObjectName("verticalLayout_pageListe");
        frameStatistiques = new QFrame(pageListe);
        frameStatistiques->setObjectName("frameStatistiques");
        frameStatistiques->setMinimumSize(QSize(0, 80));
        frameStatistiques->setMaximumSize(QSize(16777215, 80));
        horizontalLayout_stats = new QHBoxLayout(frameStatistiques);
        horizontalLayout_stats->setObjectName("horizontalLayout_stats");
        statFrame1 = new QFrame(frameStatistiques);
        statFrame1->setObjectName("statFrame1");
        verticalLayout_stat1 = new QVBoxLayout(statFrame1);
        verticalLayout_stat1->setObjectName("verticalLayout_stat1");
        labelStatTitre1 = new QLabel(statFrame1);
        labelStatTitre1->setObjectName("labelStatTitre1");
        labelStatTitre1->setAlignment(Qt::AlignCenter);

        verticalLayout_stat1->addWidget(labelStatTitre1);

        labelStatValeur1 = new QLabel(statFrame1);
        labelStatValeur1->setObjectName("labelStatValeur1");
        labelStatValeur1->setAlignment(Qt::AlignCenter);

        verticalLayout_stat1->addWidget(labelStatValeur1);


        horizontalLayout_stats->addWidget(statFrame1);

        statFrame2 = new QFrame(frameStatistiques);
        statFrame2->setObjectName("statFrame2");
        verticalLayout_stat2 = new QVBoxLayout(statFrame2);
        verticalLayout_stat2->setObjectName("verticalLayout_stat2");
        labelStatTitre2 = new QLabel(statFrame2);
        labelStatTitre2->setObjectName("labelStatTitre2");
        labelStatTitre2->setAlignment(Qt::AlignCenter);

        verticalLayout_stat2->addWidget(labelStatTitre2);

        labelStatValeur2 = new QLabel(statFrame2);
        labelStatValeur2->setObjectName("labelStatValeur2");
        labelStatValeur2->setAlignment(Qt::AlignCenter);

        verticalLayout_stat2->addWidget(labelStatValeur2);


        horizontalLayout_stats->addWidget(statFrame2);

        statFrame3 = new QFrame(frameStatistiques);
        statFrame3->setObjectName("statFrame3");
        verticalLayout_stat3 = new QVBoxLayout(statFrame3);
        verticalLayout_stat3->setObjectName("verticalLayout_stat3");
        labelStatTitre3 = new QLabel(statFrame3);
        labelStatTitre3->setObjectName("labelStatTitre3");
        labelStatTitre3->setAlignment(Qt::AlignCenter);

        verticalLayout_stat3->addWidget(labelStatTitre3);

        labelStatValeur3 = new QLabel(statFrame3);
        labelStatValeur3->setObjectName("labelStatValeur3");
        labelStatValeur3->setAlignment(Qt::AlignCenter);

        verticalLayout_stat3->addWidget(labelStatValeur3);


        horizontalLayout_stats->addWidget(statFrame3);

        statFrame4 = new QFrame(frameStatistiques);
        statFrame4->setObjectName("statFrame4");
        verticalLayout_stat4 = new QVBoxLayout(statFrame4);
        verticalLayout_stat4->setObjectName("verticalLayout_stat4");
        labelStatTitre4 = new QLabel(statFrame4);
        labelStatTitre4->setObjectName("labelStatTitre4");
        labelStatTitre4->setAlignment(Qt::AlignCenter);

        verticalLayout_stat4->addWidget(labelStatTitre4);

        labelStatValeur4 = new QLabel(statFrame4);
        labelStatValeur4->setObjectName("labelStatValeur4");
        labelStatValeur4->setAlignment(Qt::AlignCenter);

        verticalLayout_stat4->addWidget(labelStatValeur4);


        horizontalLayout_stats->addWidget(statFrame4);


        verticalLayout_pageListe->addWidget(frameStatistiques);

        frameHistorique = new QFrame(pageListe);
        frameHistorique->setObjectName("frameHistorique");
        verticalLayout_historique = new QVBoxLayout(frameHistorique);
        verticalLayout_historique->setObjectName("verticalLayout_historique");
        horizontalLayout_search = new QHBoxLayout();
        horizontalLayout_search->setObjectName("horizontalLayout_search");
        labelHistorique = new QLabel(frameHistorique);
        labelHistorique->setObjectName("labelHistorique");

        horizontalLayout_search->addWidget(labelHistorique);

        horizontalSpacer_search = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_search->addItem(horizontalSpacer_search);

        comboBoxFiltreStatut = new QComboBox(frameHistorique);
        comboBoxFiltreStatut->addItem(QString());
        comboBoxFiltreStatut->addItem(QString());
        comboBoxFiltreStatut->addItem(QString());
        comboBoxFiltreStatut->addItem(QString());
        comboBoxFiltreStatut->addItem(QString());
        comboBoxFiltreStatut->setObjectName("comboBoxFiltreStatut");
        comboBoxFiltreStatut->setMinimumSize(QSize(130, 0));

        horizontalLayout_search->addWidget(comboBoxFiltreStatut);

        lineEditRecherche = new QLineEdit(frameHistorique);
        lineEditRecherche->setObjectName("lineEditRecherche");
        lineEditRecherche->setMinimumSize(QSize(200, 0));

        horizontalLayout_search->addWidget(lineEditRecherche);


        verticalLayout_historique->addLayout(horizontalLayout_search);

        tableCommandes = new QTableWidget(frameHistorique);
        if (tableCommandes->columnCount() < 9)
            tableCommandes->setColumnCount(9);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableCommandes->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableCommandes->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableCommandes->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tableCommandes->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tableCommandes->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        tableCommandes->setHorizontalHeaderItem(5, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        tableCommandes->setHorizontalHeaderItem(6, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        tableCommandes->setHorizontalHeaderItem(7, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        tableCommandes->setHorizontalHeaderItem(8, __qtablewidgetitem8);
        tableCommandes->setObjectName("tableCommandes");
        tableCommandes->setAlternatingRowColors(true);
        tableCommandes->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableCommandes->setSortingEnabled(true);
        tableCommandes->horizontalHeader()->setStretchLastSection(true);

        verticalLayout_historique->addWidget(tableCommandes);


        verticalLayout_pageListe->addWidget(frameHistorique);

        stackedWidget->addWidget(pageListe);
        pageCreer = new QWidget();
        pageCreer->setObjectName("pageCreer");
        verticalLayout_creer = new QVBoxLayout(pageCreer);
        verticalLayout_creer->setObjectName("verticalLayout_creer");
        labelCreerTitre = new QLabel(pageCreer);
        labelCreerTitre->setObjectName("labelCreerTitre");

        verticalLayout_creer->addWidget(labelCreerTitre);

        groupBoxCreer = new QGroupBox(pageCreer);
        groupBoxCreer->setObjectName("groupBoxCreer");
        formLayout_creer = new QFormLayout(groupBoxCreer);
        formLayout_creer->setObjectName("formLayout_creer");
        formLayout_creer->setHorizontalSpacing(15);
        formLayout_creer->setVerticalSpacing(12);
        labelCreerRef = new QLabel(groupBoxCreer);
        labelCreerRef->setObjectName("labelCreerRef");

        formLayout_creer->setWidget(0, QFormLayout::LabelRole, labelCreerRef);

        lineEditCreerRef = new QLineEdit(groupBoxCreer);
        lineEditCreerRef->setObjectName("lineEditCreerRef");

        formLayout_creer->setWidget(0, QFormLayout::FieldRole, lineEditCreerRef);

        labelCreerClient = new QLabel(groupBoxCreer);
        labelCreerClient->setObjectName("labelCreerClient");

        formLayout_creer->setWidget(1, QFormLayout::LabelRole, labelCreerClient);

        comboBoxCreerClient = new QComboBox(groupBoxCreer);
        comboBoxCreerClient->addItem(QString());
        comboBoxCreerClient->addItem(QString());
        comboBoxCreerClient->setObjectName("comboBoxCreerClient");
        comboBoxCreerClient->setEditable(true);

        formLayout_creer->setWidget(1, QFormLayout::FieldRole, comboBoxCreerClient);

        labelCreerMontant = new QLabel(groupBoxCreer);
        labelCreerMontant->setObjectName("labelCreerMontant");

        formLayout_creer->setWidget(2, QFormLayout::LabelRole, labelCreerMontant);

        spinBoxCreerMontant = new QDoubleSpinBox(groupBoxCreer);
        spinBoxCreerMontant->setObjectName("spinBoxCreerMontant");
        spinBoxCreerMontant->setMaximum(999999.000000000000000);

        formLayout_creer->setWidget(2, QFormLayout::FieldRole, spinBoxCreerMontant);


        verticalLayout_creer->addWidget(groupBoxCreer);

        horizontalLayout_creer_btn = new QHBoxLayout();
        horizontalLayout_creer_btn->setObjectName("horizontalLayout_creer_btn");
        horizontalSpacer_creer = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_creer_btn->addItem(horizontalSpacer_creer);

        btnCreerAnnuler = new QPushButton(pageCreer);
        btnCreerAnnuler->setObjectName("btnCreerAnnuler");
        btnCreerAnnuler->setMinimumSize(QSize(120, 40));

        horizontalLayout_creer_btn->addWidget(btnCreerAnnuler);

        btnCreerEnregistrer = new QPushButton(pageCreer);
        btnCreerEnregistrer->setObjectName("btnCreerEnregistrer");
        btnCreerEnregistrer->setMinimumSize(QSize(120, 40));

        horizontalLayout_creer_btn->addWidget(btnCreerEnregistrer);


        verticalLayout_creer->addLayout(horizontalLayout_creer_btn);

        verticalSpacer_creer = new QSpacerItem(0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_creer->addItem(verticalSpacer_creer);

        stackedWidget->addWidget(pageCreer);
        pageModifier = new QWidget();
        pageModifier->setObjectName("pageModifier");
        verticalLayout_modifier = new QVBoxLayout(pageModifier);
        verticalLayout_modifier->setObjectName("verticalLayout_modifier");
        labelModifierTitre = new QLabel(pageModifier);
        labelModifierTitre->setObjectName("labelModifierTitre");

        verticalLayout_modifier->addWidget(labelModifierTitre);

        groupBoxModifier = new QGroupBox(pageModifier);
        groupBoxModifier->setObjectName("groupBoxModifier");
        formLayout_modifier = new QFormLayout(groupBoxModifier);
        formLayout_modifier->setObjectName("formLayout_modifier");
        formLayout_modifier->setHorizontalSpacing(15);
        formLayout_modifier->setVerticalSpacing(12);
        labelModifierId = new QLabel(groupBoxModifier);
        labelModifierId->setObjectName("labelModifierId");

        formLayout_modifier->setWidget(0, QFormLayout::LabelRole, labelModifierId);

        lineEditModifierId = new QLineEdit(groupBoxModifier);
        lineEditModifierId->setObjectName("lineEditModifierId");
        lineEditModifierId->setEnabled(false);

        formLayout_modifier->setWidget(0, QFormLayout::FieldRole, lineEditModifierId);

        labelModifierRef = new QLabel(groupBoxModifier);
        labelModifierRef->setObjectName("labelModifierRef");

        formLayout_modifier->setWidget(1, QFormLayout::LabelRole, labelModifierRef);

        lineEditModifierRef = new QLineEdit(groupBoxModifier);
        lineEditModifierRef->setObjectName("lineEditModifierRef");

        formLayout_modifier->setWidget(1, QFormLayout::FieldRole, lineEditModifierRef);

        labelModifierMontant = new QLabel(groupBoxModifier);
        labelModifierMontant->setObjectName("labelModifierMontant");

        formLayout_modifier->setWidget(2, QFormLayout::LabelRole, labelModifierMontant);

        spinBoxModifierMontant = new QDoubleSpinBox(groupBoxModifier);
        spinBoxModifierMontant->setObjectName("spinBoxModifierMontant");
        spinBoxModifierMontant->setMaximum(999999.000000000000000);

        formLayout_modifier->setWidget(2, QFormLayout::FieldRole, spinBoxModifierMontant);

        labelModifierStatut = new QLabel(groupBoxModifier);
        labelModifierStatut->setObjectName("labelModifierStatut");

        formLayout_modifier->setWidget(3, QFormLayout::LabelRole, labelModifierStatut);

        comboBoxModifierStatut = new QComboBox(groupBoxModifier);
        comboBoxModifierStatut->addItem(QString());
        comboBoxModifierStatut->addItem(QString());
        comboBoxModifierStatut->addItem(QString());
        comboBoxModifierStatut->setObjectName("comboBoxModifierStatut");

        formLayout_modifier->setWidget(3, QFormLayout::FieldRole, comboBoxModifierStatut);


        verticalLayout_modifier->addWidget(groupBoxModifier);

        horizontalLayout_modifier_btn = new QHBoxLayout();
        horizontalLayout_modifier_btn->setObjectName("horizontalLayout_modifier_btn");
        horizontalSpacer_modifier = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_modifier_btn->addItem(horizontalSpacer_modifier);

        btnModifierAnnuler = new QPushButton(pageModifier);
        btnModifierAnnuler->setObjectName("btnModifierAnnuler");
        btnModifierAnnuler->setMinimumSize(QSize(120, 40));

        horizontalLayout_modifier_btn->addWidget(btnModifierAnnuler);

        btnModifierEnregistrer = new QPushButton(pageModifier);
        btnModifierEnregistrer->setObjectName("btnModifierEnregistrer");
        btnModifierEnregistrer->setMinimumSize(QSize(120, 40));

        horizontalLayout_modifier_btn->addWidget(btnModifierEnregistrer);


        verticalLayout_modifier->addLayout(horizontalLayout_modifier_btn);

        verticalSpacer_modifier = new QSpacerItem(0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_modifier->addItem(verticalSpacer_modifier);

        stackedWidget->addWidget(pageModifier);
        pageSupprimer = new QWidget();
        pageSupprimer->setObjectName("pageSupprimer");
        verticalLayout_supprimer = new QVBoxLayout(pageSupprimer);
        verticalLayout_supprimer->setObjectName("verticalLayout_supprimer");
        labelSupprimerTitre = new QLabel(pageSupprimer);
        labelSupprimerTitre->setObjectName("labelSupprimerTitre");

        verticalLayout_supprimer->addWidget(labelSupprimerTitre);

        groupBoxSupprimer = new QGroupBox(pageSupprimer);
        groupBoxSupprimer->setObjectName("groupBoxSupprimer");
        formLayout_supprimer = new QFormLayout(groupBoxSupprimer);
        formLayout_supprimer->setObjectName("formLayout_supprimer");
        formLayout_supprimer->setHorizontalSpacing(15);
        formLayout_supprimer->setVerticalSpacing(12);
        labelSupprimerId = new QLabel(groupBoxSupprimer);
        labelSupprimerId->setObjectName("labelSupprimerId");

        formLayout_supprimer->setWidget(0, QFormLayout::LabelRole, labelSupprimerId);

        labelSupprimerIdValue = new QLabel(groupBoxSupprimer);
        labelSupprimerIdValue->setObjectName("labelSupprimerIdValue");

        formLayout_supprimer->setWidget(0, QFormLayout::FieldRole, labelSupprimerIdValue);

        labelSupprimerRef = new QLabel(groupBoxSupprimer);
        labelSupprimerRef->setObjectName("labelSupprimerRef");

        formLayout_supprimer->setWidget(1, QFormLayout::LabelRole, labelSupprimerRef);

        labelSupprimerRefValue = new QLabel(groupBoxSupprimer);
        labelSupprimerRefValue->setObjectName("labelSupprimerRefValue");

        formLayout_supprimer->setWidget(1, QFormLayout::FieldRole, labelSupprimerRefValue);

        labelSupprimerMontant = new QLabel(groupBoxSupprimer);
        labelSupprimerMontant->setObjectName("labelSupprimerMontant");

        formLayout_supprimer->setWidget(2, QFormLayout::LabelRole, labelSupprimerMontant);

        labelSupprimerMontantValue = new QLabel(groupBoxSupprimer);
        labelSupprimerMontantValue->setObjectName("labelSupprimerMontantValue");

        formLayout_supprimer->setWidget(2, QFormLayout::FieldRole, labelSupprimerMontantValue);


        verticalLayout_supprimer->addWidget(groupBoxSupprimer);

        horizontalLayout_supprimer_btn = new QHBoxLayout();
        horizontalLayout_supprimer_btn->setObjectName("horizontalLayout_supprimer_btn");
        horizontalSpacer_supprimer = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_supprimer_btn->addItem(horizontalSpacer_supprimer);

        btnSupprimerAnnuler = new QPushButton(pageSupprimer);
        btnSupprimerAnnuler->setObjectName("btnSupprimerAnnuler");
        btnSupprimerAnnuler->setMinimumSize(QSize(120, 40));

        horizontalLayout_supprimer_btn->addWidget(btnSupprimerAnnuler);

        btnSupprimerConfirmer = new QPushButton(pageSupprimer);
        btnSupprimerConfirmer->setObjectName("btnSupprimerConfirmer");
        btnSupprimerConfirmer->setMinimumSize(QSize(120, 40));

        horizontalLayout_supprimer_btn->addWidget(btnSupprimerConfirmer);


        verticalLayout_supprimer->addLayout(horizontalLayout_supprimer_btn);

        verticalSpacer_supprimer = new QSpacerItem(0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_supprimer->addItem(verticalSpacer_supprimer);

        stackedWidget->addWidget(pageSupprimer);

        verticalLayout_content->addWidget(stackedWidget);


        verticalLayout_mainScroll->addWidget(contentFrame);

        scrollAreaMain->setWidget(scrollAreaWidgetContentsMain);

        horizontalLayout_main->addWidget(scrollAreaMain);

        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        stackedWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "ERP Manufacture Pro - Gestion des Commandes", nullptr));
        labelPhoto->setText(QCoreApplication::translate("MainWindow", "<img src=\"file:///C:/Users/lynam/Downloads/cuirea.png\" width=\"50\" height=\"60\">", nullptr));
        labelUser->setText(QCoreApplication::translate("MainWindow", "ADMINISTRATEUR", nullptr));
        labelRole->setText(QCoreApplication::translate("MainWindow", "Manager Production", nullptr));
        btnTableauBord->setText(QCoreApplication::translate("MainWindow", "\360\237\223\212 Tableau de bord", nullptr));
        btnProductions->setText(QCoreApplication::translate("MainWindow", "\360\237\217\255 Productions  \342\226\274", nullptr));
        btnSubCommandes->setText(QCoreApplication::translate("MainWindow", "  \360\237\223\246 Commandes", nullptr));
        btnSubPlanification->setText(QCoreApplication::translate("MainWindow", "  \360\237\223\205 Planification", nullptr));
        btnSubStock->setText(QCoreApplication::translate("MainWindow", "  \360\237\223\212 Stock", nullptr));
        btnClients->setText(QCoreApplication::translate("MainWindow", "\360\237\221\245 Clients", nullptr));
        btnStatistiques->setText(QCoreApplication::translate("MainWindow", "\360\237\223\210 Statistiques", nullptr));
        btnRapports->setText(QCoreApplication::translate("MainWindow", "\360\237\223\204 Rapports", nullptr));
        btnParametres->setText(QCoreApplication::translate("MainWindow", "\342\232\231\357\270\217 Param\303\250tres", nullptr));
        btnDeconnexion->setText(QCoreApplication::translate("MainWindow", "\360\237\232\252 D\303\251connexion", nullptr));
        labelTitre->setText(QCoreApplication::translate("MainWindow", "Gestion des Commandes", nullptr));
        labelDate->setText(QCoreApplication::translate("MainWindow", "\360\237\227\223\357\270\217 Vendredi 30 Janvier 2026", nullptr));
        btnCreer->setText(QCoreApplication::translate("MainWindow", "\342\236\225 CR\303\211ER", nullptr));
        btnModifier->setText(QCoreApplication::translate("MainWindow", "\342\234\217\357\270\217 MODIFIER", nullptr));
        btnSupprimer->setText(QCoreApplication::translate("MainWindow", "\360\237\227\221\357\270\217 SUPPRIMER", nullptr));
        btnDupliquer->setText(QCoreApplication::translate("MainWindow", "\360\237\223\213 DUPLIQUER", nullptr));
        btnExporterPDF->setText(QCoreApplication::translate("MainWindow", "\360\237\223\204 PDF", nullptr));
        btnExporterExcel->setText(QCoreApplication::translate("MainWindow", "\360\237\223\212 EXCEL", nullptr));
        btnActualiser->setText(QCoreApplication::translate("MainWindow", "\360\237\224\204 ACTUALISER", nullptr));
        btnRetourListe->setText(QCoreApplication::translate("MainWindow", "\342\254\205\357\270\217 RETOUR", nullptr));
        labelStatTitre1->setText(QCoreApplication::translate("MainWindow", "Total Commandes", nullptr));
        labelStatValeur1->setText(QCoreApplication::translate("MainWindow", "10", nullptr));
        labelStatTitre2->setText(QCoreApplication::translate("MainWindow", "En Cours", nullptr));
        labelStatValeur2->setText(QCoreApplication::translate("MainWindow", "3", nullptr));
        labelStatTitre3->setText(QCoreApplication::translate("MainWindow", "CA Total", nullptr));
        labelStatValeur3->setText(QCoreApplication::translate("MainWindow", "138,455.50 \342\202\254", nullptr));
        labelStatTitre4->setText(QCoreApplication::translate("MainWindow", "Urgentes", nullptr));
        labelStatValeur4->setText(QCoreApplication::translate("MainWindow", "2", nullptr));
        labelHistorique->setText(QCoreApplication::translate("MainWindow", "HISTORIQUE DES COMMANDES", nullptr));
        comboBoxFiltreStatut->setItemText(0, QCoreApplication::translate("MainWindow", "\360\237\224\215 Tous statuts", nullptr));
        comboBoxFiltreStatut->setItemText(1, QCoreApplication::translate("MainWindow", "\360\237\223\235 Saisie", nullptr));
        comboBoxFiltreStatut->setItemText(2, QCoreApplication::translate("MainWindow", "\342\232\231\357\270\217 En cours", nullptr));
        comboBoxFiltreStatut->setItemText(3, QCoreApplication::translate("MainWindow", "\360\237\223\246 Exp\303\251di\303\251", nullptr));
        comboBoxFiltreStatut->setItemText(4, QCoreApplication::translate("MainWindow", "\342\234\205 Termin\303\251", nullptr));

        lineEditRecherche->setPlaceholderText(QCoreApplication::translate("MainWindow", "\360\237\224\215 Rechercher...", nullptr));
        QTableWidgetItem *___qtablewidgetitem = tableCommandes->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("MainWindow", "ID", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tableCommandes->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("MainWindow", "R\303\211F\303\211RENCE", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tableCommandes->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("MainWindow", "CLIENT", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = tableCommandes->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("MainWindow", "TYPE", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = tableCommandes->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("MainWindow", "MONTANT HT", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = tableCommandes->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("MainWindow", "DATE CR\303\211ATION", nullptr));
        QTableWidgetItem *___qtablewidgetitem6 = tableCommandes->horizontalHeaderItem(6);
        ___qtablewidgetitem6->setText(QCoreApplication::translate("MainWindow", "DATE LIVRAISON", nullptr));
        QTableWidgetItem *___qtablewidgetitem7 = tableCommandes->horizontalHeaderItem(7);
        ___qtablewidgetitem7->setText(QCoreApplication::translate("MainWindow", "STATUT", nullptr));
        QTableWidgetItem *___qtablewidgetitem8 = tableCommandes->horizontalHeaderItem(8);
        ___qtablewidgetitem8->setText(QCoreApplication::translate("MainWindow", "PRIORIT\303\211", nullptr));
        labelCreerTitre->setText(QCoreApplication::translate("MainWindow", "\360\237\223\246 Nouvelle Commande", nullptr));
        groupBoxCreer->setTitle(QCoreApplication::translate("MainWindow", "Informations", nullptr));
        labelCreerRef->setText(QCoreApplication::translate("MainWindow", "R\303\251f\303\251rence :", nullptr));
        labelCreerClient->setText(QCoreApplication::translate("MainWindow", "Client :", nullptr));
        comboBoxCreerClient->setItemText(0, QCoreApplication::translate("MainWindow", "TechCorp SA", nullptr));
        comboBoxCreerClient->setItemText(1, QCoreApplication::translate("MainWindow", "InnoSystems", nullptr));

        labelCreerMontant->setText(QCoreApplication::translate("MainWindow", "Montant HT :", nullptr));
        spinBoxCreerMontant->setSuffix(QCoreApplication::translate("MainWindow", " \342\202\254", nullptr));
        btnCreerAnnuler->setText(QCoreApplication::translate("MainWindow", "\342\235\214 Annuler", nullptr));
        btnCreerEnregistrer->setText(QCoreApplication::translate("MainWindow", "\342\234\205 Enregistrer", nullptr));
        labelModifierTitre->setText(QCoreApplication::translate("MainWindow", "\342\234\217\357\270\217 Modifier Commande", nullptr));
        groupBoxModifier->setTitle(QCoreApplication::translate("MainWindow", "Informations", nullptr));
        labelModifierId->setText(QCoreApplication::translate("MainWindow", "ID :", nullptr));
        labelModifierRef->setText(QCoreApplication::translate("MainWindow", "R\303\251f\303\251rence :", nullptr));
        labelModifierMontant->setText(QCoreApplication::translate("MainWindow", "Montant HT :", nullptr));
        spinBoxModifierMontant->setSuffix(QCoreApplication::translate("MainWindow", " \342\202\254", nullptr));
        labelModifierStatut->setText(QCoreApplication::translate("MainWindow", "Statut :", nullptr));
        comboBoxModifierStatut->setItemText(0, QCoreApplication::translate("MainWindow", "\360\237\223\235 Saisie", nullptr));
        comboBoxModifierStatut->setItemText(1, QCoreApplication::translate("MainWindow", "\342\232\231\357\270\217 En cours", nullptr));
        comboBoxModifierStatut->setItemText(2, QCoreApplication::translate("MainWindow", "\342\234\205 Termin\303\251", nullptr));

        btnModifierAnnuler->setText(QCoreApplication::translate("MainWindow", "\342\235\214 Annuler", nullptr));
        btnModifierEnregistrer->setText(QCoreApplication::translate("MainWindow", "\360\237\222\276 Enregistrer", nullptr));
        labelSupprimerTitre->setText(QCoreApplication::translate("MainWindow", "\342\232\240\357\270\217 Supprimer Commande", nullptr));
        groupBoxSupprimer->setTitle(QCoreApplication::translate("MainWindow", "D\303\251tails", nullptr));
        labelSupprimerId->setText(QCoreApplication::translate("MainWindow", "ID :", nullptr));
        labelSupprimerIdValue->setText(QCoreApplication::translate("MainWindow", "-", nullptr));
        labelSupprimerRef->setText(QCoreApplication::translate("MainWindow", "R\303\251f\303\251rence :", nullptr));
        labelSupprimerRefValue->setText(QCoreApplication::translate("MainWindow", "-", nullptr));
        labelSupprimerMontant->setText(QCoreApplication::translate("MainWindow", "Montant :", nullptr));
        labelSupprimerMontantValue->setText(QCoreApplication::translate("MainWindow", "-", nullptr));
        btnSupprimerAnnuler->setText(QCoreApplication::translate("MainWindow", "\342\206\251\357\270\217 Non, Annuler", nullptr));
        btnSupprimerConfirmer->setText(QCoreApplication::translate("MainWindow", "\360\237\227\221\357\270\217 Oui, Supprimer", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
