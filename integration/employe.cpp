#include "employe.h"
#include "connection.h"
#include <QSqlError>
#include <QDebug>

Employe::Employe()
    : id(0)
    , statut("ACTIF")
{
}

bool Employe::ajouter()
{
    QSqlQuery query(Connection::instance()->getDatabase());
    query.prepare("INSERT INTO EMPLOYES (MATRICULE, NOM, PRENOM, CIN, DATE_NAISSANCE, "
                  "SEXE, ADRESSE, TELEPHONE, EMAIL, POSTE, SPECIALITE, DEPARTEMENT, "
                  "DATE_EMBAUCHE, STATUT) "
                  "VALUES (:matricule, :nom, :prenom, :cin, :dateNaissance, "
                  ":sexe, :adresse, :telephone, :email, :poste, :specialite, "
                  ":departement, :dateEmbauche, :statut)");
    
    query.bindValue(":matricule", matricule);
    query.bindValue(":nom", nom);
    query.bindValue(":prenom", prenom);
    query.bindValue(":cin", cin);
    query.bindValue(":dateNaissance", dateNaissance);
    query.bindValue(":sexe", sexe);
    query.bindValue(":adresse", adresse);
    query.bindValue(":telephone", telephone);
    query.bindValue(":email", email);
    query.bindValue(":poste", poste);
    query.bindValue(":specialite", specialite);
    query.bindValue(":departement", departement);
    query.bindValue(":dateEmbauche", dateEmbauche);
    query.bindValue(":statut", statut);
    
    if (!query.exec()) {
        qDebug() << "❌ Erreur ajout employé:" << query.lastError().text();
        return false;
    }
    
    qDebug() << "✅ Employé ajouté avec succès";
    return true;
}

bool Employe::modifier()
{
    QSqlQuery query(Connection::instance()->getDatabase());
    query.prepare("UPDATE EMPLOYES SET "
                  "MATRICULE = :matricule, NOM = :nom, PRENOM = :prenom, "
                  "CIN = :cin, DATE_NAISSANCE = :dateNaissance, SEXE = :sexe, "
                  "ADRESSE = :adresse, TELEPHONE = :telephone, EMAIL = :email, "
                  "POSTE = :poste, SPECIALITE = :specialite, DEPARTEMENT = :departement, "
                  "DATE_EMBAUCHE = :dateEmbauche, STATUT = :statut "
                  "WHERE ID_EMPLOYE = :id");
    
    query.bindValue(":id", id);
    query.bindValue(":matricule", matricule);
    query.bindValue(":nom", nom);
    query.bindValue(":prenom", prenom);
    query.bindValue(":cin", cin);
    query.bindValue(":dateNaissance", dateNaissance);
    query.bindValue(":sexe", sexe);
    query.bindValue(":adresse", adresse);
    query.bindValue(":telephone", telephone);
    query.bindValue(":email", email);
    query.bindValue(":poste", poste);
    query.bindValue(":specialite", specialite);
    query.bindValue(":departement", departement);
    query.bindValue(":dateEmbauche", dateEmbauche);
    query.bindValue(":statut", statut);
    
    if (!query.exec()) {
        qDebug() << "❌ Erreur modification employé:" << query.lastError().text();
        return false;
    }
    
    qDebug() << "✅ Employé modifié avec succès";
    return true;
}

bool Employe::supprimer(int id)
{
    QSqlQuery query(Connection::instance()->getDatabase());
    query.prepare("DELETE FROM EMPLOYES WHERE ID_EMPLOYE = :id");
    query.bindValue(":id", id);
    
    if (!query.exec()) {
        qDebug() << "❌ Erreur suppression employé:" << query.lastError().text();
        return false;
    }
    
    qDebug() << "✅ Employé supprimé avec succès";
    return true;
}

