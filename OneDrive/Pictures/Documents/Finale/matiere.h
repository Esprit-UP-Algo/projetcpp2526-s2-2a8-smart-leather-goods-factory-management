#ifndef MATIERE_H
#define MATIERE_H

#include <QString>
#include <QDate>

struct Fournisseur {
    QString id;
    QString nom;
    QString nomEntreprise;
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
        : id(_id), nomEntreprise(_nomEntreprise), email(_email), telephone(_telephone),
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
public:
    Matiere();
    Matiere(const QString &module, const QString &reference, const QString &type,
            const QString &quantite, const QString &seuil, const QDate &dateExp);

    QString getModule() const { return m_module; }
    QString getReference() const { return m_reference; }
    QString getType() const { return m_type; }
    QString getQuantite() const { return m_quantite; }
    QString getSeuil() const { return m_seuil; }
    QDate getDateExpiration() const { return m_dateExpiration; }

    void setModule(const QString &module) { m_module = module; }
    void setReference(const QString &reference) { m_reference = reference; }
    void setType(const QString &type) { m_type = type; }
    void setQuantite(const QString &quantite) { m_quantite = quantite; }
    void setSeuil(const QString &seuil) { m_seuil = seuil; }
    void setDateExpiration(const QDate &date) { m_dateExpiration = date; }

private:
    QString m_module;
    QString m_reference;
    QString m_type;
    QString m_quantite;
    QString m_seuil;
    QDate m_dateExpiration;
};

#endif // MATIERE_H
