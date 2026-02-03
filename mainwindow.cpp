#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDate>
#include <QLocale>
#include <QTime>
#include <QTimer>
#include <QHeaderView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QString styleSheet = R"(
* {
    font-family: "Segoe UI", "Calibri", sans-serif;
}

QMainWindow {
    background: #E8E3DC;
}

#centralwidget {
    background: #E8E3DC;
}

QScrollArea {
    background: transparent;
    border: none;
}

QScrollArea > QWidget > QWidget {
    background: transparent;
}

#scrollAreaSidebar {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #1A1108, stop:0.5 #291C0E, stop:1 #3D2A18);
}

#scrollAreaMain {
    background: #E8E3DC;
}

#sidebarFrame {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #1A1108, stop:0.5 #291C0E, stop:1 #3D2A18);
    border-right: 1px solid #6E473B;
}

#profileFrame {
    background: rgba(110, 71, 59, 0.25);
    border-radius: 10px;
    padding: 15px 10px;
    margin: 10px;
}

#labelPhoto {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
        stop:0 #6E473B, stop:1 #A78D78);
    border: 3px solid #A78D78;
    border-radius: 30px;
    color: #FFFFFF;
    font-size: 32px;
    font-weight: bold;
}

#labelUser {
    color: #FFFFFF;
    font-weight: 700;
    font-size: 11px;
    letter-spacing: 1px;
}

#labelRole {
    color: #BEB5A9;
    font-weight: 500;
    font-size: 10px;
}

#sidebarFrame QPushButton {
    background-color: transparent;
    border: none;
    border-left: 3px solid transparent;
    color: #BEB5A9;
    text-align: left;
    font-weight: 600;
    font-size: 12px;
    padding: 12px 18px;
    margin: 3px 8px;
    border-radius: 6px;
}

#sidebarFrame QPushButton:hover {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
        stop:0 rgba(110, 71, 59, 0.7), stop:1 rgba(167, 141, 120, 0.4));
    border-left: 3px solid #A78D78;
    color: #FFFFFF;
}

#sidebarFrame QPushButton:pressed,
#sidebarFrame QPushButton:checked {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
        stop:0 #6E473B, stop:1 rgba(167, 141, 120, 0.5));
    border-left: 3px solid #E1D4C2;
    color: #FFFFFF;
}

#productionsSubMenu {
    background: rgba(41, 28, 14, 0.6);
    margin-left: 20px;
    border-radius: 6px;
    padding: 4px;
}

#btnDeconnexion {
    color: #EF5350 !important;
}

#btnDeconnexion:hover {
    background: rgba(239, 83, 80, 0.2) !important;
    border-left: 3px solid #EF5350 !important;
}

Line {
    background: rgba(190, 181, 169, 0.3);
}

#contentFrame {
    background-color: #E8E3DC;
}

#headerFrame {
    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
        stop:0 #FFFFFF, stop:1 #FDFCFA);
    border: 2px solid #D0C8BE;
    border-radius: 12px;
    padding: 15px 20px;
}

#labelTitre {
    color: #291C0E;
    font-size: 24px;
    font-weight: 800;
    letter-spacing: 1px;
}

#labelDate {
    color: #6E473B;
    font-size: 11px;
    font-weight: 600;
    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
        stop:0 #E8E3DC, stop:1 #E1D4C2);
    padding: 6px 12px;
    border-radius: 6px;
}

#headerFrame QPushButton {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #6E473B, stop:1 #5A3A2F);
    color: #FFFFFF;
    border: 2px solid #6E473B;
    border-radius: 8px;
    padding: 9px 14px;
    font-weight: 700;
    font-size: 10px;
    letter-spacing: 0.5px;
}

#headerFrame QPushButton:hover {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #A78D78, stop:1 #8B7566);
    border-color: #A78D78;
    color: #291C0E;
}

#frameStatistiques {
    background: transparent;
}


#statFrame1, #statFrame2, #statFrame3, #statFrame4 {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #FFFFFF, stop:1 #FAFAFA);
    border: none;
    border-radius: 16px;
    padding: 18px 16px;
    min-height: 120px;
    box-shadow: 0 4px 12px rgba(0, 0, 0, 0.08);
}

