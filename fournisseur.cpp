#include "fournisseur.h"
#include "connection.h"
#include <QSqlError>
#include <QDebug>

FournisseurData::FournisseurData()
    : statut("ACTIF")
{
}

FournisseurData::FournisseurData(const QString &id, const QString &nomEntreprise, const QString &email,
                         const QString &telephone, const QString &matriculeFiscal,
                         const QString &typeProduit, const QString &conditionPaiement,
                         const QString &statut, const QString &adresse)
    : id(id)
    , nomEntreprise(nomEntreprise)
    , email(email)
    , telephone(telephone)
    , matriculeFiscal(matriculeFiscal)
    , typeProduit(typeProduit)
    , conditionPaiement(conditionPaiement)
    , statut(statut)
    , adresse(adresse)
{
}

bool FournisseurData::ajouter()
{
    QSqlQuery query(Connection::instance()->getDatabase());
    query.prepare("INSERT INTO FOURNISSEURS (NOM_ENTREPRISE, EMAIL, TELEPHONE, "
                  "MATRICULE_FISCAL, TYPE_PRODUIT, CONDITION_PAIEMENT, STATUT, ADRESSE) "
                  "VALUES (:nomEntreprise, :email, :telephone, :matriculeFiscal, "
                  ":typeProduit, :conditionPaiement, :statut, :adresse)");

    query.bindValue(":nomEntreprise", nomEntreprise);
    query.bindValue(":email", email);
    query.bindValue(":telephone", telephone);
    query.bindValue(":matriculeFiscal", matriculeFiscal);
    query.bindValue(":typeProduit", typeProduit);
    query.bindValue(":conditionPaiement", conditionPaiement);
    query.bindValue(":statut", statut);
    query.bindValue(":adresse", adresse);

    if (!query.exec()) {
        qDebug() << "❌ Erreur ajout fournisseur:" << query.lastError().text();
        return false;
    }

    qDebug() << "✅ Fournisseur ajouté avec succès";
    return true;
}

bool FournisseurData::modifier()
{
    QSqlQuery query(Connection::instance()->getDatabase());
    query.prepare("UPDATE FOURNISSEURS SET "
                  "NOM_ENTREPRISE = :nomEntreprise, EMAIL = :email, "
                  "TELEPHONE = :telephone, MATRICULE_FISCAL = :matriculeFiscal, "
                  "TYPE_PRODUIT = :typeProduit, CONDITION_PAIEMENT = :conditionPaiement, "
                  "STATUT = :statut, ADRESSE = :adresse "
                  "WHERE ID_FOURNISSEUR = :id");
    
    query.bindValue(":id", id);
    query.bindValue(":nomEntreprise", nomEntreprise);
    query.bindValue(":email", email);
    query.bindValue(":telephone", telephone);
    query.bindValue(":matriculeFiscal", matriculeFiscal);
    query.bindValue(":typeProduit", typeProduit);
    query.bindValue(":conditionPaiement", conditionPaiement);
    query.bindValue(":statut", statut);
    query.bindValue(":adresse", adresse);
    
    if (!query.exec()) {
        qDebug() << "❌ Erreur modification fournisseur:" << query.lastError().text();
        return false;
    }
    
    qDebug() << "✅ Fournisseur modifié avec succès";
    return true;
}

bool FournisseurData::supprimer(const QString &id)
{
    QSqlDatabase db = Connection::instance()->getDatabase();
    
    // Commencer une transaction
    db.transaction();
    
    // D'abord, supprimer les livraisons associées (si la table existe)
    QSqlQuery queryLivraisons(db);
    queryLivraisons.prepare("DELETE FROM ARDUINO_DELIVERIES WHERE ID_FOURNISSEUR = :id");
    queryLivraisons.bindValue(":id", id);
    
    if (!queryLivraisons.exec()) {
        qDebug() << "⚠️ Avertissement suppression livraisons:" << queryLivraisons.lastError().text();
        // Continuer même si cette table n'existe pas
    }
    
    // Ensuite, supprimer le fournisseur
    QSqlQuery query(db);
    query.prepare("DELETE FROM FOURNISSEURS WHERE ID_FOURNISSEUR = :id");
    query.bindValue(":id", id);
    
    if (!query.exec()) {
        qDebug() << "❌ Erreur suppression fournisseur:" << query.lastError().text();
        db.rollback();
        return false;
    }
    
    // Valider la transaction
    db.commit();
    
    qDebug() << "✅ Fournisseur supprimé avec succès (ID:" << id << ")";
    return true;
}

