#include "production.h"
#include "connection.h"
#include <QDate>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVariant>

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

// ── Validation : retourne true si tous les champs obligatoires sont corrects ──
bool Production::estValide() const
{
    return !reference.isEmpty()
        && (type == "cmd" || type == "vente")
        && date_creation.isValid()
        && date_livraison_prevue.isValid()
        && date_livraison_prevue >= date_creation
        && montant >= 0
        && (statut == "En Attente" || statut == "Planifié" || statut == "En Cours"
            || statut == "En Production" || statut == "Suspendu" || statut == "Terminé" || statut == "Annulé")
        && (priorite == "Basse" || priorite == "Normale" || priorite == "Urgente")
        && (etat_paiement == "Payée" || etat_paiement == "Non payée")
        && !service_vente.isEmpty();
}

// ── Retourne la liste détaillée des erreurs de validation ────────────────────
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
    
    if (statut != "En Attente" && statut != "Planifié" && statut != "En Cours"
        && statut != "En Production" && statut != "Suspendu" && statut != "Terminé" && statut != "Annulé")
        problemes << "Statut invalide (doit être: En Attente, Planifié, En Cours, En Production, Suspendu, Terminé ou Annulé)";
    
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

// ═══════════════════════════════════════════════════════════════════════════
// IMPLÉMENTATION: ProductionCommande
// ═══════════════════════════════════════════════════════════════════════════

ProductionCommande::ProductionCommande()
    : idCommande(0), quantite(0), montant(0.0)
    , ordrePassage(0), avancement(0), retard(false)
    , alerteRetard(Aucune)
{}

int ProductionCommande::getJoursRetard() const
{
    if (!retard || !dateFinPrevue.isValid()) return 0;
    return dateFinPrevue.daysTo(QDate::currentDate());
}

// ── Calcul de l'alerte : compare la date du jour aux dates prévues ────────────
ProductionCommande::AlerteRetard ProductionCommande::calculerAlerteRetard(int joursAlerte, int seuilAvancement) const
{
    if (etatProduction == "Terminé" || !dateLivraisonPrevue.isValid()) return Aucune;

    QDate aujourdhui = QDate::currentDate();
    int joursRestants = aujourdhui.daysTo(dateLivraisonPrevue);

    if (joursRestants < 0) return Retard;

    if (joursRestants <= joursAlerte) {
        if (dateDebutPrevue.isValid() && dateFinPrevue.isValid()) {
            int dureeTotal = dateDebutPrevue.daysTo(dateFinPrevue);
            int joursEcoules = dateDebutPrevue.daysTo(aujourdhui);
    // Comparer l'avancement réel à l'avancement théorique attendu à cette date
            if (dureeTotal > 0 && joursEcoules > 0) {
                int avancementAttendu = (joursEcoules * 100) / dureeTotal;
                if (avancement < seuilAvancement && avancement < avancementAttendu) return Risque;
            }
        }
        if (avancement < seuilAvancement) return Risque;
    }
    return Aucune;
}

QString ProductionCommande::getAlerteRetardText() const
{
    switch (alerteRetard) {
    case Retard: return "⚠ RETARD";
    case Risque: return "⚡ RISQUE";
    default:     return "✓ OK";
    }
}

QString ProductionCommande::getAlerteRetardColor() const
{
    switch (alerteRetard) {
    case Retard: return "#E74C3C";
    case Risque: return "#F39C12";
    default:     return "#27AE60";
    }
}

bool ProductionCommande::validerPlanification() const
{
    if (dateDebutPrevue.isValid() && dateFinPrevue.isValid() && dateDebutPrevue > dateFinPrevue)
        return false;
    return ordrePassage > 0;
}

bool ProductionCommande::validerSuivi() const
{
    if (avancement < 0 || avancement > 100) return false;
    if (etatProduction == "Terminé" && avancement != 100) return false;
    if (etatProduction == "Planifié" && avancement != 0) return false;
    return true;
}

QString ProductionCommande::genererMessageNotification() const
{
    switch (alerteRetard) {
    case Retard:
        return QString("⚠ RETARD - Commande %1\nDate prévue: %2\nRetard: %3 jour(s)\nAvancement: %4%\nÉtat: %5")
            .arg(reference, dateLivraisonPrevue.toString("dd/MM/yyyy"))
            .arg(-QDate::currentDate().daysTo(dateLivraisonPrevue))
            .arg(avancement).arg(etatProduction);
    case Risque:
        return QString("⚡ RISQUE - Commande %1\nDate prévue: %2\nJours restants: %3\nAvancement: %4%\nÉtat: %5")
            .arg(reference, dateLivraisonPrevue.toString("dd/MM/yyyy"))
            .arg(QDate::currentDate().daysTo(dateLivraisonPrevue))
            .arg(avancement).arg(etatProduction);
    default:
        return QString("✓ Commande %1 - Dans les délais").arg(reference);
    }
}


