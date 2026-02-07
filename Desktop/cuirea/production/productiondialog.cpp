#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QInputDialog>
#include <QMenu>
#include <QAction>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDateEdit>
#include <QDate>
#include <QPrinter>
#include <QPainter>
#include <QFileDialog>
#include <QTextDocument>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QFile>
#include <QTextStream>
#include <QStringConverter>
#include <QMap>
#include <algorithm>
#include <cstdlib>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    loadSampleData();
    updateStatistics();
    
    // Connecter les boutons
    connect(ui->btnCreer, &QPushButton::clicked, this, &MainWindow::onCreerClicked);
    connect(ui->btnModifier, &QPushButton::clicked, this, &MainWindow::onModifierClicked);
    connect(ui->btnSuivi, &QPushButton::clicked, this, &MainWindow::onSuiviClicked);
    connect(ui->btnPlanification, &QPushButton::clicked, this, &MainWindow::onPlanificationClicked);
    connect(ui->btnPDF, &QPushButton::clicked, this, &MainWindow::onPDFClicked);
    connect(ui->btnExcel, &QPushButton::clicked, this, &MainWindow::onExcelClicked);
    connect(ui->btnRetour, &QPushButton::clicked, this, &MainWindow::onRetourClicked);
    connect(ui->btnTousStatuts, &QPushButton::clicked, this, &MainWindow::onTousStatutsClicked);
    connect(ui->searchBox, &QLineEdit::textChanged, this, &MainWindow::onRechercherTextChanged);
    
    // Activer le menu contextuel sur la table
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableWidget, &QTableWidget::customContextMenuRequested, this, &MainWindow::showContextMenu);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadSampleData()
{
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setAlternatingRowColors(true);
    
    QString data[][9] = {
        {"001", "CMD-2026-001", "TechCorp SA", "Standard", "12,450.00 €", "01/02/2026", "25/02/2026", "En cours", "Normale"},
        {"002", "CMD-2026-002", "InnoSystems", "Express", "8,523.50 €", "28/01/2026", "15/02/2026", "Terminé", "Urgente"},
        {"003", "VTE-2026-003", "GlobalTech", "Vente", "5,870.00 €", "30/01/2026", "10/03/2026", "Saisie", "Basse"},
        {"004", "CMD-2026-004", "MetalWorks", "Standard", "18,890.00 €", "25/01/2026", "05/03/2026", "Expédié", "Normale"},
        {"005", "CMD-2026-005", "InnoSystems", "Devis", "3,200.00 €", "28/01/2026", "20/02/2026", "Saisie", "Basse"},
        {"006", "CMD-2026-006", "AutoParts Pro", "Standard", "9,875.00 €", "26/01/2026", "12/03/2026", "En cours", "Elevée"},
        {"007", "VTE-2026-007", "TechCorp SA", "Express", "16,455.00 €", "31/01/2026", "08/02/2026", "Expédié", "Urgente"},
        {"008", "CMD-2026-008", "GlobalTech", "Devis", "15,320.00 €", "01/02/2026", "25/02/2026", "Saisie", "Basse"}
    };
    
    for (int i = 0; i < 8; ++i) {
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        
        for (int col = 0; col < 9; ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(data[i][col]);
            item->setTextAlignment(Qt::AlignCenter);
            
            // Couleurs pour STATUT
            if (col == 7) {
                QFont font = item->font();
                font.setBold(true);
                item->setFont(font);
                
                if (data[i][col] == "En cours") {
                    item->setForeground(QColor("#2196F3"));
                } else if (data[i][col] == "Terminé") {
                    item->setForeground(QColor("#4CAF50"));
                } else if (data[i][col] == "Saisie") {
                    item->setForeground(QColor("#FF9800"));
                } else if (data[i][col] == "Expédié") {
                    item->setForeground(QColor("#9C27B0"));
                }
            }
            
            // Couleurs pour PRIORITÉ
            if (col == 8) {
                QFont font = item->font();
                font.setBold(true);
                item->setFont(font);
                
                if (data[i][col] == "Urgente") {
                    item->setForeground(QColor("#F44336"));
                } else if (data[i][col] == "Elevée") {
                    item->setForeground(QColor("#FF9800"));
                } else if (data[i][col] == "Normale") {
                    item->setForeground(QColor("#FBC02D"));
                } else if (data[i][col] == "Basse") {
                    item->setForeground(QColor("#4CAF50"));
                }
            }
            
            ui->tableWidget->setItem(row, col, item);
        }
    }
    
    // AJUSTER LA TAILLE DES COLONNES
    ui->tableWidget->setColumnWidth(0, 60);   // ID
    ui->tableWidget->setColumnWidth(1, 140);  // RÉFÉRENCE
    ui->tableWidget->setColumnWidth(2, 150);  // CLIENT
    ui->tableWidget->setColumnWidth(3, 100);  // TYPE
    ui->tableWidget->setColumnWidth(4, 120);  // MONTANT HT
    ui->tableWidget->setColumnWidth(5, 110);  // DATE CRÉATION
    ui->tableWidget->setColumnWidth(6, 120);  // DATE LIVRAISON
    ui->tableWidget->setColumnWidth(7, 100);  // STATUT
    ui->tableWidget->setColumnWidth(8, 100);  // PRIORITÉ
}


void MainWindow::updateStatistics()
{
    // Fonction vide - statistiques supprimées de l'interface
}