#statFrame1 {
    border-left: 6px solid #2196F3;
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
        stop:0 rgba(33, 150, 243, 0.05), stop:1 #FFFFFF);
}

#statFrame2 {
    border-left: 6px solid #FF9800;
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
        stop:0 rgba(255, 152, 0, 0.05), stop:1 #FFFFFF);
}

#statFrame3 {
    border-left: 6px solid #4CAF50;
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
        stop:0 rgba(76, 175, 80, 0.05), stop:1 #FFFFFF);
}

#statFrame4 {
    border-left: 6px solid #F44336;
    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
        stop:0 rgba(244, 67, 54, 0.05), stop:1 #FFFFFF);
}

#labelStatTitre1, #labelStatTitre2, #labelStatTitre3, #labelStatTitre4 {
    color: #6E473B;
    font-size: 12px;
    font-weight: 700;
    letter-spacing: 1.2px;
    text-transform: uppercase;
    padding: 0px 0px 8px 0px;
}

#labelStatValeur1, #labelStatValeur2, #labelStatValeur3, #labelStatValeur4 {
    color: #291C0E;
    font-size: 36px;
    font-weight: 800;
    letter-spacing: 0px;
    padding: 5px 0px;
}

/* Effet hover sur les KPI cards */
#statFrame1:hover, #statFrame2:hover, #statFrame3:hover, #statFrame4:hover {
    transform: translateY(-2px);
    box-shadow: 0 6px 20px rgba(0, 0, 0, 0.12);
}

#frameHistorique {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #FFFFFF, stop:1 #FDFCFA);
    border: 3px solid #BEB5A9;
    border-radius: 12px;
    padding: 15px;
}

#labelHistorique {
    color: #291C0E;
    font-size: 14px;
    font-weight: 800;
    background: rgba(110, 71, 59, 0.1);
    padding: 10px 15px;
    border-radius: 6px;
    border-left: 4px solid #6E473B;
}

#lineEditRecherche {
    background-color: #FFFFFF;
    border: 2px solid #D0C8BE;
    border-radius: 8px;
    padding: 8px 12px;
    color: #291C0E;
    font-size: 12px;
    font-weight: 500;
}

#lineEditRecherche:focus {
    border: 3px solid #6E473B;
    padding: 7px 11px;
}

QTableWidget {
    background-color: #FFFFFF;
    alternate-background-color: #F8F6F3;
    border: none;
    gridline-color: #E8E3DC;
    selection-background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0,
        stop:0 #6E473B, stop:1 #8B6A5C);
    selection-color: #FFFFFF;
    font-size: 11px;
    font-weight: 500;
color: #000000;
/* 1. Couleur de fond quand on clique (Marron comme ton en-tête) */
    selection-background-color: #6E473B;
    selection-color: #000000;
    color: #000000;
}
QTableWidget::item:selected {
    background-color: #6E473B;
    color: #000000;
}
QTableWidget::item {
    padding: 8px;
    border: none;
}

QTableWidget::item:hover {
    background-color: rgba(110, 71, 59, 0.08);
}

QHeaderView::section {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #6E473B, stop:1 #5A3A2F);
    color: #FFFFFF;
    padding: 10px 8px;
    border: none;
    font-weight: 700;
    font-size: 10px;
    letter-spacing: 1px;
    text-transform: uppercase;
}

QHeaderView::section:first {
    border-top-left-radius: 8px;
}

QHeaderView::section:last {
    border-top-right-radius: 8px;
}

QComboBox {
    background-color: #FFFFFF;
    border: 2px solid #D0C8BE;
    border-radius: 8px;
    padding: 8px 12px;
    color: #291C0E;
    font-size: 12px;
    font-weight: 500;
    min-height: 30px;
}

QComboBox:hover {
    border: 2px solid #6E473B;
}

QComboBox:focus {
    border: 3px solid #6E473B;
    padding: 7px 11px;
}

