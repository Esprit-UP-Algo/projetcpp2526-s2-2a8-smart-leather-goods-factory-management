// mainwindow.cpp
#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QMessageBox>
#include <QDate>
#include <QPixmap>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDateEdit>

// ────────────────────────────────────────────────
//   Dialog d'ajout / modification client (popup)
// ────────────────────────────────────────────────
class ClientDialog : public QDialog
{
public:
    ClientDialog(QWidget *parent = nullptr, bool isEdit = false)
        : QDialog(parent)
    {
        setWindowTitle(isEdit ? "Modifier Client" : "Créer Client");
        setMinimumSize(520, 580);

        QVBoxLayout *mainLay = new QVBoxLayout(this);
        mainLay->setContentsMargins(28, 24, 28, 24);
        mainLay->setSpacing(16);

        QLabel *title = new QLabel(isEdit ? "Modifier le client" : "Nouveau client");
        title->setStyleSheet("font-size: 20px; font-weight: bold; color: #5C4033;");
        mainLay->addWidget(title);

        QFormLayout *form = new QFormLayout();
        form->setLabelAlignment(Qt::AlignRight);
        form->setHorizontalSpacing(14);
        form->setVerticalSpacing(12);

        nomEdit       = new QLineEdit(); nomEdit->setPlaceholderText("Obligatoire");
        prenomEdit    = new QLineEdit(); prenomEdit->setPlaceholderText("Obligatoire");
        sexeCombo     = new QComboBox(); sexeCombo->addItems({"Homme", "Femme"});
        cinEdit       = new QLineEdit();
        paysEdit      = new QLineEdit();
        villeEdit     = new QLineEdit();
        adresseEdit   = new QLineEdit();
        emailEdit     = new QLineEdit();
        dateInscrit   = new QDateEdit(QDate::currentDate());
        dateInscrit->setCalendarPopup(true);
        dateInscrit->setDisplayFormat("dd/MM/yyyy");

        form->addRow("Nom :", nomEdit);
        form->addRow("Prénom :", prenomEdit);
        form->addRow("Sexe :", sexeCombo);
        form->addRow("CIN :", cinEdit);
        form->addRow("Pays :", paysEdit);
        form->addRow("Ville :", villeEdit);
        form->addRow("Adresse :", adresseEdit);
        form->addRow("Email :", emailEdit);
        form->addRow("Date inscription :", dateInscrit);

        mainLay->addLayout(form);

        QDialogButtonBox *btnBox = new QDialogButtonBox(
            QDialogButtonBox::Save | QDialogButtonBox::Cancel);
        QPushButton *saveBtn = btnBox->button(QDialogButtonBox::Save);
        saveBtn->setText("Enregistrer");
        saveBtn->setObjectName("btnAdd");   // pour réutiliser le style QSS

        mainLay->addWidget(btnBox);

        connect(btnBox, &QDialogButtonBox::accepted, this, &ClientDialog::accept);
        connect(btnBox, &QDialogButtonBox::rejected, this, &ClientDialog::reject);
    }

    // Récupérer les valeurs saisies
    void fillFromClient(const Client &c) {
        nomEdit->setText(c.getNom());
        prenomEdit->setText(c.getPrenom());
        sexeCombo->setCurrentText(c.getSexe());
        cinEdit->setText(c.getCin());
        paysEdit->setText(c.getPays());
        villeEdit->setText(c.getVille());
        adresseEdit->setText(c.getAdresse());
        emailEdit->setText(c.getEmail());
        dateInscrit->setDate(c.getDateInscrit());
    }

