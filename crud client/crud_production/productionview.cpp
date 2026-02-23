#include "productionview.h"
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
        case COL_ID: return cmd.idCommande;
        case COL_REFERENCE: return cmd.reference;
        case COL_PRIORITE: return cmd.priorite;
        case COL_DATE_LIVRAISON: return cmd.dateLivraisonPrevue.toString("dd/MM/yyyy");
        case COL_DATE_DEBUT_PREVUE: return cmd.dateDebutPrevue.toString("dd/MM/yyyy");
        case COL_DATE_FIN_PREVUE: return cmd.dateFinPrevue.toString("dd/MM/yyyy");
        case COL_ATELIER: return cmd.atelier;
        case COL_ORDRE_PASSAGE: return cmd.ordrePassage;
        case COL_ETAT_PRODUCTION: return cmd.etatProduction;
        case COL_ETAPE_ACTUELLE: return cmd.etapeActuelle;
        case COL_AVANCEMENT: return cmd.getAvancementText();
        case COL_RETARD: return cmd.getRetardText();
        case COL_SOCIETE_LIVRAISON: return cmd.societeLivraison;
        case COL_NUMERO_SUIVI: return cmd.numeroSuiviColis;
        case COL_DATE_EXPEDITION_PREVUE: return cmd.dateExpeditionPrevue.toString("dd/MM/yyyy");
        case COL_DATE_EXPEDITION_REELLE: return cmd.dateExpeditionReelle.isValid() ? 
                                                cmd.dateExpeditionReelle.toString("dd/MM/yyyy") : "-";
        case COL_STATUT_LIVRAISON: return cmd.statutLivraison;
        }
    }
    
    if (role == Qt::BackgroundRole) {
        if (index.column() == COL_ETAT_PRODUCTION) {
            if (cmd.etatProduction == "Terminé") return QBrush(QColor("#4CAF50"));
            if (cmd.etatProduction == "En cours") return QBrush(QColor("#FF9800"));
            if (cmd.etatProduction == "Bloqué") return QBrush(QColor("#F44336"));
            if (cmd.etatProduction == "Planifié") return QBrush(QColor("#2196F3"));
        }
        
        if (index.column() == COL_RETARD && cmd.retard) {
            return QBrush(QColor("#FFCDD2"));
        }
        
        if (index.column() == COL_STATUT_LIVRAISON) {
            if (cmd.statutLivraison == "Livrée") return QBrush(QColor("#4CAF50"));
            if (cmd.statutLivraison == "En livraison") return QBrush(QColor("#FF9800"));
        }
    }
    
    if (role == Qt::ForegroundRole) {
        if (index.column() == COL_ETAT_PRODUCTION) {
            return QBrush(Qt::white);
        }
        if (index.column() == COL_RETARD && cmd.retard) {
            return QBrush(QColor("#C62828"));
        }
        if (index.column() == COL_STATUT_LIVRAISON && 
            (cmd.statutLivraison == "Livrée" || cmd.statutLivraison == "En livraison")) {
            return QBrush(Qt::white);
        }
    }
    
    if (role == Qt::TextAlignmentRole) {
        if (index.column() == COL_ID || index.column() == COL_ORDRE_PASSAGE || 
            index.column() == COL_AVANCEMENT) {
            return Qt::AlignCenter;
        }
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
    case COL_ID: return "ID";
    case COL_REFERENCE: return "Référence";
    case COL_PRIORITE: return "Priorité";
    case COL_DATE_LIVRAISON: return "Date Livraison";
    case COL_DATE_DEBUT_PREVUE: return "Début Prévu";
    case COL_DATE_FIN_PREVUE: return "Fin Prévue";
    case COL_ATELIER: return "Atelier";
    case COL_ORDRE_PASSAGE: return "Ordre";
    case COL_ETAT_PRODUCTION: return "État Production";
    case COL_ETAPE_ACTUELLE: return "Étape";
    case COL_AVANCEMENT: return "Avancement";
    case COL_RETARD: return "Retard";
    case COL_SOCIETE_LIVRAISON: return "Société Livraison";
    case COL_NUMERO_SUIVI: return "N° Suivi";
    case COL_DATE_EXPEDITION_PREVUE: return "Expédition Prévue";
    case COL_DATE_EXPEDITION_REELLE: return "Expédition Réelle";
    case COL_STATUT_LIVRAISON: return "Statut Livraison";
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
    query.prepare("SELECT C.ID_COMMANDE, C.REFERENCE, C.PRIORITE, C.DATE_LIVRAISON, "
                  "C.STATUT, (E.NOM || ' ' || E.PRENOM) AS EMPLOYE, C.TYPE, C.MONTANT, "
                  "C.DATE_CREATION "
                  "FROM COMMANDES C "
                  "LEFT JOIN EMPLOYES E ON C.ID_EMPLOYE = E.ID_EMPLOYE "
                  "ORDER BY C.DATE_CREATION DESC");
    
    if (query.exec()) {
        int count = 0;
        while (query.next()) {
            ProductionCommande cmd;
            cmd.idCommande = query.value(0).toInt();
            cmd.reference = query.value(1).toString();
            cmd.priorite = query.value(2).toString();
            cmd.dateLivraisonPrevue = query.value(3).toDate();
            
            cmd.dateDebutPrevue = query.value(8).toDate();
            cmd.dateFinPrevue = query.value(3).toDate();
            cmd.atelier = "Atelier Principal";
            cmd.ordrePassage = count + 1;
            cmd.etatProduction = query.value(4).toString();
            cmd.etapeActuelle = "Découpe";
            cmd.avancement = 0;
            
            if (cmd.dateFinPrevue.isValid() && QDate::currentDate() > cmd.dateFinPrevue && 
                cmd.etatProduction != "Terminé") {
                cmd.retard = true;
            } else {
                cmd.retard = false;
            }
            
            cmd.societeLivraison = "DHL Express";
            cmd.numeroSuiviColis = "";
            cmd.dateExpeditionPrevue = cmd.dateLivraisonPrevue.addDays(-2);
            cmd.statutLivraison = "Non expédiée";
            
            m_commandes.append(cmd);
            count++;
        }
    }
    
    endResetModel();
}