QSqlQueryModel* Employe::afficher()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT ID_EMPLOYE, MATRICULE, NOM, PRENOM, CIN, DATE_NAISSANCE, "
                    "DEPARTEMENT, POSTE, TELEPHONE, EMAIL "
                    "FROM EMPLOYES ORDER BY MATRICULE", 
                    Connection::instance()->getDatabase());
    
    if (model->lastError().isValid()) {
        qDebug() << "❌ Erreur affichage employés:" << model->lastError().text();
        delete model;
        return nullptr;
    }
    
    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Matricule");
    model->setHeaderData(2, Qt::Horizontal, "Nom");
    model->setHeaderData(3, Qt::Horizontal, "Prénom");
    model->setHeaderData(4, Qt::Horizontal, "CIN");
    model->setHeaderData(5, Qt::Horizontal, "Date Naissance");
    model->setHeaderData(6, Qt::Horizontal, "Département");
    model->setHeaderData(7, Qt::Horizontal, "Poste");
    model->setHeaderData(8, Qt::Horizontal, "Téléphone");
    model->setHeaderData(9, Qt::Horizontal, "Email");
    
    return model;
}

QSqlQueryModel* Employe::rechercher(const QString &terme)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    
    QString queryStr = QString(
        "SELECT ID_EMPLOYE, MATRICULE, NOM, PRENOM, CIN, DATE_NAISSANCE, "
        "DEPARTEMENT, POSTE, TELEPHONE, EMAIL "
        "FROM EMPLOYES "
        "WHERE UPPER(MATRICULE) LIKE UPPER('%%1%') "
        "OR UPPER(NOM) LIKE UPPER('%%1%') "
        "OR UPPER(PRENOM) LIKE UPPER('%%1%') "
        "OR UPPER(CIN) LIKE UPPER('%%1%') "
        "OR UPPER(DEPARTEMENT) LIKE UPPER('%%1%') "
        "OR UPPER(POSTE) LIKE UPPER('%%1%') "
        "ORDER BY MATRICULE"
    ).arg(terme);
    
    model->setQuery(queryStr, Connection::instance()->getDatabase());
    
    if (model->lastError().isValid()) {
        qDebug() << "❌ Erreur recherche employés:" << model->lastError().text();
        delete model;
        return nullptr;
    }
    
    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Matricule");
    model->setHeaderData(2, Qt::Horizontal, "Nom");
    model->setHeaderData(3, Qt::Horizontal, "Prénom");
    model->setHeaderData(4, Qt::Horizontal, "CIN");
    model->setHeaderData(5, Qt::Horizontal, "Date Naissance");
    model->setHeaderData(6, Qt::Horizontal, "Département");
    model->setHeaderData(7, Qt::Horizontal, "Poste");
    model->setHeaderData(8, Qt::Horizontal, "Téléphone");
    model->setHeaderData(9, Qt::Horizontal, "Email");
    
    return model;
}

QSqlQueryModel* Employe::trierPar(const QString &colonne)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    
    QString queryStr = QString("SELECT ID_EMPLOYE, MATRICULE, NOM, PRENOM, CIN, DATE_NAISSANCE, "
                               "DEPARTEMENT, POSTE, TELEPHONE, EMAIL "
                               "FROM EMPLOYES ORDER BY %1").arg(colonne);
    
    model->setQuery(queryStr, Connection::instance()->getDatabase());
    
    if (model->lastError().isValid()) {
        qDebug() << "❌ Erreur tri employés:" << model->lastError().text();
    }
    
    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Matricule");
    model->setHeaderData(2, Qt::Horizontal, "Nom");
    model->setHeaderData(3, Qt::Horizontal, "Prénom");
    model->setHeaderData(4, Qt::Horizontal, "CIN");
    model->setHeaderData(5, Qt::Horizontal, "Date Naissance");
    model->setHeaderData(6, Qt::Horizontal, "Département");
    model->setHeaderData(7, Qt::Horizontal, "Poste");
    model->setHeaderData(8, Qt::Horizontal, "Téléphone");
    model->setHeaderData(9, Qt::Horizontal, "Email");
    
    return model;
}
