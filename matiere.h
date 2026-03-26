#ifndef MATIERE_H
#define MATIERE_H

#include <QString>
#include <QStringList>
#include <QDate>
#include <QSqlQuery>
#include <QSqlQueryModel>

struct Fournisseur {
    QString id;
    QString nomEntreprise;
    QString nom;
    QString contact;
    QString telephone;
    QString email;
    QString matriculeFiscal;
    QString typeProduit;
    QString conditionPaiement;
    QString statut;
    QStringList matieres;
    
    // Constructor
    Fournisseur() {}
    Fournisseur(const QString &_id, const QString &_nomEntreprise, const QString &_email,
                const QString &_telephone, const QString &_matriculeFiscal,
                const QString &_typeProduit, const QString &_conditionPaiement,
                const QString &_statut)
        : id(_id), nomEntreprise(_nomEntreprise), telephone(_telephone), email(_email),
          matriculeFiscal(_matriculeFiscal), typeProduit(_typeProduit),
          conditionPaiement(_conditionPaiement), statut(_statut) {}
    
    // Getters
    QString getId() const { return id; }
    QString getNomEntreprise() const { return nomEntreprise; }
    QString getEmail() const { return email; }
    QString getTelephone() const { return telephone; }
    QString getMatriculeFiscal() const { return matriculeFiscal; }
    QString getTypeProduit() const { return typeProduit; }
    QString getConditionPaiement() const { return conditionPaiement; }
    QString getStatut() const { return statut; }
};

struct MouvementStock {
    QString date;
    QString matiere;
    QString fournisseur;
    int quantite;
    QString type; // "Entrée" ou "Sortie"
};

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
