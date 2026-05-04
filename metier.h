#ifndef METIER_H
#define METIER_H

#include <QString>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QMap>

class Metier
{
private:
    int     id;
    QString code;
    QString libelle;
    QString description;
    QString domaine;
    QString niveauQualification;
    double  salaireBase;
    bool    actif;

public:
    Metier();

    // Getters
    int     getId()                const { return id; }
    QString getCode()              const { return code; }
    QString getLibelle()           const { return libelle; }
    QString getDescription()       const { return description; }
    QString getDomaine()           const { return domaine; }
    QString getNiveauQualification() const { return niveauQualification; }
    double  getSalaireBase()       const { return salaireBase; }
    bool    isActif()              const { return actif; }

    // Setters
    void setId(int v)                          { id = v; }
    void setCode(const QString &v)             { code = v; }
    void setLibelle(const QString &v)          { libelle = v; }
    void setDescription(const QString &v)      { description = v; }
    void setDomaine(const QString &v)          { domaine = v; }
    void setNiveauQualification(const QString &v) { niveauQualification = v; }
    void setSalaireBase(double v)              { salaireBase = v; }
    void setActif(bool v)                      { actif = v; }

    // CRUD
    bool ajouter();
    bool modifier();
    bool supprimer(int id);

    // Queries
    QSqlQueryModel* afficher();
    QSqlQueryModel* rechercher(const QString &terme);
    QSqlQueryModel* trierPar(const QString &colonne);

    // Statistics
    int              getTotalMetiers();
    int              getNombreDomaines();
    QMap<QString,int> getEffectifParDomaine();
    QMap<QString,int> getEffectifParNiveau();
};

#endif // METIER_H
