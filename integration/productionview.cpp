#include "productionview.h"
#include "productiondao.h"
#include "connection.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialog>
#include <QFormLayout>
#include <QDateEdit>
#include <QSpinBox>
#include <QScrollArea>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QBrush>
#include <QColor>
#include <QTableWidgetItem>
#include <QMenu>
#include <QCursor>
#include <QTextEdit>
#include <QFileDialog>
#include <QPrinter>
#include <QTextDocument>
#include <QChart>
#include <QChartView>
#include <QPieSeries>
#include <QPieSlice>
#include <QBarSet>
#include <QBarSeries>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QPainter>
#include <QTime>
#include <QDateTime>
#include <QFrame>
#include <QGroupBox>

// ═══════════════════════════════════════════════════════════════════════════
// IMPLÉMENTATION: ProductionViewModel
// ═══════════════════════════════════════════════════════════════════════════

ProductionViewModel::ProductionViewModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int ProductionViewModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_commandes.count();
}

int ProductionViewModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return COL_COUNT;
}

QVariant ProductionViewModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_commandes.count())
        return QVariant();
    
    const ProductionCommande &cmd = m_commandes.at(index.row());
    
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case COL_REFERENCE: return cmd.reference;
        case COL_EMPLOYE: return cmd.employe;
        case COL_TYPE: return cmd.type;
        case COL_QUANTITE: return cmd.quantite;
        case COL_DATE_CREATION: return cmd.dateCreation.toString("dd/MM/yyyy");
        case COL_STATUT: return cmd.etatProduction;
        case COL_PRIORITE: return cmd.priorite;
        case COL_MONTANT: return QString::number(cmd.montant, 'f', 2) + " DT";
        }
    }
    
    if (role == Qt::BackgroundRole) {
        if (index.column() == COL_STATUT) {
            if (cmd.etatProduction == "Terminé") return QBrush(QColor("#27AE60"));
            if (cmd.etatProduction == "En cours") return QBrush(QColor("#F39C12"));
            if (cmd.etatProduction == "Bloqué") return QBrush(QColor("#E74C3C"));
            if (cmd.etatProduction == "Planifié") return QBrush(QColor("#3498DB"));
        }
    }
    
    if (role == Qt::ForegroundRole) {
        if (index.column() == COL_STATUT) {
            return QBrush(Qt::white);
        }
    }
    
    if (role == Qt::TextAlignmentRole) {
        if (index.column() == COL_MONTANT) {
            return int(Qt::AlignRight | Qt::AlignVCenter);
        }
        return int(Qt::AlignLeft | Qt::AlignVCenter);
    }
    
    return QVariant();
}

QVariant ProductionViewModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    
    if (orientation == Qt::Horizontal) {
        return getColumnName(section);
    }
    
    return section + 1;
}

QString ProductionViewModel::getColumnName(int column) const
{
    switch (column) {
    case COL_REFERENCE: return "Référence";
    case COL_EMPLOYE: return "Employé";
    case COL_TYPE: return "Type";
    case COL_QUANTITE: return "Quantité";
    case COL_DATE_CREATION: return "Date Création";
    case COL_STATUT: return "Statut";
    case COL_PRIORITE: return "Priorité";
    case COL_MONTANT: return "Montant";
    default: return QString();
    }
}

void ProductionViewModel::setCommandes(const QList<ProductionCommande> &commandes)
{
    beginResetModel();
    m_commandes = commandes;
    endResetModel();
}

void ProductionViewModel::addCommande(const ProductionCommande &commande)
{
    beginInsertRows(QModelIndex(), m_commandes.count(), m_commandes.count());
    m_commandes.append(commande);
    endInsertRows();
}

void ProductionViewModel::updateCommande(int row, const ProductionCommande &commande)
{
    if (row >= 0 && row < m_commandes.count()) {
        m_commandes[row] = commande;
        emit dataChanged(index(row, 0), index(row, COL_COUNT - 1));
    }
}

ProductionCommande ProductionViewModel::getCommande(int row) const
{
    if (row >= 0 && row < m_commandes.count()) {
        return m_commandes.at(row);
    }
    return ProductionCommande();
}

void ProductionViewModel::clear()
{
    beginResetModel();
    m_commandes.clear();
    endResetModel();
}