void MainWindow::onCreerClicked()
{
    // Créer un dialogue personnalisé
    QDialog dialog(this);
    dialog.setWindowTitle("Créer une nouvelle commande");
    dialog.setMinimumWidth(550);
    dialog.setStyleSheet(
        "QDialog { background-color: #FFFFFF; }"
        "QLabel { color: #2C2416; font-size: 13px; font-weight: 500; }"
        "QLineEdit { "
        "   background-color: #F8F8F8; "
        "   border: 2px solid #E0E0E0; "
        "   border-radius: 6px; "
        "   padding: 10px; "
        "   font-size: 13px; "
        "   color: #2C2416; "
        "}"
        "QLineEdit:focus { "
        "   border: 2px solid #6B5444; "
        "   background-color: #FFFFFF; "
        "}"
        "QComboBox { "
        "   background-color: #F8F8F8; "
        "   border: 2px solid #E0E0E0; "
        "   border-radius: 6px; "
        "   padding: 10px; "
        "   font-size: 13px; "
        "   color: #2C2416; "
        "}"
        "QComboBox:focus { border: 2px solid #6B5444; }"
        "QDateEdit { "
        "   background-color: #F8F8F8; "
        "   border: 2px solid #E0E0E0; "
        "   border-radius: 6px; "
        "   padding: 10px; "
        "   font-size: 13px; "
        "   color: #2C2416; "
        "}"
        "QDateEdit:focus { border: 2px solid #6B5444; }"
        "QPushButton { "
        "   background-color: #6B5444; "
        "   color: white; "
        "   border: none; "
        "   border-radius: 6px; "
        "   padding: 10px 20px; "
        "   font-size: 13px; "
        "   font-weight: 500; "
        "}"
        "QPushButton:hover { background-color: #7D6555; }"
    );
    
    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    
    QLabel *titre = new QLabel("➕ Créer une nouvelle commande");
    titre->setStyleSheet("font-size: 20px; font-weight: bold; color: #6B5444; margin-bottom: 10px;");
    mainLayout->addWidget(titre);
    
    QFormLayout *layout = new QFormLayout();
    layout->setSpacing(15);
    layout->setLabelAlignment(Qt::AlignRight);
    
    QLineEdit *refEdit = new QLineEdit(&dialog);
    refEdit->setPlaceholderText("Ex: CMD-2026-009");
    
    QLineEdit *clientEdit = new QLineEdit(&dialog);
    clientEdit->setPlaceholderText("Nom du client");
    
    QComboBox *typeCombo = new QComboBox(&dialog);
    typeCombo->addItems({"Standard", "Express", "Vente", "Devis"});
    
    QLineEdit *montantEdit = new QLineEdit(&dialog);
    montantEdit->setPlaceholderText("Ex: 15,000.00 €");
    
    QDateEdit *dateCreationEdit = new QDateEdit(QDate::currentDate(), &dialog);
    dateCreationEdit->setCalendarPopup(true);
    dateCreationEdit->setDisplayFormat("dd/MM/yyyy");
    
    QDateEdit *dateLivraisonEdit = new QDateEdit(QDate::currentDate().addDays(30), &dialog);
    dateLivraisonEdit->setCalendarPopup(true);
    dateLivraisonEdit->setDisplayFormat("dd/MM/yyyy");
    
    QComboBox *statutCombo = new QComboBox(&dialog);
    statutCombo->addItems({"Saisie", "En cours", "Terminé", "Expédié"});
    
    QComboBox *prioriteCombo = new QComboBox(&dialog);
    prioriteCombo->addItems({"Basse", "Normale", "Elevée", "Urgente"});
    prioriteCombo->setCurrentIndex(1);
    
    layout->addRow("Référence *:", refEdit);
    layout->addRow("Client *:", clientEdit);
    layout->addRow("Type:", typeCombo);
    layout->addRow("Montant HT *:", montantEdit);
    layout->addRow("Date Création:", dateCreationEdit);
    layout->addRow("Date Livraison:", dateLivraisonEdit);
    layout->addRow("Statut:", statutCombo);
    layout->addRow("Priorité:", prioriteCombo);
    
    mainLayout->addLayout(layout);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    QPushButton *btnOk = new QPushButton("✓ Créer");
    QPushButton *btnCancel = new QPushButton("✗ Annuler");
    btnCancel->setStyleSheet("QPushButton { background-color: #95877C; } QPushButton:hover { background-color: #A5978C; }");
    buttonLayout->addWidget(btnOk);
    buttonLayout->addWidget(btnCancel);
    mainLayout->addLayout(buttonLayout);
    
    connect(btnOk, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(btnCancel, &QPushButton::clicked, &dialog, &QDialog::reject);
    
    if (dialog.exec() == QDialog::Accepted) {
        if (refEdit->text().isEmpty() || clientEdit->text().isEmpty() || montantEdit->text().isEmpty()) {
            QMessageBox::warning(this, "Champs manquants", "Veuillez remplir tous les champs obligatoires (*)");
            return;
        }
        
        ajouterCommande(refEdit->text(), clientEdit->text(), typeCombo->currentText(),
                       montantEdit->text(), dateCreationEdit->date().toString("dd/MM/yyyy"),
                       dateLivraisonEdit->date().toString("dd/MM/yyyy"),
                       statutCombo->currentText(), prioriteCombo->currentText());
        
        updateStatistics();
        QMessageBox::information(this, "Succès", "Commande créée avec succès !");
    }
}

void MainWindow::onModifierClicked()
{
    if (ui->tableWidget->currentRow() < 0) {
        QMessageBox::warning(this, "Attention", "Veuillez sélectionner une commande");
        return;
    }
    
    int row = ui->tableWidget->currentRow();
    
    QDialog dialog(this);
    dialog.setWindowTitle("Modifier la commande");
    dialog.setMinimumWidth(550);
    dialog.setStyleSheet(
        "QDialog { background-color: #FFFFFF; }"
        "QLabel { color: #2C2416; font-size: 13px; font-weight: 500; }"
        "QLineEdit { "
        "   background-color: #F8F8F8; "
        "   border: 2px solid #E0E0E0; "
        "   border-radius: 6px; "
        "   padding: 10px; "
        "   font-size: 13px; "
        "   color: #2C2416; "
        "}"
        "QLineEdit:focus { border: 2px solid #6B5444; background-color: #FFFFFF; }"
        "QComboBox { "
        "   background-color: #F8F8F8; "
        "   border: 2px solid #E0E0E0; "
        "   border-radius: 6px; "
        "   padding: 10px; "
        "   font-size: 13px; "
        "   color: #2C2416; "
        "}"
        "QComboBox:focus { border: 2px solid #6B5444; }"
        "QDateEdit { "
        "   background-color: #F8F8F8; "
        "   border: 2px solid #E0E0E0; "
        "   border-radius: 6px; "
        "   padding: 10px; "
        "   font-size: 13px; "
        "   color: #2C2416; "
        "}"
        "QDateEdit:focus { border: 2px solid #6B5444; }"
        "QPushButton { "
        "   background-color: #6B5444; "
        "   color: white; "
        "   border: none; "
        "   border-radius: 6px; "
        "   padding: 10px 20px; "
        "   font-size: 13px; "
        "   font-weight: 500; "
        "}"
        "QPushButton:hover { background-color: #7D6555; }"
    );
    
    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    
    QLabel *titre = new QLabel("✏️ Modifier la commande");
    titre->setStyleSheet("font-size: 20px; font-weight: bold; color: #6B5444; margin-bottom: 10px;");
    mainLayout->addWidget(titre);
    
    QFormLayout *layout = new QFormLayout();
    layout->setSpacing(15);
    layout->setLabelAlignment(Qt::AlignRight);
    
    QLineEdit *refEdit = new QLineEdit(ui->tableWidget->item(row, 1)->text(), &dialog);
    QLineEdit *clientEdit = new QLineEdit(ui->tableWidget->item(row, 2)->text(), &dialog);
    
    QComboBox *typeCombo = new QComboBox(&dialog);
    typeCombo->addItems({"Standard", "Express", "Vente", "Devis"});
    typeCombo->setCurrentText(ui->tableWidget->item(row, 3)->text());
    
    QLineEdit *montantEdit = new QLineEdit(ui->tableWidget->item(row, 4)->text(), &dialog);
    
    QDateEdit *dateCreationEdit = new QDateEdit(QDate::fromString(ui->tableWidget->item(row, 5)->text(), "dd/MM/yyyy"), &dialog);
    dateCreationEdit->setCalendarPopup(true);
    dateCreationEdit->setDisplayFormat("dd/MM/yyyy");
    
    QDateEdit *dateLivraisonEdit = new QDateEdit(QDate::fromString(ui->tableWidget->item(row, 6)->text(), "dd/MM/yyyy"), &dialog);
    dateLivraisonEdit->setCalendarPopup(true);
    dateLivraisonEdit->setDisplayFormat("dd/MM/yyyy");
    
    QComboBox *statutCombo = new QComboBox(&dialog);
    statutCombo->addItems({"Saisie", "En cours", "Terminé", "Expédié"});
    statutCombo->setCurrentText(ui->tableWidget->item(row, 7)->text());
    
    QComboBox *prioriteCombo = new QComboBox(&dialog);
    prioriteCombo->addItems({"Basse", "Normale", "Elevée", "Urgente"});
    prioriteCombo->setCurrentText(ui->tableWidget->item(row, 8)->text());
    
    layout->addRow("Référence *:", refEdit);
    layout->addRow("Client *:", clientEdit);
    layout->addRow("Type:", typeCombo);
    layout->addRow("Montant HT *:", montantEdit);
    layout->addRow("Date Création:", dateCreationEdit);
    layout->addRow("Date Livraison:", dateLivraisonEdit);
    layout->addRow("Statut:", statutCombo);
    layout->addRow("Priorité:", prioriteCombo);
    
    mainLayout->addLayout(layout);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    QPushButton *btnOk = new QPushButton("✓ Enregistrer");
    QPushButton *btnCancel = new QPushButton("✗ Annuler");
    btnCancel->setStyleSheet("QPushButton { background-color: #95877C; } QPushButton:hover { background-color: #A5978C; }");
    buttonLayout->addWidget(btnOk);
    buttonLayout->addWidget(btnCancel);
    mainLayout->addLayout(buttonLayout);
    
    connect(btnOk, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(btnCancel, &QPushButton::clicked, &dialog, &QDialog::reject);
    
    if (dialog.exec() == QDialog::Accepted) {
        if (refEdit->text().isEmpty() || clientEdit->text().isEmpty() || montantEdit->text().isEmpty()) {
            QMessageBox::warning(this, "Champs manquants", "Veuillez remplir tous les champs obligatoires (*)");
            return;
        }
        
        ui->tableWidget->item(row, 1)->setText(refEdit->text());
        ui->tableWidget->item(row, 2)->setText(clientEdit->text());
        ui->tableWidget->item(row, 3)->setText(typeCombo->currentText());
        ui->tableWidget->item(row, 4)->setText(montantEdit->text());
        ui->tableWidget->item(row, 5)->setText(dateCreationEdit->date().toString("dd/MM/yyyy"));
        ui->tableWidget->item(row, 6)->setText(dateLivraisonEdit->date().toString("dd/MM/yyyy"));
        ui->tableWidget->item(row, 7)->setText(statutCombo->currentText());
        ui->tableWidget->item(row, 8)->setText(prioriteCombo->currentText());
        
        QTableWidgetItem *statutItem = ui->tableWidget->item(row, 7);
        if (statutItem->text() == "En cours") {
            statutItem->setForeground(QColor("#2196F3"));
        } else if (statutItem->text() == "Terminé") {
            statutItem->setForeground(QColor("#4CAF50"));
        } else if (statutItem->text() == "Saisie") {
            statutItem->setForeground(QColor("#FF9800"));
        } else if (statutItem->text() == "Expédié") {
            statutItem->setForeground(QColor("#9C27B0"));
        }
        
        QTableWidgetItem *prioriteItem = ui->tableWidget->item(row, 8);
        if (prioriteItem->text() == "Urgente") {
            prioriteItem->setForeground(QColor("#F44336"));
        } else if (prioriteItem->text() == "Elevée") {
            prioriteItem->setForeground(QColor("#FF9800"));
        } else if (prioriteItem->text() == "Normale") {
            prioriteItem->setForeground(QColor("#FBC02D"));
        } else if (prioriteItem->text() == "Basse") {
            prioriteItem->setForeground(QColor("#4CAF50"));
        }
        
        updateStatistics();
        QMessageBox::information(this, "Succès", "Commande modifiée avec succès !");
    }
}

void MainWindow::onPDFClicked()
{
    if (ui->tableWidget->currentRow() < 0) {
        QMessageBox::warning(this, "Attention", "Veuillez sélectionner une commande pour générer la facture");
        return;
    }
    
    int row = ui->tableWidget->currentRow();
    
    // Récupérer les données de la commande
    QString ref = ui->tableWidget->item(row, 1)->text();
    QString client = ui->tableWidget->item(row, 2)->text();
    QString type = ui->tableWidget->item(row, 3)->text();
    QString montantHT = ui->tableWidget->item(row, 4)->text();
    QString dateCreation = ui->tableWidget->item(row, 5)->text();
    QString dateLivraison = ui->tableWidget->item(row, 6)->text();
    QString statut = ui->tableWidget->item(row, 7)->text();
    
    // Créer le dialogue de facture
    QDialog factureDialog(this);
    factureDialog.setWindowTitle("Facture - " + ref);
    factureDialog.setMinimumSize(850, 950);
    factureDialog.setStyleSheet(
        "QDialog { background-color: #FFFFFF; }"
        "QTextEdit { "
        "   background-color: #FAFAFA; "
        "   border: 2px solid #E0E0E0; "
        "   border-radius: 10px; "
        "   padding: 15px; "
        "}"
        "QPushButton { "
        "   background-color: #6B5444; "
        "   color: white; "
        "   border: none; "
        "   border-radius: 8px; "
        "   padding: 12px 24px; "
        "   font-size: 14px; "
        "   font-weight: 500; "
        "}"
        "QPushButton:hover { background-color: #7D6555; }"
    );
    
    QVBoxLayout *mainLayout = new QVBoxLayout(&factureDialog);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(25, 25, 25, 25);
    
    // Titre du dialogue
    QLabel *dialogTitle = new QLabel("📄 FACTURE");
    dialogTitle->setStyleSheet("font-size: 24px; font-weight: bold; color: #6B5444; padding: 10px;");
    dialogTitle->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(dialogTitle);
    
    // Zone de texte pour afficher la facture
    QTextEdit *factureView = new QTextEdit(&factureDialog);
    factureView->setReadOnly(true);
    
    // Générer le contenu HTML de la facture
    QString factureHTML = QString(
        "<html><head><style>"
        "body { font-family: 'Segoe UI', Arial, sans-serif; margin: 30px; background-color: #FFFFFF; color: #2C2416; }"
        ".header { text-align: center; border-bottom: 4px solid #6B5444; padding-bottom: 25px; margin-bottom: 35px; }"
        ".company { font-size: 28px; font-weight: bold; color: #6B5444; letter-spacing: 1px; }"
        ".company-info { font-size: 13px; color: #666; margin-top: 8px; }"
        ".facture-title { font-size: 36px; font-weight: bold; color: #2C2416; margin: 25px 0; text-align: center; }"
        ".info-section { margin: 25px 0; padding: 20px; background-color: #F8F8F8; border-radius: 8px; }"
        ".info-label { font-weight: 600; color: #6B5444; font-size: 14px; }"
        ".info-value { color: #2C2416; font-size: 14px; margin-left: 10px; }"
        ".client-box { background-color: #F5F5F0; padding: 25px; border-radius: 10px; margin: 25px 0; border-left: 5px solid #6B5444; }"
        ".client-title { font-weight: bold; color: #6B5444; font-size: 15px; margin-bottom: 12px; }"
        ".client-name { font-size: 18px; font-weight: bold; color: #2C2416; margin: 8px 0; }"
        "table { width: 100%%; border-collapse: collapse; margin: 30px 0; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }"
        "th { background-color: #6B5444; color: white; padding: 16px; text-align: left; font-size: 13px; font-weight: 600; }"
        "td { padding: 14px; border-bottom: 1px solid #E8E8E8; font-size: 13px; color: #2C2416; }"
        "tr:hover { background-color: #F9F9F9; }"
        ".total-section { text-align: right; margin-top: 35px; padding: 20px; background-color: #F8F8F8; border-radius: 8px; }"
        ".total-line { margin: 12px 0; font-size: 16px; color: #2C2416; }"
        ".total-final { font-size: 26px; font-weight: bold; color: #6B5444; border-top: 3px solid #6B5444; padding-top: 18px; margin-top: 15px; }"
        ".footer { text-align: center; margin-top: 50px; padding-top: 25px; border-top: 2px solid #E0E0E0; color: #888; font-size: 11px; line-height: 1.6; }"
        "</style></head><body>"
        "<div class='header'>"
        "<div class='company'>VOTRE ENTREPRISE</div>"
        "<div class='company-info'>123 Rue de l'Industrie, 75000 Paris</div>"
        "<div class='company-info'>Tél: +33 1 23 45 67 89 | Email: contact@entreprise.fr</div>"
        "</div>"
        "<div class='facture-title'>FACTURE</div>"
        "<div class='info-section'>"
        "<span class='info-label'>Numéro de facture:</span><span class='info-value'>%1</span><br>"
        "<span class='info-label'>Date d'émission:</span><span class='info-value'>%2</span><br>"
        "<span class='info-label'>Date de livraison:</span><span class='info-value'>%3</span><br>"
        "<span class='info-label'>Statut:</span><span class='info-value'>%4</span>"
        "</div>"
        "<div class='client-box'>"
        "<div class='client-title'>FACTURÉ À:</div>"
        "<div class='client-name'>%5</div>"
        "<div style='color: #666; font-size: 13px; margin-top: 5px;'>Adresse du client</div>"
        "<div style='color: #666; font-size: 13px;'>Code Postal, Ville</div>"
        "</div>"
        "<table>"
        "<tr><th>Description</th><th>Type</th><th>Quantité</th><th>Prix Unitaire HT</th><th>Total HT</th></tr>"
        "<tr><td>Commande %1</td><td>%6</td><td>1</td><td>%7</td><td>%7</td></tr>"
        "</table>"
        "<div class='total-section'>"
        "<div class='total-line'><strong>Sous-total HT:</strong> %7</div>"
        "<div class='total-line'><strong>TVA (20%%):</strong> %8</div>"
        "<div class='total-final'><strong>TOTAL TTC:</strong> %9</div>"
        "</div>"
        "<div class='footer'>"
        "<p><strong>Merci pour votre confiance !</strong></p>"
        "<p>Conditions de paiement: 30 jours | SIRET: 123 456 789 00012 | TVA: FR12345678901</p>"
        "</div>"
        "</body></html>"
    ).arg(ref)
     .arg(dateCreation)
     .arg(dateLivraison)
     .arg(statut)
     .arg(client)
     .arg(type)
     .arg(montantHT);
    
    // Calculer TVA et TTC
    QString montantStr = montantHT;
    montantStr.remove(" €").remove(",").remove(" ");
    bool ok;
    double montant = montantStr.toDouble(&ok);
    if (ok) {
        double tva = montant * 0.20;
        double ttc = montant + tva;
        QString tvaStr = QString::number(tva, 'f', 2) + " €";
        QString ttcStr = QString::number(ttc, 'f', 2) + " €";
        factureHTML = factureHTML.arg(tvaStr).arg(ttcStr);
    } else {
        factureHTML = factureHTML.arg("N/A").arg("N/A");
    }
    
    factureView->setHtml(factureHTML);
    mainLayout->addWidget(factureView);
    
    // Ajouter le QR Code sous la facture
    QLabel *qrLabel = new QLabel();
    QPixmap qrCode = generateQRCode(ref, 150);
    qrLabel->setPixmap(qrCode);
    qrLabel->setAlignment(Qt::AlignCenter);
    QLabel *qrText = new QLabel("<center><b>Scannez pour le suivi</b></center>");
    qrText->setStyleSheet("color: #6B5444; font-size: 12px;");
    mainLayout->addWidget(qrLabel);
    mainLayout->addWidget(qrText);
    
    // Boutons d'action
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *btnTelecharger = new QPushButton("📥 Télécharger PDF", &factureDialog);
    QPushButton *btnFermer = new QPushButton("Fermer", &factureDialog);
    
    btnFermer->setStyleSheet("QPushButton { background-color: #95877C; } QPushButton:hover { background-color: #A5978C; }");
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(btnTelecharger);
    buttonLayout->addWidget(btnFermer);
    mainLayout->addLayout(buttonLayout);
    
    // Connexion du bouton télécharger
    connect(btnTelecharger, &QPushButton::clicked, [&]() {
        QString fileName = QFileDialog::getSaveFileName(&factureDialog, "Enregistrer la facture", 
                                                        "Facture_" + ref + ".pdf", 
                                                        "PDF Files (*.pdf)");
        if (!fileName.isEmpty()) {
            QPrinter printer(QPrinter::HighResolution);
            printer.setOutputFormat(QPrinter::PdfFormat);
            printer.setOutputFileName(fileName);
            printer.setPageSize(QPageSize::A4);
            
            QTextDocument document;
            document.setHtml(factureHTML);
            document.print(&printer);
            
            QMessageBox::information(&factureDialog, "Succès", "Facture enregistrée avec succès !");
        }
    });
    
    connect(btnFermer, &QPushButton::clicked, &factureDialog, &QDialog::accept);
    
    factureDialog.exec();
}

void MainWindow::onExcelClicked()
{
    // Créer le dialogue de bilan financier
    QDialog bilanDialog(this);
    bilanDialog.setWindowTitle("📊 Bilan Financier");
    bilanDialog.setMinimumSize(950, 750);
    bilanDialog.setStyleSheet(
        "QDialog { background-color: #FFFFFF; }"
        "QLabel { color: #2C2416; }"
        "QTextEdit { "
        "   background-color: #FAFAFA; "
        "   border: 2px solid #E0E0E0; "
        "   border-radius: 10px; "
        "   padding: 15px; "
        "}"
        "QPushButton { "
        "   background-color: #6B5444; "
        "   color: white; "
        "   border: none; "
        "   border-radius: 8px; "
        "   padding: 12px 24px; "
        "   font-size: 14px; "
        "   font-weight: 500; "
        "}"
        "QPushButton:hover { background-color: #7D6555; }"
    );
    
    QVBoxLayout *mainLayout = new QVBoxLayout(&bilanDialog);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    
    // En-tête
    QLabel *lblTitre = new QLabel("📊 BILAN FINANCIER");
    lblTitre->setStyleSheet("font-size: 28px; font-weight: bold; color: #6B5444; padding: 10px;");
    lblTitre->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(lblTitre);
    
    QLabel *lblPeriode = new QLabel("Période: Février 2026");
    lblPeriode->setStyleSheet("font-size: 15px; color: #666; padding: 5px;");
    lblPeriode->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(lblPeriode);
    
    // Calculer les statistiques financières
    double totalHT = 0;
    double totalEnCours = 0;
    double totalTermine = 0;
    int nbCommandes = ui->tableWidget->rowCount();
    int nbEnCours = 0;
    int nbTermine = 0;
    int nbSaisie = 0;
    int nbExpedie = 0;
    
    QMap<QString, double> caParClient;
    QMap<QString, int> commandesParType;
    
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        QString montantStr = ui->tableWidget->item(row, 4)->text();
        QString statut = ui->tableWidget->item(row, 7)->text();
        QString client = ui->tableWidget->item(row, 2)->text();
        QString type = ui->tableWidget->item(row, 3)->text();
        
        // Extraire le montant
        montantStr.remove(" €").remove(",").replace(".", "");
        bool ok;
        double montant = montantStr.toDouble(&ok);
        if (ok) {
            montant = montant / 100.0; // Convertir en euros
            totalHT += montant;
            
            // CA par client
            caParClient[client] += montant;
            
            // CA par statut
            if (statut == "En cours") {
                totalEnCours += montant;
                nbEnCours++;
            } else if (statut == "Terminé") {
                totalTermine += montant;
                nbTermine++;
            } else if (statut == "Saisie") {
                nbSaisie++;
            } else if (statut == "Expédié") {
                nbExpedie++;
            }
        }
        
        // Commandes par type
        commandesParType[type]++;
    }
    
    double totalTVA = totalHT * 0.20;
    double totalTTC = totalHT + totalTVA;
    double moyenneCommande = nbCommandes > 0 ? totalHT / nbCommandes : 0;
    
    // Zone de texte pour afficher le bilan
    QTextEdit *bilanView = new QTextEdit(&bilanDialog);
    bilanView->setReadOnly(true);
    
    QString bilanHTML = QString(
        "<html><head><style>"
        "body { font-family: Arial, sans-serif; padding: 20px; background-color: #FAFAFA; }"
        ".section { background-color: white; margin: 15px 0; padding: 20px; border-radius: 10px; border-left: 4px solid #6B5444; }"
        ".section-title { font-size: 18px; font-weight: bold; color: #6B5444; margin-bottom: 15px; border-bottom: 2px solid #E0E0E0; padding-bottom: 10px; }"
        ".stat-row { display: flex; justify-content: space-between; padding: 10px 0; border-bottom: 1px solid #F0F0F0; }"
        ".stat-label { font-weight: 500; color: #333; }"
        ".stat-value { font-weight: bold; color: #6B5444; font-size: 16px; }"
        ".stat-value-big { font-weight: bold; color: #6B5444; font-size: 24px; }"
        ".highlight { background-color: #F5F5F0; padding: 15px; border-radius: 8px; margin: 10px 0; }"
        ".success { color: #52BE80; }"
        ".warning { color: #F39C12; }"
        ".info { color: #5DADE2; }"
        "table { width: 100%%; border-collapse: collapse; margin: 10px 0; }"
        "th { background-color: #6B5444; color: white; padding: 12px; text-align: left; }"
        "td { padding: 10px; border-bottom: 1px solid #E0E0E0; }"
        "tr:hover { background-color: #F5F5F0; }"
        "</style></head><body>"
    );
    
    // Section 1: Résumé Global
    bilanHTML += "<div class='section'>";
    bilanHTML += "<div class='section-title'>💰 RÉSUMÉ GLOBAL</div>";
    bilanHTML += "<div class='highlight'>";
    bilanHTML += QString("<div class='stat-row'><span class='stat-label'>Chiffre d'Affaires Total HT:</span><span class='stat-value-big'>%1 €</span></div>")
                 .arg(QString::number(totalHT, 'f', 2));
    bilanHTML += QString("<div class='stat-row'><span class='stat-label'>TVA (20%):</span><span class='stat-value'>%1 €</span></div>")
                 .arg(QString::number(totalTVA, 'f', 2));
    bilanHTML += QString("<div class='stat-row'><span class='stat-label'>Total TTC:</span><span class='stat-value-big success'>%1 €</span></div>")
                 .arg(QString::number(totalTTC, 'f', 2));
    bilanHTML += "</div>";
    bilanHTML += QString("<div class='stat-row'><span class='stat-label'>Nombre de commandes:</span><span class='stat-value'>%1</span></div>").arg(nbCommandes);
    bilanHTML += QString("<div class='stat-row'><span class='stat-label'>Montant moyen par commande:</span><span class='stat-value'>%1 €</span></div>")
                 .arg(QString::number(moyenneCommande, 'f', 2));
    bilanHTML += "</div>";
    
    // Section 2: Répartition par Statut
    bilanHTML += "<div class='section'>";
    bilanHTML += "<div class='section-title'>📈 RÉPARTITION PAR STATUT</div>";
    bilanHTML += QString("<div class='stat-row'><span class='stat-label'>✓ Terminées:</span><span class='stat-value success'>%1 commandes (%2 €)</span></div>")
                 .arg(nbTermine).arg(QString::number(totalTermine, 'f', 2));
    bilanHTML += QString("<div class='stat-row'><span class='stat-label'>⏳ En cours:</span><span class='stat-value info'>%1 commandes (%2 €)</span></div>")
                 .arg(nbEnCours).arg(QString::number(totalEnCours, 'f', 2));
    bilanHTML += QString("<div class='stat-row'><span class='stat-label'>📦 Expédiées:</span><span class='stat-value warning'>%1 commandes</span></div>").arg(nbExpedie);
    bilanHTML += QString("<div class='stat-row'><span class='stat-label'>📝 En saisie:</span><span class='stat-value'>%1 commandes</span></div>").arg(nbSaisie);
    
    // Taux de complétion
    double tauxCompletion = nbCommandes > 0 ? (double)nbTermine / nbCommandes * 100 : 0;
    bilanHTML += QString("<div class='highlight'><b>Taux de complétion:</b> <span class='stat-value success'>%1%</span></div>")
                 .arg(QString::number(tauxCompletion, 'f', 1));
    bilanHTML += "</div>";
    
    // Section 3: Top Clients
    bilanHTML += "<div class='section'>";
    bilanHTML += "<div class='section-title'>👥 TOP CLIENTS</div>";
    bilanHTML += "<table>";
    bilanHTML += "<tr><th>Client</th><th>CA HT</th><th>% du Total</th></tr>";
    
    // Trier les clients par CA
    QList<QPair<QString, double>> clientsList;
    for (auto it = caParClient.begin(); it != caParClient.end(); ++it) {
        clientsList.append(qMakePair(it.key(), it.value()));
    }
    std::sort(clientsList.begin(), clientsList.end(), 
              [](const QPair<QString, double> &a, const QPair<QString, double> &b) {
                  return a.second > b.second;
              });
    
    for (const auto &client : clientsList) {
        double pourcentage = totalHT > 0 ? (client.second / totalHT * 100) : 0;
        bilanHTML += QString("<tr><td>%1</td><td>%2 €</td><td>%3%</td></tr>")
                     .arg(client.first)
                     .arg(QString::number(client.second, 'f', 2))
                     .arg(QString::number(pourcentage, 'f', 1));
    }
    bilanHTML += "</table>";
    bilanHTML += "</div>";
    
    // Section 4: Répartition par Type
    bilanHTML += "<div class='section'>";
    bilanHTML += "<div class='section-title'>📊 RÉPARTITION PAR TYPE</div>";
    bilanHTML += "<table>";
    bilanHTML += "<tr><th>Type</th><th>Nombre</th><th>% du Total</th></tr>";
    
    for (auto it = commandesParType.begin(); it != commandesParType.end(); ++it) {
        double pourcentage = nbCommandes > 0 ? ((double)it.value() / nbCommandes * 100) : 0;
        bilanHTML += QString("<tr><td>%1</td><td>%2</td><td>%3%</td></tr>")
                     .arg(it.key())
                     .arg(it.value())
                     .arg(QString::number(pourcentage, 'f', 1));
    }
    bilanHTML += "</table>";
    bilanHTML += "</div>";
    
    // Section 5: Indicateurs de Performance
    bilanHTML += "<div class='section'>";
    bilanHTML += "<div class='section-title'>⚡ INDICATEURS DE PERFORMANCE</div>";
    
    double tauxEnCours = nbCommandes > 0 ? (double)nbEnCours / nbCommandes * 100 : 0;
    QString statusEnCours = tauxEnCours > 50 ? "warning" : "success";
    
    bilanHTML += QString("<div class='stat-row'><span class='stat-label'>Taux de commandes en cours:</span><span class='stat-value %1'>%2%</span></div>")
                 .arg(statusEnCours).arg(QString::number(tauxEnCours, 'f', 1));
    bilanHTML += QString("<div class='stat-row'><span class='stat-label'>CA moyen par client:</span><span class='stat-value'>%1 €</span></div>")
                 .arg(QString::number(totalHT / caParClient.size(), 'f', 2));
    bilanHTML += QString("<div class='stat-row'><span class='stat-label'>Nombre de clients actifs:</span><span class='stat-value'>%1</span></div>")
                 .arg(caParClient.size());
    bilanHTML += "</div>";
    
    bilanHTML += "</body></html>";
    
    bilanView->setHtml(bilanHTML);
    mainLayout->addWidget(bilanView);
    
    // Boutons d'action
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *btnExporter = new QPushButton("📥 Exporter en CSV", &bilanDialog);
    QPushButton *btnImprimer = new QPushButton("🖨️ Imprimer PDF", &bilanDialog);
    QPushButton *btnFermer = new QPushButton("Fermer", &bilanDialog);
    
    btnExporter->setStyleSheet("QPushButton { background-color: #4CAF50; } QPushButton:hover { background-color: #5CBF60; }");
    btnFermer->setStyleSheet("QPushButton { background-color: #95877C; } QPushButton:hover { background-color: #A5978C; }");
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(btnExporter);
    buttonLayout->addWidget(btnImprimer);
    buttonLayout->addWidget(btnFermer);
    mainLayout->addLayout(buttonLayout);
    
    // Connexion du bouton exporter
    connect(btnExporter, &QPushButton::clicked, [&]() {
        QString fileName = QFileDialog::getSaveFileName(&bilanDialog, "Exporter le bilan", 
                                                        "Bilan_Financier_" + QDate::currentDate().toString("yyyy-MM-dd") + ".csv", 
                                                        "CSV Files (*.csv)");
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file);
                out.setEncoding(QStringConverter::Utf8);
                
                out << "BILAN FINANCIER - " << QDate::currentDate().toString("dd/MM/yyyy") << "\n\n";
                out << "RÉSUMÉ GLOBAL\n";
                out << "CA Total HT;" << QString::number(totalHT, 'f', 2) << " €\n";
                out << "TVA (20%);" << QString::number(totalTVA, 'f', 2) << " €\n";
                out << "Total TTC;" << QString::number(totalTTC, 'f', 2) << " €\n";
                out << "Nombre de commandes;" << nbCommandes << "\n";
                out << "Montant moyen;" << QString::number(moyenneCommande, 'f', 2) << " €\n\n";
                
                out << "RÉPARTITION PAR STATUT\n";
                out << "Terminées;" << nbTermine << ";" << QString::number(totalTermine, 'f', 2) << " €\n";
                out << "En cours;" << nbEnCours << ";" << QString::number(totalEnCours, 'f', 2) << " €\n";
                out << "Expédiées;" << nbExpedie << "\n";
                out << "En saisie;" << nbSaisie << "\n\n";
                
                out << "TOP CLIENTS\n";
                out << "Client;CA HT;% du Total\n";
                for (const auto &client : clientsList) {
                    double pourcentage = totalHT > 0 ? (client.second / totalHT * 100) : 0;
                    out << client.first << ";" << QString::number(client.second, 'f', 2) << " €;" 
                        << QString::number(pourcentage, 'f', 1) << "%\n";
                }
                
                file.close();
                QMessageBox::information(&bilanDialog, "Succès", "Bilan exporté avec succès !");
            }
        }
    });
    
    // Connexion du bouton imprimer
    connect(btnImprimer, &QPushButton::clicked, [&]() {
        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        
        QString fileName = QFileDialog::getSaveFileName(&bilanDialog, "Imprimer le bilan", 
                                                        "Bilan_Financier_" + QDate::currentDate().toString("yyyy-MM-dd") + ".pdf", 
                                                        "PDF Files (*.pdf)");
        if (!fileName.isEmpty()) {
            printer.setOutputFileName(fileName);
            printer.setPageSize(QPageSize::A4);
            
            QTextDocument document;
            document.setHtml(bilanHTML);
            document.print(&printer);
            
            QMessageBox::information(&bilanDialog, "Succès", "Bilan imprimé avec succès !");
        }
    });
    
    connect(btnFermer, &QPushButton::clicked, &bilanDialog, &QDialog::accept);
    
    bilanDialog.exec();
}

