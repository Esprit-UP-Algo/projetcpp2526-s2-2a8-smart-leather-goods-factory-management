#ifndef MATIERE_H
#define MATIERE_H

#include <QString>
#include <QDate>

struct Fournisseur {
    QString nom;
    QString contact;
    QString telephone;
    QString email;
    QStringList matieres;
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
