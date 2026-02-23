#ifndef EMPLOYE_H
#define EMPLOYE_H

#include <QString>
#include <QDate>
#include <QSqlQuery>
#include <QSqlQueryModel>

class Employe
{
private:
    int id;
    QString matricule;
    QString nom;
    QString prenom;
    QString cin;
    QDate dateNaissance;
    QString sexe;
    QString adresse;
    QString telephone;
    QString email;
    QString poste;
    QString specialite;
    QString departement;
    QDate dateEmbauche;
    QString statut;

public:
    Employe();
    
    // Getters
    int getId() const { return id; }
    QString getMatricule() const { return matricule; }
    QString getNom() const { return nom; }
    QString getPrenom() const { return prenom; }
    QString getCin() const { return cin; }
    QDate getDateNaissance() const { return dateNaissance; }
    QString getSexe() const { return sexe; }
    QString getAdresse() const { return adresse; }
    QString getTelephone() const { return telephone; }
    QString getEmail() const { return email; }
    QString getPoste() const { return poste; }
    QString getSpecialite() const { return specialite; }
    QString getDepartement() const { return departement; }
    QDate getDateEmbauche() const { return dateEmbauche; }
    QString getStatut() const { return statut; }
    
    // Setters
    void setId(int value) { id = value; }
    void setMatricule(const QString &value) { matricule = value; }
    void setNom(const QString &value) { nom = value; }
    void setPrenom(const QString &value) { prenom = value; }
    void setCin(const QString &value) { cin = value; }
    void setDateNaissance(const QDate &value) { dateNaissance = value; }
    void setSexe(const QString &value) { sexe = value; }
    void setAdresse(const QString &value) { adresse = value; }
    void setTelephone(const QString &value) { telephone = value; }
    void setEmail(const QString &value) { email = value; }
    void setPoste(const QString &value) { poste = value; }
    void setSpecialite(const QString &value) { specialite = value; }
    void setDepartement(const QString &value) { departement = value; }
    void setDateEmbauche(const QDate &value) { dateEmbauche = value; }
    void setStatut(const QString &value) { statut = value; }
    
    // Méthodes CRUD
    bool ajouter();
    bool modifier();
    bool supprimer(int id);
    QSqlQueryModel* afficher();
    QSqlQueryModel* rechercher(const QString &terme);
    QSqlQueryModel* trierPar(const QString &colonne);
};

#endif // EMPLOYE_H