    Client getClient() const {
        Client c;
        c.setNom(nomEdit->text().trimmed());
        c.setPrenom(prenomEdit->text().trimmed());
        c.setSexe(sexeCombo->currentText());
        c.setCin(cinEdit->text().trimmed());
        c.setPays(paysEdit->text().trimmed());
        c.setVille(villeEdit->text().trimmed());
        c.setAdresse(adresseEdit->text().trimmed());
        c.setEmail(emailEdit->text().trimmed());
        c.setDateInscrit(dateInscrit->date());
        return c;
    }

private:
    QLineEdit *nomEdit, *prenomEdit, *cinEdit, *paysEdit,
        *villeEdit, *adresseEdit, *emailEdit;
    QComboBox *sexeCombo;
    QDateEdit *dateInscrit;
};


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


    // ────────────────────── MAIN CONTENT ──────────────────────────
    QWidget *contentArea = new QWidget();
    contentArea->setObjectName("contentArea");

    QHBoxLayout *contentLayout = new QHBoxLayout(contentArea);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);

    // ────────────────────── LEFT NAVIGATION SIDEBAR ───────────────
    QWidget *Sidebar = new QWidget();
    Sidebar->setObjectName("sidebar");
    Sidebar->setFixedWidth(280);

    QVBoxLayout *navLay = new QVBoxLayout(Sidebar);
    navLay->setContentsMargins(20, 20, 20, 30);
    navLay->setSpacing(12);

    // Logo
    QLabel *logoLabel = new QLabel();
    logoLabel->setObjectName("sidebarLogo");
    logoLabel->setAlignment(Qt::AlignCenter);
    logoLabel->setFixedHeight(140);
    logoLabel->setMinimumWidth(140);

    QPixmap logoPix("C:/Users/USER/Desktop/cpp/test2/logo.PNG");
    if (!logoPix.isNull()) {
        logoLabel->setPixmap(logoPix.scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));
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

    // User role
    QLabel *role = new QLabel("Manager de Clients");
    role->setObjectName("sidebarRole");
    role->setAlignment(Qt::AlignCenter);
    navLay->addWidget(role);
    navLay->addSpacing(30);

    // Navigation items
    QStringList menuItems = {"Tableau de bord", "Clients", "Statistiques", "Paramètres"};
    for (const QString &text : menuItems) {
        QPushButton *item = new QPushButton(text);
        item->setProperty("navItem", true);
        if (text == "Clients") {
            item->setObjectName("btnClients");
        }
        navLay->addWidget(item);
    }

    navLay->addStretch();

    QPushButton *logout = new QPushButton("Déconnexion");
    logout->setObjectName("logout");
    navLay->addWidget(logout);

    contentLayout->addWidget(Sidebar);

    // ────────────────────── TABLEAU CENTRAL (seul maintenant) ────────────────
    QWidget *listPanel = new QWidget();
    QVBoxLayout *listLay = new QVBoxLayout(listPanel);
    listLay->setContentsMargins(24, 24, 24, 24);
    listLay->setSpacing(16);

    QLabel *listTitle = new QLabel("HISTORIQUE DES CLIENTS");
    listTitle->setObjectName("listTitle");

    // Add to layout **with center alignment**
    listLay->addWidget(listTitle, 0, Qt::AlignHCenter);

    QLineEdit *searchEdit = new QLineEdit();
    searchEdit->setPlaceholderText("Rechercher un client...");
    searchEdit->setObjectName("searchBox");
    listLay->addWidget(searchEdit);
    searchEdit->setFixedWidth(400);



    // Ligne de boutons au-dessus du tableau
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(10);

    QPushButton *btnNew     = new QPushButton(" + Créer");
    QPushButton *btnDelete  = new QPushButton(" 🗑 Supprimer");
    QPushButton *btnExport  = new QPushButton(" 📄 Exporter PDF");
    QPushButton *btnPrint   = new QPushButton(" 🖨 Imprimer");
    QPushButton *btnRefresh = new QPushButton(" 🔄 Actualiser");

    btnNew->setObjectName("btnAdd");
    btnDelete->setObjectName("btnDelete");
    btnExport->setObjectName("btnExport");
    btnPrint->setObjectName("btnPrint");
    btnRefresh->setObjectName("btnRefresh");

    for (auto btn : {btnNew, btnDelete, btnExport, btnPrint, btnRefresh})
        btnLayout->addWidget(btn);

    listLay->addLayout(btnLayout); // ← ajoute au-dessus du tableau







    tableClients = new QTableWidget(0, 8);
    tableClients->setObjectName("employeeTable");
    tableClients->setHorizontalHeaderLabels({"Nom", "Prénom", "Sexe", "CIN", "Pays", "Ville", "Adresse", "Email"});
    tableClients->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableClients->setAlternatingRowColors(true);
    listLay->addWidget(tableClients, 1);

    contentLayout->addWidget(listPanel, 1);   // ← prend tout l'espace restant

    mainLayout->addWidget(contentArea);

    // ────────────────────── CONNECTIONS ───────────────────────────

    connect(btnNew, &QPushButton::clicked, this, [this](){
        // Ouvre le popup pour créer un nouveau client
        ClientDialog dlg(this); // isEdit = false par défaut
        if (dlg.exec() == QDialog::Accepted) {
            Client client = dlg.getClient();

            // Vérifie les champs obligatoires
            if (client.getNom().isEmpty() || client.getPrenom().isEmpty()) {
                QMessageBox::warning(this, "Champs obligatoires",
                                     "Le Nom et le Prénom sont obligatoires.");
                return;
            }

            // Ajoute le client à la liste
            clients.append(client);

            // Optionnel : rafraîchir le tableau si tu as une fonction refreshTable()

        }
    });


    // ... ajoute ici les autres connexions (delete, edit, etc.) quand tu les implémenteras ...
}

// (tu gardes tes fonctions refreshTable(), clearForm(), fillFormFromCurrent() si tu les avais déjà)
