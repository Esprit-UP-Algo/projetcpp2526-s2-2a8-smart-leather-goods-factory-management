// mainwindow.cpp
#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QSplitter>
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QMessageBox>
#include <QDate>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    resize(1420, 920);
    setWindowTitle("ERP Manufacture Pro - Gestion des Clients");

    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // ────────────────────── TOP BAR ───────────────────────────────
    QWidget *topBar = new QWidget();
    topBar->setFixedHeight(90);
    topBar->setStyleSheet("background-color: #A3835F;");  // ← plus clair que #8C6642

    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(25, 12, 25, 12);
    topLayout->setSpacing(16);

    // Title + date
    QVBoxLayout *titleBox = new QVBoxLayout();
    titleBox->setSpacing(2);

    QLabel *titleLabel = new QLabel("Gestion des Clients");
    titleLabel->setStyleSheet("color: white; font-size: 22px; font-weight: bold;");
    titleBox->addWidget(titleLabel);

    QLabel *dateLabel = new QLabel(QDate::currentDate().toString("dddd dd MMMM yyyy"));
    dateLabel->setStyleSheet("color: #F8F1E5; font-size: 13px;");  // ← plus clair que #F0E4D2
    titleBox->addWidget(dateLabel);

    topLayout->addLayout(titleBox);
    topLayout->addStretch();

    // Action buttons – palette plus claire
    QString btnStyle = R"(
        QPushButton {
            background-color: #C0A070;          // plus clair que #A57F50
            color: white;
            border: none;
            border-radius: 8px;
            padding: 0 18px;
            font-weight: bold;
            min-height: 46px;
            font-size: 14px;
        }
        QPushButton:hover {
            background-color: #D0B080;          // plus clair que #B89467
        }
        QPushButton:pressed {
            background-color: #A3835F;          // même que top bar
        }
    )";

    QPushButton *btnNew     = new QPushButton(" + Créer");
    QPushButton *btnDelete  = new QPushButton(" 🗑 Supprimer");
    QPushButton *btnExport  = new QPushButton(" 📄 Exporter PDF");
    QPushButton *btnPrint   = new QPushButton(" 🖨 Imprimer");
    QPushButton *btnRefresh = new QPushButton(" 🔄 Actualiser");

    for (auto btn : {btnNew, btnDelete, btnExport, btnPrint, btnRefresh}) {
        btn->setStyleSheet(btnStyle);
        topLayout->addWidget(btn);
    }

    mainLayout->addWidget(topBar);

    // ────────────────────── MAIN CONTENT ──────────────────────────
    QWidget *contentArea = new QWidget();
    QHBoxLayout *contentLayout = new QHBoxLayout(contentArea);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);

    // ────────────────────── LEFT NAVIGATION SIDEBAR ───────────────
    QWidget *navSidebar = new QWidget();
    navSidebar->setFixedWidth(280);
    navSidebar->setStyleSheet("background-color: #8A6A4A;");  // ← plus clair que #6B4C2F

    QVBoxLayout *navLay = new QVBoxLayout(navSidebar);
    navLay->setContentsMargins(20, 20, 20, 30);
    navLay->setSpacing(12);

    // ────────────────────── LOGO ──────────────────────
    QLabel *logoLabel = new QLabel();
    logoLabel->setAlignment(Qt::AlignCenter);
    logoLabel->setFixedHeight(140);
    logoLabel->setMinimumWidth(140);

    QPixmap logoPix("C:/Users/USER/Desktop/cpp/test2/logo.PNG");

    if (!logoPix.isNull()) {
        logoLabel->setPixmap(
            logoPix.scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation)
            );
    } else {
        logoLabel->setText("LOGO\nnon trouvé\nVérifiez le chemin");
        logoLabel->setStyleSheet(
            "color: #ffcccc;"
            "font-size: 15px;"
            "font-weight: bold;"
            "background: rgba(255,0,0,0.15);"
            "border-radius: 8px;"
            "padding: 20px;"
            );
    }

    navLay->addWidget(logoLabel);
    navLay->addSpacing(25);

    // ────────────────────── User info ──────────────────────
    QLabel *role = new QLabel("Manager de Clients");
    role->setStyleSheet("color: #F8F1E5; font-size: 18px; font-weight: 700;");  // ← plus clair
    role->setAlignment(Qt::AlignCenter);
    navLay->addWidget(role);

    navLay->addSpacing(30);

    // Navigation items
    QStringList menuItems = {"Tableau de bord", "Clients", "Statistiques", "Paramètres"};
    for (const QString &text : menuItems) {
        QPushButton *item = new QPushButton(text);
        item->setProperty("navItem", true);
        item->setProperty("active", text == "Clients");
        item->setStyleSheet(
            text == "Clients"
                ? "background: #A3835F; color: white; text-align: left; padding: 14px 20px; border: none; border-radius: 6px;"
                : "color: #F8F1E5; text-align: left; padding: 14px 20px; border: none;"
                  "QPushButton:hover { background: rgba(255,255,255,0.18); border-radius: 6px; }"
            );
        navLay->addWidget(item);
    }

    navLay->addStretch();

    QPushButton *logout = new QPushButton("Déconnexion");
    logout->setObjectName("logout");
    navLay->addWidget(logout);

    contentLayout->addWidget(navSidebar);

    // ────────────────────── SPLIT VIEW ────────────────────────────
    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    splitter->setHandleWidth(3);
    splitter->setStyleSheet("QSplitter::handle { background: #C9A875; }");  // ← handle plus clair

    // Left: Clients list
    QWidget *listPanel = new QWidget();
    QVBoxLayout *listLay = new QVBoxLayout(listPanel);
    listLay->setContentsMargins(24, 24, 24, 24);
    listLay->setSpacing(16);

    QLabel *listTitle = new QLabel("HISTORIQUE DES CLIENTS");
    listTitle->setObjectName("listTitle");
    listLay->addWidget(listTitle);

    QLineEdit *searchEdit = new QLineEdit();
    searchEdit->setPlaceholderText("Rechercher un client...");
    searchEdit->setObjectName("search");
    listLay->addWidget(searchEdit);

    tableClients = new QTableWidget(0, 8);
    tableClients->setHorizontalHeaderLabels({"Nom", "Prénom", "Sexe", "CIN", "Pays", "Ville", "Adresse", "Email"});
    tableClients->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableClients->setAlternatingRowColors(true);
    listLay->addWidget(tableClients, 1);

    splitter->addWidget(listPanel);

    // Right: Details form
    QWidget *detailsPanel = new QWidget();
    detailsPanel->setObjectName("detailsPanel");

    QVBoxLayout *detailsLay = new QVBoxLayout(detailsPanel);
    detailsLay->setContentsMargins(28, 28, 28, 28);
    detailsLay->setSpacing(20);

    QLabel *detailsTitle = new QLabel("Détails Client");
    detailsTitle->setObjectName("detailsTitle");
    detailsLay->addWidget(detailsTitle);

    QFormLayout *formLay = new QFormLayout();
    formLay->setLabelAlignment(Qt::AlignRight);
    formLay->setHorizontalSpacing(16);
    formLay->setVerticalSpacing(14);

    editNom     = new QLineEdit(); editNom->setPlaceholderText("Obligatoire");
    editPrenom  = new QLineEdit(); editPrenom->setPlaceholderText("Obligatoire");
    comboSexe   = new QComboBox(); comboSexe->addItems({"Homme", "Femme"});
    editCin     = new QLineEdit();
    editPays    = new QLineEdit();
    editVille   = new QLineEdit();
    editAdresse = new QLineEdit();
    editEmail   = new QLineEdit();

    formLay->addRow("Nom :", editNom);
    formLay->addRow("Prénom :", editPrenom);
    formLay->addRow("Sexe :", comboSexe);
    formLay->addRow("CIN :", editCin);
    formLay->addRow("Pays :", editPays);
    formLay->addRow("Ville :", editVille);
    formLay->addRow("Adresse :", editAdresse);
    formLay->addRow("Email :", editEmail);

    detailsLay->addLayout(formLay);

    // ── SAVE BUTTON ────────────────────────────────────────────────
    QPushButton *btnSave = new QPushButton("Enregistrer Client");
    btnSave->setStyleSheet(R"(
        QPushButton {
            background-color: #C0A070;          // plus clair que #8C6642
            color: white;
            border: none;
            border-radius: 8px;
            padding: 14px 40px;
            font-size: 15px;
            font-weight: bold;
            min-height: 52px;
        }
        QPushButton:hover { background-color: #D0B080; }
        QPushButton:pressed { background-color: #A3835F; }
    )");
    detailsLay->addWidget(btnSave);
    detailsLay->addStretch();

    splitter->addWidget(detailsPanel);
    splitter->setSizes({860, 560});

    contentLayout->addWidget(splitter);
    mainLayout->addWidget(contentArea);

    // ────────────────────── CONNECTIONS ───────────────────────────
    connect(btnRefresh, &QPushButton::clicked, this, &MainWindow::refreshTable);
    connect(btnNew, &QPushButton::clicked, this, [=](){
        currentClientIndex = -1;
        clearForm();
        tableClients->clearSelection();
    });
    connect(tableClients, &QTableWidget::itemSelectionChanged, this, [=](){
        int row = tableClients->currentRow();
        if (row >= 0 && row < clients.size()) {
            currentClientIndex = row;
            fillFormFromCurrent();
        }
    });
    connect(btnDelete, &QPushButton::clicked, this, [=](){
        if (currentClientIndex < 0) {
            QMessageBox::warning(this, "Sélection requise", "Veuillez sélectionner un client dans la liste.");
            return;
        }
        if (QMessageBox::question(this, "Confirmer suppression",
                                  "Voulez-vous vraiment supprimer ce client ?") == QMessageBox::Yes) {
            clients.removeAt(currentClientIndex);
            currentClientIndex = -1;
            clearForm();
            refreshTable();
        }
    });
    // SAVE BUTTON ACTION
    connect(btnSave, &QPushButton::clicked, this, [=](){
        QString nom = editNom->text().trimmed();
        QString prenom = editPrenom->text().trimmed();
        if (nom.isEmpty() || prenom.isEmpty()) {
            QMessageBox::warning(this, "Champs obligatoires",
                                 "Le Nom et le Prénom sont obligatoires.");
            return;
        }
        Client client;
        client.setNom(nom);
        client.setPrenom(prenom);
        client.setSexe(comboSexe->currentText());
        client.setCin(editCin->text().trimmed());
        client.setPays(editPays->text().trimmed());
        client.setVille(editVille->text().trimmed());
        client.setAdresse(editAdresse->text().trimmed());
        client.setEmail(editEmail->text().trimmed());
        if (currentClientIndex >= 0 && currentClientIndex < clients.size()) {
            clients[currentClientIndex] = client;
            QMessageBox::information(this, "Succès", "Client mis à jour avec succès.");
        } else {
            clients.append(client);
            currentClientIndex = clients.size() - 1;
            tableClients->selectRow(currentClientIndex);
            QMessageBox::information(this, "Succès", "Nouveau client ajouté avec succès.");
        }
        refreshTable();
    });

    // Initial load
    refreshTable();
}

// ────────────────────── HELPER METHODS ────────────────────────────
void MainWindow::refreshTable()
{
    tableClients->setRowCount(clients.size());
    for (int i = 0; i < clients.size(); ++i) {
        const Client &c = clients.at(i);
        tableClients->setItem(i, 0, new QTableWidgetItem(c.getNom()));
        tableClients->setItem(i, 1, new QTableWidgetItem(c.getPrenom()));
        tableClients->setItem(i, 2, new QTableWidgetItem(c.getSexe()));
        tableClients->setItem(i, 3, new QTableWidgetItem(c.getCin()));
        tableClients->setItem(i, 4, new QTableWidgetItem(c.getPays()));
        tableClients->setItem(i, 5, new QTableWidgetItem(c.getVille()));
        tableClients->setItem(i, 6, new QTableWidgetItem(c.getAdresse()));
        tableClients->setItem(i, 7, new QTableWidgetItem(c.getEmail()));
    }
    if (currentClientIndex >= 0 && currentClientIndex < clients.size()) {
        tableClients->selectRow(currentClientIndex);
    }
}

void MainWindow::clearForm()
{
    editNom->clear();
    editPrenom->clear();
    comboSexe->setCurrentIndex(0);
    editCin->clear();
    editPays->clear();
    editVille->clear();
    editAdresse->clear();
    editEmail->clear();
}

void MainWindow::fillFormFromCurrent()
{
    if (currentClientIndex < 0 || currentClientIndex >= clients.size()) return;
    const Client &c = clients.at(currentClientIndex);
    editNom->setText(c.getNom());
    editPrenom->setText(c.getPrenom());
    comboSexe->setCurrentText(c.getSexe());
    editCin->setText(c.getCin());
    editPays->setText(c.getPays());
    editVille->setText(c.getVille());
    editAdresse->setText(c.getAdresse());
    editEmail->setText(c.getEmail());
}
