#ifndef PRODUCTIONMANAGER_H
#define PRODUCTIONMANAGER_H

#include <QObject>
#include <QTableWidget>
#include <QWidget>
#include <QLabel>
#include "production.h"

class ProductionManager : public QObject
{
    Q_OBJECT

public:
    explicit ProductionManager(QWidget *parent = nullptr);
    
    // Configuration
    void setProductionTable(QTableWidget *table);
    void setStatsLabels(QLabel *total, QLabel *enProd, QLabel *terminees, QLabel *montant = nullptr);
    
    // Opérations CRUD
    void creerProduction();
    void modifierProduction();
    void supprimerProduction();
    
    // Affichage et mise à jour
    void loadProductionData();
    void updateStatistics();
    void rechercherProduction(const QString &text);
    void trierProduction();
    
    // Fonctionnalités avancées
    void afficherSuivi();
    void afficherPlanification();
    void genererFacture();
    void exporterBilanFinancier();
    void afficherStatistiques();
    
    // Menu contextuel
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
    
    // Helpers
    QString cellText(int row, int col) const;
    void ajouterLigneTable(const QString &ref, const QString &client,
                           const QString &type, const QString &montant,
                           const QString &dc, const QString &dl,
                           const QString &statut, const QString &priorite);
    void updateStatsCards();
};

#endif // PRODUCTIONMANAGER_H