// ═══════════════════════════════════════════════════════════════════════════
// IMPLÉMENTATION : ProductionDAO
// ═══════════════════════════════════════════════════════════════════════════




ProductionDAO::ProductionDAO() {}

// ── INSERT : utilise prepare()/bindValue() pour éviter les injections SQL ────
bool ProductionDAO::ajouter(const Production &production)
{
    QSqlDatabase db = Connection::instance()->getDatabase();
    if (!db.isOpen()) { qDebug() << "❌ Base de données non connectée!"; return false; }

    QSqlQuery query(db);
    query.prepare("INSERT INTO COMMANDES "
                  "(REFERENCE, PRODUIT, DATE_CREATION, DATE_LIVRAISON, "
                  "STATUT, PRIORITE, MONTANT, QUANTITE, ETAT, ID_EMPLOYE, MAIL_CLIENT) "
                  "VALUES (:reference, :produit, :dateCreation, :dateLivraison, "
                  ":statut, :priorite, :montant, :quantite, :etat, :idEmploye, :mailClient)");
    query.bindValue(":reference", production.getReference());
    query.bindValue(":produit",   production.getType());
    query.bindValue(":dateCreation",  production.getDateCreation());
    query.bindValue(":dateLivraison", production.getDateLivraisonPrevue());
    query.bindValue(":statut",    production.getStatut());
    query.bindValue(":priorite",  production.getPriorite());
    query.bindValue(":montant",   production.getMontant());
    query.bindValue(":quantite",  production.getQuantite());
    query.bindValue(":etat",      "En cours");
    query.bindValue(":idEmploye", production.getIdEmploye());
    query.bindValue(":mailClient", production.getMailClient());

    qDebug() << "Ajout commande - Ref:" << production.getReference() << "ID_EMPLOYE:" << production.getIdEmploye();
    if (!query.exec()) { qDebug() << "❌ Erreur ajout commande:" << query.lastError().text(); return false; }
    qDebug() << "✅ Commande ajoutée";
    return true;
}




bool ProductionDAO::modifier(const Production &production)
{
    QSqlDatabase db = Connection::instance()->getDatabase();
    if (!db.isOpen()) { qDebug() << "❌ Base de données non connectée!"; return false; }

    QSqlQuery query(db);
    query.prepare("UPDATE COMMANDES SET "
                  "REFERENCE=:reference, PRODUIT=:produit, DATE_CREATION=:dateCreation, "
                  "DATE_LIVRAISON=:dateLivraison, STATUT=:statut, PRIORITE=:priorite, "
                  "MONTANT=:montant, QUANTITE=:quantite, ETAT=:etat, MAIL_CLIENT=:mailClient, "
                  "ID_EMPLOYE=:idEmploye "
                  "WHERE ID_COMMANDE=:id");
    query.bindValue(":id",           production.getIdCommande());
    query.bindValue(":reference",    production.getReference());
    query.bindValue(":produit",      production.getType());
    query.bindValue(":dateCreation", production.getDateCreation());
    query.bindValue(":dateLivraison",production.getDateLivraisonPrevue());
    query.bindValue(":statut",       production.getStatut());
    query.bindValue(":priorite",     production.getPriorite());
    query.bindValue(":montant",      production.getMontant());
    query.bindValue(":quantite",     production.getQuantite());
    query.bindValue(":etat",         production.getEtatPaiement());
    query.bindValue(":mailClient",   production.getMailClient());
    query.bindValue(":idEmploye",    production.getIdEmploye());

    if (!query.exec()) { qDebug() << "❌ Erreur modification production:" << query.lastError().text(); return false; }
    db.commit();
    return true;
}

bool ProductionDAO::supprimer(int idCommande)
{
    QSqlDatabase db = Connection::instance()->getDatabase();
    if (!db.isOpen()) { qDebug() << "❌ Base de données non connectée!"; return false; }

    QSqlQuery query(db);
    query.prepare("DELETE FROM COMMANDES WHERE ID_COMMANDE = :id");
    query.bindValue(":id", idCommande);
    if (!query.exec()) { qDebug() << "❌ Erreur suppression production:" << query.lastError().text(); return false; }
    return true;
}