QComboBox::drop-down {
    subcontrol-origin: padding;
    subcontrol-position: top right;
    width: 25px;
    border-left: 1px solid #D0C8BE;
    border-top-right-radius: 6px;
    border-bottom-right-radius: 6px;
}

QComboBox::down-arrow {
    image: none;
    width: 0;
    height: 0;
    border-left: 5px solid transparent;
    border-right: 5px solid transparent;
    border-top: 7px solid #6E473B;
}

QComboBox QAbstractItemView {
    background-color: #FFFFFF;
    border: 2px solid #6E473B;
    border-radius: 6px;
    selection-background-color: #6E473B;
    selection-color: #FFFFFF;
    padding: 5px;
}

#stackedWidget {
    background: transparent;
}

#pageListe, #pageCreer, #pageModifier, #pageSupprimer {
    background: transparent;
}

QGroupBox {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #FFFFFF, stop:1 #FDFCFA);
    border: 3px solid #BEB5A9;
    border-radius: 12px;
    margin-top: 20px;
    padding: 15px;
    font-weight: 700;
    font-size: 13px;
    color: #6E473B;
}

QGroupBox::title {
    subcontrol-origin: margin;
    subcontrol-position: top left;
    padding: 5px 15px;
    background: #6E473B;
    color: #FFFFFF;
    border-radius: 6px;
    margin-left: 10px;
}

QLabel {
    color: #291C0E;
    font-size: 12px;
    font-weight: 500;
}

QLineEdit {
    background-color: #FFFFFF;
    border: 2px solid #D0C8BE;
    border-radius: 8px;
    padding: 8px 12px;
    color: #291C0E;
    font-size: 12px;
    font-weight: 500;
    min-height: 30px;
}

QLineEdit:focus {
    border: 3px solid #6E473B;
    padding: 7px 11px;
}

QDoubleSpinBox, QSpinBox {
    background-color: #FFFFFF;
    border: 2px solid #D0C8BE;
    border-radius: 8px;
    padding: 8px 12px;
    color: #291C0E;
    font-size: 12px;
    font-weight: 500;
    min-height: 30px;
}

QDoubleSpinBox:focus, QSpinBox:focus {
    border: 3px solid #6E473B;
    padding: 7px 11px;
}

QPushButton {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #6E473B, stop:1 #5A3A2F);
    color: #FFFFFF;
    border: 2px solid #6E473B;
    border-radius: 8px;
    padding: 10px 16px;
    font-weight: 700;
    font-size: 11px;
    letter-spacing: 0.5px;
    min-width: 90px;
}

QPushButton:hover {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #A78D78, stop:1 #8B7566);
    border-color: #A78D78;
    color: #291C0E;
}

QPushButton:pressed {
    background: #5A3A2F;
    border-color: #5A3A2F;
    color: #FFFFFF;
}

#btnModifierAnnuler, #btnSupprimerAnnuler, #btnCreerAnnuler {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #E0E0E0, stop:1 #BDBDBD);
    color: #424242;
    border: 2px solid #BDBDBD;
}

#btnModifierAnnuler:hover, #btnSupprimerAnnuler:hover, #btnCreerAnnuler:hover {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #F5F5F5, stop:1 #E0E0E0);
    color: #212121;
    border-color: #9E9E9E;
}

#btnSupprimerConfirmer {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #F44336, stop:1 #D32F2F);
    color: #FFFFFF;
    border: 2px solid #F44336;
}

#btnSupprimerConfirmer:hover {
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
        stop:0 #E57373, stop:1 #EF5350);
    border-color: #E57373;
}