void MainWindow::onRetourClicked()
{
    QMessageBox::information(this, "Retour", "Retour au menu principal");
}

void MainWindow::onTousStatutsClicked()
{
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        ui->tableWidget->setRowHidden(row, false);
    }
}

void MainWindow::onRechercherTextChanged(const QString &text)
{
    QString searchText = text.trimmed().toLower();
    
    if (searchText.isEmpty()) {
        for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
            ui->tableWidget->setRowHidden(row, false);
        }
        return;
    }
    
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        bool match = false;
        for (int col = 0; col < ui->tableWidget->columnCount(); ++col) {
            QTableWidgetItem *item = ui->tableWidget->item(row, col);
            if (item && item->text().toLower().contains(searchText)) {
                match = true;
                break;
            }
        }
        ui->tableWidget->setRowHidden(row, !match);
    }
}

void MainWindow::showContextMenu(const QPoint &pos)
{
    QPoint globalPos = ui->tableWidget->viewport()->mapToGlobal(pos);
    
    QMenu contextMenu;
    QAction *supprimerAction = contextMenu.addAction("Supprimer");
    
    QAction *selectedAction = contextMenu.exec(globalPos);
    
    if (selectedAction == supprimerAction) {
        supprimerLigneSelectionnee();
    }
}

void MainWindow::supprimerLigneSelectionnee()
{
    int row = ui->tableWidget->currentRow();
    
    if (row < 0) {
        QMessageBox::warning(this, "Attention", "Veuillez sélectionner une commande");
        return;
    }
    
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirmer", 
                                 "Voulez-vous vraiment supprimer cette commande?",
                                 QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        ui->tableWidget->removeRow(row);
        updateStatistics();
        QMessageBox::information(this, "Succès", "Commande supprimée avec succès !");
    }
}

