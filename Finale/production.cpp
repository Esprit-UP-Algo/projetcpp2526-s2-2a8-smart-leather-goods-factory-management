#include "production.h"
#include <algorithm>

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

// ========== MÉTIERS BASIQUES : GESTION DES COMMANDES ==========

// a) Recherche et filtrage

// Recherche par date : filtre les productions entre deux dates
QList<ProductionData> ProductionData::rechercherParDate(const QList<ProductionData> &liste, 
                                                         const QDate &dateDebut, 
                                                         const QDate &dateFin)
{
    QList<ProductionData> resultat;
    
    for (const ProductionData &prod : liste) {
        if (prod.getDateDebut() >= dateDebut && prod.getDateDebut() <= dateFin) {
            resultat.append(prod);
        }
    }
    
    return resultat;
}

// Recherche par statut : filtre par statut (En cours, Terminé, En attente, etc.)
QList<ProductionData> ProductionData::rechercherParStatut(const QList<ProductionData> &liste, 
                                                           const QString &statut)
{
    QList<ProductionData> resultat;
    
    std::copy_if(liste.begin(), liste.end(), std::back_inserter(resultat),
                 [&statut](const ProductionData &prod) {
                     return prod.getStatut() == statut;
                 });
    
    return resultat;
}

// Recherche par priorité
QList<ProductionData> ProductionData::rechercherParPriorite(const QList<ProductionData> &liste,
                                                             const QString &priorite)
{
    QList<ProductionData> resultat;
    
    std::copy_if(liste.begin(), liste.end(), std::back_inserter(resultat),
                 [&priorite](const ProductionData &prod) {
                     return prod.getPriorite() == priorite;
                 });
    
    return resultat;
}

// Recherche par responsable
QList<ProductionData> ProductionData::rechercherParResponsable(const QList<ProductionData> &liste,
                                                                const QString &responsable)
{
    QList<ProductionData> resultat;
    
    std::copy_if(liste.begin(), liste.end(), std::back_inserter(resultat),
                 [&responsable](const ProductionData &prod) {
                     return prod.getResponsable() == responsable;
                 });
    
    return resultat;
}

// b) Suivi des délais et priorités

// Obtenir les productions urgentes (priorité haute et non terminées)
QList<ProductionData> ProductionData::obtenirProductionsUrgentes(const QList<ProductionData> &liste)
{
    QList<ProductionData> resultat;
    
    std::copy_if(liste.begin(), liste.end(), std::back_inserter(resultat),
                 [](const ProductionData &prod) {
                     return (prod.getPriorite() == "Haute" || prod.getPriorite() == "Urgent") 
                            && prod.getStatut() != "Terminé";
                 });
    
    return resultat;
}

// Obtenir les productions en retard
QList<ProductionData> ProductionData::obtenirProductionsEnRetard(const QList<ProductionData> &liste)
{
    QList<ProductionData> resultat;
    
    std::copy_if(liste.begin(), liste.end(), std::back_inserter(resultat),
                 [](const ProductionData &prod) {
                     return prod.estEnRetard();
                 });
    
    return resultat;
}

// Obtenir les productions à risque (échéance proche)
QList<ProductionData> ProductionData::obtenirProductionsARisque(const QList<ProductionData> &liste, 
                                                                 int joursAlerte)
{
    QList<ProductionData> resultat;
    
    std::copy_if(liste.begin(), liste.end(), std::back_inserter(resultat),
                 [joursAlerte](const ProductionData &prod) {
                     return prod.estARisque(joursAlerte);
                 });
    
    return resultat;
}

// Compter les productions par statut
int ProductionData::compterProductionsParStatut(const QList<ProductionData> &liste, 
                                                 const QString &statut)
{
    return std::count_if(liste.begin(), liste.end(),
                         [&statut](const ProductionData &prod) {
                             return prod.getStatut() == statut;
                         });
}

// c) Analyse commerciale / Reporting

// Calcul de la quantité totale produite (par défaut pour statut "Terminé")
int ProductionData::calculerQuantiteTotale(const QList<ProductionData> &liste, 
                                            const QString &statut)
{
    int total = 0;
    
    for (const ProductionData &prod : liste) {
        if (statut.isEmpty() || prod.getStatut() == statut) {
            total += prod.getQuantite().toInt();
        }
    }
    
    return total;
}

// Statistiques des productions par produit
QMap<QString, int> ProductionData::statistiquesParProduit(const QList<ProductionData> &liste)
{
    QMap<QString, int> stats;
    
    for (const ProductionData &prod : liste) {
        QString produit = prod.getProduit();
        int quantite = prod.getQuantite().toInt();
        
        if (stats.contains(produit)) {
            stats[produit] += quantite;
        } else {
            stats[produit] = quantite;
        }
    }
    
    return stats;
}