#labelCreerTitre, #labelModifierTitre, #labelSupprimerTitre {
    font-size: 20px;
    font-weight: 800;
    color: #291C0E;
    padding: 10px 0px;
    letter-spacing: 1px;
}
)";

    this->setStyleSheet(styleSheet);

    // ========================================
    // INITIALISATION
    // ========================================

    initialiserTableau();
    calculerStatistiques();
    ameliorerAffichageKPI();

    // Date et heure
    QLocale locale(QLocale::French, QLocale::France);
    QString dateActuelle = locale.toString(QDate::currentDate(), "dddd dd MMMM yyyy");
    dateActuelle[0] = dateActuelle[0].toUpper();
    ui->labelDate->setText("🗓️ " + dateActuelle);

    // ========================================
    // CONNEXION DES SIGNAUX
    // ========================================

    // Filtrage
    connect(ui->comboBoxFiltreStatut, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::filtrerParStatut);

    // Chargement des données avant navigation
    connect(ui->btnModifier, &QPushButton::clicked, this, [this]() {
        if (ui->tableCommandes->currentRow() >= 0) {
            chargerDonneesModifier();
            ui->stackedWidget->setCurrentIndex(2);
        } else {
            QMessageBox::warning(this, "Sélection", "⚠️ Veuillez sélectionner une commande !");
        }
    });

    connect(ui->btnSupprimer, &QPushButton::clicked, this, [this]() {
        if (ui->tableCommandes->currentRow() >= 0) {
            chargerDonneesSupprimer();
            ui->stackedWidget->setCurrentIndex(3);
        } else {
            QMessageBox::warning(this, "Sélection", "⚠️ Veuillez sélectionner une commande !");
        }
    });

    // Navigation
    connect(ui->btnCreer, &QPushButton::clicked, this, [this]() {
        viderFormulaireCreer();
        ui->stackedWidget->setCurrentIndex(1);
    });

    connect(ui->btnDupliquer, &QPushButton::clicked, this, &MainWindow::dupliquerCommande);

    connect(ui->btnRetourListe, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentIndex(0);
    });

    // Boutons d'annulation
    connect(ui->btnCreerAnnuler, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentIndex(0);
    });

    connect(ui->btnModifierAnnuler, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentIndex(0);
    });

    connect(ui->btnSupprimerAnnuler, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentIndex(0);
    });

    // Validation et enregistrement
    connect(ui->btnCreerEnregistrer, &QPushButton::clicked, this, [this]() {
        if (validerFormulaireCreer()) {
            ajouterNouvelleCommande();
            calculerStatistiques();
            ameliorerAffichageKPI();
            QMessageBox::information(this, "Succès", "✅ Commande créée avec succès !");
            ui->stackedWidget->setCurrentIndex(0);
        }
    });

    connect(ui->btnModifierEnregistrer, &QPushButton::clicked, this, [this]() {
        if (validerFormulaireModifier()) {
            mettreAJourCommande();
            calculerStatistiques();
            ameliorerAffichageKPI();
            QMessageBox::information(this, "Succès", "💾 Commande modifiée avec succès !");
            ui->stackedWidget->setCurrentIndex(0);
        }
    });

    connect(ui->btnSupprimerConfirmer, &QPushButton::clicked, this, [this]() {
        supprimerCommande();
        calculerStatistiques();
        ameliorerAffichageKPI();
        QMessageBox::information(this, "Suppression", "🗑️ Commande supprimée avec succès !");
        ui->stackedWidget->setCurrentIndex(0);
    });

    // ======================================
    // MENU DÉROULANT PRODUCTIONS
    // ======================================

    connect(ui->btnProductions, &QPushButton::toggled, this, [this](bool checked) {
        ui->productionsSubMenu->setVisible(checked);
        if (checked) {
            ui->btnProductions->setText("🏭 Productions  ▲");
        } else {
            ui->btnProductions->setText("🏭 Productions  ▼");
        }
    });

    connect(ui->btnSubCommandes, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentIndex(0);
        ui->btnProductions->setChecked(false);
    });

    // ======================================
    // RECHERCHE
    // ======================================

    connect(ui->lineEditRecherche, &QLineEdit::textChanged, this, &MainWindow::rechercherCommandes);

    // Page par défaut
    ui->stackedWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// =====================================================================