void MainWindow::ajouterCommande(const QString &ref, const QString &client, const QString &type, 
                                const QString &montant, const QString &dateCreation, 
                                const QString &dateLivraison, const QString &statut, const QString &priorite)
{
    int row = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(row);
    
    // Générer un nouvel ID
    QString id = QString("%1").arg(row + 1, 3, 10, QChar('0'));
    
    QString data[9] = {id, ref, client, type, montant, dateCreation, dateLivraison, statut, priorite};
    
    for (int col = 0; col < 9; ++col) {
        QTableWidgetItem *item = new QTableWidgetItem(data[col]);
        item->setTextAlignment(Qt::AlignCenter);
        
        // Couleur de fond alternée
        if (row % 2 == 0) {
            item->setBackground(QColor("#3D3228"));
        } else {
            item->setBackground(QColor("#4A3D32"));
        }
        
        item->setForeground(QColor("#FFFFFF"));
        
        // Couleur pour le statut
        if (col == 7) {
            if (statut == "En cours") {
                item->setForeground(QColor("#5DADE2"));
            } else if (statut == "Terminé") {
                item->setForeground(QColor("#52BE80"));
            } else if (statut == "Saisie") {
                item->setForeground(QColor("#F39C12"));
            } else if (statut == "Expédié") {
                item->setForeground(QColor("#E67E22"));
            }
        }
        
        // Couleur pour la priorité
        if (col == 8) {
            if (priorite == "Urgente") {
                item->setForeground(QColor("#E74C3C"));
            } else if (priorite == "Normale") {
                item->setForeground(QColor("#F39C12"));
            } else if (priorite == "Basse") {
                item->setForeground(QColor("#52BE80"));
            } else if (priorite == "Elevée") {
                item->setForeground(QColor("#E67E22"));
            }
        }
        
        ui->tableWidget->setItem(row, col, item);
    }
}

