#ifndef MATIERE_H
#define MATIERE_H

#include <QString>
#include <QDate>
#include <QSqlQuery>
#include <QSqlQueryModel>

class Matiere
{
private:
    int id;
    QString nom;
    QString reference;
    QString type;
    double quantite;
    int seuil;
    QDate dateExpiration;
    int idFournisseur;
    QString photoUrl;

public:
    Matiere();

    // Getters
    int getId() const { return id; }
    QString getNom() const { return nom; }
    QString getReference() const { return reference; }
    QString getType() const { return type; }
    double getQuantite() const { return quantite; }
    int getSeuil() const { return seuil; }
    QDate getDateExpiration() const { return dateExpiration; }
    int getIdFournisseur() const { return idFournisseur; }
    QString getPhotoUrl() const { return photoUrl; }

    // Setters
    void setId(int value) { id = value; }
    void setNom(const QString &value) { nom = value; }
    void setReference(const QString &value) { reference = value; }
    void setType(const QString &value) { type = value; }
    void setQuantite(double value) { quantite = value; }
    void setSeuil(int value) { seuil = value; }
    void setDateExpiration(const QDate &value) { dateExpiration = value; }
    void setIdFournisseur(int value) { idFournisseur = value; }
    void setPhotoUrl(const QString &value) { photoUrl = value; }

    // Méthodes CRUD
    bool ajouter();
    bool modifier();
    bool supprimer(int id);
    QSqlQueryModel* afficher();
    QSqlQueryModel* rechercher(const QString &terme);
    QSqlQueryModel* trierPar(const QString &colonne);
};

#endif // MATIERE_H
