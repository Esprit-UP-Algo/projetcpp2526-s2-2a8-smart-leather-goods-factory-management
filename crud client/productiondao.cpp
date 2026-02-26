#include "productiondao.h"
#include "connection.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVariant>

// ═══════════════════════════════════════════════════════════════════════════
// IMPLÉMENTATION : ProductionDAO
// TOUTES les requêtes SQL sont ici avec prepare() et bindValue()
// ═══════════════════════════════════════════════════════════════════════════

ProductionDAO::ProductionDAO()
{
}

// ═══════════════════════════════════════════════════════════════════════════
// AJOUTER : INSERT avec prepare() et bindValue()
// ═══════════════════════════════════════════════════════════════════════════
bool ProductionDAO::ajouter(const Production &production)
{
    QSqlDatabase db = Connection::instance()->getDatabase();
    
    if (!db.isOpen()) {
        qDebug() << "❌ Base de données non connectée!";
        return false;
    }
    
    QSqlQuery query(db);
    
    // Préparer la requête avec des placeholders
    QString sql = "INSERT INTO COMMANDES "
                  "(REFERENCE, TYPE, DATE_CREATION, DATE_LIVRAISON, "
                  "STATUT, PRIORITE, MONTANT, QUANTITE, ETAT, ID_EMPLOYE) "
                  "VALUES "
                  "(:reference, :type, :dateCreation, :dateLivraison, "
                  ":statut, :priorite, :montant, :quantite, :etat, :idEmploye)";
    
    query.prepare(sql);
    
    // Lier les valeurs de manière sécurisée
    query.bindValue(":reference", production.getReference());
    query.bindValue(":type", production.getType());
    query.bindValue(":dateCreation", production.getDateCreation());
    query.bindValue(":dateLivraison", production.getDateLivraisonPrevue());
    query.bindValue(":statut", production.getStatut());
    query.bindValue(":priorite", production.getPriorite());
    query.bindValue(":montant", production.getMontant());
    query.bindValue(":quantite", production.getQuantite());
    query.bindValue(":etat", "En cours");  // Valeur par défaut
    query.bindValue(":idEmploye", production.getIdEmploye());
    
    // Debug détaillé
    qDebug() << "═══════════════════════════════════════════════════════";
    qDebug() << "🔍 TENTATIVE D'AJOUT DE COMMANDE";
    qDebug() << "Reference:" << production.getReference();
    qDebug() << "Type:" << production.getType();
    qDebug() << "ID_EMPLOYE:" << production.getIdEmploye();
    qDebug() << "Montant:" << production.getMontant();
    qDebug() << "Quantite:" << production.getQuantite();
    qDebug() << "═══════════════════════════════════════════════════════";
    
    if (!query.exec()) {
        qDebug() << "❌❌❌ ERREUR AJOUT COMMANDE ❌❌❌";
        qDebug() << "Erreur SQL:" << query.lastError().text();
        qDebug() << "Code erreur:" << query.lastError().nativeErrorCode();
        qDebug() << "Type erreur:" << query.lastError().type();
        qDebug() << "Requête SQL:" << query.lastQuery();
        qDebug() << "═══════════════════════════════════════════════════════";
        return false;
    }
    
    qDebug() << "✅ Commande ajoutée avec succès";
    return true;
}

