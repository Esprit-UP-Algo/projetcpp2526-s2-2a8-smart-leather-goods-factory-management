#ifndef PRODUCTIONVIEW_H
#define PRODUCTIONVIEW_H

#include <QMainWindow>
#include <QDialog>
#include <QTableView>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QDateEdit>
#include <QDoubleSpinBox>
#include <QSortFilterProxyModel>
#include <QAbstractTableModel>
#include <QList>
#include <QObject>
#include <QWidget>
#include <QTimer>
#include <QSet>
#include <QMap>
#include "production.h"
#include "notification.h"

// ─────────────────────────────────────────────────────────────────────────────

/**
 * @class ProductionDialog
 * @brief Dialogue CRUD pour créer, modifier ou supprimer une commande.
 *
 * Trois modes disponibles via l'enum DialogMode :
 *  - AddMode    : formulaire vide, référence auto-générée (CMD-YYYY-NNNN)
 *  - EditMode   : formulaire pré-rempli, bouton "Mettre à Jour"
 *  - DeleteMode : formulaire en lecture seule, confirmation de suppression
 *
 * La liste des employés est chargée dynamiquement depuis la table EMPLOYES.
 * L'ID de l'employé sélectionné est accessible via getEmployeId().
 */
class ProductionDialog : public QDialog
{
    Q_OBJECT
public:
    /** @brief Mode d'ouverture du dialogue. */
    enum DialogMode { AddMode, EditMode, DeleteMode };

    /**
     * @brief Constructeur — construit l'UI selon le mode et charge les employés.
     * @param mode Détermine le titre, les champs actifs et les boutons visibles.
     */
    explicit ProductionDialog(QWidget *parent = nullptr, DialogMode mode = AddMode);
    ~ProductionDialog();

    /**
     * @brief Pré-remplit le formulaire avec les données d'une commande existante.
     * Utilisé en mode EditMode et DeleteMode.
     */
    void setProductionData(const QString &id, const QString &reference, const QString &produit,
                           const QString &quantite, const QString &statut, const QString &dateDebut,
                           const QString &dateFin, const QString &responsable, const QString &priorite,
                           const QString &mailClient = QString());

    // ── Getters des champs du formulaire ─────────────────────────────────────
    QString getId()          const;
    QString getReference()   const;
    QString getProduit()     const;
    QString getQuantite()    const; ///< Retourne le prix saisi (champ nommé "Prix")
    QString getStatut()      const;
    QString getDateDebut()   const; ///< Format "dd/MM/yyyy"
    QString getDateFin()     const; ///< Format "dd/MM/yyyy"
    QString getResponsable() const; ///< Nom complet de l'employé sélectionné
    QString getPriorite()    const;
    QString getMailClient()  const;

    /** @brief Retourne l'ID de l'employé sélectionné (stocké dans currentData du ComboBox). */
    int getEmployeId() const;

private slots:
    /** @brief Valide le formulaire et appelle accept() si tout est correct. */
    void onSaveClicked();

    /** @brief Demande confirmation puis appelle accept(). */
    void onDeleteConfirmed();

private:
    /** @brief Construit tous les widgets et connecte les signaux. */
    void setupUI();

    /**
     * @brief Génère une référence unique au format CMD-YYYY-NNNN
     * en interrogeant le MAX(ID_COMMANDE) de la table COMMANDES.
     */
    void generateAutoReference();

    /**
     * @brief Charge la liste des employés depuis EMPLOYES et remplit le ComboBox.
     * Essaie d'abord en minuscules puis en majuscules pour la compatibilité Oracle.
     */
    void loadEmployes();
    void loadClients();
    void loadArticles();

    DialogMode   m_mode;
    QLabel      *lblTitle, *lblDeleteWarning;
    QLineEdit   *txtId, *txtReference;
    QDoubleSpinBox *spnPrix;
    QComboBox   *cmbProduit, *cmbStatut, *cmbResponsable, *cmbPriorite, *cmbClient;
    QDateEdit   *dateDebut, *dateFin;
    QPushButton *btnSave, *btnCancel, *btnDelete;

    /** @brief Cache ID_EMPLOYE → Nom complet pour usage interne. */
    QMap<int, QString> m_employeMap;
};

// ─────────────────────────────────────────────────────────────────────────────

/**
 * @class ProductionViewModel
 * @brief Modèle de données Qt pour afficher les ProductionCommande dans un QTableView.
 *
 * Hérite de QAbstractTableModel pour un contrôle total sur l'affichage :
 *  - Coloration des cellules "Statut" selon l'état (vert/orange/rouge/bleu)
 *  - Alignement à droite pour la colonne Montant
 *  - Chargement direct depuis la base de données via loadFromDatabase()
 */
class ProductionViewModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    /** @brief Indices des colonnes du tableau. */
    enum Column {
        COL_REFERENCE = 0,
        COL_EMPLOYE,
        COL_TYPE,
        COL_QUANTITE,
        COL_DATE_CREATION,
        COL_STATUT,
        COL_PRIORITE,
        COL_MONTANT,
        COL_COUNT  ///< Nombre total de colonnes (sentinelle)
    };

    explicit ProductionViewModel(QObject *parent = nullptr);

    // ── Interface QAbstractTableModel ────────────────────────────────────────
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    /**
     * @brief Fournit les données pour chaque cellule.
     * Gère Qt::DisplayRole, Qt::BackgroundRole (couleur statut),
     * Qt::ForegroundRole et Qt::TextAlignmentRole.
     */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // ── Manipulation des données ──────────────────────────────────────────────
    /** @brief Remplace toutes les données et notifie la vue. */
    void setCommandes(const QList<ProductionCommande> &commandes);

    /** @brief Ajoute une commande en fin de liste. */
    void addCommande(const ProductionCommande &commande);

    /** @brief Met à jour une ligne existante et notifie la vue. */
    void updateCommande(int row, const ProductionCommande &commande);

    /** @brief Retourne la commande à la ligne donnée (objet vide si hors limites). */
    ProductionCommande getCommande(int row) const;

    /** @brief Vide le modèle et notifie la vue. */
    void clear();

    /**
     * @brief Recharge toutes les commandes depuis la base de données.
     * Requête : COMMANDES JOIN EMPLOYES, triées par DATE_CREATION DESC.
     */
    void loadFromDatabase();