void ProductionViewModel::loadFromDatabase()
{
    beginResetModel();
    m_commandes.clear();
    
    QSqlQuery query(Connection::instance()->getDatabase());
    query.prepare("SELECT C.REFERENCE, (E.NOM || ' ' || E.PRENOM) AS EMPLOYE, "
                  "C.TYPE, C.QUANTITE, C.DATE_CREATION, C.STATUT, C.PRIORITE, C.MONTANT, C.ID_COMMANDE "
                  "FROM COMMANDES C "
                  "LEFT JOIN EMPLOYES E ON C.ID_EMPLOYE = E.ID_EMPLOYE "
                  "ORDER BY C.DATE_CREATION DESC");
    
    if (query.exec()) {
        while (query.next()) {
            ProductionCommande cmd;
            cmd.reference = query.value(0).toString();
            cmd.employe = query.value(1).toString();
            cmd.type = query.value(2).toString();
            cmd.quantite = query.value(3).toInt();
            cmd.dateCreation = query.value(4).toDate();
            cmd.etatProduction = query.value(5).toString();
            cmd.priorite = query.value(6).toString();
            cmd.montant = query.value(7).toDouble();
            cmd.idCommande = query.value(8).toInt();
            
            m_commandes.append(cmd);
        }
    } else {
        qDebug() << "❌ Erreur chargement données:" << query.lastError().text();
    }
    
    endResetModel();
}

// ═══════════════════════════════════════════════════════════════════════════
// IMPLÉMENTATION: ProductionView
// ═══════════════════════════════════════════════════════════════════════════

ProductionView::ProductionView(QWidget *parent)
    : QMainWindow(parent)
    , m_timerAlertes(nullptr)
    , m_joursAlerte(3)
    , m_seuilAvancement(50)
    , m_intervalleVerification(5)
{
    setWindowTitle("Vue Production Complète - Planification, Suivi & Livraison");
    setMinimumSize(1200, 650);  // Taille réduite
    resize(1200, 650);          // Taille par défaut
    
    setupUI();
    setupConnections();
    setupTimer();
    loadData();
}

ProductionView::~ProductionView()
{
}