QPixmap MainWindow::generateQRCode(const QString &text, int size)
{
    // Créer une image simple représentant un QR code
    QPixmap qrPixmap(size, size);
    qrPixmap.fill(Qt::white);
    
    QPainter painter(&qrPixmap);
    painter.setPen(Qt::black);
    painter.setBrush(Qt::black);
    
    // Dessiner un motif de QR code simplifié
    int moduleSize = size / 25;
    
    // Bordure
    painter.drawRect(0, 0, size-1, size-1);
    
    // Coins de positionnement (3 carrés)
    int cornerSize = moduleSize * 7;
    // Coin supérieur gauche
    painter.drawRect(moduleSize, moduleSize, cornerSize, cornerSize);
    painter.fillRect(moduleSize*2, moduleSize*2, cornerSize-moduleSize*2, cornerSize-moduleSize*2, Qt::white);
    painter.fillRect(moduleSize*3, moduleSize*3, cornerSize-moduleSize*4, cornerSize-moduleSize*4, Qt::black);
    
    // Coin supérieur droit
    painter.drawRect(size-cornerSize-moduleSize, moduleSize, cornerSize, cornerSize);
    painter.fillRect(size-cornerSize-moduleSize+moduleSize, moduleSize*2, cornerSize-moduleSize*2, cornerSize-moduleSize*2, Qt::white);
    painter.fillRect(size-cornerSize-moduleSize+moduleSize*2, moduleSize*3, cornerSize-moduleSize*4, cornerSize-moduleSize*4, Qt::black);
    
    // Coin inférieur gauche
    painter.drawRect(moduleSize, size-cornerSize-moduleSize, cornerSize, cornerSize);
    painter.fillRect(moduleSize*2, size-cornerSize-moduleSize+moduleSize, cornerSize-moduleSize*2, cornerSize-moduleSize*2, Qt::white);
    painter.fillRect(moduleSize*3, size-cornerSize-moduleSize+moduleSize*2, cornerSize-moduleSize*4, cornerSize-moduleSize*4, Qt::black);
    
    // Motif de données aléatoire basé sur le texte
    QByteArray data = text.toUtf8();
    int seed = 0;
    for (char c : data) {
        seed += static_cast<int>(c);
    }
    srand(seed);
    
    for (int i = 9; i < 23; i++) {
        for (int j = 2; j < 23; j++) {
            if ((i < 9 && j < 9) || (i < 9 && j > 16) || (i > 16 && j < 9)) {
                continue; // Skip corners
            }
            if (rand() % 2 == 0) {
                painter.fillRect(j * moduleSize, i * moduleSize, moduleSize, moduleSize, Qt::black);
            }
        }
    }
    
    // Ajouter le texte en bas
    painter.setPen(Qt::black);
    QFont font = painter.font();
    font.setPixelSize(moduleSize);
    painter.setFont(font);
    painter.drawText(QRect(0, size - moduleSize*2, size, moduleSize*2), Qt::AlignCenter, text.left(15));
    
    return qrPixmap;
}

