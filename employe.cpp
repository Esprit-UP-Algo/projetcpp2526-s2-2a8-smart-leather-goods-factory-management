#include "employe.h"
#include "connection.h"
#include <QSqlError>
#include <QDebug>

Employe::Employe()
    : id(0)
    , statut("ACTIF")
    , roleSysteme("EMPLOYE")
    , actif(true)
{
}

bool Employe::ajouter()
{
    QSqlDatabase db = Connection::instance()->getDatabase();
    QSqlQuery query(db);
    
    qDebug() << "=== AJOUT EMPLOYE ===";
    qDebug() << "Photo path:" << photoPath;
    
    query.prepare("INSERT INTO CUIREA.EMPLOYES (ID_EMPLOYE, MATRICULE, NOM, PRENOM, CIN, DATE_NAISSANCE, "
                  "SEXE, ADRESSE, TELEPHONE, EMAIL, POSTE, DEPARTEMENT, "
                  "DATE_EMBAUCHE, PHOTO_URL, MOT_DE_PASSE, ROLE_SYSTEME, ACTIF) "
                  "VALUES (SEQ_EMPLOYE.NEXTVAL, :matricule, :nom, :prenom, :cin, :dateNaissance, "
                  ":sexe, :adresse, :telephone, :email, :poste, "
                  ":departement, :dateEmbauche, :photoUrl, :motDePasse, :roleSysteme, :actif)");
    
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
    query.bindValue(":departement", departement);
    query.bindValue(":dateEmbauche", dateEmbauche);
    query.bindValue(":photoUrl", photoPath);
    query.bindValue(":motDePasse", motDePasse);
    query.bindValue(":roleSysteme", roleSysteme);
    query.bindValue(":actif", actif ? 1 : 0);
    
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
                  "POSTE = :poste, DEPARTEMENT = :departement, "
                  "DATE_EMBAUCHE = :dateEmbauche, PHOTO_URL = :photoUrl, "
                  "MOT_DE_PASSE = :motDePasse, ROLE_SYSTEME = :roleSysteme, ACTIF = :actif "
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
    query.bindValue(":departement", departement);
    query.bindValue(":dateEmbauche", dateEmbauche);
    query.bindValue(":photoUrl", photoPath);
    query.bindValue(":motDePasse", motDePasse);
    query.bindValue(":roleSysteme", roleSysteme);
    query.bindValue(":actif", actif ? 1 : 0);
    
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
                    "ADRESSE, TELEPHONE, EMAIL, POSTE, DEPARTEMENT, "
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
    model->setHeaderData(10, Qt::Horizontal, "Département");
    model->setHeaderData(11, Qt::Horizontal, "Date Embauche");
    model->setHeaderData(12, Qt::Horizontal, "Photo");
    model->setHeaderData(13, Qt::Horizontal, "ID");
    
    return model;
}