// ═══════════════════════════════════════════════════════════════════════════
// MODIFIER : UPDATE avec prepare() et bindValue()
// ═══════════════════════════════════════════════════════════════════════════
bool ProductionDAO::modifier(const Production &production)
{
    QSqlDatabase db = Connection::instance()->getDatabase();
    
    if (!db.isOpen()) {
        qDebug() << "❌ Base de données non connectée!";
        return false;
    }
    
    QSqlQuery query(db);
    
    // Préparer la requête avec des placeholders
    QString sql = "UPDATE COMMANDES SET "
                  "REFERENCE = :reference, "
                  "TYPE = :type, "
                  "DATE_CREATION = :dateCreation, "
                  "DATE_LIVRAISON = :dateLivraison, "
                  "STATUT = :statut, "
                  "PRIORITE = :priorite, "
                  "MONTANT = :montant, "
                  "QUANTITE = :quantite, "
                  "ETAT = :etat "
                  "WHERE ID_COMMANDE = :id";
    
    query.prepare(sql);
    
    // Lier les valeurs de manière sécurisée
    query.bindValue(":id", production.getIdCommande());
    query.bindValue(":reference", production.getReference());
    query.bindValue(":type", production.getType());
    query.bindValue(":dateCreation", production.getDateCreation());
    query.bindValue(":dateLivraison", production.getDateLivraisonPrevue());
    query.bindValue(":statut", production.getStatut());
    query.bindValue(":priorite", production.getPriorite());
    query.bindValue(":montant", production.getMontant());
    query.bindValue(":quantite", production.getQuantite());
    query.bindValue(":etat", production.getEtatPaiement());
    
    if (!query.exec()) {
        qDebug() << "❌ Erreur modification production:" << query.lastError().text();
        return false;
    }
    
    qDebug() << "✅ Commande modifiée avec succès";
    return true;
}

// ═══════════════════════════════════════════════════════════════════════════
// SUPPRIMER : DELETE avec prepare() et bindValue()
// ═══════════════════════════════════════════════════════════════════════════
bool ProductionDAO::supprimer(int idCommande)
{
    QSqlDatabase db = Connection::instance()->getDatabase();
    
    if (!db.isOpen()) {
        qDebug() << "❌ Base de données non connectée!";
        return false;
    }
    
    QSqlQuery query(db);
    
    // Préparer la requête avec placeholder
    query.prepare("DELETE FROM COMMANDES WHERE ID_COMMANDE = :id");
    query.bindValue(":id", idCommande);
    
    if (!query.exec()) {
        qDebug() << "❌ Erreur suppression production:" << query.lastError().text();
        return false;
    }
    
    qDebug() << "✅ Commande supprimée avec succès";
    return true;
}

// ═══════════════════════════════════════════════════════════════════════════
// AFFICHER : SELECT toutes les productions
// ═══════════════════════════════════════════════════════════════════════════
QSqlQueryModel* ProductionDAO::afficher()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    
    QString sql = "SELECT C.ID_COMMANDE, C.REFERENCE, (E.NOM || ' ' || E.PRENOM) AS EMPLOYE, "
                  "C.TYPE, C.DATE_CREATION, C.DATE_LIVRAISON, C.STATUT, C.PRIORITE, C.MONTANT "
                  "FROM COMMANDES C "
                  "LEFT JOIN EMPLOYES E ON C.ID_EMPLOYE = E.ID_EMPLOYE "
                  "ORDER BY C.DATE_CREATION DESC";
    
    model->setQuery(sql, Connection::instance()->getDatabase());
    
    if (model->lastError().isValid()) {
        qDebug() << "❌ Erreur affichage productions:" << model->lastError().text();
        delete model;
        return nullptr;
    }
    
    configurerEntetes(model);
    return model;
}

// ═══════════════════════════════════════════════════════════════════════════
// RECHERCHER : SELECT avec LIKE et prepare() / bindValue()
// ═══════════════════════════════════════════════════════════════════════════
QSqlQueryModel* ProductionDAO::rechercher(const QString &terme)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlDatabase db = Connection::instance()->getDatabase();
    
    QSqlQuery query(db);
    
    // Préparer la requête avec placeholders
    QString sql = "SELECT C.ID_COMMANDE, C.REFERENCE, (E.NOM || ' ' || E.PRENOM) AS EMPLOYE, "
                  "C.TYPE, C.DATE_CREATION, C.DATE_LIVRAISON, C.STATUT, C.PRIORITE, C.MONTANT "
                  "FROM COMMANDES C "
                  "LEFT JOIN EMPLOYES E ON C.ID_EMPLOYE = E.ID_EMPLOYE "
                  "WHERE UPPER(C.REFERENCE) LIKE UPPER(:terme) "
                  "OR UPPER(C.TYPE) LIKE UPPER(:terme) "
                  "OR UPPER(C.STATUT) LIKE UPPER(:terme) "
                  "OR UPPER(C.PRIORITE) LIKE UPPER(:terme) "
                  "OR UPPER(E.NOM) LIKE UPPER(:terme) "
                  "OR UPPER(E.PRENOM) LIKE UPPER(:terme) "
                  "ORDER BY C.DATE_CREATION DESC";
    
    query.prepare(sql);
    query.bindValue(":terme", "%" + terme + "%");
    
    if (!query.exec()) {
        qDebug() << "❌ Erreur recherche productions:" << query.lastError().text();
        delete model;
        return nullptr;
    }
    
    model->setQuery(query);
    configurerEntetes(model);
    return model;
}