void MainWindow::onSuiviClicked()
{
    if (ui->tableWidget->currentRow() < 0) {
        QMessageBox::warning(this, "Attention", "Veuillez sélectionner une commande pour voir le suivi");
        return;
    }
    
    int row = ui->tableWidget->currentRow();
    
    // Récupérer les données de la commande
    QString ref = ui->tableWidget->item(row, 1)->text();
    QString client = ui->tableWidget->item(row, 2)->text();
    QString type = ui->tableWidget->item(row, 3)->text();
    QString montantHT = ui->tableWidget->item(row, 4)->text();
    QString dateCreation = ui->tableWidget->item(row, 5)->text();
    QString dateLivraison = ui->tableWidget->item(row, 6)->text();
    QString statut = ui->tableWidget->item(row, 7)->text();
    QString priorite = ui->tableWidget->item(row, 8)->text();
    
    // Créer le dialogue de suivi
    QDialog suiviDialog(this);
    suiviDialog.setWindowTitle("Suivi de commande - " + ref);
    suiviDialog.setMinimumSize(750, 850);
    suiviDialog.setStyleSheet(
        "QDialog { background-color: #FFFFFF; }"
        "QLabel { color: #2C2416; font-size: 14px; }"
        "QTextEdit { "
        "   background-color: #FAFAFA; "
        "   border: 2px solid #E0E0E0; "
        "   border-radius: 10px; "
        "   padding: 15px; "
        "}"
        "QPushButton { "
        "   background-color: #6B5444; "
        "   color: white; "
        "   border: none; "
        "   border-radius: 8px; "
        "   padding: 12px 30px; "
        "   font-size: 14px; "
        "   font-weight: 500; "
        "}"
        "QPushButton:hover { background-color: #7D6555; }"
    );
    
    QVBoxLayout *mainLayout = new QVBoxLayout(&suiviDialog);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    
    // Titre principal
    QLabel *mainTitle = new QLabel("📦 SUIVI DE COMMANDE");
    mainTitle->setStyleSheet("font-size: 24px; font-weight: bold; color: #6B5444; padding: 10px;");
    mainTitle->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(mainTitle);
    
    // En-tête avec QR Code
    QHBoxLayout *headerLayout = new QHBoxLayout();
    
    // Informations de la commande
    QVBoxLayout *infoLayout = new QVBoxLayout();
    
    QFrame *infoFrame = new QFrame();
    infoFrame->setStyleSheet("background-color: #F8F8F8; border-radius: 10px; padding: 20px; border: 2px solid #E0E0E0;");
    QVBoxLayout *infoFrameLayout = new QVBoxLayout(infoFrame);
    
    QLabel *lblRef = new QLabel("<b style='color: #6B5444;'>Référence:</b> " + ref);
    QLabel *lblClient = new QLabel("<b style='color: #6B5444;'>Client:</b> " + client);
    QLabel *lblType = new QLabel("<b style='color: #6B5444;'>Type:</b> " + type);
    QLabel *lblMontant = new QLabel("<b style='color: #6B5444;'>Montant HT:</b> " + montantHT);
    QLabel *lblPriorite = new QLabel("<b style='color: #6B5444;'>Priorité:</b> <span style='font-weight: bold;'>" + priorite + "</span>");
    
    lblRef->setStyleSheet("font-size: 14px; padding: 5px; color: #2C2416;");
    lblClient->setStyleSheet("font-size: 14px; padding: 5px; color: #2C2416;");
    lblType->setStyleSheet("font-size: 14px; padding: 5px; color: #2C2416;");
    lblMontant->setStyleSheet("font-size: 14px; padding: 5px; color: #2C2416;");
    
    // Appliquer la couleur de priorité
    if (priorite == "Urgente") {
        lblPriorite->setStyleSheet("font-size: 14px; padding: 5px; color: #F44336; font-weight: bold;");
    } else if (priorite == "Elevée") {
        lblPriorite->setStyleSheet("font-size: 14px; padding: 5px; color: #FF9800; font-weight: bold;");
    } else if (priorite == "Normale") {
        lblPriorite->setStyleSheet("font-size: 14px; padding: 5px; color: #FBC02D; font-weight: bold;");
    } else {
        lblPriorite->setStyleSheet("font-size: 14px; padding: 5px; color: #4CAF50; font-weight: bold;");
    }
    
    infoFrameLayout->addWidget(lblRef);
    infoFrameLayout->addWidget(lblClient);
    infoFrameLayout->addWidget(lblType);
    infoFrameLayout->addWidget(lblMontant);
    infoFrameLayout->addWidget(lblPriorite);
    
    infoLayout->addWidget(infoFrame);
    
    // QR Code avec cadre
    QFrame *qrFrame = new QFrame();
    qrFrame->setStyleSheet("background-color: #F8F8F8; border-radius: 10px; padding: 15px; border: 2px solid #E0E0E0;");
    QVBoxLayout *qrFrameLayout = new QVBoxLayout(qrFrame);
    
    QLabel *qrLabel = new QLabel();
    QPixmap qrCode = generateQRCode(ref, 180);
    qrLabel->setPixmap(qrCode);
    qrLabel->setAlignment(Qt::AlignCenter);
    
    QLabel *qrText = new QLabel("Scannez pour le suivi");
    qrText->setStyleSheet("color: #6B5444; font-size: 12px; font-weight: 500;");
    qrText->setAlignment(Qt::AlignCenter);
    
    qrFrameLayout->addWidget(qrLabel);
    qrFrameLayout->addWidget(qrText);
    
    headerLayout->addLayout(infoLayout, 2);
    headerLayout->addWidget(qrFrame, 1);
    mainLayout->addLayout(headerLayout);
    
    // Timeline de suivi
    QLabel *lblTimeline = new QLabel("📍 Historique de Suivi");
    lblTimeline->setStyleSheet("font-size: 18px; font-weight: bold; color: #6B5444; margin-top: 15px; padding: 10px 0;");
    mainLayout->addWidget(lblTimeline);
    
    // Zone de texte pour le suivi
    QTextEdit *timelineView = new QTextEdit(&suiviDialog);
    timelineView->setReadOnly(true);
    
    QString timelineHTML = QString(
        "<html><head><style>"
        "body { font-family: 'Segoe UI', Arial, sans-serif; padding: 15px; background-color: #FAFAFA; }"
        ".timeline-item { margin: 18px 0; padding: 18px; background-color: #FFFFFF; border-left: 5px solid #6B5444; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.08); }"
        ".timeline-date { font-weight: 600; color: #6B5444; font-size: 13px; margin-bottom: 8px; }"
        ".timeline-status { font-size: 16px; font-weight: bold; margin: 8px 0; }"
        ".timeline-desc { color: #666; font-size: 13px; line-height: 1.5; }"
        ".status-saisie { color: #FF9800; }"
        ".status-encours { color: #2196F3; }"
        ".status-expedie { color: #9C27B0; }"
        ".status-termine { color: #4CAF50; }"
        ".status-pending { color: #999; opacity: 0.6; }"
        "</style></head><body>"
    );
    
    // Générer l'historique basé sur le statut actuel
    timelineHTML += "<div class='timeline-item'>";
    timelineHTML += "<div class='timeline-date'>📅 " + dateCreation + "</div>";
    timelineHTML += "<div class='timeline-status status-saisie'>✓ Commande créée</div>";
    timelineHTML += "<div class='timeline-desc'>La commande a été enregistrée dans le système</div>";
    timelineHTML += "</div>";
    
    if (statut == "En cours" || statut == "Expédié" || statut == "Terminé") {
        QDate dateCreationParsed = QDate::fromString(dateCreation, "dd/MM/yyyy");
        QString dateTraitement = dateCreationParsed.addDays(1).toString("dd/MM/yyyy");
        
        timelineHTML += "<div class='timeline-item'>";
        timelineHTML += "<div class='timeline-date'>📅 " + dateTraitement + "</div>";
        timelineHTML += "<div class='timeline-status status-encours'>✓ En cours de traitement</div>";
        timelineHTML += "<div class='timeline-desc'>La commande est en cours de préparation</div>";
        timelineHTML += "</div>";
    }
    
    if (statut == "Expédié" || statut == "Terminé") {
        QDate dateCreationParsed = QDate::fromString(dateCreation, "dd/MM/yyyy");
        QString dateExpedition = dateCreationParsed.addDays(3).toString("dd/MM/yyyy");
        
        timelineHTML += "<div class='timeline-item'>";
        timelineHTML += "<div class='timeline-date'>📅 " + dateExpedition + "</div>";
        timelineHTML += "<div class='timeline-status status-expedie'>✓ Expédiée</div>";
        timelineHTML += "<div class='timeline-desc'>La commande a été expédiée vers le client</div>";
        timelineHTML += "</div>";
    }
    
    if (statut == "Terminé") {
        timelineHTML += "<div class='timeline-item'>";
        timelineHTML += "<div class='timeline-date'>📅 " + dateLivraison + "</div>";
        timelineHTML += "<div class='timeline-status status-termine'>✓ Livrée</div>";
        timelineHTML += "<div class='timeline-desc'>La commande a été livrée avec succès</div>";
        timelineHTML += "</div>";
    } else {
        timelineHTML += "<div class='timeline-item' style='opacity: 0.5;'>";
        timelineHTML += "<div class='timeline-date'>📅 " + dateLivraison + " (Prévu)</div>";
        timelineHTML += "<div class='timeline-status'>⏳ Livraison prévue</div>";
        timelineHTML += "<div class='timeline-desc'>Date de livraison estimée</div>";
        timelineHTML += "</div>";
    }
    
    timelineHTML += "</body></html>";
    
    timelineView->setHtml(timelineHTML);
    mainLayout->addWidget(timelineView);
    
    // Bouton fermer
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 15, 0, 0);
    QPushButton *btnFermer = new QPushButton("Fermer", &suiviDialog);
    buttonLayout->addStretch();
    buttonLayout->addWidget(btnFermer);
    mainLayout->addLayout(buttonLayout);
    
    connect(btnFermer, &QPushButton::clicked, &suiviDialog, &QDialog::accept);
    
    suiviDialog.exec();
}

