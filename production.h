#ifndef PRODUCTION_H
#define PRODUCTION_H

#include <QString>
#include <QDate>
#include <QStringList>
#include <QList>
#include <QSqlQueryModel>

/**
 * @class Production
 * @brief Entité métier représentant une commande/production en base de données.
 *
 * Correspond à une ligne de la table COMMANDES.
 * Contient les données brutes ainsi que des méthodes de validation et de calcul.
 *
 * Types possibles : "cmd" (commande client) ou "vente" (vente directe).
 * Statuts possibles : "Saisie", "Encours", "Terminé", "Expédié".
 */
class Production
{
public:
    /** @brief Constructeur par défaut — initialise les valeurs numériques à 0. */
    Production();

    /**
     * @brief Constructeur complet avec tous les champs principaux.
     * @note quantite et id_employe ne sont pas dans ce constructeur,
     *       utiliser les setters correspondants après construction.
     */
    Production(int id, const QString &reference, const QString &type,
               const QDate &dateCreation, const QDate &dateLivraisonPrevue,
               const QString &statut, const QString &priorite,
               double montant, const QString &etatPaiement,
               const QString &serviceVente);

    // ── Getters ──────────────────────────────────────────────────────────────
    int     getIdCommande()        const { return id_commande; }
    QString getReference()         const { return reference; }
    QString getType()              const { return type; }           ///< "cmd" ou "vente"
    QDate   getDateCreation()      const { return date_creation; }
    QDate   getDateLivraisonPrevue()const{ return date_livraison_prevue; }
    QString getStatut()            const { return statut; }
    QString getPriorite()          const { return priorite; }
    double  getMontant()           const { return montant; }
    int     getQuantite()          const { return quantite; }
    QString getEtatPaiement()      const { return etat_paiement; }  ///< "Payée" ou "Non payée"
    QString getServiceVente()      const { return service_vente; }
    int     getIdEmploye()         const { return id_employe; }

    // ── Setters ──────────────────────────────────────────────────────────────
    void setIdCommande(int value)              { id_commande = value; }
    void setReference(const QString &value)    { reference = value; }
    void setType(const QString &value)         { type = value; }
    void setDateCreation(const QDate &value)   { date_creation = value; }
    void setDateLivraisonPrevue(const QDate &value) { date_livraison_prevue = value; }
    void setStatut(const QString &value)       { statut = value; }
    void setPriorite(const QString &value)     { priorite = value; }
    void setMontant(double value)              { montant = value; }
    void setQuantite(int value)                { quantite = value; }
    void setEtatPaiement(const QString &value) { etat_paiement = value; }
    void setServiceVente(const QString &value) { service_vente = value; }
    void setIdEmploye(int value)               { id_employe = value; }
    void setMailClient(const QString &value)   { mail_client = value; }
    QString getMailClient()                    const { return mail_client; }

    // ── Méthodes métier ───────────────────────────────────────────────────────

    /** @brief Retourne true si tous les champs obligatoires sont valides. */
    bool estValide() const;

    /**
     * @brief Retourne la liste des problèmes de validation.
     * @return Liste vide si la production est valide.
     */
    QStringList obtenirProblemes() const;

    /**
     * @brief Retourne true si la date de livraison est dépassée et la commande non terminée.
     */
    bool estEnRetard() const;

    /**
     * @brief Retourne le nombre de jours restants avant la date de livraison.
     * @return Valeur négative si la date est dépassée, 0 si la date est invalide.
     */
    int joursAvantLivraison() const;

private:
    int     id_commande;
    QString reference;
    QString type;                  ///< "cmd" ou "vente"
    QDate   date_creation;
    QDate   date_livraison_prevue;
    QString statut;                ///< "Saisie", "Encours", "Terminé", "Expédié"
    QString priorite;              ///< "Basse", "Normale", "Urgente"
    double  montant;
    int     quantite;
    QString etat_paiement;         ///< "Payée", "Non payée"
    QString service_vente;
    int     id_employe;            ///< Clé étrangère vers la table EMPLOYES
    QString mail_client;           ///< Email du client associé à la commande
};

// ─────────────────────────────────────────────────────────────────────────────

/**
 * @class ProductionCommande
 * @brief Structure de données enrichie pour l'affichage dans la vue production.
 *
 * Agrège les données de la table COMMANDES avec des informations de planification,
 * de suivi et de livraison. Utilisée par ProductionViewModel pour alimenter le tableau.
 *
 * Contient aussi la logique de calcul des alertes de retard.
 */
class ProductionCommande
{
public:
    /**
     * @brief Niveaux d'alerte de retard calculés automatiquement.
     * - Aucune : dans les délais
     * - Risque : proche de la date limite avec avancement insuffisant
     * - Retard : date de livraison dépassée
     */
    enum AlerteRetard { Aucune, Risque, Retard };

    /** @brief Constructeur — initialise tous les numériques à 0/false. */
    ProductionCommande();

    // ── Identification ────────────────────────────────────────────────────────
    int     idCommande;
    QString reference;
    QString priorite;
    QDate   dateLivraisonPrevue;
    QString employe;       ///< Nom complet de l'employé responsable
    QString type;          ///< "cmd" ou "vente"
    int     quantite;
    QDate   dateCreation;
    double  montant;