private:
    QList<ProductionCommande> m_commandes; ///< Données en mémoire

    /** @brief Retourne le libellé français d'une colonne par son index. */
    QString getColumnName(int column) const;
};

// ─────────────────────────────────────────────────────────────────────────────

/**
 * @class ProductionView
 * @brief Fenêtre principale de gestion de la production.
 *
 * Affiche un tableau de toutes les commandes avec :
 *  - Recherche en temps réel (filtre sur la référence)
 *  - Filtres par état de production et statut de livraison
 *  - Bouton "Détails" : affiche toutes les informations d'une commande
 *  - Bouton "Planification" : permet de modifier les dates et l'atelier
 *  - Bouton "Actualiser" : recharge les données depuis la DB
 *
 * Un timer vérifie périodiquement les alertes de retard et affiche
 * des notifications pour les commandes en risque ou en retard.
 */
class ProductionView : public QMainWindow
{
    Q_OBJECT

public:
    explicit ProductionView(QWidget *parent = nullptr);
    ~ProductionView();

    /** @brief Recharge les données depuis la DB et reconfigure les largeurs de colonnes. */
    void loadData();

    // ── Configuration du système d'alertes ───────────────────────────────────

    /** @brief Nombre de jours avant la date limite pour déclencher l'alerte "Risque". */
    void setJoursAlerte(int jours)          { m_joursAlerte = jours; }

    /** @brief Pourcentage d'avancement minimum en dessous duquel "Risque" est déclenché. */
    void setSeuilAvancement(int seuil)      { m_seuilAvancement = seuil; }

    /** @brief Intervalle en minutes entre deux vérifications automatiques des alertes. */
    void setIntervalleVerification(int min) { m_intervalleVerification = min; }

    /** @brief Injecte le pipeline IA pour les notifications intelligentes. */
    void setNotificationPipeline(NotificationPipeline *pipeline) { m_pipeline = pipeline; }

private slots:
    /** @brief Filtre le tableau en temps réel sur la colonne Référence. */
    void onSearchTextChanged(const QString &text);

    /** @brief Filtre le tableau par état de production (ou réinitialise si index=0). */
    void onEtatFilterChanged(int index);

    /** @brief Filtre par statut de livraison (ou réinitialise si index=0). */
    void onStatutLivraisonFilterChanged(int index);

    /** @brief Ouvre un dialogue de détails complets pour la commande sélectionnée. */
    void onDetailsClicked();

    /** @brief Ouvre un dialogue de modification de planification pour la commande sélectionnée. */
    void onPlanificationClicked();

    /** @brief Recharge les données et affiche un message de confirmation. */
    void onRefreshClicked();

    /**
     * @brief Vérifie toutes les commandes et affiche une notification
     * pour chaque nouvelle alerte détectée (évite les doublons via m_alertesNotifiees).
     */
    void verifierAlertes();

private:
    /** @brief Construit tous les widgets (titre, filtres, tableau, légende). */
    void setupUI();

    /** @brief Connecte tous les signaux/slots des widgets. */
    void setupConnections();

    /**
     * @brief Configure le timer de vérification périodique des alertes.
     * L'intervalle est défini par m_intervalleVerification (en minutes).
     */
    void setupTimer();

    /**
     * @brief Affiche une QMessageBox stylisée pour une alerte de retard ou de risque.
     * @param commande Commande concernée par l'alerte.
     */
    void afficherNotification(const ProductionCommande &commande);

    /**
     * @brief Recalcule les alertes de toutes les commandes et réinitialise
     * l'historique des notifications déjà envoyées.
     */
    void recalculerToutesLesAlertes();

    // ── Widgets ───────────────────────────────────────────────────────────────
    QTableView  *m_tableView;
    QLineEdit   *m_searchBox;
    QComboBox   *m_etatFilter;
    QComboBox   *m_statutLivraisonFilter;
    QPushButton *m_btnDetails;
    QPushButton *m_btnPlanification;
    QPushButton *m_btnRefresh;

    // ── Modèles ───────────────────────────────────────────────────────────────
    ProductionViewModel   *m_model;      ///< Modèle source avec les données
    QSortFilterProxyModel *m_proxyModel; ///< Proxy pour le filtrage/tri sans modifier le modèle source

    // ── Système d'alertes ─────────────────────────────────────────────────────
    QTimer *m_timerAlertes;          ///< Déclenche verifierAlertes() périodiquement
    int     m_joursAlerte;           ///< Seuil en jours pour l'alerte "Risque" (défaut: 3)
    int     m_seuilAvancement;       ///< Seuil d'avancement % pour "Risque" (défaut: 50)
    int     m_intervalleVerification;///< Intervalle de vérification en minutes (défaut: 5)

    /**
     * @brief Ensemble des clés d'alertes déjà notifiées.
     * Clé = idCommande * 100 + typeAlerte
     */
    QSet<int> m_alertesNotifiees;

    /** @brief Pipeline IA pour les toasts intelligents (optionnel). */
    NotificationPipeline *m_pipeline = nullptr;
};

#endif // PRODUCTIONVIEW_H
