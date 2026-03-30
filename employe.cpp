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
    QSqlDatabase db = Connection::instance()->getDatabase();
    QSqlQuery query(db);
    
    qDebug() << "=== AJOUT EMPLOYE ===";
    qDebug() << "Photo path:" << photoPath;
    
    query.prepare("INSERT INTO CUIREA.EMPLOYES (ID_EMPLOYE, MATRICULE, NOM, PRENOM, CIN, DATE_NAISSANCE, "
                  "SEXE, ADRESSE, TELEPHONE, EMAIL, POSTE, SPECIALITE, DEPARTEMENT, "
                  "DATE_EMBAUCHE, PHOTO_URL) "
                  "VALUES (SEQ_EMPLOYE.NEXTVAL, :matricule, :nom, :prenom, :cin, :dateNaissance, "
                  ":sexe, :adresse, :telephone, :email, :poste, :specialite, "
                  ":departement, :dateEmbauche, :photoUrl)");
    
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
    query.bindValue(":photoUrl", photoPath);
    
    if (!query.exec()) {
        qDebug() << "Erreur SQL:" << query.lastError().text();
        return false;
    }
    
    db.commit();
    qDebug() << "Employé ajouté avec succès!";
    return true;
}

bool Employe::modifier()
{
    qDebug() << "=== MODIFICATION EMPLOYE ===";
    qDebug() << "ID:" << id;
    qDebug() << "Photo path:" << photoPath;
    
    QSqlQuery query(Connection::instance()->getDatabase());
    query.prepare("UPDATE CUIREA.EMPLOYES SET "
                  "MATRICULE = :matricule, NOM = :nom, PRENOM = :prenom, "
                  "CIN = :cin, DATE_NAISSANCE = :dateNaissance, SEXE = :sexe, "
                  "ADRESSE = :adresse, TELEPHONE = :telephone, EMAIL = :email, "
                  "POSTE = :poste, SPECIALITE = :specialite, DEPARTEMENT = :departement, "
                  "DATE_EMBAUCHE = :dateEmbauche, PHOTO_URL = :photoUrl "
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
    query.bindValue(":photoUrl", photoPath);
    
    bool result = query.exec();
    if (!result) {
        qDebug() << "Erreur SQL:" << query.lastError().text();
    } else {
        qDebug() << "Modification réussie!";
    }
    
    return result;
}

bool Employe::supprimer(int id)
{
    QSqlQuery query(Connection::instance()->getDatabase());
    query.prepare("DELETE FROM CUIREA.EMPLOYES WHERE ID_EMPLOYE = :id");
    query.bindValue(":id", id);
    return query.exec();
}

QSqlQueryModel* Employe::afficher()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT MATRICULE, NOM, PRENOM, CIN, DATE_NAISSANCE, SEXE, "
                    "ADRESSE, TELEPHONE, EMAIL, POSTE, SPECIALITE, DEPARTEMENT, "
                    "DATE_EMBAUCHE, PHOTO_URL, ID_EMPLOYE "
                    "FROM CUIREA.EMPLOYES ORDER BY MATRICULE", 
                    Connection::instance()->getDatabase());
    
    if (model->lastError().isValid()) {
        delete model;
        return nullptr;
    }
    
    model->setHeaderData(0, Qt::Horizontal, "Matricule");
    model->setHeaderData(1, Qt::Horizontal, "Nom");
    model->setHeaderData(2, Qt::Horizontal, "Prénom");
    model->setHeaderData(3, Qt::Horizontal, "CIN");
    model->setHeaderData(4, Qt::Horizontal, "Date Naissance");
    model->setHeaderData(5, Qt::Horizontal, "Sexe");
    model->setHeaderData(6, Qt::Horizontal, "Adresse");
    model->setHeaderData(7, Qt::Horizontal, "Téléphone");
    model->setHeaderData(8, Qt::Horizontal, "Email");
    model->setHeaderData(9, Qt::Horizontal, "Poste");
    model->setHeaderData(10, Qt::Horizontal, "Spécialité");
    model->setHeaderData(11, Qt::Horizontal, "Département");
    model->setHeaderData(12, Qt::Horizontal, "Date Embauche");
    model->setHeaderData(13, Qt::Horizontal, "Photo");
    model->setHeaderData(14, Qt::Horizontal, "ID");
    
    return model;
}

QSqlQueryModel* Employe::rechercher(const QString &terme)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    
    QString queryStr = QString(
        "SELECT ID_EMPLOYE, MATRICULE, NOM, PRENOM, CIN, DATE_NAISSANCE, "
        "DEPARTEMENT, POSTE, TELEPHONE, EMAIL, PHOTO_URL "
        "FROM CUIREA.EMPLOYES "
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
    model->setHeaderData(10, Qt::Horizontal, "Photo");
    
    return model;
}

QSqlQueryModel* Employe::trierPar(const QString &colonne)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    
    QString queryStr = QString("SELECT ID_EMPLOYE, MATRICULE, NOM, PRENOM, CIN, DATE_NAISSANCE, "
                               "DEPARTEMENT, POSTE, TELEPHONE, EMAIL, PHOTO_URL "
                               "FROM CUIREA.EMPLOYES ORDER BY %1").arg(colonne);
    
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
    model->setHeaderData(10, Qt::Horizontal, "Photo");
    
    return model;
}