// ═══════════════════════════════════════════════════════════════════════════
// IMPLÉMENTATION: ProductionView
// ═══════════════════════════════════════════════════════════════════════════

ProductionView::ProductionView(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Vue Production Complète - Planification, Suivi & Livraison");
    setMinimumSize(1400, 800);
    
    setupUI();
    setupConnections();
    loadData();
}

ProductionView::~ProductionView()
{
}

void ProductionView::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    centralWidget->setStyleSheet("background-color: #F5F5F5;");
    
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(25, 25, 25, 25);
    mainLayout->setSpacing(20);
    
    // Titre avec fond
    QWidget *titleWidget = new QWidget(this);
    titleWidget->setStyleSheet(
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "stop:0 #6D4C41, stop:1 #8D6E63); "
        "border-radius: 10px; padding: 15px;"
    );
    QHBoxLayout *titleLayout = new QHBoxLayout(titleWidget);
    QLabel *title = new QLabel("📊 GESTION COMPLÈTE DE LA PRODUCTION");
    title->setStyleSheet("font-size: 24px; font-weight: bold; color: white;");
    title->setAlignment(Qt::AlignCenter);
    titleLayout->addWidget(title);
    mainLayout->addWidget(titleWidget);
    
    // Barre de filtres avec fond
    QWidget *filterWidget = new QWidget(this);
    filterWidget->setStyleSheet(
        "QWidget { background-color: white; border-radius: 10px; padding: 15px; }"
        "QLabel { color: #424242; font-weight: bold; font-size: 13px; }"
    );
    QHBoxLayout *filterLayout = new QHBoxLayout(filterWidget);
    filterLayout->setSpacing(20);
    
    m_searchBox = new QLineEdit(this);
    m_searchBox->setPlaceholderText("🔍 Rechercher par référence...");
    m_searchBox->setMinimumWidth(320);
    m_searchBox->setMinimumHeight(40);
    m_searchBox->setStyleSheet(
        "QLineEdit { "
        "background-color: #FAFAFA; "
        "border: 2px solid #BDBDBD; "
        "border-radius: 8px; "
        "padding: 10px 15px; "
        "font-size: 14px; "
        "color: #212121; "
        "}"
        "QLineEdit:focus { "
        "border-color: #8D6E63; "
        "background-color: white; "
        "}"
    );
    
    m_etatFilter = new QComboBox(this);
    m_etatFilter->addItems({"Tous les états", "Planifié", "En cours", "Bloqué", "Terminé"});
    m_etatFilter->setMinimumWidth(180);
    m_etatFilter->setMinimumHeight(40);
    m_etatFilter->setStyleSheet(
        "QComboBox { "
        "background-color: #FAFAFA; "
        "border: 2px solid #BDBDBD; "
        "border-radius: 8px; "
        "padding: 8px 15px; "
        "font-size: 14px; "
        "color: #212121; "
        "font-weight: 600; "
        "}"
        "QComboBox:hover { "
        "border-color: #8D6E63; "
        "background-color: white; "
        "}"
        "QComboBox::drop-down { "
        "border: none; "
        "width: 30px; "
        "}"
        "QComboBox::down-arrow { "
        "image: none; "
        "border-left: 5px solid transparent; "
        "border-right: 5px solid transparent; "
        "border-top: 6px solid #8D6E63; "
        "}"
    );
    
    m_statutLivraisonFilter = new QComboBox(this);
    m_statutLivraisonFilter->addItems({"Tous les statuts", "Non expédiée", "En livraison", "Livrée"});
    m_statutLivraisonFilter->setMinimumWidth(180);
    m_statutLivraisonFilter->setMinimumHeight(40);
    m_statutLivraisonFilter->setStyleSheet(m_etatFilter->styleSheet());
    
    QLabel *lblRecherche = new QLabel("Recherche:", this);
    QLabel *lblEtat = new QLabel("État Production:", this);
    QLabel *lblStatut = new QLabel("Statut Livraison:", this);
    
    filterLayout->addWidget(lblRecherche);
    filterLayout->addWidget(m_searchBox);
    filterLayout->addWidget(lblEtat);
    filterLayout->addWidget(m_etatFilter);
    filterLayout->addWidget(lblStatut);
    filterLayout->addWidget(m_statutLivraisonFilter);
    filterLayout->addStretch();
    
    mainLayout->addWidget(filterWidget);
    
    // Boutons d'action
    QWidget *buttonWidget = new QWidget(this);
    buttonWidget->setStyleSheet("background-color: transparent;");
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);
    buttonLayout->setSpacing(15);
    
    QString btnStyle = 
        "QPushButton { "
        "background-color: #8D6E63; "
        "color: white; "
        "border: none; "
        "border-radius: 8px; "
        "padding: 12px 25px; "
        "font-size: 14px; "
        "font-weight: bold; "
        "min-height: 45px; "
        "}"
        "QPushButton:hover { "
        "background-color: #6D4C41; "
        "}"
        "QPushButton:pressed { "
        "background-color: #5D4037; "
        "}";
    
    m_btnPlanification = new QPushButton("📅 Modifier Planification", this);
    m_btnPlanification->setStyleSheet(btnStyle);
    m_btnPlanification->setCursor(Qt::PointingHandCursor);
    
    m_btnDetails = new QPushButton("📊 Détails Complets", this);
    m_btnDetails->setStyleSheet(btnStyle);
    m_btnDetails->setCursor(Qt::PointingHandCursor);
    
    m_btnRefresh = new QPushButton("🔄 Actualiser", this);
    m_btnRefresh->setStyleSheet(
        "QPushButton { "
        "background-color: #5C6BC0; "
        "color: white; "
        "border: none; "
        "border-radius: 8px; "
        "padding: 12px 25px; "
        "font-size: 14px; "
        "font-weight: bold; "
        "min-height: 45px; "
        "}"
        "QPushButton:hover { "
        "background-color: #3F51B5; "
        "}"
        "QPushButton:pressed { "
        "background-color: #303F9F; "
        "}"
    );
    m_btnRefresh->setCursor(Qt::PointingHandCursor);
    
    buttonLayout->addWidget(m_btnPlanification);
    buttonLayout->addWidget(m_btnDetails);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_btnRefresh);
    
    mainLayout->addWidget(buttonWidget);
    
    // TableView avec style amélioré
    m_tableView = new QTableView(this);
    m_tableView->setStyleSheet(
        "QTableView { "
        "background-color: white; "
        "border: 2px solid #BDBDBD; "
        "border-radius: 10px; "
        "gridline-color: #E0E0E0; "
        "selection-background-color: #8D6E63; "
        "selection-color: white; "
        "font-size: 13px; "
        "}"
        "QTableView::item { "
        "padding: 10px 8px; "
        "border-bottom: 1px solid #F5F5F5; "
        "}"
        "QTableView::item:selected { "
        "background-color: #8D6E63; "
        "color: white; "
        "}"
        "QTableView::item:hover { "
        "background-color: #FFF3E0; "
        "}"
        "QHeaderView::section { "
        "background-color: #6D4C41; "
        "color: white; "
        "padding: 14px 10px; "
        "border: none; "
        "border-right: 1px solid #5D4037; "
        "font-weight: bold; "
        "font-size: 12px; "
        "text-transform: uppercase; "
        "}"
        "QHeaderView::section:hover { "
        "background-color: #5D4037; "
        "}"
        "QTableView QTableCornerButton::section { "
        "background-color: #6D4C41; "
        "border: none; "
        "}"
    );
    m_tableView->setAlternatingRowColors(true);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableView->setSortingEnabled(true);
    m_tableView->verticalHeader()->setVisible(false);
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    m_tableView->setShowGrid(true);
    m_tableView->setGridStyle(Qt::SolidLine);
    
    // Modèle et proxy
    m_model = new ProductionViewModel(this);
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_model);
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_proxyModel->setFilterKeyColumn(ProductionViewModel::COL_REFERENCE);
    
    m_tableView->setModel(m_proxyModel);
    
    mainLayout->addWidget(m_tableView);
    
    // Légende améliorée
    QWidget *legendWidget = new QWidget(this);
    legendWidget->setStyleSheet(
        "background: white; "
        "border: 2px solid #E0E0E0; "
        "border-radius: 10px; "
        "padding: 15px;"
    );
    QHBoxLayout *legendLayout = new QHBoxLayout(legendWidget);
    
    QLabel *legend = new QLabel(
        "💡 <b style='color:#424242; font-size:14px;'>Légende des États:</b> "
        "<span style='background:#4CAF50;color:white;padding:5px 12px;border-radius:5px;font-weight:bold;margin-left:10px;'>Terminé</span> "
        "<span style='background:#FF9800;color:white;padding:5px 12px;border-radius:5px;font-weight:bold;margin-left:8px;'>En cours</span> "
        "<span style='background:#F44336;color:white;padding:5px 12px;border-radius:5px;font-weight:bold;margin-left:8px;'>Bloqué</span> "
        "<span style='background:#2196F3;color:white;padding:5px 12px;border-radius:5px;font-weight:bold;margin-left:8px;'>Planifié</span>"
    );
    legend->setStyleSheet("font-size: 13px;");
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
    m_tableView->resizeColumnsToContents();
    
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
        m_proxyModel->setFilterKeyColumn(ProductionViewModel::COL_ETAT_PRODUCTION);
        m_proxyModel->setFilterFixedString(m_etatFilter->currentText());
    }
}