void ProductionView::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    centralWidget->setStyleSheet("background-color: #F8F9FA;");  // Gris très clair moderne
    
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);
    
    // Titre moderne avec dégradé bleu
    QWidget *titleWidget = new QWidget(this);
    titleWidget->setStyleSheet(
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "stop:0 #2C3E50, stop:1 #3498DB); "
        "border-radius: 8px; padding: 12px;"
    );
    QHBoxLayout *titleLayout = new QHBoxLayout(titleWidget);
    QLabel *title = new QLabel("📊 GESTION DE LA PRODUCTION");
    title->setStyleSheet("font-size: 20px; font-weight: bold; color: white; letter-spacing: 1px;");
    title->setAlignment(Qt::AlignCenter);
    titleLayout->addWidget(title);
    mainLayout->addWidget(titleWidget);
    
    // Barre de filtres moderne
    QWidget *filterWidget = new QWidget(this);
    filterWidget->setStyleSheet(
        "QWidget { background-color: white; border-radius: 8px; padding: 12px; "
        "border: 1px solid #E0E0E0; }"
        "QLabel { color: #2C3E50; font-weight: 600; font-size: 12px; }"
    );
    QHBoxLayout *filterLayout = new QHBoxLayout(filterWidget);
    filterLayout->setSpacing(15);
    
    m_searchBox = new QLineEdit(this);
    m_searchBox->setPlaceholderText("🔍 Rechercher...");
    m_searchBox->setMinimumWidth(250);
    m_searchBox->setMinimumHeight(36);
    m_searchBox->setStyleSheet(
        "QLineEdit { "
        "background-color: #F8F9FA; "
        "border: 2px solid #E0E0E0; "
        "border-radius: 6px; "
        "padding: 8px 12px; "
        "font-size: 13px; "
        "color: #2C3E50; "
        "}"
        "QLineEdit:focus { "
        "border-color: #3498DB; "
        "background-color: white; "
        "}"
    );
    
    m_etatFilter = new QComboBox(this);
    m_etatFilter->addItems({"Tous les états", "Planifié", "En cours", "Bloqué", "Terminé"});
    m_etatFilter->setMinimumWidth(150);
    m_etatFilter->setMinimumHeight(36);
    m_etatFilter->setStyleSheet(
        "QComboBox { "
        "background-color: #F8F9FA; "
        "border: 2px solid #E0E0E0; "
        "border-radius: 6px; "
        "padding: 6px 12px; "
        "font-size: 13px; "
        "color: #2C3E50; "
        "font-weight: 500; "
        "}"
        "QComboBox:hover { "
        "border-color: #3498DB; "
        "}"
        "QComboBox::drop-down { "
        "border: none; "
        "width: 25px; "
        "}"
        "QComboBox::down-arrow { "
        "image: none; "
        "border-left: 4px solid transparent; "
        "border-right: 4px solid transparent; "
        "border-top: 5px solid #3498DB; "
        "}"
    );
    
    m_statutLivraisonFilter = new QComboBox(this);
    m_statutLivraisonFilter->addItems({"Tous les statuts", "Non expédiée", "En livraison", "Livrée"});
    m_statutLivraisonFilter->setMinimumWidth(150);
    m_statutLivraisonFilter->setMinimumHeight(36);
    m_statutLivraisonFilter->setStyleSheet(m_etatFilter->styleSheet());
    
    QLabel *lblRecherche = new QLabel("Recherche:", this);
    QLabel *lblEtat = new QLabel("État:", this);
    QLabel *lblStatut = new QLabel("Livraison:", this);
    
    filterLayout->addWidget(lblRecherche);
    filterLayout->addWidget(m_searchBox);
    filterLayout->addWidget(lblEtat);
    filterLayout->addWidget(m_etatFilter);
    filterLayout->addWidget(lblStatut);
    filterLayout->addWidget(m_statutLivraisonFilter);
    filterLayout->addStretch();
    
    mainLayout->addWidget(filterWidget);
    
    // Boutons d'action modernes
    QWidget *buttonWidget = new QWidget(this);
    buttonWidget->setStyleSheet("background-color: transparent;");
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);
    buttonLayout->setSpacing(12);
    
    QString btnStyle = 
        "QPushButton { "
        "background-color: #3498DB; "
        "color: white; "
        "border: none; "
        "border-radius: 6px; "
        "padding: 10px 20px; "
        "font-size: 13px; "
        "font-weight: 600; "
        "min-height: 38px; "
        "}"
        "QPushButton:hover { "
        "background-color: #2980B9; "
        "}"
        "QPushButton:pressed { "
        "background-color: #21618C; "
        "}";
    
    m_btnPlanification = new QPushButton("📅 Planification", this);
    m_btnPlanification->setStyleSheet(btnStyle);
    m_btnPlanification->setCursor(Qt::PointingHandCursor);
    
    m_btnDetails = new QPushButton("📊 Détails", this);
    m_btnDetails->setStyleSheet(btnStyle);
    m_btnDetails->setCursor(Qt::PointingHandCursor);
    
    m_btnRefresh = new QPushButton("🔄 Actualiser", this);
    m_btnRefresh->setStyleSheet(
        "QPushButton { "
        "background-color: #27AE60; "
        "color: white; "
        "border: none; "
        "border-radius: 6px; "
        "padding: 10px 20px; "
        "font-size: 13px; "
        "font-weight: 600; "
        "min-height: 38px; "
        "}"
        "QPushButton:hover { "
        "background-color: #229954; "
        "}"
        "QPushButton:pressed { "
        "background-color: #1E8449; "
        "}"
    );
    m_btnRefresh->setCursor(Qt::PointingHandCursor);
    
    buttonLayout->addWidget(m_btnPlanification);
    buttonLayout->addWidget(m_btnDetails);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_btnRefresh);
    
    mainLayout->addWidget(buttonWidget);
    
    // TableView avec design moderne et professionnel
    m_tableView = new QTableView(this);
    m_tableView->setStyleSheet(
        "QTableView { "
        "background-color: white; "
        "border: 1px solid #E0E0E0; "
        "border-radius: 8px; "
        "gridline-color: #F0F0F0; "
        "selection-background-color: #3498DB; "
        "selection-color: white; "
        "font-size: 12px; "
        "font-family: 'Segoe UI', Arial, sans-serif; "
        "}"
        "QTableView::item { "
        "padding: 8px 6px; "
        "border-bottom: 1px solid #F5F5F5; "
        "}"
        "QTableView::item:selected { "
        "background-color: #3498DB; "
        "color: white; "
        "}"
        "QTableView::item:hover { "
        "background-color: #EBF5FB; "
        "}"
        "QHeaderView::section { "
        "background-color: #34495E; "
        "color: white; "
        "padding: 10px 8px; "
        "border: none; "
        "border-right: 1px solid #2C3E50; "
        "font-weight: 600; "
        "font-size: 11px; "
        "text-transform: uppercase; "
        "letter-spacing: 0.5px; "
        "}"
        "QHeaderView::section:hover { "
        "background-color: #2C3E50; "
        "}"
        "QTableView QTableCornerButton::section { "
        "background-color: #34495E; "
        "border: none; "
        "}"
    );
    m_tableView->setAlternatingRowColors(true);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableView->setSortingEnabled(true);
    m_tableView->verticalHeader()->setVisible(false);
    m_tableView->horizontalHeader()->setStretchLastSection(false);
    m_tableView->setShowGrid(true);
    m_tableView->setGridStyle(Qt::SolidLine);
    m_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    
    // Modèle et proxy
    m_model = new ProductionViewModel(this);
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_model);
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_proxyModel->setFilterKeyColumn(ProductionViewModel::COL_REFERENCE);
    
    m_tableView->setModel(m_proxyModel);
    
    mainLayout->addWidget(m_tableView);
    
    // Légende moderne et compacte
    QWidget *legendWidget = new QWidget(this);
    legendWidget->setStyleSheet(
        "background: white; "
        "border: 1px solid #E0E0E0; "
        "border-radius: 8px; "
        "padding: 10px;"
    );
    QHBoxLayout *legendLayout = new QHBoxLayout(legendWidget);
    
    QLabel *legend = new QLabel(
        "<b style='color:#2C3E50; font-size:12px;'>États:</b> "
        "<span style='background:#27AE60;color:white;padding:4px 10px;border-radius:4px;font-weight:600;margin-left:8px;font-size:11px;'>Terminé</span> "
        "<span style='background:#F39C12;color:white;padding:4px 10px;border-radius:4px;font-weight:600;margin-left:6px;font-size:11px;'>En cours</span> "
        "<span style='background:#E74C3C;color:white;padding:4px 10px;border-radius:4px;font-weight:600;margin-left:6px;font-size:11px;'>Bloqué</span> "
        "<span style='background:#3498DB;color:white;padding:4px 10px;border-radius:4px;font-weight:600;margin-left:6px;font-size:11px;'>Planifié</span>"
    );
    legend->setStyleSheet("font-size: 12px;");
    legendLayout->addWidget(legend);
    legendLayout->addStretch();
    
    mainLayout->addWidget(legendWidget);
}