// Statistiques par statut
QMap<QString, int> ProductionData::statistiquesParStatut(const QList<ProductionData> &liste)
{
    QMap<QString, int> stats;
    
    for (const ProductionData &prod : liste) {
        QString statut = prod.getStatut();
        
        if (stats.contains(statut)) {
            stats[statut]++;
        } else {
            stats[statut] = 1;
        }
    }
    
    return stats;
}

// Statistiques par responsable
QMap<QString, int> ProductionData::statistiquesParResponsable(const QList<ProductionData> &liste)
{
    QMap<QString, int> stats;
    
    for (const ProductionData &prod : liste) {
        QString responsable = prod.getResponsable();
        int quantite = prod.getQuantite().toInt();
        
        if (stats.contains(responsable)) {
            stats[responsable] += quantite;
        } else {
            stats[responsable] = quantite;
        }
    }
    
    return stats;
}

// Calculer le taux d'achèvement global (% de productions terminées)
double ProductionData::calculerTauxAchevementGlobal(const QList<ProductionData> &liste)
{
    if (liste.isEmpty()) return 0.0;
    
    int terminees = compterProductionsParStatut(liste, "Terminé");
    return (static_cast<double>(terminees) / liste.size()) * 100.0;
}

// Calculer le taux d'achèvement par produit
QMap<QString, double> ProductionData::calculerTauxAchevementParProduit(const QList<ProductionData> &liste)
{
    QMap<QString, double> taux;
    QMap<QString, int> total;
    QMap<QString, int> terminees;
    
    for (const ProductionData &prod : liste) {
        QString produit = prod.getProduit();
        
        if (total.contains(produit)) {
            total[produit]++;
        } else {
            total[produit] = 1;
            terminees[produit] = 0;
        }
        
        if (prod.getStatut() == "Terminé") {
            terminees[produit]++;
        }
    }
    
    for (auto it = total.begin(); it != total.end(); ++it) {
        QString produit = it.key();
        taux[produit] = (static_cast<double>(terminees[produit]) / it.value()) * 100.0;
    }
    
    return taux;
}

// d) Tri et organisation

// Tri par priorité (Haute > Moyenne > Basse)
void ProductionData::trierParPriorite(QList<ProductionData> &liste, bool decroissant)
{
    std::sort(liste.begin(), liste.end(), 
              [decroissant](const ProductionData &a, const ProductionData &b) {
                  int prioriteA = prioriteToInt(a.getPriorite());
                  int prioriteB = prioriteToInt(b.getPriorite());
                  return decroissant ? (prioriteA > prioriteB) : (prioriteA < prioriteB);
              });
}

// Tri par quantité
void ProductionData::trierParQuantite(QList<ProductionData> &liste, bool decroissant)
{
    std::sort(liste.begin(), liste.end(),
              [decroissant](const ProductionData &a, const ProductionData &b) {
                  int qteA = a.getQuantite().toInt();
                  int qteB = b.getQuantite().toInt();
                  return decroissant ? (qteA > qteB) : (qteA < qteB);
              });
}

// Tri par date de début
void ProductionData::trierParDateDebut(QList<ProductionData> &liste, bool decroissant)
{
    std::sort(liste.begin(), liste.end(),
              [decroissant](const ProductionData &a, const ProductionData &b) {
                  return decroissant ? (a.getDateDebut() > b.getDateDebut()) 
                                     : (a.getDateDebut() < b.getDateDebut());
              });
}

// Tri par date de fin (échéance)
void ProductionData::trierParDateFin(QList<ProductionData> &liste, bool decroissant)
{
    std::sort(liste.begin(), liste.end(),
              [decroissant](const ProductionData &a, const ProductionData &b) {
                  return decroissant ? (a.getDateFin() > b.getDateFin()) 
                                     : (a.getDateFin() < b.getDateFin());
              });
}

// ========== MÉTIERS INNOVANTS : VALEUR AJOUTÉE ==========

// a) Traçabilité / Suivi QR Code

// Génération de données basiques pour QR Code (ID encodé pour suivi rapide)
QString ProductionData::genererQRCodeData() const
{
    return QString("PROD:%1|REF:%2|STATUT:%3|DATE:%4")
        .arg(id)
        .arg(reference)
        .arg(statut)
        .arg(dateFin.toString("yyyy-MM-dd"));
}

