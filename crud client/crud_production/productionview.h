#ifndef PRODUCTIONVIEW_H
#define PRODUCTIONVIEW_H

#include <QMainWindow>
#include <QTableView>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QSortFilterProxyModel>
#include <QAbstractTableModel>
#include <QList>
#include <QObject>
#include <QTableWidget>
#include <QWidget>
#include <QLabel>
#include "productiondialog.h"
#include "production.h"

// ═══════════════════════════════════════════════════════════════════════════
// CLASSE: ProductionViewModel - Modèle pour le tableau
// ═══════════════════════════════════════════════════════════════════════════
class ProductionViewModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Column {
        COL_ID = 0,
        COL_REFERENCE,
        COL_PRIORITE,
        COL_DATE_LIVRAISON,
        COL_DATE_DEBUT_PREVUE,
        COL_DATE_FIN_PREVUE,
        COL_ATELIER,
        COL_ORDRE_PASSAGE,
        COL_ETAT_PRODUCTION,
        COL_ETAPE_ACTUELLE,
        COL_AVANCEMENT,
        COL_RETARD,
        COL_SOCIETE_LIVRAISON,
        COL_NUMERO_SUIVI,
        COL_DATE_EXPEDITION_PREVUE,
        COL_DATE_EXPEDITION_REELLE,
        COL_STATUT_LIVRAISON,
        COL_COUNT
    };

    explicit ProductionViewModel(QObject *parent = nullptr);
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    
    void setCommandes(const QList<ProductionCommande> &commandes);
    void addCommande(const ProductionCommande &commande);
    void updateCommande(int row, const ProductionCommande &commande);
    ProductionCommande getCommande(int row) const;
    void clear();
    void loadFromDatabase();

private:
    QList<ProductionCommande> m_commandes;
    QString getColumnName(int column) const;
};

// ═══════════════════════════════════════════════════════════════════════════
// CLASSE: ProductionView - Vue complète (Planification/Suivi/Livraison)
// ═══════════════════════════════════════════════════════════════════════════
class ProductionView : public QMainWindow
{
    Q_OBJECT

public:
    explicit ProductionView(QWidget *parent = nullptr);
    ~ProductionView();
    
    void loadData();

private slots:
    void onSearchTextChanged(const QString &text);
    void onEtatFilterChanged(int index);
    void onStatutLivraisonFilterChanged(int index);
    void onDetailsClicked();
    void onPlanificationClicked();
    void onRefreshClicked();

private:
    void setupUI();
    void setupConnections();
    
    QTableView *m_tableView;
    QLineEdit *m_searchBox;
    QComboBox *m_etatFilter;
    QComboBox *m_statutLivraisonFilter;
    QPushButton *m_btnDetails;
    QPushButton *m_btnPlanification;
    QPushButton *m_btnRefresh;
    
    ProductionViewModel *m_model;
    QSortFilterProxyModel *m_proxyModel;
};

// ═══════════════════════════════════════════════════════════════════════════
// CLASSE: ProductionManager - Gestionnaire principal
// ═══════════════════════════════════════════════════════════════════════════
class ProductionManager : public QObject
{
    Q_OBJECT

public:
    explicit ProductionManager(QWidget *parent = nullptr);
    
    void setProductionTable(QTableWidget *table);
    void setStatsLabels(QLabel *total, QLabel *enProd, QLabel *terminees, QLabel *montant = nullptr);
    
    void creerProduction();
    void modifierProduction();
    void supprimerProduction();
    
    void loadProductionData();
    void updateStatistics();
    void rechercherProduction(const QString &text);
    void trierProduction();
    
    void afficherSuivi();
    void afficherPlanification();
    void genererFacture();
    void exporterBilanFinancier();
    void afficherStatistiques();
    
    void showContextMenu(const QPoint &pos);
    
signals:
    void productionDataChanged();
    
private:
    QWidget *m_parent;
    QTableWidget *m_table;
    QLabel *m_statsTotal;
    QLabel *m_statsEnProd;
    QLabel *m_statsTerminees;
    QLabel *m_statsMontant;
    
    QString cellText(int row, int col) const;
    void ajouterLigneTable(const QString &ref, const QString &employe,
                           const QString &type, const QString &montant,
                           const QString &dc, const QString &dl,
                           const QString &statut, const QString &priorite);
    void updateStatsCards();
};

#endif // PRODUCTIONVIEW_H