void ProductionView::setupConnections()
{
    connect(m_searchBox, &QLineEdit::textChanged, this, &ProductionView::onSearchTextChanged);
    connect(m_etatFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &ProductionView::onEtatFilterChanged);
    connect(m_statutLivraisonFilter, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &ProductionView::onStatutLivraisonFilterChanged);
    connect(m_btnDetails, &QPushButton::clicked, this, &ProductionView::onDetailsClicked);
    connect(m_btnPlanification, &QPushButton::clicked, this, &ProductionView::onPlanificationClicked);
    connect(m_btnRefresh, &QPushButton::clicked, this, &ProductionView::onRefreshClicked);
}

void ProductionView::loadData()
{
    qDebug() << "ProductionView::loadData() appelée";
    m_model->loadFromDatabase();
    qDebug() << "Nombre de lignes dans le modèle:" << m_model->rowCount();
    
    // Configurer les largeurs de colonnes optimisées
    m_tableView->setColumnWidth(ProductionViewModel::COL_REFERENCE, 150);
    m_tableView->setColumnWidth(ProductionViewModel::COL_EMPLOYE, 180);
    m_tableView->setColumnWidth(ProductionViewModel::COL_TYPE, 150);
    m_tableView->setColumnWidth(ProductionViewModel::COL_QUANTITE, 80);
    m_tableView->setColumnWidth(ProductionViewModel::COL_DATE_CREATION, 120);
    m_tableView->setColumnWidth(ProductionViewModel::COL_STATUT, 120);
    m_tableView->setColumnWidth(ProductionViewModel::COL_PRIORITE, 100);
    m_tableView->setColumnWidth(ProductionViewModel::COL_MONTANT, 120);
    
    // Vérifier les alertes au démarrage
    verifierAlertes();
    
    if (m_model->rowCount() == 0) {
        qDebug() << "⚠ Aucune donnée chargée!";
        QMessageBox::information(this, "Information", 
            "Aucune commande trouvée dans la base de données.\n\n"
            "Ajoutez d'abord des commandes depuis l'onglet Production.");
    }
}

void ProductionView::onSearchTextChanged(const QString &text)
{
    m_proxyModel->setFilterFixedString(text);
}

void ProductionView::onEtatFilterChanged(int index)
{
    if (index == 0) {
        m_proxyModel->setFilterKeyColumn(ProductionViewModel::COL_REFERENCE);
        m_proxyModel->setFilterFixedString(m_searchBox->text());
    } else {
        m_proxyModel->setFilterKeyColumn(ProductionViewModel::COL_STATUT);
        m_proxyModel->setFilterFixedString(m_etatFilter->currentText());
    }
}

void ProductionView::onStatutLivraisonFilterChanged(int index)
{
    // Cette fonction n'est plus utilisée avec le modèle simplifié
    // On peut la garder vide ou la supprimer
    if (index == 0) {
        m_proxyModel->setFilterKeyColumn(ProductionViewModel::COL_REFERENCE);
        m_proxyModel->setFilterFixedString(m_searchBox->text());
    }
}

