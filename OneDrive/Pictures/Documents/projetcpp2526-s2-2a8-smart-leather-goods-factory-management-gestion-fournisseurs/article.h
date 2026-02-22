#ifndef ARTICLE_H
#define ARTICLE_H

#include <QString>
#include <QDate>

class Article
{
public:
    Article();
    
    // Getters
    int getIdProduit() const { return idProduit; }
    QString getReference() const { return reference; }
    QString getNom() const { return nom; }
    QString getCategorie() const { return categorie; }
    QString getType() const { return type; }
    QString getCouleur() const { return couleur; }
    QString getDimensions() const { return dimensions; }
    double getPrixUnitaire() const { return prixUnitaire; }
    double getCoutFabrication() const { return coutFabrication; }
    QString getStatut() const { return statut; }
    QDate getDateCreation() const { return dateCreation; }
    
    // Setters
    void setIdProduit(int id) { idProduit = id; }
    void setReference(const QString &ref) { reference = ref; }
    void setNom(const QString &n) { nom = n; }
    void setCategorie(const QString &cat) { categorie = cat; }
    void setType(const QString &t) { type = t; }
    void setCouleur(const QString &c) { couleur = c; }
    void setDimensions(const QString &dim) { dimensions = dim; }
    void setPrixUnitaire(double prix) { prixUnitaire = prix; }
    void setCoutFabrication(double cout) { coutFabrication = cout; }
    void setStatut(const QString &s) { statut = s; }
    void setDateCreation(const QDate &date) { dateCreation = date; }

private:
    int idProduit;
    QString reference;
    QString nom;
    QString categorie;
    QString type;
    QString couleur;
    QString dimensions;
    double prixUnitaire;
    double coutFabrication;
    QString statut;
    QDate dateCreation;
};

#endif // ARTICLE_H