QSqlQueryModel* ProductionDAO::afficher()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    QString sql = "SELECT C.ID_COMMANDE, C.REFERENCE, (E.NOM || ' ' || E.PRENOM) AS EMPLOYE, "
                  "C.PRODUIT, C.DATE_CREATION, C.DATE_LIVRAISON, C.STATUT, C.PRIORITE, C.MONTANT, C.MAIL_CLIENT "
                  "FROM COMMANDES C LEFT JOIN EMPLOYES E ON C.ID_EMPLOYE = E.ID_EMPLOYE "
                  "ORDER BY C.DATE_CREATION DESC";
    model->setQuery(sql, Connection::instance()->getDatabase());
    if (model->lastError().isValid()) { delete model; return nullptr; }
    configurerEntetes(model);
    return model;
}

QSqlQueryModel* ProductionDAO::rechercher(const QString &terme)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlQuery query(Connection::instance()->getDatabase());
    query.prepare(
        "SELECT C.ID_COMMANDE, C.REFERENCE, (E.NOM || ' ' || E.PRENOM) AS EMPLOYE, "
        "C.PRODUIT, C.DATE_CREATION, C.DATE_LIVRAISON, C.STATUT, C.PRIORITE, C.MONTANT, C.MAIL_CLIENT "
        "FROM COMMANDES C LEFT JOIN EMPLOYES E ON C.ID_EMPLOYE = E.ID_EMPLOYE "
        "WHERE UPPER(C.REFERENCE)   LIKE UPPER(:t) "
        "OR UPPER(C.PRODUIT)        LIKE UPPER(:t) "
        "OR UPPER(C.STATUT)         LIKE UPPER(:t) "
        "OR UPPER(C.PRIORITE)       LIKE UPPER(:t) "
        "OR UPPER(C.MAIL_CLIENT)    LIKE UPPER(:t) "
        "OR UPPER(E.NOM)            LIKE UPPER(:t) "
        "OR UPPER(E.PRENOM)         LIKE UPPER(:t) "
        "ORDER BY C.DATE_CREATION DESC");
    query.bindValue(":t", "%" + terme + "%");
    if (!query.exec()) {
        qDebug() << "❌ Erreur recherche:" << query.lastError().text();
        delete model; return nullptr;
    }
    model->setQuery(std::move(query));
    configurerEntetes(model);
    return model;
}

// ── trierPar : la colonne est validée contre une liste blanche (anti-injection) ─
QSqlQueryModel* ProductionDAO::trierPar(const QString &colonne, bool croissant)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    QStringList colonnesAutorisees = {"ID_COMMANDE","REFERENCE","PRODUIT","DATE_CREATION","DATE_LIVRAISON_PREVUE","STATUT","PRIORITE","MONTANT"};
    QString col = colonnesAutorisees.contains(colonne.toUpper()) ? colonne.toUpper() : "DATE_CREATION";
    QString sql = QString("SELECT C.ID_COMMANDE, C.REFERENCE, (E.NOM || ' ' || E.PRENOM) AS EMPLOYE, "
                          "C.PRODUIT, C.DATE_CREATION, C.DATE_LIVRAISON, C.STATUT, C.PRIORITE, C.MONTANT "
                          "FROM COMMANDES C LEFT JOIN EMPLOYES E ON C.ID_EMPLOYE = E.ID_EMPLOYE "
                          "ORDER BY C.%1 %2").arg(col, croissant ? "ASC" : "DESC");
    model->setQuery(sql, Connection::instance()->getDatabase());
    if (model->lastError().isValid()) { qDebug() << "❌ Erreur tri:" << model->lastError().text(); delete model; return nullptr; }
    configurerEntetes(model);
    return model;
}

QSqlQueryModel* ProductionDAO::filtrerParStatut(const QString &statut)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlQuery query(Connection::instance()->getDatabase());
    query.prepare("SELECT C.ID_COMMANDE, C.REFERENCE, (E.NOM || ' ' || E.PRENOM) AS EMPLOYE, "
                  "C.PRODUIT, C.DATE_CREATION, C.DATE_LIVRAISON, C.STATUT, C.PRIORITE, C.MONTANT "
                  "FROM COMMANDES C LEFT JOIN EMPLOYES E ON C.ID_EMPLOYE = E.ID_EMPLOYE "
                  "WHERE C.STATUT = :statut ORDER BY C.DATE_CREATION DESC");
    query.bindValue(":statut", statut);
    if (!query.exec()) { qDebug() << "❌ Erreur filtre statut:" << query.lastError().text(); delete model; return nullptr; }
    model->setQuery(std::move(query));
    configurerEntetes(model);
    return model;
}