void ProductionView::onDetailsClicked()
{
    QModelIndex currentIndex = m_tableView->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, "Attention", "Veuillez sélectionner une commande.");
        return;
    }
    
    QModelIndex sourceIndex = m_proxyModel->mapToSource(currentIndex);
    ProductionCommande cmd = m_model->getCommande(sourceIndex.row());
    
    QDialog dlg(this);
    dlg.setWindowTitle("Détails Complets - " + cmd.reference);
    dlg.setMinimumSize(750, 650);
    dlg.setStyleSheet(
        "QDialog { background-color: #F5F5F5; }"
        "QLabel { color: #424242; }"
    );
    
    QVBoxLayout *layout = new QVBoxLayout(&dlg);
    layout->setContentsMargins(25, 25, 25, 25);
    layout->setSpacing(20);
    
    // Titre avec fond
    QWidget *titleWidget = new QWidget(&dlg);
    titleWidget->setStyleSheet(
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "stop:0 #6D4C41, stop:1 #8D6E63); "
        "border-radius: 10px; padding: 15px;"
    );
    QHBoxLayout *titleLayout = new QHBoxLayout(titleWidget);
    QLabel *title = new QLabel("📋 DÉTAILS COMPLETS DE LA COMMANDE");
    title->setStyleSheet("font-size: 20px; font-weight: bold; color: white;");
    title->setAlignment(Qt::AlignCenter);
    titleLayout->addWidget(title);
    layout->addWidget(titleWidget);
    
    // Scroll area pour le contenu
    QScrollArea *scrollArea = new QScrollArea(&dlg);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("QScrollArea { border: none; background-color: transparent; }");
    
    QWidget *contentWidget = new QWidget();
    contentWidget->setStyleSheet("background-color: transparent;");
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setSpacing(15);
    
    auto addSection = [&](const QString &sectionTitle, const QString &icon) {
        QWidget *sectionWidget = new QWidget();
        sectionWidget->setStyleSheet(
            "background-color: white; "
            "border-radius: 10px; "
            "border: 2px solid #E0E0E0;"
        );
        QVBoxLayout *sectionLayout = new QVBoxLayout(sectionWidget);
        sectionLayout->setContentsMargins(20, 15, 20, 15);
        sectionLayout->setSpacing(12);
        
        QLabel *section = new QLabel(icon + " " + sectionTitle);
        section->setStyleSheet(
            "font-size: 16px; "
            "font-weight: bold; "
            "color: #6D4C41; "
            "background-color: transparent; "
            "border: none; "
            "padding-bottom: 10px; "
            "border-bottom: 2px solid #8D6E63;"
        );
        sectionLayout->addWidget(section);
        
        contentLayout->addWidget(sectionWidget);
        return sectionLayout;
    };
    
    auto addField = [&](QVBoxLayout *sectionLayout, const QString &label, const QString &value) {
        QWidget *fieldWidget = new QWidget();
        fieldWidget->setStyleSheet("background-color: transparent;");
        QHBoxLayout *fieldLayout = new QHBoxLayout(fieldWidget);
        fieldLayout->setContentsMargins(0, 5, 0, 5);
        
        QLabel *labelWidget = new QLabel(label + ":");
        labelWidget->setStyleSheet(
            "font-weight: bold; "
            "color: #616161; "
            "font-size: 13px; "
            "min-width: 180px;"
        );
        
        QLabel *valueLabel = new QLabel(value);
        valueLabel->setStyleSheet(
            "background: #FAFAFA; "
            "padding: 10px 15px; "
            "border-radius: 6px; "
            "border: 1px solid #E0E0E0; "
            "color: #212121; "
            "font-size: 13px; "
            "font-weight: 600;"
        );
        valueLabel->setWordWrap(true);
        
        fieldLayout->addWidget(labelWidget);
        fieldLayout->addWidget(valueLabel, 1);
        sectionLayout->addWidget(fieldWidget);
    };
    
    QVBoxLayout *identSection = addSection("IDENTIFICATION", "🔖");
    addField(identSection, "ID Commande", QString::number(cmd.idCommande));
    addField(identSection, "Référence", cmd.reference);
    addField(identSection, "Priorité", cmd.priorite);
    addField(identSection, "Date Livraison Prévue", cmd.dateLivraisonPrevue.toString("dd/MM/yyyy"));
    
    QVBoxLayout *planSection = addSection("PLANIFICATION", "📅");
    addField(planSection, "Date Début Prévue", cmd.dateDebutPrevue.toString("dd/MM/yyyy"));
    addField(planSection, "Date Fin Prévue", cmd.dateFinPrevue.toString("dd/MM/yyyy"));
    addField(planSection, "Atelier", cmd.atelier);
    addField(planSection, "Ordre de Passage", QString::number(cmd.ordrePassage));
    
    QVBoxLayout *suiviSection = addSection("SUIVI DE PRODUCTION", "🏭");
    addField(suiviSection, "État Production", cmd.etatProduction);
    addField(suiviSection, "Étape Actuelle", cmd.etapeActuelle);
    addField(suiviSection, "Avancement", cmd.getAvancementText());
    addField(suiviSection, "Retard", cmd.getRetardText());
    
    QVBoxLayout *livraisonSection = addSection("LIVRAISON", "🚚");
    addField(livraisonSection, "Société de Livraison", cmd.societeLivraison);
    addField(livraisonSection, "Numéro de Suivi", cmd.numeroSuiviColis.isEmpty() ? "Non attribué" : cmd.numeroSuiviColis);
    addField(livraisonSection, "Date Expédition Prévue", cmd.dateExpeditionPrevue.toString("dd/MM/yyyy"));
    addField(livraisonSection, "Date Expédition Réelle", cmd.dateExpeditionReelle.isValid() ? 
             cmd.dateExpeditionReelle.toString("dd/MM/yyyy") : "Non expédiée");
    addField(livraisonSection, "Statut Livraison", cmd.statutLivraison);
    
    scrollArea->setWidget(contentWidget);
    layout->addWidget(scrollArea);
    
    QPushButton *closeBtn = new QPushButton("✖ Fermer", &dlg);
    closeBtn->setStyleSheet(
        "QPushButton { "
        "background-color: #757575; "
        "color: white; "
        "border: none; "
        "border-radius: 8px; "
        "padding: 12px 35px; "
        "font-size: 14px; "
        "font-weight: bold; "
        "min-height: 45px; "
        "}"
        "QPushButton:hover { "
        "background-color: #616161; "
        "}"
    );
    closeBtn->setCursor(Qt::PointingHandCursor);
    connect(closeBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
    
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(closeBtn);
    layout->addLayout(btnLayout);
    
    dlg.exec();
}

