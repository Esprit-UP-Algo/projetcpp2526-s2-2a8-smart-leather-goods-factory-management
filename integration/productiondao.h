#ifndef PRODUCTIONDAO_H
#define PRODUCTIONDAO_H

#include "production.h"
#include <QList>
#include <QSqlQueryModel>

// ═══════════════════════════════════════════════════════════════════════════
// CLASSE DAO : ProductionDAO
// Gère TOUTES les requêtes SQL pour l'entité Production
// Utilise le Singleton Connection existant
// ═══════════════════════════════════════════════════════════════════════════
class ProductionDAO
{
public:
    ProductionDAO();
    
    // ========== MÉTHODES CRUD ==========
    
    // Ajouter une production (INSERT)
    bool ajouter(const Production &production);
    
    // Modifier une production (UPDATE)
    bool modifier(const Production &production);
    
    // Supprimer une production (DELETE)
    bool supprimer(int idCommande);
    
    // Afficher toutes les productions (SELECT)
    QSqlQueryModel* afficher();
    
    // ========== MÉTHODES DE RECHERCHE ==========
    
    // Rechercher par terme (référence, type, statut, etc.)
    QSqlQueryModel* rechercher(const QString &terme);
    
    // Trier par colonne
    QSqlQueryModel* trierPar(const QString &colonne, bool croissant = true);
    
    // Filtrer par statut
    QSqlQueryModel* filtrerParStatut(const QString &statut);
    
    // Filtrer par priorité
    QSqlQueryModel* filtrerParPriorite(const QString &priorite);
    
    // Obtenir une production par ID
    Production obtenirParId(int idCommande);
    
    // Obtenir toutes les productions sous forme de liste
    QList<Production> obtenirTout();

private:
    // Méthode helper pour configurer les en-têtes du modèle
    void configurerEntetes(QSqlQueryModel* model);
};

#endif // PRODUCTIONDAO_H
