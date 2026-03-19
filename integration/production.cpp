#include "production.h"
#include <QDate>

// ═══════════════════════════════════════════════════════════════════════════
// IMPLÉMENTATION : Production (Classe Métier Pure)
// ═══════════════════════════════════════════════════════════════════════════

Production::Production()
    : id_commande(0)
    , montant(0.0)
    , quantite(1)
    , id_employe(0)  // Pas d'employé par défaut
{
}

Production::Production(int id, const QString &ref, const QString &t,
                       const QDate &dc, const QDate &dlp,
                       const QString &st, const QString &pr,
                       double mt, const QString &ep,
                       const QString &sv)
    : id_commande(id)
    , reference(ref)
    , type(t)
    , date_creation(dc)
    , date_livraison_prevue(dlp)
    , statut(st)
    , priorite(pr)
    , montant(mt)
    , etat_paiement(ep)
    , service_vente(sv)
{
}

// ═══════════════════════════════════════════════════════════════════════════
// MÉTHODES MÉTIER : Validation et Logique
// ═══════════════════════════════════════════════════════════════════════════

bool Production::estValide() const
{
    return !reference.isEmpty()
        && (type == "cmd" || type == "vente")
        && date_creation.isValid()
        && date_livraison_prevue.isValid()
        && date_livraison_prevue >= date_creation
        && montant >= 0
        && (statut == "Saisie" || statut == "Encours" || statut == "Terminé" || statut == "Expédié")
        && (priorite == "Basse" || priorite == "Normale" || priorite == "Urgente")
        && (etat_paiement == "Payée" || etat_paiement == "Non payée")
        && !service_vente.isEmpty();
}

QStringList Production::obtenirProblemes() const
{
    QStringList problemes;
    
    if (reference.isEmpty())
        problemes << "La référence est obligatoire";
    
    if (type != "cmd" && type != "vente")
        problemes << "Le type doit être 'cmd' ou 'vente'";
    
    if (!date_creation.isValid())
        problemes << "La date de création est invalide";
    
    if (!date_livraison_prevue.isValid())
        problemes << "La date de livraison prévue est invalide";
    
    if (date_creation.isValid() && date_livraison_prevue.isValid() 
        && date_livraison_prevue < date_creation)
        problemes << "La date de livraison doit être >= à la date de création";
    
    if (montant < 0)
        problemes << "Le montant doit être >= 0";
    
    if (statut != "Saisie" && statut != "Encours" && statut != "Terminé" && statut != "Expédié")
        problemes << "Statut invalide (doit être: Saisie, Encours, Terminé ou Expédié)";
    
    if (priorite != "Basse" && priorite != "Normale" && priorite != "Urgente")
        problemes << "Priorité invalide (doit être: Basse, Normale ou Urgente)";
    
    if (etat_paiement != "Payée" && etat_paiement != "Non payée")
        problemes << "État paiement invalide (doit être: Payée ou Non payée)";
    
    if (service_vente.isEmpty())
        problemes << "Le service vente est obligatoire";
    
    return problemes;
}

bool Production::estEnRetard() const
{
    return date_livraison_prevue.isValid() 
        && QDate::currentDate() > date_livraison_prevue
        && statut != "Terminé" 
        && statut != "Expédié";
}

int Production::joursAvantLivraison() const
{
    if (!date_livraison_prevue.isValid())
        return 0;
    
    return QDate::currentDate().daysTo(date_livraison_prevue);
}
