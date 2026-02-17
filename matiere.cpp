#include "matiere.h"

Matiere::Matiere() {}

Matiere::Matiere(const QString &module, const QString &reference, const QString &type,
                 const QString &quantite, const QString &seuil, const QDate &dateExp)
    : m_module(module), m_reference(reference), m_type(type),
      m_quantite(quantite), m_seuil(seuil), m_dateExpiration(dateExp) {}