void ProductionView::onPlanificationClicked()
{
    QModelIndex currentIndex = m_tableView->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, "Attention", "Veuillez sélectionner une commande.");
        return;
    }
    
    QModelIndex sourceIndex = m_proxyModel->mapToSource(currentIndex);
    ProductionCommande cmd = m_model->getCommande(sourceIndex.row());
    
    QDialog dlg(this);
    dlg.setWindowTitle("Modifier Planification - " + cmd.reference);
    dlg.setMinimumSize(450, 380);  // Taille réduite
    dlg.setMaximumSize(500, 420);  // Limite la taille maximale
    dlg.setStyleSheet("QDialog { background-color: #F5F5F5; }");
    
    QVBoxLayout *layout = new QVBoxLayout(&dlg);
    layout->setContentsMargins(25, 25, 25, 25);
    layout->setSpacing(20);
    
    // Titre avec fond
    QWidget *titleWidget = new QWidget(&dlg);
    titleWidget->setStyleSheet(
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "stop:0 #6D4C41, stop:1 #8D6E63); "
        "border-radius: 10px; padding: 15px;"
    );
    QHBoxLayout *titleLayout = new QHBoxLayout(titleWidget);
    QLabel *title = new QLabel("📅 MODIFIER LA PLANIFICATION");
    title->setStyleSheet("font-size: 20px; font-weight: bold; color: white;");
    title->setAlignment(Qt::AlignCenter);
    titleLayout->addWidget(title);
    layout->addWidget(titleWidget);
    
    // Formulaire dans un widget blanc
    QWidget *formWidget = new QWidget(&dlg);
    formWidget->setStyleSheet(
        "QWidget { "
        "background-color: white; "
        "border-radius: 10px; "
        "border: 2px solid #E0E0E0; "
        "}"
    );
    QVBoxLayout *formContainer = new QVBoxLayout(formWidget);
    formContainer->setContentsMargins(25, 25, 25, 25);
    
    QFormLayout *form = new QFormLayout();
    form->setSpacing(20);
    form->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    
    QString fieldStyle = 
        "background: #FAFAFA; "
        "border: 2px solid #BDBDBD; "
        "border-radius: 8px; "
        "padding: 10px 15px; "
        "font-size: 14px; "
        "color: #212121; "
        "min-height: 40px;";
    
    QString labelStyle = 
        "color: #424242; "
        "font-weight: bold; "
        "font-size: 14px;";
    
    QLabel *lblDateDebut = new QLabel("Date Début Prévue:");
    lblDateDebut->setStyleSheet(labelStyle);
    QDateEdit *dateDebut = new QDateEdit(&dlg);
    dateDebut->setDate(cmd.dateDebutPrevue);
    dateDebut->setCalendarPopup(true);
    dateDebut->setDisplayFormat("dd/MM/yyyy");
    dateDebut->setStyleSheet(fieldStyle);
    
    QLabel *lblDateFin = new QLabel("Date Fin Prévue:");
    lblDateFin->setStyleSheet(labelStyle);
    QDateEdit *dateFin = new QDateEdit(&dlg);
    dateFin->setDate(cmd.dateFinPrevue);
    dateFin->setCalendarPopup(true);
    dateFin->setDisplayFormat("dd/MM/yyyy");
    dateFin->setStyleSheet(fieldStyle);
    
    QLabel *lblAtelier = new QLabel("Atelier:");
    lblAtelier->setStyleSheet(labelStyle);
    QLineEdit *atelier = new QLineEdit(cmd.atelier, &dlg);
    atelier->setStyleSheet(fieldStyle);
    
    QLabel *lblOrdre = new QLabel("Ordre de Passage:");
    lblOrdre->setStyleSheet(labelStyle);
    QSpinBox *ordre = new QSpinBox(&dlg);
    ordre->setRange(1, 999);
    ordre->setValue(cmd.ordrePassage);
    ordre->setStyleSheet(fieldStyle);
    
    form->addRow(lblDateDebut, dateDebut);
    form->addRow(lblDateFin, dateFin);
    form->addRow(lblAtelier, atelier);
    form->addRow(lblOrdre, ordre);
    
    formContainer->addLayout(form);
    layout->addWidget(formWidget);
    layout->addStretch();
    
    // Boutons
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(15);
    
    QPushButton *saveBtn = new QPushButton("💾 Enregistrer", &dlg);
    saveBtn->setStyleSheet(
        "QPushButton { "
        "background-color: #4CAF50; "
        "color: white; "
        "border: none; "
        "border-radius: 8px; "
        "padding: 12px 30px; "
        "font-size: 14px; "
        "font-weight: bold; "
        "min-height: 45px; "
        "min-width: 150px; "
        "}"
        "QPushButton:hover { "
        "background-color: #45A049; "
        "}"
        "QPushButton:pressed { "
        "background-color: #3D8B40; "
        "}"
    );
    saveBtn->setCursor(Qt::PointingHandCursor);
    
    QPushButton *cancelBtn = new QPushButton("✖ Annuler", &dlg);
    cancelBtn->setStyleSheet(
        "QPushButton { "
        "background-color: #757575; "
        "color: white; "
        "border: none; "
        "border-radius: 8px; "
        "padding: 12px 30px; "
        "font-size: 14px; "
        "font-weight: bold; "
        "min-height: 45px; "
        "min-width: 150px; "
        "}"
        "QPushButton:hover { "
        "background-color: #616161; "
        "}"
        "QPushButton:pressed { "
        "background-color: #424242; "
        "}"
    );
    cancelBtn->setCursor(Qt::PointingHandCursor);
    
    connect(saveBtn, &QPushButton::clicked, [&]() {
        cmd.dateDebutPrevue = dateDebut->date();
        cmd.dateFinPrevue = dateFin->date();
        cmd.atelier = atelier->text();
        cmd.ordrePassage = ordre->value();
        
        m_model->updateCommande(sourceIndex.row(), cmd);
        
        QMessageBox msgBox(&dlg);
        msgBox.setWindowTitle("Succès");
        msgBox.setText("✅ Planification mise à jour avec succès!");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setStyleSheet(
            "QMessageBox { background-color: white; }"
            "QLabel { color: #212121; font-size: 14px; padding: 10px; }"
            "QPushButton { "
            "background-color: #4CAF50; color: white; border: none; "
            "border-radius: 6px; padding: 8px 20px; font-weight: bold; "
            "}"
        );
        msgBox.exec();
        dlg.accept();
    });
    
    connect(cancelBtn, &QPushButton::clicked, &dlg, &QDialog::reject);
    
    btnLayout->addStretch();
    btnLayout->addWidget(saveBtn);
    btnLayout->addWidget(cancelBtn);
    btnLayout->addStretch();
    layout->addLayout(btnLayout);
    
    dlg.exec();
}