void MainWindow::onPlanificationClicked()
{
    // Créer le dialogue de planification
    QDialog planDialog(this);
    planDialog.setWindowTitle("📅 Planification des Livraisons");
    planDialog.setMinimumSize(1050, 750);
    planDialog.setStyleSheet(
        "QDialog { background-color: #FFFFFF; }"
        "QLabel { color: #2C2416; }"
        "QPushButton { "
        "   background-color: #6B5444; "
        "   color: white; "
        "   border: none; "
        "   border-radius: 8px; "
        "   padding: 12px 24px; "
        "   font-size: 14px; "
        "   font-weight: 500; "
        "}"
        "QPushButton:hover { background-color: #7D6555; }"
    );
    
    QVBoxLayout *mainLayout = new QVBoxLayout(&planDialog);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    
    // En-tête
    QLabel *lblTitre = new QLabel("📅 PLANIFICATION DES LIVRAISONS");
    lblTitre->setStyleSheet("font-size: 26px; font-weight: bold; color: #6B5444; padding: 10px;");
    lblTitre->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(lblTitre);
    
    // Tableau de planification
    QTableWidget *planTable = new QTableWidget(&planDialog);
    planTable->setColumnCount(7);
    planTable->setHorizontalHeaderLabels({"Référence", "Client", "Date Livraison", "Société Livraison", "Statut", "Retard", "Actions"});
    planTable->setAlternatingRowColors(true);
    planTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    planTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    // Style du tableau
    planTable->setStyleSheet(
        "QTableWidget { "
        "   background-color: #FAFAFA; "
        "   border: 2px solid #E0E0E0; "
        "   border-radius: 10px; "
        "   gridline-color: #F0F0F0; "
        "}"
        "QTableWidget::item { "
        "   padding: 14px; "
        "   border-bottom: 1px solid #F0F0F0; "
        "   color: #2C2416; "
        "   font-size: 13px; "
        "}"
        "QTableWidget::item:selected { "
        "   background-color: rgba(107, 84, 68, 0.12); "
        "   color: #2C2416; "
        "}"
        "QTableWidget::item:alternate { "
        "   background-color: #FFFFFF; "
        "}"
        "QHeaderView::section { "
        "   background-color: #6B5444; "
        "   color: white; "
        "   padding: 16px; "
        "   border: none; "
        "   font-weight: bold; "
        "   font-size: 12px; "
        "   text-transform: uppercase; "
        "   letter-spacing: 0.5px; "
        "}"
    );
    
    // Données de planification
    struct Livraison {
        QString ref;
        QString client;
        QString dateLivraison;
        QString societe;
        QString statut;
        int joursRetard;
    };
    
    QList<Livraison> livraisons = {
        {"CMD-2026-001", "TechCorp SA", "25/02/2026", "DHL Express", "En transit", 0},
        {"CMD-2026-002", "InnoSystems", "15/02/2026", "Chronopost", "Livré", 0},
        {"CMD-2026-003", "GlobalTech", "10/03/2026", "UPS", "Planifié", 0},
        {"CMD-2026-004", "MetalWorks", "05/03/2026", "FedEx", "En transit", 0},
        {"CMD-2026-005", "InnoSystems", "20/02/2026", "Colissimo", "En retard", 3},
        {"CMD-2026-006", "AutoParts Pro", "12/03/2026", "DHL Express", "Planifié", 0},
        {"CMD-2026-007", "TechCorp SA", "08/02/2026", "TNT", "Livré", 0},
        {"CMD-2026-008", "GlobalTech", "25/02/2026", "UPS", "En retard", 1}
    };
    
    planTable->setRowCount(livraisons.size());
    
    for (int i = 0; i < livraisons.size(); ++i) {
        const Livraison &liv = livraisons[i];
        
        // Référence
        QTableWidgetItem *refItem = new QTableWidgetItem(liv.ref);
        refItem->setTextAlignment(Qt::AlignCenter);
        QFont boldFont = refItem->font();
        boldFont.setBold(true);
        refItem->setFont(boldFont);
        refItem->setForeground(QColor("#6B5444"));
        planTable->setItem(i, 0, refItem);
        
        // Client
        QTableWidgetItem *clientItem = new QTableWidgetItem(liv.client);
        clientItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        planTable->setItem(i, 1, clientItem);
        
        // Date Livraison
        QTableWidgetItem *dateItem = new QTableWidgetItem("📅 " + liv.dateLivraison);
        dateItem->setTextAlignment(Qt::AlignCenter);
        planTable->setItem(i, 2, dateItem);
        
        // Société de Livraison
        QTableWidgetItem *societeItem = new QTableWidgetItem("🚚 " + liv.societe);
        societeItem->setTextAlignment(Qt::AlignCenter);
        QFont societeFont = societeItem->font();
        societeFont.setBold(true);
        societeItem->setFont(societeFont);
        
        // Couleurs par société
        if (liv.societe.contains("DHL")) {
            societeItem->setForeground(QColor("#FFD700"));
        } else if (liv.societe.contains("UPS")) {
            societeItem->setForeground(QColor("#351C15"));
        } else if (liv.societe.contains("FedEx")) {
            societeItem->setForeground(QColor("#4D148C"));
        } else if (liv.societe.contains("Chronopost")) {
            societeItem->setForeground(QColor("#FFD100"));
        } else {
            societeItem->setForeground(QColor("#2196F3"));
        }
        planTable->setItem(i, 3, societeItem);
        
        // Statut
        QTableWidgetItem *statutItem = new QTableWidgetItem(liv.statut);
        statutItem->setTextAlignment(Qt::AlignCenter);
        QFont statutFont = statutItem->font();
        statutFont.setBold(true);
        statutItem->setFont(statutFont);
        
        if (liv.statut == "Livré") {
            statutItem->setForeground(QColor("#4CAF50"));
            statutItem->setText("✅ " + liv.statut);
        } else if (liv.statut == "En transit") {
            statutItem->setForeground(QColor("#2196F3"));
            statutItem->setText("🚚 " + liv.statut);
        } else if (liv.statut == "Planifié") {
            statutItem->setForeground(QColor("#FF9800"));
            statutItem->setText("📋 " + liv.statut);
        } else if (liv.statut == "En retard") {
            statutItem->setForeground(QColor("#F44336"));
            statutItem->setText("⚠️ " + liv.statut);
        }
        planTable->setItem(i, 4, statutItem);
        
        // Retard
        QTableWidgetItem *retardItem = new QTableWidgetItem();
        retardItem->setTextAlignment(Qt::AlignCenter);
        if (liv.joursRetard > 0) {
            retardItem->setText(QString::number(liv.joursRetard) + " jour(s)");
            retardItem->setForeground(QColor("#F44336"));
            QFont retardFont = retardItem->font();
            retardFont.setBold(true);
            retardItem->setFont(retardFont);
            retardItem->setBackground(QColor("#FFEBEE"));
        } else {
            retardItem->setText("✓ À temps");
            retardItem->setForeground(QColor("#4CAF50"));
        }
        planTable->setItem(i, 5, retardItem);
        
        // Actions
        QTableWidgetItem *actionsItem = new QTableWidgetItem("🔔 Notifier");
        actionsItem->setTextAlignment(Qt::AlignCenter);
        actionsItem->setForeground(QColor("#2196F3"));
        planTable->setItem(i, 6, actionsItem);
    }
    
    // Ajuster les colonnes
    planTable->setColumnWidth(0, 130);  // Référence
    planTable->setColumnWidth(1, 150);  // Client
    planTable->setColumnWidth(2, 130);  // Date
    planTable->setColumnWidth(3, 150);  // Société
    planTable->setColumnWidth(4, 120);  // Statut
    planTable->setColumnWidth(5, 100);  // Retard
    planTable->setColumnWidth(6, 100);  // Actions
    
    mainLayout->addWidget(planTable);
    
    // Section notifications
    QFrame *notifFrame = new QFrame(&planDialog);
    notifFrame->setStyleSheet(
        "background-color: #FFF3E0; "
        "border-radius: 10px; "
        "padding: 18px; "
        "border: 2px solid #FFE0B2;"
    );
    QHBoxLayout *notifLayout = new QHBoxLayout(notifFrame);
    
    QLabel *notifIcon = new QLabel("⚠️");
    notifIcon->setStyleSheet("font-size: 28px;");
    notifLayout->addWidget(notifIcon);
    
    QLabel *notifText = new QLabel("<b style='color: #E65100;'>Alertes:</b> <span style='color: #2C2416;'>2 commandes en retard nécessitent votre attention!</span>");
    notifText->setStyleSheet("font-size: 14px;");
    notifLayout->addWidget(notifText);
    notifLayout->addStretch();
    
    mainLayout->addWidget(notifFrame);
    
    // Boutons d'action
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 10, 0, 0);
    QPushButton *btnExporter = new QPushButton("📥 Exporter Planning", &planDialog);
    QPushButton *btnNotifier = new QPushButton("🔔 Envoyer Notifications", &planDialog);
    QPushButton *btnFermer = new QPushButton("Fermer", &planDialog);
    
    btnExporter->setStyleSheet("QPushButton { background-color: #4CAF50; } QPushButton:hover { background-color: #5CBF60; }");
    btnNotifier->setStyleSheet("QPushButton { background-color: #FF9800; } QPushButton:hover { background-color: #FFA726; }");
    btnFermer->setStyleSheet("QPushButton { background-color: #95877C; } QPushButton:hover { background-color: #A5978C; }");
    
    buttonLayout->addStretch();
    buttonLayout->addWidget(btnExporter);
    buttonLayout->addWidget(btnNotifier);
    buttonLayout->addWidget(btnFermer);
    mainLayout->addLayout(buttonLayout);
    
    // Connexions
    connect(btnExporter, &QPushButton::clicked, [&]() {
        QMessageBox::information(&planDialog, "Export", "Planning exporté avec succès!");
    });
    
    connect(btnNotifier, &QPushButton::clicked, [&]() {
        QMessageBox::information(&planDialog, "Notifications", "Notifications envoyées aux clients concernés!");
    });
    
    connect(btnFermer, &QPushButton::clicked, &planDialog, &QDialog::accept);
    
    planDialog.exec();
}