QSqlQueryModel* ProductionDAO::filtrerParPriorite(const QString &priorite)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlQuery query(Connection::instance()->getDatabase());
    query.prepare("SELECT C.ID_COMMANDE, C.REFERENCE, (E.NOM || ' ' || E.PRENOM) AS EMPLOYE, "
                  "C.PRODUIT, C.DATE_CREATION, C.DATE_LIVRAISON, C.STATUT, C.PRIORITE, C.MONTANT "
                  "FROM COMMANDES C LEFT JOIN EMPLOYES E ON C.ID_EMPLOYE = E.ID_EMPLOYE "
                  "WHERE C.PRIORITE = :priorite ORDER BY C.DATE_CREATION DESC");
    query.bindValue(":priorite", priorite);
    if (!query.exec()) { qDebug() << "❌ Erreur filtre priorité:" << query.lastError().text(); delete model; return nullptr; }
    model->setQuery(std::move(query));
    configurerEntetes(model);
    return model;
}

Production ProductionDAO::obtenirParId(int idCommande)
{
    Production production;
    QSqlQuery query(Connection::instance()->getDatabase());
    query.prepare("SELECT ID_COMMANDE, REFERENCE, PRODUIT, DATE_CREATION, DATE_LIVRAISON, "
                  "STATUT, PRIORITE, MONTANT, QUANTITE, ETAT FROM COMMANDES WHERE ID_COMMANDE = :id");
    query.bindValue(":id", idCommande);
    if (query.exec() && query.next()) {
        production.setIdCommande(query.value(0).toInt());
        production.setReference(query.value(1).toString());
        production.setType(query.value(2).toString());
        production.setDateCreation(query.value(3).toDate());
        production.setDateLivraisonPrevue(query.value(4).toDate());
        production.setStatut(query.value(5).toString());
        production.setPriorite(query.value(6).toString());
        production.setMontant(query.value(7).toDouble());
        production.setQuantite(query.value(8).toInt());
        production.setEtatPaiement(query.value(9).toString());
    } else {
        qDebug() << "❌ Production non trouvée:" << query.lastError().text();
    }
    return production;
}

QList<Production> ProductionDAO::obtenirTout()
{
    QList<Production> liste;
    QSqlQuery query(Connection::instance()->getDatabase());
    query.prepare("SELECT ID_COMMANDE, REFERENCE, PRODUIT, DATE_CREATION, DATE_LIVRAISON, "
                  "STATUT, PRIORITE, MONTANT, QUANTITE, ETAT FROM COMMANDES ORDER BY DATE_CREATION DESC");
    if (query.exec()) {
        while (query.next()) {
            Production prod(query.value(0).toInt(), query.value(1).toString(), query.value(2).toString(),
                            query.value(3).toDate(), query.value(4).toDate(), query.value(5).toString(),
                            query.value(6).toString(), query.value(7).toDouble(), query.value(9).toString(), "");
            prod.setQuantite(query.value(8).toInt());
            liste.append(prod);
        }
    } else {
        qDebug() << "❌ Erreur obtenir tout:" << query.lastError().text();
    }
    return liste;
}

void ProductionDAO::configurerEntetes(QSqlQueryModel* model)
{
    if (!model) return;
    // La requête SELECT retourne : 0=ID_COMMANDE, 1=REFERENCE, 2=EMPLOYE,
    // 3=TYPE, 4=DATE_CREATION, 5=DATE_LIVRAISON, 6=STATUT, 7=PRIORITE, 8=MONTANT
    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Référence");
    model->setHeaderData(2, Qt::Horizontal, "Employé");
    model->setHeaderData(3, Qt::Horizontal, "Produit");
    model->setHeaderData(4, Qt::Horizontal, "Date Création");
    model->setHeaderData(5, Qt::Horizontal, "Date Livraison");
    model->setHeaderData(6, Qt::Horizontal, "Statut");
    model->setHeaderData(7, Qt::Horizontal, "Priorité");
    model->setHeaderData(8, Qt::Horizontal, "Montant");
    model->setHeaderData(9, Qt::Horizontal, "Mail Client");
}



