QSqlQueryModel* Employe::rechercher(const QString &terme)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    
    QString queryStr = QString(
        "SELECT ID_EMPLOYE, MATRICULE, NOM, PRENOM, CIN, DATE_NAISSANCE, SEXE, "
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
    model->setHeaderData(6, Qt::Horizontal, "Sexe");
    model->setHeaderData(7, Qt::Horizontal, "Département");
    model->setHeaderData(8, Qt::Horizontal, "Poste");
    model->setHeaderData(9, Qt::Horizontal, "Téléphone");
    model->setHeaderData(10, Qt::Horizontal, "Email");
    model->setHeaderData(11, Qt::Horizontal, "Photo");
    
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

QSqlQueryModel* Employe::rechercherParCritere(const QString &critere, const QString &valeur)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlQuery query(Connection::instance()->getDatabase());
    
    QString queryStr = "SELECT ID_EMPLOYE, MATRICULE, NOM, PRENOM, CIN, DATE_NAISSANCE, SEXE, "
                      "DEPARTEMENT, POSTE, TELEPHONE, EMAIL, PHOTO_URL "
                      "FROM CUIREA.EMPLOYES WHERE ";
    
    if (critere == "nom") {
        queryStr += "UPPER(NOM) LIKE UPPER(:valeur)";
    } else if (critere == "prenom") {
        queryStr += "UPPER(PRENOM) LIKE UPPER(:valeur)";
    } else if (critere == "id") {
        queryStr += "ID_EMPLOYE = :valeur";
    } else if (critere == "matricule") {
        queryStr += "UPPER(MATRICULE) LIKE UPPER(:valeur)";
    } else if (critere == "departement") {
        queryStr += "UPPER(DEPARTEMENT) LIKE UPPER(:valeur)";
    } else if (critere == "poste") {
        queryStr += "UPPER(POSTE) LIKE UPPER(:valeur)";
    } else {
        queryStr += "1=1";
    }
    
    queryStr += " ORDER BY MATRICULE";
    
    query.prepare(queryStr);
    
    if (critere == "id") {
        query.bindValue(":valeur", valeur.toInt());
    } else {
        query.bindValue(":valeur", "%" + valeur + "%");
    }
    
    if (!query.exec()) {
        qDebug() << "❌ Erreur recherche par critère:" << query.lastError().text();
        delete model;
        return nullptr;
    }
    
    model->setQuery(std::move(query));
    
    if (model->lastError().isValid()) {
        qDebug() << "❌ Erreur recherche par critère:" << model->lastError().text();
        delete model;
        return nullptr;
    }
    
    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Matricule");
    model->setHeaderData(2, Qt::Horizontal, "Nom");
    model->setHeaderData(3, Qt::Horizontal, "Prénom");
    model->setHeaderData(4, Qt::Horizontal, "CIN");
    model->setHeaderData(5, Qt::Horizontal, "Date Naissance");
    model->setHeaderData(6, Qt::Horizontal, "Sexe");
    model->setHeaderData(7, Qt::Horizontal, "Département");
    model->setHeaderData(8, Qt::Horizontal, "Poste");
    model->setHeaderData(9, Qt::Horizontal, "Téléphone");
    model->setHeaderData(10, Qt::Horizontal, "Email");
    model->setHeaderData(11, Qt::Horizontal, "Photo");
    
    return model;
}

// Statistiques RH
QMap<QString, int> Employe::getEffectifParDepartement()
{
    QMap<QString, int> stats;
    QSqlQuery query(Connection::instance()->getDatabase());
    
    query.prepare("SELECT DEPARTEMENT, COUNT(*) as EFFECTIF "
                 "FROM CUIREA.EMPLOYES "
                 "GROUP BY DEPARTEMENT "
                 "ORDER BY EFFECTIF DESC");
    
    if (query.exec()) {
        while (query.next()) {
            stats[query.value(0).toString()] = query.value(1).toInt();
        }
    }
    
    return stats;
}

QMap<QString, int> Employe::getEffectifParPoste()
{
    QMap<QString, int> stats;
    QSqlQuery query(Connection::instance()->getDatabase());
    
    query.prepare("SELECT POSTE, COUNT(*) as EFFECTIF "
                 "FROM CUIREA.EMPLOYES "
                 "GROUP BY POSTE "
                 "ORDER BY EFFECTIF DESC");
    
    if (query.exec()) {
        while (query.next()) {
            stats[query.value(0).toString()] = query.value(1).toInt();
        }
    }
    
    return stats;
}

QMap<QString, int> Employe::getEffectifParSexe()
{
    QMap<QString, int> stats;
    QSqlQuery query(Connection::instance()->getDatabase());
    
    query.prepare("SELECT SEXE, COUNT(*) as EFFECTIF "
                 "FROM CUIREA.EMPLOYES "
                 "GROUP BY SEXE");
    
    if (query.exec()) {
        while (query.next()) {
            QString sexe = query.value(0).toString();
            stats[sexe == "M" ? "Hommes" : "Femmes"] = query.value(1).toInt();
        }
    }
    
    return stats;
}

int Employe::getTotalEmployes()
{
    QSqlQuery query(Connection::instance()->getDatabase());
    query.prepare("SELECT COUNT(*) FROM CUIREA.EMPLOYES");
    
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    
    return 0;
}

int Employe::getNombreDepartements()
{
    QSqlQuery query(Connection::instance()->getDatabase());
    query.prepare("SELECT COUNT(DISTINCT DEPARTEMENT) FROM CUIREA.EMPLOYES");
    
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    
    return 0;
}

int Employe::getNombrePostes()
{
    QSqlQuery query(Connection::instance()->getDatabase());
    query.prepare("SELECT COUNT(DISTINCT POSTE) FROM CUIREA.EMPLOYES");
    
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    
    return 0;
}