// ═══════════════════════════════════════════════════════════════════════════
// TRIER : SELECT avec ORDER BY et prepare() / bindValue()
// ═══════════════════════════════════════════════════════════════════════════
QSqlQueryModel* ProductionDAO::trierPar(const QString &colonne, bool croissant)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlDatabase db = Connection::instance()->getDatabase();
    
    // Liste blanche des colonnes autorisées pour éviter l'injection SQL
    QStringList colonnesAutorisees = {
        "ID_COMMANDE", "REFERENCE", "TYPE", "DATE_CREATION",
        "DATE_LIVRAISON_PREVUE", "STATUT", "PRIORITE", "MONTANT"
    };
    
    QString colonneSecurisee = "DATE_CREATION";
    if (colonnesAutorisees.contains(colonne.toUpper())) {
        colonneSecurisee = colonne.toUpper();
    }
    
    QString ordre = croissant ? "ASC" : "DESC";
    
    QString sql = QString("SELECT C.ID_COMMANDE, C.REFERENCE, (E.NOM || ' ' || E.PRENOM) AS EMPLOYE, "
                          "C.TYPE, C.DATE_CREATION, C.DATE_LIVRAISON, C.STATUT, C.PRIORITE, C.MONTANT "
                          "FROM COMMANDES C "
                          "LEFT JOIN EMPLOYES E ON C.ID_EMPLOYE = E.ID_EMPLOYE "
                          "ORDER BY C.%1 %2").arg(colonneSecurisee, ordre);
    
    model->setQuery(sql, db);
    
    if (model->lastError().isValid()) {
        qDebug() << "❌ Erreur tri productions:" << model->lastError().text();
        delete model;
        return nullptr;
    }
    
    configurerEntetes(model);
    return model;
}

// ═══════════════════════════════════════════════════════════════════════════
// FILTRER PAR STATUT : SELECT avec WHERE et prepare() / bindValue()
// ═══════════════════════════════════════════════════════════════════════════
QSqlQueryModel* ProductionDAO::filtrerParStatut(const QString &statut)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlDatabase db = Connection::instance()->getDatabase();
    
    QSqlQuery query(db);
    
    QString sql = "SELECT C.ID_COMMANDE, C.REFERENCE, (E.NOM || ' ' || E.PRENOM) AS EMPLOYE, "
                  "C.TYPE, C.DATE_CREATION, C.DATE_LIVRAISON, C.STATUT, C.PRIORITE, C.MONTANT "
                  "FROM COMMANDES C "
                  "LEFT JOIN EMPLOYES E ON C.ID_EMPLOYE = E.ID_EMPLOYE "
                  "WHERE C.STATUT = :statut "
                  "ORDER BY C.DATE_CREATION DESC";
    
    query.prepare(sql);
    query.bindValue(":statut", statut);
    
    if (!query.exec()) {
        qDebug() << "❌ Erreur filtre par statut:" << query.lastError().text();
        delete model;
        return nullptr;
    }
    
    model->setQuery(query);
    configurerEntetes(model);
    return model;
}

// ═══════════════════════════════════════════════════════════════════════════
// FILTRER PAR PRIORITÉ : SELECT avec WHERE et prepare() / bindValue()
// ═══════════════════════════════════════════════════════════════════════════
QSqlQueryModel* ProductionDAO::filtrerParPriorite(const QString &priorite)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlDatabase db = Connection::instance()->getDatabase();
    
    QSqlQuery query(db);
    
    QString sql = "SELECT C.ID_COMMANDE, C.REFERENCE, (E.NOM || ' ' || E.PRENOM) AS EMPLOYE, "
                  "C.TYPE, C.DATE_CREATION, C.DATE_LIVRAISON, C.STATUT, C.PRIORITE, C.MONTANT "
                  "FROM COMMANDES C "
                  "LEFT JOIN EMPLOYES E ON C.ID_EMPLOYE = E.ID_EMPLOYE "
                  "WHERE C.PRIORITE = :priorite "
                  "ORDER BY C.DATE_CREATION DESC";
    
    query.prepare(sql);
    query.bindValue(":priorite", priorite);
    
    if (!query.exec()) {
        qDebug() << "❌ Erreur filtre par priorité:" << query.lastError().text();
        delete model;
        return nullptr;
    }
    
    model->setQuery(query);
    configurerEntetes(model);
    return model;
}