    // ── Planification (données prévues) ───────────────────────────────────────
    QDate   dateDebutPrevue;
    QDate   dateFinPrevue;
    QString atelier;
    int     ordrePassage;  ///< Ordre de passage dans l'atelier

    // ── Suivi de production (données réelles) ─────────────────────────────────
    QString etatProduction; ///< "Planifié", "En cours", "Bloqué", "Terminé"
    QString etapeActuelle;
    int     avancement;     ///< Pourcentage d'avancement (0-100)
    bool    retard;

    // ── Livraison ─────────────────────────────────────────────────────────────
    QString societeLivraison;
    QString numeroSuiviColis;
    QDate   dateExpeditionPrevue;
    QDate   dateExpeditionReelle;
    QString statutLivraison; ///< "Non expédiée", "En livraison", "Livrée"

    AlerteRetard alerteRetard; ///< Calculé par calculerAlerteRetard()

    // ── Accesseurs utilitaires ────────────────────────────────────────────────
    QString getRetardText()     const { return retard ? "Oui" : "Non"; }
    QString getAvancementText() const { return QString::number(avancement) + " %"; }

    /**
     * @brief Retourne le nombre de jours de retard par rapport à dateFinPrevue.
     * @return 0 si pas en retard ou date invalide.
     */
    int getJoursRetard() const;

    // ── Logique d'alerte ──────────────────────────────────────────────────────

    /**
     * @brief Calcule le niveau d'alerte en fonction des dates et de l'avancement.
     * @param joursAlerte      Seuil en jours avant la date limite pour déclencher "Risque".
     * @param seuilAvancement  Pourcentage d'avancement minimum attendu.
     * @return Niveau d'alerte calculé.
     */
    AlerteRetard calculerAlerteRetard(int joursAlerte = 3, int seuilAvancement = 50) const;

    /** @brief Retourne le texte de l'alerte avec emoji (ex: "⚠ RETARD"). */
    QString getAlerteRetardText() const;

    /** @brief Retourne la couleur hexadécimale associée au niveau d'alerte. */
    QString getAlerteRetardColor() const;

    // ── Validation métier ─────────────────────────────────────────────────────

    /** @brief Vérifie que dateDebutPrevue ≤ dateFinPrevue et ordrePassage > 0. */
    bool validerPlanification() const;

    /**
     * @brief Vérifie la cohérence du suivi :
     *  - avancement ∈ [0, 100]
     *  - si "Terminé" → avancement = 100
     *  - si "Planifié" → avancement = 0
     */
    bool validerSuivi() const;

    /**
     * @brief Génère un message de notification formaté selon le niveau d'alerte.
     * @return Texte multi-lignes prêt à afficher dans une QMessageBox.
     */
    QString genererMessageNotification() const;
};

// ─────────────────────────────────────────────────────────────────────────────

/**
 * @class ProductionDAO
 * @brief Couche d'accès aux données (DAO) pour la table COMMANDES.
 *
 * Toutes les requêtes SQL passent par cette classe avec prepare()/bindValue()
 * pour éviter les injections SQL.
 *
 * Utilise le singleton Connection pour obtenir la connexion à la base de données.
 */
class ProductionDAO
{
public:
    ProductionDAO();

    // ── CRUD ─────────────────────────────────────────────────────────────────

    /** @brief INSERT une nouvelle commande. @return true si succès. */
    bool ajouter(const Production &production);

    /** @brief UPDATE une commande existante par son ID. @return true si succès. */
    bool modifier(const Production &production);

    /** @brief DELETE une commande par son ID. @return true si succès. */
    bool supprimer(int idCommande);

    // ── Requêtes SELECT ───────────────────────────────────────────────────────

    /**
     * @brief Retourne toutes les commandes avec jointure EMPLOYES, triées par date DESC.
     * @return Modèle prêt à être assigné à un QTableView (nullptr si erreur).
     */
    QSqlQueryModel* afficher();

    /**
     * @brief Recherche multi-champs avec LIKE insensible à la casse.
     * @param terme Texte recherché (référence, type, statut, priorité, nom employé).
     */
    QSqlQueryModel* rechercher(const QString &terme);

    /**
     * @brief Retourne toutes les commandes triées par une colonne donnée.
     * @param colonne  Nom de colonne SQL (validé contre une liste blanche).
     * @param croissant true = ASC, false = DESC.
     */
    QSqlQueryModel* trierPar(const QString &colonne, bool croissant = true);

    /** @brief Filtre les commandes par statut exact. */
    QSqlQueryModel* filtrerParStatut(const QString &statut);

    /** @brief Filtre les commandes par priorité exacte. */
    QSqlQueryModel* filtrerParPriorite(const QString &priorite);

    /**
     * @brief Retourne un objet Production complet pour un ID donné.
     * @return Production par défaut (id=0) si non trouvée.
     */
    Production obtenirParId(int idCommande);

    /** @brief Retourne toutes les commandes sous forme de liste d'objets Production. */
    QList<Production> obtenirTout();

private:
    /**
     * @brief Configure les libellés des colonnes du modèle (en-têtes français).
     * @param model Modèle à configurer (doit être non-null).
     */
    void configurerEntetes(QSqlQueryModel* model);
};

#endif // PRODUCTION_H