void ProductionView::onStatutLivraisonFilterChanged(int index)
{
    if (index == 0) {
        m_proxyModel->setFilterKeyColumn(ProductionViewModel::COL_REFERENCE);
        m_proxyModel->setFilterFixedString(m_searchBox->text());
    } else {
        m_proxyModel->setFilterKeyColumn(ProductionViewModel::COL_STATUT_LIVRAISON);
        m_proxyModel->setFilterFixedString(m_statutLivraisonFilter->currentText());
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
    dlg.setMinimumSize(550, 450);
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
// IMPLÉMENTATION: ProductionManager
// ═══════════════════════════════════════════════════════════════════════════

ProductionManager::ProductionManager(QWidget *parent)
    : QObject(parent)
    , m_parent(parent)
    , m_table(nullptr)
    , m_statsTotal(nullptr)
    , m_statsEnProd(nullptr)
    , m_statsTerminees(nullptr)
    , m_statsMontant(nullptr)
{
}

void ProductionManager::setProductionTable(QTableWidget *table)
{
    m_table = table;
    if (m_table) {
        m_table->setColumnCount(9);
        m_table->setHorizontalHeaderLabels({
            "ID", "Référence", "Employé", "Type", "Montant",
            "Date Création", "Date Livraison", "Statut", "Priorité"
        });
        m_table->setColumnHidden(0, true);
        m_table->verticalHeader()->setVisible(false);
        m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_table->setSelectionMode(QAbstractItemView::SingleSelection);
        
        int widths[] = {50,120,150,120,100,110,110,100,80};
        for (int i = 0; i < 9; ++i) {
            m_table->setColumnWidth(i, widths[i]);
        }
    }
}

void ProductionManager::setStatsLabels(QLabel *total, QLabel *enProd, QLabel *terminees, QLabel *montant)
{
    m_statsTotal = total;
    m_statsEnProd = enProd;
    m_statsTerminees = terminees;
    m_statsMontant = montant;
}

QString ProductionManager::cellText(int row, int col) const
{
    if (!m_table || row < 0 || row >= m_table->rowCount() || col < 0 || col >= m_table->columnCount())
        return QString();
    auto *item = m_table->item(row, col);
    return item ? item->text() : QString();
}

void ProductionManager::ajouterLigneTable(const QString &ref, const QString &employe,
                                          const QString &type, const QString &montant,
                                          const QString &dc, const QString &dl,
                                          const QString &statut, const QString &priorite)
{
    if (!m_table) return;
    
    int row = m_table->rowCount();
    m_table->insertRow(row);
    m_table->setItem(row, 0, new QTableWidgetItem(QString::number(row + 1)));
    m_table->setItem(row, 1, new QTableWidgetItem(ref));
    m_table->setItem(row, 2, new QTableWidgetItem(employe));
    m_table->setItem(row, 3, new QTableWidgetItem(type));
    m_table->setItem(row, 4, new QTableWidgetItem(montant));
    m_table->setItem(row, 5, new QTableWidgetItem(dc));
    m_table->setItem(row, 6, new QTableWidgetItem(dl));
    m_table->setItem(row, 7, new QTableWidgetItem(statut));
    m_table->setItem(row, 8, new QTableWidgetItem(priorite));
}

void ProductionManager::loadProductionData()
{
    if (!m_table) return;
    
    m_table->setRowCount(0);
    
    Production prod;
    QSqlQueryModel* model = prod.afficher();
    
    if (!model) {
        qDebug() << "❌ Erreur lors du chargement des commandes";
        return;
    }
    
    for (int i = 0; i < model->rowCount(); ++i) {
        int row = m_table->rowCount();
        m_table->insertRow(row);
        
        m_table->setItem(row, 0, new QTableWidgetItem(model->data(model->index(i, 0)).toString()));
        m_table->setItem(row, 1, new QTableWidgetItem(model->data(model->index(i, 1)).toString()));
        m_table->setItem(row, 2, new QTableWidgetItem(model->data(model->index(i, 2)).toString()));
        m_table->setItem(row, 3, new QTableWidgetItem(model->data(model->index(i, 3)).toString()));
        m_table->setItem(row, 4, new QTableWidgetItem(model->data(model->index(i, 4)).toString() + " DT"));
        m_table->setItem(row, 5, new QTableWidgetItem(model->data(model->index(i, 5)).toDate().toString("dd/MM/yyyy")));
        m_table->setItem(row, 6, new QTableWidgetItem(model->data(model->index(i, 6)).toDate().toString("dd/MM/yyyy")));
        m_table->setItem(row, 7, new QTableWidgetItem(model->data(model->index(i, 7)).toString()));
        m_table->setItem(row, 8, new QTableWidgetItem(model->data(model->index(i, 8)).toString()));
    }
    
    delete model;
    updateStatsCards();
}

void ProductionManager::updateStatistics()
{
    updateStatsCards();
}

void ProductionManager::updateStatsCards()
{
    if (!m_table) return;
    
    int total = 0, enProd = 0, terminees = 0;
    double montantTotal = 0;
    
    for (int r = 0; r < m_table->rowCount(); ++r) {
        if (!cellText(r, 1).startsWith("PROD-")) continue;
        ++total;
        QString m = cellText(r, 4);
        montantTotal += m.remove(" DT").remove(",").replace(" ", "").toDouble();
        QString st = cellText(r, 7);
        if (st == "En Production") ++enProd;
        else if (st == "Terminé") ++terminees;
    }
    
    if (m_statsTotal) m_statsTotal->setText(QString::number(total));
    if (m_statsEnProd) m_statsEnProd->setText(QString::number(enProd));
    if (m_statsTerminees) m_statsTerminees->setText(QString::number(terminees));
    if (m_statsMontant) m_statsMontant->setText(QString::number(montantTotal, 'f', 2) + " DT");
}

// NOTE: Le reste de ProductionManager (creerProduction, modifierProduction, etc.)
// est déjà dans productionmanager.cpp. Copiez TOUT le contenu restant de 
// productionmanager.cpp ici (environ 600 lignes restantes).
// Pour gagner du temps, utilisez: type productionmanager.cpp >> productionview.cpp