// Génération de données complètes pour QR Code (toutes les infos)
QString ProductionData::genererQRCodeComplet() const
{
    return QString("ID:%1|REF:%2|PRODUIT:%3|QTE:%4|STATUT:%5|DEBUT:%6|FIN:%7|RESP:%8|PRIO:%9")
        .arg(id)
        .arg(reference)
        .arg(produit)
        .arg(quantite)
        .arg(statut)
        .arg(dateDebut.toString("yyyy-MM-dd"))
        .arg(dateFin.toString("yyyy-MM-dd"))
        .arg(responsable)
        .arg(priorite);
}

// b) Alerte prédictive / Retard

// Vérifier si la production est en retard
bool ProductionData::estEnRetard() const
{
    QDate aujourdhui = QDate::currentDate();
    return aujourdhui > dateFin && statut != "Terminé";
}

// Vérifier si la production est à risque (échéance proche)
bool ProductionData::estARisque(int joursAlerte) const
{
    if (statut == "Terminé") return false;
    
    int jours = joursAvantEcheance();
    return jours >= 0 && jours <= joursAlerte;
}

// Calculer les jours avant l'échéance (négatif si en retard)
int ProductionData::joursAvantEcheance() const
{
    QDate aujourdhui = QDate::currentDate();
    return aujourdhui.daysTo(dateFin);
}

// Obtenir une alerte de retard prédictive avec émojis
QString ProductionData::getAlerteRetard() const
{
    if (statut == "Terminé") {
        return "✅ Production terminée";
    }
    
    int jours = joursAvantEcheance();
    
    if (jours < 0) {
        return QString("⚠️ RETARD : %1 jour(s) de retard !").arg(-jours);
    } else if (jours == 0) {
        return "🔴 URGENT : Échéance aujourd'hui !";
    } else if (jours <= 3) {
        return QString("🟠 ATTENTION : %1 jour(s) restant(s)").arg(jours);
    } else if (jours <= 7) {
        return QString("🟡 À surveiller : %1 jour(s) restant(s)").arg(jours);
    } else {
        return QString("🟢 Dans les délais : %1 jour(s) restant(s)").arg(jours);
    }
}

// Obtenir le niveau d'urgence (pour tri ou affichage)
QString ProductionData::getNiveauUrgence() const
{
    if (statut == "Terminé") return "Terminé";
    
    int jours = joursAvantEcheance();
    
    if (jours < 0) return "Retard";
    if (jours == 0) return "Urgent";
    if (jours <= 3) return "Critique";
    if (jours <= 7) return "Attention";
    return "Normal";
}

// c) Validation et contrôle qualité

// Vérifier si la production est valide (toutes les données sont correctes)
bool ProductionData::estValide() const
{
    if (id.isEmpty() || reference.isEmpty()) return false;
    if (produit.isEmpty()) return false;
    if (quantite.toInt() <= 0) return false;
    if (statut.isEmpty()) return false;
    if (!dateDebut.isValid() || !dateFin.isValid()) return false;
    if (dateDebut > dateFin) return false;
    if (responsable.isEmpty()) return false;
    if (priorite.isEmpty()) return false;
    
    return true;
}

// Obtenir la liste des problèmes de validation
QStringList ProductionData::obtenirProblemes() const
{
    QStringList problemes;
    
    if (id.isEmpty()) problemes << "ID manquant";
    if (reference.isEmpty()) problemes << "Référence manquante";
    if (produit.isEmpty()) problemes << "Produit manquant";
    if (quantite.toInt() <= 0) problemes << "Quantité invalide";
    if (statut.isEmpty()) problemes << "Statut manquant";
    if (!dateDebut.isValid()) problemes << "Date de début invalide";
    if (!dateFin.isValid()) problemes << "Date de fin invalide";
    if (dateDebut.isValid() && dateFin.isValid() && dateDebut > dateFin) 
        problemes << "Date de début postérieure à la date de fin";
    if (responsable.isEmpty()) problemes << "Responsable manquant";
    if (priorite.isEmpty()) problemes << "Priorité manquante";
    
    return problemes;
}

// Helper : convertir priorité en valeur numérique pour tri
int ProductionData::prioriteToInt(const QString &priorite)
{
    if (priorite == "Haute" || priorite == "Urgent") return 3;
    if (priorite == "Moyenne" || priorite == "Normal") return 2;
    if (priorite == "Basse" || priorite == "Faible") return 1;
    return 0;
}

// ========== CRUD : OPÉRATIONS BASE DE DONNÉES ==========

