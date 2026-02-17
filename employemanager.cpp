#include "employemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

EmployeManager::EmployeManager(QObject *parent)
    : QObject(parent)
{
    // S'assurer que la connexion existe
    Connection::getInstance();
}

EmployeManager::~EmployeManager()
{
}

bool EmployeManager::ajouterEmploye(const Employe &employe)
{
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO EMPLOYE (
            MATRICULE, NOM, PRENOM, CIN, DATE_NAISSANCE, SEXE, ADRESSE,
            TELEPHONE, EMAIL, POSTE, SPECIALITE, DEPARTEMENT, DATE_EMBAUCHE
        ) VALUES (
            :matricule, :nom, :prenom, :cin, :dateNaissance, :sexe, :adresse,
            :telephone, :email, :poste, :specialite, :departement, :dateEmbauche
        )
    )");
    
    query.bindValue(":matricule", employe.getMatricule());
    query.bindValue(":nom", employe.getNom());
    query.bindValue(":prenom", employe.getPrenom());
    query.bindValue(":cin", employe.getCin());
    query.bindValue(":dateNaissance", employe.getDateNaissance());
    query.bindValue(":sexe", employe.getSexe());
    query.bindValue(":adresse", employe.getAdresse());
    query.bindValue(":telephone", employe.getTelephone());
    query.bindValue(":email", employe.getEmail());
    query.bindValue(":poste", employe.getPoste());
    query.bindValue(":specialite", employe.getSpecialite());
    query.bindValue(":departement", employe.getDepartement());
    query.bindValue(":dateEmbauche", employe.getDateEmbauche());
    
    if (!query.exec()) {
        qDebug() << "Erreur ajout:" << query.lastError().text();
        return false;
    }
    return true;
}

bool EmployeManager::modifierEmploye(const Employe &employe)
{
    QSqlQuery query;
    query.prepare(R"(
        UPDATE EMPLOYE SET
            MATRICULE = :matricule,
            NOM = :nom,
            PRENOM = :prenom,
            CIN = :cin,
            DATE_NAISSANCE = :dateNaissance,
            SEXE = :sexe,
            ADRESSE = :adresse,
            TELEPHONE = :telephone,
            EMAIL = :email,
            POSTE = :poste,
            SPECIALITE = :specialite,
            DEPARTEMENT = :departement,
            DATE_EMBAUCHE = :dateEmbauche
        WHERE ID_EMPLOYE = :id
    )");
    
    query.bindValue(":id", employe.getId());
    query.bindValue(":matricule", employe.getMatricule());
    query.bindValue(":nom", employe.getNom());
    query.bindValue(":prenom", employe.getPrenom());
    query.bindValue(":cin", employe.getCin());
    query.bindValue(":dateNaissance", employe.getDateNaissance());
    query.bindValue(":sexe", employe.getSexe());
    query.bindValue(":adresse", employe.getAdresse());
    query.bindValue(":telephone", employe.getTelephone());
    query.bindValue(":email", employe.getEmail());
    query.bindValue(":poste", employe.getPoste());
    query.bindValue(":specialite", employe.getSpecialite());
    query.bindValue(":departement", employe.getDepartement());
    query.bindValue(":dateEmbauche", employe.getDateEmbauche());
    
    if (!query.exec()) {
        qDebug() << "Erreur modification:" << query.lastError().text();
        return false;
    }
    return true;
}

bool EmployeManager::supprimerEmploye(int id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM EMPLOYE WHERE ID_EMPLOYE = :id");
    query.bindValue(":id", id);
    
    if (!query.exec()) {
        qDebug() << "Erreur suppression:" << query.lastError().text();
        return false;
    }
    return true;
}

QList<Employe> EmployeManager::getAllEmployes()
{
    QList<Employe> liste;
    QSqlQuery query("SELECT * FROM EMPLOYE ORDER BY ID_EMPLOYE");
    
    while (query.next()) {
        Employe e;
        e.setId(query.value("ID_EMPLOYE").toInt());
        e.setMatricule(query.value("MATRICULE").toString());
        e.setNom(query.value("NOM").toString());
        e.setPrenom(query.value("PRENOM").toString());
        e.setCin(query.value("CIN").toString());
        e.setDateNaissance(query.value("DATE_NAISSANCE").toDate());
        e.setSexe(query.value("SEXE").toString());
        e.setAdresse(query.value("ADRESSE").toString());
        e.setTelephone(query.value("TELEPHONE").toString());
        e.setEmail(query.value("EMAIL").toString());
        e.setPoste(query.value("POSTE").toString());
        e.setSpecialite(query.value("SPECIALITE").toString());
        e.setDepartement(query.value("DEPARTEMENT").toString());
        e.setDateEmbauche(query.value("DATE_EMBAUCHE").toDate());
        e.setStatut(query.value("STATUT").toString());
        
        liste.append(e);
    }
    
    return liste;
}

Employe EmployeManager::getEmployeById(int id)
{
    Employe e;
    QSqlQuery query;
    query.prepare("SELECT * FROM EMPLOYE WHERE ID_EMPLOYE = :id");
    query.bindValue(":id", id);
    
    if (query.exec() && query.next()) {
        e.setId(query.value("ID_EMPLOYE").toInt());
        e.setMatricule(query.value("MATRICULE").toString());
        e.setNom(query.value("NOM").toString());
        e.setPrenom(query.value("PRENOM").toString());
        e.setCin(query.value("CIN").toString());
        e.setDateNaissance(query.value("DATE_NAISSANCE").toDate());
        e.setSexe(query.value("SEXE").toString());
        e.setAdresse(query.value("ADRESSE").toString());
        e.setTelephone(query.value("TELEPHONE").toString());
        e.setEmail(query.value("EMAIL").toString());
        e.setPoste(query.value("POSTE").toString());
        e.setSpecialite(query.value("SPECIALITE").toString());
        e.setDepartement(query.value("DEPARTEMENT").toString());
        e.setDateEmbauche(query.value("DATE_EMBAUCHE").toDate());
        e.setStatut(query.value("STATUT").toString());
    }
    
    return e;
}

int EmployeManager::getTotalEmployes()
{
    QSqlQuery query("SELECT COUNT(*) FROM EMPLOYE");
    if (query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

int EmployeManager::getEmployesActifs()
{
    QSqlQuery query("SELECT COUNT(*) FROM EMPLOYE WHERE STATUT = 'ACTIF'");
    if (query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

QMap<QString, int> EmployeManager::getEmployesParDepartement()
{
    QMap<QString, int> stats;
    QSqlQuery query("SELECT DEPARTEMENT, COUNT(*) FROM EMPLOYE GROUP BY DEPARTEMENT");
    
    while (query.next()) {
        stats[query.value(0).toString()] = query.value(1).toInt();
    }
    
    return stats;
}
