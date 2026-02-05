#include "fournisseur.h"

FournisseurData::FournisseurData()
{
}

FournisseurData::FournisseurData(const QString &id, const QString &nomEntreprise, const QString &email,
                         const QString &telephone, const QString &matriculeFiscal,
                         const QString &typeProduit, const QString &conditionPaiement,
                         const QString &statut)
    : id(id)
    , nomEntreprise(nomEntreprise)
    , email(email)
    , telephone(telephone)
    , matriculeFiscal(matriculeFiscal)
    , typeProduit(typeProduit)
    , conditionPaiement(conditionPaiement)
    , statut(statut)
{
}