QSqlQueryModel* FournisseurData::afficher()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT ID_FOURNISSEUR, NOM_ENTREPRISE, EMAIL, TELEPHONE, MATRICULE_FISCAL, "
                    "TYPE_PRODUIT, CONDITION_PAIEMENT, STATUT, ADRESSE, "
                    "NVL(QUANTITE_COMMANDEE, 0) AS QTE_COMMANDEE, NVL(QUANTITE_MESUREE, 0) AS QTE_MESUREE "
                    "FROM FOURNISSEURS ORDER BY NOM_ENTREPRISE",
                    Connection::instance()->getDatabase());

    if (model->lastError().isValid()) {
        qDebug() << "❌ Erreur affichage fournisseurs:" << model->lastError().text();
        delete model;
        return nullptr;
    }

    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Nom Entreprise");
    model->setHeaderData(2, Qt::Horizontal, "Email");
    model->setHeaderData(3, Qt::Horizontal, "Téléphone");
    model->setHeaderData(4, Qt::Horizontal, "Matricule Fiscal");
    model->setHeaderData(5, Qt::Horizontal, "Type Produit");
    model->setHeaderData(6, Qt::Horizontal, "Condition Paiement");
    model->setHeaderData(7, Qt::Horizontal, "Statut");
    model->setHeaderData(8, Qt::Horizontal, "Adresse");
    model->setHeaderData(9, Qt::Horizontal, "Qté Commandée (kg)");
    model->setHeaderData(10, Qt::Horizontal, "Qté Mesurée (kg)");
    
    return model;
}

QSqlQueryModel* FournisseurData::rechercher(const QString &terme)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    
    QString queryStr = QString(
        "SELECT ID_FOURNISSEUR, NOM_ENTREPRISE, EMAIL, TELEPHONE, MATRICULE_FISCAL, "
        "TYPE_PRODUIT, CONDITION_PAIEMENT, STATUT "
        "FROM FOURNISSEURS "
        "WHERE UPPER(NOM_ENTREPRISE) LIKE UPPER('%%1%') "
        "OR UPPER(EMAIL) LIKE UPPER('%%1%') "
        "OR UPPER(TELEPHONE) LIKE UPPER('%%1%') "
        "OR UPPER(MATRICULE_FISCAL) LIKE UPPER('%%1%') "
        "OR UPPER(TYPE_PRODUIT) LIKE UPPER('%%1%') "
        "ORDER BY NOM_ENTREPRISE"
    ).arg(terme);
    
    model->setQuery(queryStr, Connection::instance()->getDatabase());
    
    if (model->lastError().isValid()) {
        qDebug() << "❌ Erreur recherche fournisseurs:" << model->lastError().text();
        delete model;
        return nullptr;
    }
    
    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Nom Entreprise");
    model->setHeaderData(2, Qt::Horizontal, "Email");
    model->setHeaderData(3, Qt::Horizontal, "Téléphone");
    model->setHeaderData(4, Qt::Horizontal, "Matricule Fiscal");
    model->setHeaderData(5, Qt::Horizontal, "Type Produit");
    model->setHeaderData(6, Qt::Horizontal, "Condition Paiement");
    model->setHeaderData(7, Qt::Horizontal, "Statut");
    
    return model;
}

QSqlQueryModel* FournisseurData::trierPar(const QString &colonne)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    
    QString queryStr = QString("SELECT ID_FOURNISSEUR, NOM_ENTREPRISE, EMAIL, TELEPHONE, MATRICULE_FISCAL, "
                               "TYPE_PRODUIT, CONDITION_PAIEMENT, STATUT "
                               "FROM FOURNISSEURS ORDER BY %1").arg(colonne);
    
    model->setQuery(queryStr, Connection::instance()->getDatabase());
    
    if (model->lastError().isValid()) {
        qDebug() << "❌ Erreur tri fournisseurs:" << model->lastError().text();
    }
    
    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Nom Entreprise");
    model->setHeaderData(2, Qt::Horizontal, "Email");
    model->setHeaderData(3, Qt::Horizontal, "Téléphone");
    model->setHeaderData(4, Qt::Horizontal, "Matricule Fiscal");
    model->setHeaderData(5, Qt::Horizontal, "Type Produit");
    model->setHeaderData(6, Qt::Horizontal, "Condition Paiement");
    model->setHeaderData(7, Qt::Horizontal, "Statut");
    
    return model;
}
