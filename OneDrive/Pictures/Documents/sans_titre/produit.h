#ifndef PRODUIT_H
#define PRODUIT_H

#include <QString>
#include <QDate>

class Produit
{
private:
    QString reference;
    QString nom;
    QString categorie;
    QString type;
    QString couleur;
    QString dimensions;
    double prixUnitaire;
    double coutFabrication;
    int stock;
    QString statut;
    QString description;
    QDate dateCreation;

public:
    Produit() : prixUnitaire(0.0), coutFabrication(0.0), stock(0), dateCreation(QDate::currentDate()) {}

    // Getters
    QString getReference() const { return reference; }
    QString getNom() const { return nom; }
    QString getCategorie() const { return categorie; }
    QString getType() const { return type; }
    QString getCouleur() const { return couleur; }
    QString getDimensions() const { return dimensions; }
    double getPrixUnitaire() const { return prixUnitaire; }
    double getCoutFabrication() const { return coutFabrication; }
    int getStock() const { return stock; }
    QString getStatut() const { return statut; }
    QString getDescription() const { return description; }
    QDate getDateCreation() const { return dateCreation; }

    // Setters
    void setReference(const QString &ref) { reference = ref; }
    void setNom(const QString &n) { nom = n; }
    void setCategorie(const QString &cat) { categorie = cat; }
    void setType(const QString &t) { type = t; }
    void setCouleur(const QString &c) { couleur = c; }
    void setDimensions(const QString &d) { dimensions = d; }
    void setPrixUnitaire(double prix) { prixUnitaire = prix; }
    void setCoutFabrication(double cout) { coutFabrication = cout; }
    void setStock(int s) { stock = s; }
    void setStatut(const QString &st) { statut = st; }
    void setDescription(const QString &desc) { description = desc; }
    void setDateCreation(const QDate &date) { dateCreation = date; }
};

#endif // PRODUIT_H