// AMÉLIORATION MAJEURE DE L'AFFICHAGE DES KPI
// =====================================================================
void MainWindow::ameliorerAffichageKPI()
{
    // ===== KPI 1: Total Commandes =====
    if (ui->labelStatValeur1) {
        int total = ui->tableCommandes->rowCount();

        QString html = QString(
                           "<div style='text-align: center;'>"
                           "  <div style='font-size: 42px; font-weight: 800; color: #2196F3; margin: 8px 0px;'>%1</div>"
                           "  <div style='font-size: 11px; color: #28a745; font-weight: 600; margin-top: 5px;'>"
                           "    <span style='font-size: 14px;'>▲</span> +12% vs mois dernier"
                           "  </div>"
                           "</div>"
                           ).arg(total);

        ui->labelStatValeur1->setText(html);
        ui->labelStatValeur1->setTextFormat(Qt::RichText);
    }

    // ===== KPI 2: En Cours =====
    if (ui->labelStatValeur2) {
        int enCours = 0;
        for (int row = 0; row < ui->tableCommandes->rowCount(); ++row) {
            QString statut = ui->tableCommandes->item(row, 7)->text();
            if (statut.contains("En cours")) {
                enCours++;
            }
        }

        QString html = QString(
                           "<div style='text-align: center;'>"
                           "  <div style='font-size: 42px; font-weight: 800; color: #FF9800; margin: 8px 0px;'>%1</div>"
                           "  <div style='font-size: 11px; color: #28a745; font-weight: 600; margin-top: 5px;'>"
                           "    <span style='font-size: 14px;'>▲</span> +8% activité"
                           "  </div>"
                           "</div>"
                           ).arg(enCours);

        ui->labelStatValeur2->setText(html);
        ui->labelStatValeur2->setTextFormat(Qt::RichText);
    }

    // ===== KPI 3: CA Total =====
    if (ui->labelStatValeur3) {
        double caTotal = 0.0;
        for (int row = 0; row < ui->tableCommandes->rowCount(); ++row) {
            QString montantStr = ui->tableCommandes->item(row, 4)->text();
            montantStr.remove(" €").replace(",", "").replace(" ", "");
            caTotal += montantStr.toDouble();
        }

        QLocale locale(QLocale::French);
        QString caFormate = locale.toString(caTotal, 'f', 0);

        QString html = QString(
                           "<div style='text-align: center;'>"
                           "  <div style='font-size: 34px; font-weight: 800; color: #4CAF50; margin: 8px 0px;'>%1 €</div>"
                           "  <div style='font-size: 11px; color: #28a745; font-weight: 600; margin-top: 5px;'>"
                           "    <span style='font-size: 14px;'>▲</span> +15% croissance"
                           "  </div>"
                           "</div>"
                           ).arg(caFormate);

        ui->labelStatValeur3->setText(html);
        ui->labelStatValeur3->setTextFormat(Qt::RichText);
    }

    // ===== KPI 4: Urgentes =====
    if (ui->labelStatValeur4) {
        int urgentes = 0;
        for (int row = 0; row < ui->tableCommandes->rowCount(); ++row) {
            QString priorite = ui->tableCommandes->item(row, 8)->text();
            if (priorite.contains("Urgente")) {
                urgentes++;
            }
        }

        QString html = QString(
                           "<div style='text-align: center;'>"
                           "  <div style='font-size: 42px; font-weight: 800; color: #F44336; margin: 8px 0px;'>%1</div>"
                           "  <div style='font-size: 11px; color: #dc3545; font-weight: 600; margin-top: 5px;'>"
                           "    <span style='font-size: 14px;'>▼</span> -5% amélioration"
                           "  </div>"
                           "</div>"
                           ).arg(urgentes);

        ui->labelStatValeur4->setText(html);
        ui->labelStatValeur4->setTextFormat(Qt::RichText);
    }

    // Mettre à jour les titres des KPI avec des icônes
    if (ui->labelStatTitre1) {
        ui->labelStatTitre1->setText("📦 TOTAL COMMANDES");
    }
    if (ui->labelStatTitre2) {
        ui->labelStatTitre2->setText("⚙️ EN COURS");
    }
    if (ui->labelStatTitre3) {
        ui->labelStatTitre3->setText("💰 CA TOTAL");
    }
    if (ui->labelStatTitre4) {
        ui->labelStatTitre4->setText("🔴 URGENTES");
    }
}