void ProductionView::onRefreshClicked()
{
    loadData();
    QMessageBox::information(this, "Actualisation", "Données actualisées avec succès!");
}

// ═══════════════════════════════════════════════════════════════════════════
// IMPLÉMENTATION: Système d'alertes de retard
// ═══════════════════════════════════════════════════════════════════════════

void ProductionView::setupTimer()
{
    qDebug() << "ProductionView::setupTimer() - Configuration du timer d'alertes";
    
    // Créer le timer pour la vérification périodique
    m_timerAlertes = new QTimer(this);
    
    // Convertir l'intervalle de minutes en millisecondes
    int intervalleMs = m_intervalleVerification * 60 * 1000;
    m_timerAlertes->setInterval(intervalleMs);
    
    // Connecter le signal timeout au slot de vérification
    connect(m_timerAlertes, &QTimer::timeout, this, &ProductionView::verifierAlertes);
    
    // Démarrer le timer
    m_timerAlertes->start();
    
    qDebug() << "Timer configuré avec intervalle de" << m_intervalleVerification << "minutes";
}

void ProductionView::verifierAlertes()
{
    qDebug() << "ProductionView::verifierAlertes() - Vérification des alertes à" << QTime::currentTime().toString();
    
    if (!m_model) {
        qDebug() << "⚠ Modèle non initialisé";
        return;
    }
    
    int nbAlertes = 0;
    int nbRetards = 0;
    int nbRisques = 0;
    
    // Parcourir toutes les commandes
    for (int row = 0; row < m_model->rowCount(); ++row) {
        ProductionCommande cmd = m_model->getCommande(row);
        
        // Recalculer l'alerte
        ProductionCommande::AlerteRetard ancienneAlerte = cmd.alerteRetard;
        cmd.alerteRetard = cmd.calculerAlerteRetard(m_joursAlerte, m_seuilAvancement);
        
        // Si l'alerte a changé et n'est pas "Aucune"
        if (cmd.alerteRetard != ancienneAlerte && cmd.alerteRetard != ProductionCommande::Aucune) {
            // Créer une clé unique pour cette alerte (ID + type d'alerte)
            int cleAlerte = cmd.idCommande * 10 + static_cast<int>(cmd.alerteRetard);
            
            // Vérifier si cette alerte n'a pas déjà été notifiée
            if (!m_alertesNotifiees.contains(cleAlerte)) {
                // Afficher la notification
                afficherNotification(cmd);
                
                // Marquer comme notifiée
                m_alertesNotifiees.insert(cleAlerte);
                nbAlertes++;
            }
        }
        
        // Mettre à jour le modèle avec la nouvelle alerte
        m_model->updateCommande(row, cmd);
        
        // Compter les alertes
        if (cmd.alerteRetard == ProductionCommande::Retard) {
            nbRetards++;
        } else if (cmd.alerteRetard == ProductionCommande::Risque) {
            nbRisques++;
        }
    }
    
    qDebug() << "Vérification terminée:" << nbAlertes << "nouvelles alertes,"
             << nbRetards << "retards," << nbRisques << "risques";
}

