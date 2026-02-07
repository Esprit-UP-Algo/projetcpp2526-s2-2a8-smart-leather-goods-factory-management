#ifndef PRODUCTION_H
#define PRODUCTION_H

#include <QString>
#include <QDate>

class ProductionData
{
public:
    ProductionData();
    ProductionData(const QString &id, const QString &reference, const QString &produit,
                const QString &quantite, const QString &statut, const QDate &dateDebut,
                const QDate &dateFin, const QString &responsable, const QString &priorite);

    // Getters
    QString getId() const { return id; }
    QString getReference() const { return reference; }
    QString getProduit() const { return produit; }
    QString getQuantite() const { return quantite; }
    QString getStatut() const { return statut; }
    QDate getDateDebut() const { return dateDebut; }
    QDate getDateFin() const { return dateFin; }
    QString getResponsable() const { return responsable; }
    QString getPriorite() const { return priorite; }

    // Setters
    void setId(const QString &value) { id = value; }
    void setReference(const QString &value) { reference = value; }
    void setProduit(const QString &value) { produit = value; }
    void setQuantite(const QString &value) { quantite = value; }
    void setStatut(const QString &value) { statut = value; }
    void setDateDebut(const QDate &value) { dateDebut = value; }
    void setDateFin(const QDate &value) { dateFin = value; }
    void setResponsable(const QString &value) { responsable = value; }
    void setPriorite(const QString &value) { priorite = value; }

private:
    QString id;
    QString reference;
    QString produit;
    QString quantite;
    QString statut;
    QDate dateDebut;
    QDate dateFin;
    QString responsable;
    QString priorite;
};

#endif // PRODUCTION_H