void MainWindow::initialiserTableau()
{
    QStringList headers = {"ID", "RÉFÉRENCE", "CLIENT", "TYPE", "MONTANT HT",
                           "DATE CRÉATION", "DATE LIVRAISON", "STATUT", "PRIORITÉ"};
    ui->tableCommandes->setColumnCount(headers.size());
    ui->tableCommandes->setHorizontalHeaderLabels(headers);

    // Activer l'alternance de couleurs pour les lignes
    ui->tableCommandes->setAlternatingRowColors(true);

    // Désactiver l'édition directe
    ui->tableCommandes->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Sélection par ligne entière
    ui->tableCommandes->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableCommandes->setSelectionMode(QAbstractItemView::SingleSelection);

    // Améliorer la grille
    ui->tableCommandes->setShowGrid(true);
    ui->tableCommandes->setGridStyle(Qt::SolidLine);

    QList<QStringList> donnees = {
                                  {"001", "CMD-2026-001", "TechCorp SA", "Standard", "12,450.00 €", "01/02/2026", "28/02/2026", "⚙️ En cours", "🟡 Normale"},
                                  {"002", "CMD-2026-002", "InnoSystems", "Express", "8,320.50 €", "28/01/2026", "15/02/2026", "✅ Terminé", "🔴 Urgente"},
                                  {"003", "VTE-2026-003", "GlobalTech", "Vente", "5,670.00 €", "30/01/2026", "10/03/2026", "📝 Saisie", "🟢 Basse"},
                                  {"004", "CMD-2026-004", "MetalWorks", "Standard", "15,890.00 €", "25/01/2026", "05/03/2026", "📦 Expédié", "🟡 Normale"},
                                  {"005", "CMD-2026-005", "AutoParts Pro", "Express", "22,100.00 €", "29/01/2026", "12/02/2026", "⚙️ En cours", "🔴 Urgente"},
                                  {"006", "CMD-2026-006", "TechCorp SA", "Standard", "9,875.00 €", "02/02/2026", "01/03/2026", "📝 Saisie", "🟡 Normale"},
                                  {"007", "VTE-2026-007", "InnoSystems", "Vente", "18,500.00 €", "24/01/2026", "20/02/2026", "⚙️ En cours", "🟠 Élevée"},
                                  {"008", "CMD-2026-008", "GlobalTech", "Devis", "13,220.00 €", "01/02/2026", "25/03/2026", "📝 Saisie", "🟢 Basse"},
                                  {"009", "CMD-2026-009", "MetalWorks", "Standard", "7,650.00 €", "27/01/2026", "18/03/2026", "⚙️ En cours", "🟡 Normale"},
                                  {"010", "CMD-2026-010", "AutoParts Pro", "Express", "24,890.00 €", "26/01/2026", "10/03/2026", "✅ Terminé", "🟠 Élevée"},
                                  };

    ui->tableCommandes->setRowCount(donnees.size());
    for (int row = 0; row < donnees.size(); ++row) {
        for (int col = 0; col < donnees[row].size(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(donnees[row][col]);
            item->setTextAlignment(Qt::AlignCenter);

            // Colorer selon le statut
            if (col == 7) { // Colonne Statut
                QString statut = donnees[row][col];
                if (statut.contains("Terminé")) {
                    item->setBackground(QColor(212, 237, 218));
                    item->setForeground(QColor(25, 135, 84));
                } else if (statut.contains("En cours")) {
                    item->setBackground(QColor(255, 243, 205));
                    item->setForeground(QColor(133, 100, 4));
                } else if (statut.contains("Expédié")) {
                    item->setBackground(QColor(207, 226, 255));
                    item->setForeground(QColor(13, 110, 253));
                }
            }

            // Colorer selon la priorité
            if (col == 8) { // Colonne Priorité
                QString priorite = donnees[row][col];
                if (priorite.contains("Urgente")) {
                    item->setBackground(QColor(248, 215, 218));
                    item->setForeground(QColor(220, 53, 69));
                } else if (priorite.contains("Élevée")) {
                    item->setBackground(QColor(255, 237, 213));
                    item->setForeground(QColor(253, 126, 20));
                }
            }

            ui->tableCommandes->setItem(row, col, item);
        }

        // Ajuster la hauteur des lignes
        ui->tableCommandes->setRowHeight(row, 45);
    }

    // Ajuster les colonnes
    ui->tableCommandes->resizeColumnsToContents();

    // Étirer certaines colonnes
    ui->tableCommandes->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch); // Client
    ui->tableCommandes->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch); // Référence

    if (ui->tableCommandes->rowCount() > 0) {
        ui->tableCommandes->selectRow(0);
    }
}

