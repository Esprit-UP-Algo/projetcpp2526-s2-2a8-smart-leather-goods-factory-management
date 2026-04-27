#ifndef FOURNISSEUR_H
#define FOURNISSEUR_H

#include <QString>
#include <QSqlQuery>
#include <QSqlQueryModel>

class FournisseurData
{
public:
    FournisseurData();
    FournisseurData(const QString &id, const QString &nomEntreprise, const QString &email,
                const QString &telephone, const QString &matriculeFiscal,
                const QString &typeProduit, const QString &conditionPaiement,
                const QString &statut, const QString &adresse = QString());

    // Getters
    QString getId() const { return id; }
    QString getNomEntreprise() const { return nomEntreprise; }
    QString getEmail() const { return email; }
    QString getTelephone() const { return telephone; }
    QString getMatriculeFiscal() const { return matriculeFiscal; }
    QString getTypeProduit() const { return typeProduit; }
    QString getConditionPaiement() const { return conditionPaiement; }
    QString getStatut() const { return statut; }
    QString getAdresse() const { return adresse; }
    double  getQuantiteCommandee() const { return quantiteCommandee; }

    // Setters
    void setId(const QString &value) { id = value; }
    void setNomEntreprise(const QString &value) { nomEntreprise = value; }
    void setEmail(const QString &value) { email = value; }
    void setTelephone(const QString &value) { telephone = value; }
    void setMatriculeFiscal(const QString &value) { matriculeFiscal = value; }
    void setTypeProduit(const QString &value) { typeProduit = value; }
    void setConditionPaiement(const QString &value) { conditionPaiement = value; }
    void setStatut(const QString &value) { statut = value; }
    void setAdresse(const QString &value) { adresse = value; }
    void setQuantiteCommandee(double value) { quantiteCommandee = value; }

    // Méthodes CRUD
    bool ajouter();
    bool modifier();
    bool supprimer(const QString &id);
    QSqlQueryModel* afficher();
    QSqlQueryModel* rechercher(const QString &terme);
    QSqlQueryModel* trierPar(const QString &colonne);

private:
    QString id;
    QString nomEntreprise;
    QString email;
    QString telephone;
    QString matriculeFiscal;
    QString typeProduit;
    QString conditionPaiement;
    QString statut;
    QString adresse;
    double  quantiteCommandee = 0.0;
};

#endif // FOURNISSEUR_H
