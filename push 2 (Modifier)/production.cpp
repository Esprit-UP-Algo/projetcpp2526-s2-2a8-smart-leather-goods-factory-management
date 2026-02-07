#include "production.h"

ProductionData::ProductionData()
{
}

ProductionData::ProductionData(const QString &id, const QString &reference, const QString &produit,
                         const QString &quantite, const QString &statut, const QDate &dateDebut,
                         const QDate &dateFin, const QString &responsable, const QString &priorite)
    : id(id)
    , reference(reference)
    , produit(produit)
    , quantite(quantite)
    , statut(statut)
    , dateDebut(dateDebut)
    , dateFin(dateFin)
    , responsable(responsable)
    , priorite(priorite)
{
}