void MainWindow::calculerStatistiques()
{
    int total = ui->tableCommandes->rowCount();
    int enCours = 0;
    int urgentes = 0;
    double caTotal = 0.0;

    for (int row = 0; row < total; ++row) {
        // Compter les "En cours"
        QString statut = ui->tableCommandes->item(row, 7)->text();
        if (statut.contains("En cours")) {
            enCours++;
        }

        // Compter les urgentes
        QString priorite = ui->tableCommandes->item(row, 8)->text();
        if (priorite.contains("Urgente")) {
            urgentes++;
        }

        // Calculer CA total
        QString montantStr = ui->tableCommandes->item(row, 4)->text();
        montantStr.remove(" €").replace(",", "");
        caTotal += montantStr.toDouble();
    }

    // Les valeurs seront mises à jour par ameliorerAffichageKPI()
    // qui sera appelé juste après
}

void MainWindow::filtrerParStatut(int index)
{
    QString filtre = ui->comboBoxFiltreStatut->currentText();

    for (int row = 0; row < ui->tableCommandes->rowCount(); ++row) {
        bool visible = true;

        if (index > 0) { // Si pas "Tous les statuts"
            QString statut = ui->tableCommandes->item(row, 7)->text();
            visible = statut.contains(filtre.mid(2)); // Enlever l'emoji
        }

        ui->tableCommandes->setRowHidden(row, !visible);
    }
}

void MainWindow::rechercherCommandes(const QString &text)
{
    for (int row = 0; row < ui->tableCommandes->rowCount(); ++row) {
        bool match = false;
        for (int col = 0; col < ui->tableCommandes->columnCount(); ++col) {
            QTableWidgetItem *item = ui->tableCommandes->item(row, col);
            if (item && item->text().contains(text, Qt::CaseInsensitive)) {
                match = true;
                break;
            }
        }
        ui->tableCommandes->setRowHidden(row, !match);
    }
}

void MainWindow::dupliquerCommande()
{
    int row = ui->tableCommandes->currentRow();
    if (row < 0) return;

    int newRow = ui->tableCommandes->rowCount();
    ui->tableCommandes->insertRow(newRow);

    QString newId = QString::number(newRow + 1).rightJustified(3, '0');

    ui->tableCommandes->setItem(newRow, 0, new QTableWidgetItem(newId));

    for (int col = 1; col < ui->tableCommandes->columnCount(); ++col) {
        QString text = ui->tableCommandes->item(row, col)->text();
        if (col == 1) { // Référence
            text += "-COPIE";
        }
        QTableWidgetItem *item = new QTableWidgetItem(text);
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableCommandes->setItem(newRow, col, item);
    }

    ui->tableCommandes->setRowHeight(newRow, 45);

    calculerStatistiques();
    ameliorerAffichageKPI();
    QMessageBox::information(this, "Duplication", "📋 Commande dupliquée avec succès !");
}

void MainWindow::viderFormulaireCreer()
{
    ui->lineEditCreerRef->clear();
    ui->comboBoxCreerClient->setCurrentIndex(-1);
    ui->spinBoxCreerMontant->setValue(0.0);
}

