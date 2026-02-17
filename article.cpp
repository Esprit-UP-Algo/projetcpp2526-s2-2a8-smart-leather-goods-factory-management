#include "article.h"

Article::Article()
    : idProduit(0)
    , prixUnitaire(0.0)
    , coutFabrication(0.0)
    , statut("disponible")
    , dateCreation(QDate::currentDate())
{
}
