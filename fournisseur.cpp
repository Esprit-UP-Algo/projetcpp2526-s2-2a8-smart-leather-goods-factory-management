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
    query.prepare("INSERT INTO Fournisseurs (nom_entreprise, email, telephone, "
                  "matricule_fiscal, type_produit, condition_paiement, statut, adresse, quantite_commandee) "
                  "VALUES (:nomEntreprise, :email, :telephone, :matriculeFiscal, "
                  ":typeProduit, :conditionPaiement, :statut, :adresse, :quantiteCommandee)");

    query.bindValue(":nomEntreprise", nomEntreprise);
    query.bindValue(":email", email);
    query.bindValue(":telephone", telephone);
    query.bindValue(":matriculeFiscal", matriculeFiscal);
    query.bindValue(":typeProduit", typeProduit);
    query.bindValue(":conditionPaiement", conditionPaiement);
    query.bindValue(":statut", statut);
    query.bindValue(":adresse", adresse);
    query.bindValue(":quantiteCommandee", quantiteCommandee);

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
    query.prepare("UPDATE Fournisseurs SET "
                  "nom_entreprise = :nomEntreprise, email = :email, "
                  "telephone = :telephone, matricule_fiscal = :matriculeFiscal, "
                  "type_produit = :typeProduit, condition_paiement = :conditionPaiement, "
                  "statut = :statut, adresse = :adresse, quantite_commandee = :quantiteCommandee "
                  "WHERE id_fournisseur = :id");
    
    query.bindValue(":id", id);
    query.bindValue(":nomEntreprise", nomEntreprise);
    query.bindValue(":email", email);
    query.bindValue(":telephone", telephone);
    query.bindValue(":matriculeFiscal", matriculeFiscal);
    query.bindValue(":typeProduit", typeProduit);
    query.bindValue(":conditionPaiement", conditionPaiement);
    query.bindValue(":statut", statut);
    query.bindValue(":adresse", adresse);
    query.bindValue(":quantiteCommandee", quantiteCommandee);
    
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
    query.prepare("DELETE FROM Fournisseurs WHERE id_fournisseur = :id");
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
    model->setQuery("SELECT id_fournisseur, nom_entreprise, email, telephone, matricule_fiscal, "
                    "type_produit, condition_paiement, statut, adresse, quantite_commandee, quantite_mesuree "
                    "FROM Fournisseurs ORDER BY nom_entreprise",
                    Connection::instance()->getDatabase());

    if (model->lastError().isValid()) {
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
    
    return model;
}

QSqlQueryModel* FournisseurData::rechercher(const QString &terme)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    
    QString queryStr = QString(
        "SELECT id_fournisseur, nom_entreprise, email, telephone, matricule_fiscal, "
        "type_produit, condition_paiement, statut "
        "FROM Fournisseurs "
        "WHERE UPPER(nom_entreprise) LIKE UPPER('%%1%') "
        "OR UPPER(email) LIKE UPPER('%%1%') "
        "OR UPPER(telephone) LIKE UPPER('%%1%') "
        "OR UPPER(matricule_fiscal) LIKE UPPER('%%1%') "
        "OR UPPER(type_produit) LIKE UPPER('%%1%') "
        "ORDER BY nom_entreprise"
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
    
    QString queryStr = QString("SELECT id_fournisseur, nom_entreprise, email, telephone, matricule_fiscal, "
                               "type_produit, condition_paiement, statut "
                               "FROM Fournisseurs ORDER BY %1").arg(colonne);
    
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