void MainWindow::chargerDonneesModifier()
{
    int row = ui->tableCommandes->currentRow();
    if (row < 0) return;

    ui->lineEditModifierId->setText(ui->tableCommandes->item(row, 0)->text());
    ui->lineEditModifierRef->setText(ui->tableCommandes->item(row, 1)->text());

    QString montant = ui->tableCommandes->item(row, 4)->text();
    montant.remove(" €").replace(",", "");
    ui->spinBoxModifierMontant->setValue(montant.toDouble());
}

void MainWindow::chargerDonneesSupprimer()
{
    int row = ui->tableCommandes->currentRow();
    if (row < 0) return;

    ui->labelSupprimerIdValue->setText(ui->tableCommandes->item(row, 0)->text());
    ui->labelSupprimerRefValue->setText(ui->tableCommandes->item(row, 1)->text());
    ui->labelSupprimerMontantValue->setText(ui->tableCommandes->item(row, 4)->text());
}

bool MainWindow::validerFormulaireCreer()
{
    if (ui->lineEditCreerRef->text().isEmpty()) {
        QMessageBox::warning(this, "Validation", "❌ La référence est obligatoire !");
        return false;
    }

    if (ui->comboBoxCreerClient->currentText().isEmpty()) {
        QMessageBox::warning(this, "Validation", "❌ Le client est obligatoire !");
        return false;
    }

    if (ui->spinBoxCreerMontant->value() <= 0) {
        QMessageBox::warning(this, "Validation", "❌ Le montant doit être supérieur à 0 !");
        return false;
    }

    return true;
}

bool MainWindow::validerFormulaireModifier()
{
    if (ui->lineEditModifierRef->text().isEmpty()) {
        QMessageBox::warning(this, "Validation", "❌ La référence est obligatoire !");
        return false;
    }

    if (ui->spinBoxModifierMontant->value() <= 0) {
        QMessageBox::warning(this, "Validation", "❌ Le montant doit être supérieur à 0 !");
        return false;
    }

    return true;
}

void MainWindow::ajouterNouvelleCommande()
{
    int newRow = ui->tableCommandes->rowCount();
    ui->tableCommandes->insertRow(newRow);

    QString newId = QString::number(newRow + 1).rightJustified(3, '0');

    ui->tableCommandes->setItem(newRow, 0, new QTableWidgetItem(newId));
    ui->tableCommandes->setItem(newRow, 1, new QTableWidgetItem(ui->lineEditCreerRef->text()));
    ui->tableCommandes->setItem(newRow, 2, new QTableWidgetItem(ui->comboBoxCreerClient->currentText()));
    ui->tableCommandes->setItem(newRow, 3, new QTableWidgetItem("Standard"));

    QLocale locale(QLocale::French);
    QString montantFormate = locale.toString(ui->spinBoxCreerMontant->value(), 'f', 2);
    ui->tableCommandes->setItem(newRow, 4, new QTableWidgetItem(montantFormate + " €"));
    ui->tableCommandes->setItem(newRow, 5, new QTableWidgetItem(QDate::currentDate().toString("dd/MM/yyyy")));
    ui->tableCommandes->setItem(newRow, 6, new QTableWidgetItem(QDate::currentDate().addDays(14).toString("dd/MM/yyyy")));
    ui->tableCommandes->setItem(newRow, 7, new QTableWidgetItem("📝 Saisie"));
    ui->tableCommandes->setItem(newRow, 8, new QTableWidgetItem("🟡 Normale"));

    ui->tableCommandes->setRowHeight(newRow, 45);
}

void MainWindow::mettreAJourCommande()
{
    int row = ui->tableCommandes->currentRow();
    if (row < 0) return;

    ui->tableCommandes->item(row, 1)->setText(ui->lineEditModifierRef->text());

    QLocale locale(QLocale::French);
    QString montantFormate = locale.toString(ui->spinBoxModifierMontant->value(), 'f', 2);
    ui->tableCommandes->item(row, 4)->setText(montantFormate + " €");
    ui->tableCommandes->item(row, 7)->setText(ui->comboBoxModifierStatut->currentText());
}

void MainWindow::supprimerCommande()
{
    int row = ui->tableCommandes->currentRow();
    if (row >= 0) {
        ui->tableCommandes->removeRow(row);
    }
}