void ProductionView::afficherNotification(const ProductionCommande &commande)
{
    qDebug() << "ProductionView::afficherNotification() - Affichage notification pour" << commande.reference;
    
    // Générer le message de notification
    QString message = commande.genererMessageNotification();
    
    // Créer une QMessageBox stylisée
    QMessageBox msgBox(this);
    
    // Configurer selon le type d'alerte
    if (commande.alerteRetard == ProductionCommande::Retard) {
        msgBox.setWindowTitle("⚠ ALERTE RETARD");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setStyleSheet(
            "QMessageBox { background-color: white; }"
            "QLabel { color: #D32F2F; font-size: 14px; padding: 15px; font-weight: bold; }"
            "QPushButton { "
            "background-color: #D32F2F; color: white; border: none; "
            "border-radius: 6px; padding: 10px 25px; font-weight: bold; "
            "}"
            "QPushButton:hover { background-color: #F44336; }"
        );
    } else if (commande.alerteRetard == ProductionCommande::Risque) {
        msgBox.setWindowTitle("⚡ ALERTE RISQUE");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStyleSheet(
            "QMessageBox { background-color: white; }"
            "QLabel { color: #F57C00; font-size: 14px; padding: 15px; font-weight: bold; }"
            "QPushButton { "
            "background-color: #FF9800; color: white; border: none; "
            "border-radius: 6px; padding: 10px 25px; font-weight: bold; "
            "}"
            "QPushButton:hover { background-color: #FB8C00; }"
        );
    }
    
    msgBox.setText(message);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

void ProductionView::recalculerToutesLesAlertes()
{
    qDebug() << "ProductionView::recalculerToutesLesAlertes() - Recalcul de toutes les alertes";
    
    if (!m_model) {
        qDebug() << "⚠ Modèle non initialisé";
        return;
    }
    
    // Réinitialiser le set des alertes notifiées
    m_alertesNotifiees.clear();
    
    // Parcourir toutes les commandes et recalculer les alertes
    for (int row = 0; row < m_model->rowCount(); ++row) {
        ProductionCommande cmd = m_model->getCommande(row);
        cmd.alerteRetard = cmd.calculerAlerteRetard(m_joursAlerte, m_seuilAvancement);
        m_model->updateCommande(row, cmd);
    }
    
    qDebug() << "Recalcul terminé pour" << m_model->rowCount() << "commandes";
}
