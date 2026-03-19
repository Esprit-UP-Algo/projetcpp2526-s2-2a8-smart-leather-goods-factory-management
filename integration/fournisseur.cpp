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
                         const QString &statut)
    : id(id)
    , nomEntreprise(nomEntreprise)
    , email(email)
    , telephone(telephone)
    , matriculeFiscal(matriculeFiscal)
    , typeProduit(typeProduit)
    , conditionPaiement(conditionPaiement)
    , statut(statut)
{
}

bool FournisseurData::ajouter()
{
    QSqlQuery query(Connection::instance()->getDatabase());
    query.prepare("INSERT INTO FOURNISSEURS (NOM_ENTREPRISE, EMAIL, TELEPHONE, "
                  "MATRICULE_FISCAL, TYPE_PRODUIT, CONDITION_PAIEMENT, STATUT) "
                  "VALUES (:nomEntreprise, :email, :telephone, :matriculeFiscal, "
                  ":typeProduit, :conditionPaiement, :statut)");
    
    query.bindValue(":nomEntreprise", nomEntreprise);
    query.bindValue(":email", email);
    query.bindValue(":telephone", telephone);
    query.bindValue(":matriculeFiscal", matriculeFiscal);
    query.bindValue(":typeProduit", typeProduit);
    query.bindValue(":conditionPaiement", conditionPaiement);
    query.bindValue(":statut", statut);
    
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
                  "STATUT = :statut "
                  "WHERE ID_FOURNISSEUR = :id");
    
    query.bindValue(":id", id);
    query.bindValue(":nomEntreprise", nomEntreprise);
    query.bindValue(":email", email);
    query.bindValue(":telephone", telephone);
    query.bindValue(":matriculeFiscal", matriculeFiscal);
    query.bindValue(":typeProduit", typeProduit);
    query.bindValue(":conditionPaiement", conditionPaiement);
    query.bindValue(":statut", statut);
    
    if (!query.exec()) {
        qDebug() << "❌ Erreur modification fournisseur:" << query.lastError().text();
        return false;
    }
    
    qDebug() << "✅ Fournisseur modifié avec succès";
    return true;
}

bool FournisseurData::supprimer(const QString &id)
{
    QSqlQuery query(Connection::instance()->getDatabase());
    query.prepare("DELETE FROM FOURNISSEURS WHERE ID_FOURNISSEUR = :id");
    query.bindValue(":id", id);
    
    if (!query.exec()) {
        qDebug() << "❌ Erreur suppression fournisseur:" << query.lastError().text();
        return false;
    }
    
    qDebug() << "✅ Fournisseur supprimé avec succès";
    return true;
}

QSqlQueryModel* FournisseurData::afficher()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT ID_FOURNISSEUR, NOM_ENTREPRISE, EMAIL, TELEPHONE, MATRICULE_FISCAL, "
                    "TYPE_PRODUIT, CONDITION_PAIEMENT, STATUT "
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