// ═══════════════════════════════════════════════════════════════════════════
// OBTENIR PAR ID : SELECT avec WHERE et prepare() / bindValue()
// ═══════════════════════════════════════════════════════════════════════════
Production ProductionDAO::obtenirParId(int idCommande)
{
    Production production;
    QSqlDatabase db = Connection::instance()->getDatabase();
    
    QSqlQuery query(db);
    
    query.prepare("SELECT ID_COMMANDE, REFERENCE, TYPE, "
                  "DATE_CREATION, DATE_LIVRAISON, "
                  "STATUT, PRIORITE, MONTANT, QUANTITE, "
                  "ETAT "
                  "FROM COMMANDES "
                  "WHERE ID_COMMANDE = :id");
    
    query.bindValue(":id", idCommande);
    
    if (query.exec() && query.next()) {
        production.setIdCommande(query.value(0).toInt());
        production.setReference(query.value(1).toString());
        production.setType(query.value(2).toString());
        production.setDateCreation(query.value(3).toDate());
        production.setDateLivraisonPrevue(query.value(4).toDate());
        production.setStatut(query.value(5).toString());
        production.setPriorite(query.value(6).toString());
        production.setMontant(query.value(7).toDouble());
        production.setQuantite(query.value(8).toInt());
        production.setEtatPaiement(query.value(9).toString());
    } else {
        qDebug() << "❌ Production non trouvée ou erreur:" << query.lastError().text();
    }
    
    return production;
}

// ═══════════════════════════════════════════════════════════════════════════
// OBTENIR TOUT : Retourne une liste de toutes les productions
// ═══════════════════════════════════════════════════════════════════════════
QList<Production> ProductionDAO::obtenirTout()
{
    QList<Production> liste;
    QSqlDatabase db = Connection::instance()->getDatabase();
    
    QSqlQuery query(db);
    
    query.prepare("SELECT ID_COMMANDE, REFERENCE, TYPE, "
                  "DATE_CREATION, DATE_LIVRAISON, "
                  "STATUT, PRIORITE, MONTANT, QUANTITE, "
                  "ETAT "
                  "FROM COMMANDES "
                  "ORDER BY DATE_CREATION DESC");
    
    if (query.exec()) {
        while (query.next()) {
            Production prod(
                query.value(0).toInt(),
                query.value(1).toString(),
                query.value(2).toString(),
                query.value(3).toDate(),
                query.value(4).toDate(),
                query.value(5).toString(),
                query.value(6).toString(),
                query.value(7).toDouble(),
                query.value(9).toString(),
                ""  // SERVICE_VENTE n'existe plus
            );
            prod.setQuantite(query.value(8).toInt());
            liste.append(prod);
        }
    } else {
        qDebug() << "❌ Erreur obtenir tout:" << query.lastError().text();
    }
    
    return liste;
}

// ═══════════════════════════════════════════════════════════════════════════
// HELPER : Configurer les en-têtes du modèle
// ═══════════════════════════════════════════════════════════════════════════
void ProductionDAO::configurerEntetes(QSqlQueryModel* model)
{
    if (!model) return;
    
    model->setHeaderData(0, Qt::Horizontal, "Référence");
    model->setHeaderData(1, Qt::Horizontal, "Employé");
    model->setHeaderData(2, Qt::Horizontal, "Type");
    model->setHeaderData(3, Qt::Horizontal, "Date Création");
    model->setHeaderData(4, Qt::Horizontal, "Statut");
    model->setHeaderData(5, Qt::Horizontal, "Priorité");
    model->setHeaderData(6, Qt::Horizontal, "Montant");
}