// CREATE : Ajouter une nouvelle production
bool ProductionData::ajouter()
{
    QSqlQuery query(QSqlDatabase::database("ProductionConnection"));
    
    query.prepare("INSERT INTO PRODUCTION (ID, REFERENCE, PRODUIT, QUANTITE, STATUT, "
                  "DATE_DEBUT, DATE_FIN, RESPONSABLE, PRIORITE) "
                  "VALUES (:id, :reference, :produit, :quantite, :statut, "
                  ":dateDebut, :dateFin, :responsable, :priorite)");
    
    query.bindValue(":id", id);
    query.bindValue(":reference", reference);
    query.bindValue(":produit", produit);
    query.bindValue(":quantite", quantite);
    query.bindValue(":statut", statut);
    query.bindValue(":dateDebut", dateDebut);
    query.bindValue(":dateFin", dateFin);
    query.bindValue(":responsable", responsable);
    query.bindValue(":priorite", priorite);
    
    if (!query.exec()) {
        qDebug() << "Erreur lors de l'ajout de la production:" << query.lastError().text();
        return false;
    }
    
    return true;
}

// READ : Afficher toutes les productions
QList<ProductionData> ProductionData::afficher()
{
    QList<ProductionData> liste;
    QSqlQuery query(QSqlDatabase::database("ProductionConnection"));
    
    query.prepare("SELECT ID, REFERENCE, PRODUIT, QUANTITE, STATUT, "
                  "DATE_DEBUT, DATE_FIN, RESPONSABLE, PRIORITE FROM PRODUCTION");
    
    if (!query.exec()) {
        qDebug() << "Erreur lors de la récupération des productions:" << query.lastError().text();
        return liste;
    }
    
    while (query.next()) {
        ProductionData prod(
            query.value(0).toString(),  // ID
            query.value(1).toString(),  // REFERENCE
            query.value(2).toString(),  // PRODUIT
            query.value(3).toString(),  // QUANTITE
            query.value(4).toString(),  // STATUT
            query.value(5).toDate(),    // DATE_DEBUT
            query.value(6).toDate(),    // DATE_FIN
            query.value(7).toString(),  // RESPONSABLE
            query.value(8).toString()   // PRIORITE
        );
        liste.append(prod);
    }
    
    return liste;
}

// READ : Rechercher une production par ID
ProductionData ProductionData::rechercherParId(const QString &id)
{
    QSqlQuery query(QSqlDatabase::database("ProductionConnection"));
    
    query.prepare("SELECT ID, REFERENCE, PRODUIT, QUANTITE, STATUT, "
                  "DATE_DEBUT, DATE_FIN, RESPONSABLE, PRIORITE FROM PRODUCTION "
                  "WHERE ID = :id");
    query.bindValue(":id", id);
    
    if (query.exec() && query.next()) {
        return ProductionData(
            query.value(0).toString(),  // ID
            query.value(1).toString(),  // REFERENCE
            query.value(2).toString(),  // PRODUIT
            query.value(3).toString(),  // QUANTITE
            query.value(4).toString(),  // STATUT
            query.value(5).toDate(),    // DATE_DEBUT
            query.value(6).toDate(),    // DATE_FIN
            query.value(7).toString(),  // RESPONSABLE
            query.value(8).toString()   // PRIORITE
        );
    }
    
    qDebug() << "Production non trouvée ou erreur:" << query.lastError().text();
    return ProductionData();
}

// UPDATE : Modifier une production existante
bool ProductionData::modifier()
{
    QSqlQuery query(QSqlDatabase::database("ProductionConnection"));
    
    query.prepare("UPDATE PRODUCTION SET REFERENCE = :reference, PRODUIT = :produit, "
                  "QUANTITE = :quantite, STATUT = :statut, DATE_DEBUT = :dateDebut, "
                  "DATE_FIN = :dateFin, RESPONSABLE = :responsable, PRIORITE = :priorite "
                  "WHERE ID = :id");
    
    query.bindValue(":id", id);
    query.bindValue(":reference", reference);
    query.bindValue(":produit", produit);
    query.bindValue(":quantite", quantite);
    query.bindValue(":statut", statut);
    query.bindValue(":dateDebut", dateDebut);
    query.bindValue(":dateFin", dateFin);
    query.bindValue(":responsable", responsable);
    query.bindValue(":priorite", priorite);
    
    if (!query.exec()) {
        qDebug() << "Erreur lors de la modification de la production:" << query.lastError().text();
        return false;
    }
    
    return true;
}

// DELETE : Supprimer une production
bool ProductionData::supprimer()
{
    QSqlQuery query(QSqlDatabase::database("ProductionConnection"));
    
    query.prepare("DELETE FROM PRODUCTION WHERE ID = :id");
    query.bindValue(":id", id);
    
    if (!query.exec()) {
        qDebug() << "Erreur lors de la suppression de la production:" << query.lastError().text();
        return false;
    }
    
    return true;
}
