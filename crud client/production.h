#ifndef PRODUCTION_H
#define PRODUCTION_H

#include <QString>
#include <QDate>

// ═══════════════════════════════════════════════════════════════════════════
// CLASSE MÉTIER PURE : Production
// Représente une entité de la table PRODUCTION
// ═══════════════════════════════════════════════════════════════════════════
class Production
{
public:
    Production();
    Production(int id, const QString &reference, const QString &type,
               const QDate &dateCreation, const QDate &dateLivraisonPrevue,
               const QString &statut, const QString &priorite,
               double montant, const QString &etatPaiement,
               const QString &serviceVente);

    // Getters
    int getIdCommande() const { return id_commande; }
    QString getReference() const { return reference; }
    QString getType() const { return type; }
    QDate getDateCreation() const { return date_creation; }
    QDate getDateLivraisonPrevue() const { return date_livraison_prevue; }
    QString getStatut() const { return statut; }
    QString getPriorite() const { return priorite; }
    double getMontant() const { return montant; }
    int getQuantite() const { return quantite; }
    QString getEtatPaiement() const { return etat_paiement; }
    QString getServiceVente() const { return service_vente; }
    int getIdEmploye() const { return id_employe; }

    // Setters
    void setIdCommande(int value) { id_commande = value; }
    void setReference(const QString &value) { reference = value; }
    void setType(const QString &value) { type = value; }
    void setDateCreation(const QDate &value) { date_creation = value; }
    void setDateLivraisonPrevue(const QDate &value) { date_livraison_prevue = value; }
    void setStatut(const QString &value) { statut = value; }
    void setPriorite(const QString &value) { priorite = value; }
    void setMontant(double value) { montant = value; }
    void setQuantite(int value) { quantite = value; }
    void setEtatPaiement(const QString &value) { etat_paiement = value; }
    void setServiceVente(const QString &value) { service_vente = value; }
    void setIdEmploye(int value) { id_employe = value; }

    // Méthodes métier (validation, calculs, etc.)
    bool estValide() const;
    QStringList obtenirProblemes() const;
    bool estEnRetard() const;
    int joursAvantLivraison() const;

private:
    int id_commande;
    QString reference;
    QString type;                    // "cmd" ou "vente"
    QDate date_creation;
    QDate date_livraison_prevue;
    QString statut;                  // "Saisie", "Encours", "Terminé", "Expédié"
    QString priorite;                // "Basse", "Normale", "Urgente"
    double montant;
    int quantite;                    // Quantité de produits
    QString etat_paiement;           // "Payée", "Non payée"
    QString service_vente;
    int id_employe;                  // ID de l'employé responsable
};

#endif // PRODUCTION_H
