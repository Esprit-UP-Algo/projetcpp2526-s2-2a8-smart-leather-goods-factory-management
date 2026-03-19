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
#include <QTimer>
#include <QSet>
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
        COL_REFERENCE = 0,
        COL_EMPLOYE,
        COL_TYPE,
        COL_QUANTITE,
        COL_DATE_CREATION,
        COL_STATUT,
        COL_PRIORITE,
        COL_MONTANT,
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
    
    // Configuration des alertes
    void setJoursAlerte(int jours) { m_joursAlerte = jours; }
    void setSeuilAvancement(int seuil) { m_seuilAvancement = seuil; }
    void setIntervalleVerification(int minutes) { m_intervalleVerification = minutes; }

private slots:
    void onSearchTextChanged(const QString &text);
    void onEtatFilterChanged(int index);
    void onStatutLivraisonFilterChanged(int index);
    void onDetailsClicked();
    void onPlanificationClicked();
    void onRefreshClicked();
    
    // Slot pour la vérification périodique des alertes
    void verifierAlertes();

private:
    void setupUI();
    void setupConnections();
    void setupTimer();
    void afficherNotification(const ProductionCommande &commande);
    void recalculerToutesLesAlertes();
    
    QTableView *m_tableView;
    QLineEdit *m_searchBox;
    QComboBox *m_etatFilter;
    QComboBox *m_statutLivraisonFilter;
    QPushButton *m_btnDetails;
    QPushButton *m_btnPlanification;
    QPushButton *m_btnRefresh;
    
    ProductionViewModel *m_model;
    QSortFilterProxyModel *m_proxyModel;
    
    // Timer pour vérification périodique
    QTimer *m_timerAlertes;
    
    // Configuration des alertes
    int m_joursAlerte;          // Nombre de jours avant la date limite pour déclencher "Risque"
    int m_seuilAvancement;      // Seuil d'avancement en % pour déclencher "Risque"
    int m_intervalleVerification; // Intervalle en minutes entre chaque vérification
    
    // Suivi des alertes déjà notifiées (pour éviter les doublons)
    QSet<int> m_alertesNotifiees;
};

#endif // PRODUCTIONVIEW_H
