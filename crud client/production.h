#ifndef PRODUCTION_H
#define PRODUCTION_H

#include <QString>
#include <QDate>
#include <QList>
#include <QMap>
#include <QSqlQuery>
#include <QSqlQueryModel>

class ProductionData
{
public:
    ProductionData();
    ProductionData(const QString &id, const QString &reference, const QString &produit,
                const QString &quantite, const QString &statut, const QDate &dateDebut,
                const QDate &dateFin, const QString &responsable, const QString &priorite);

    // Getters
    QString getId() const { return id; }
    QString getReference() const { return reference; }
    QString getProduit() const { return produit; }
    QString getQuantite() const { return quantite; }
    QString getStatut() const { return statut; }
    QDate getDateDebut() const { return dateDebut; }
    QDate getDateFin() const { return dateFin; }
    QString getResponsable() const { return responsable; }
    QString getPriorite() const { return priorite; }

    // Setters
    void setId(const QString &value) { id = value; }
    void setReference(const QString &value) { reference = value; }
    void setProduit(const QString &value) { produit = value; }
    void setQuantite(const QString &value) { quantite = value; }
    void setStatut(const QString &value) { statut = value; }
    void setDateDebut(const QDate &value) { dateDebut = value; }
    void setDateFin(const QDate &value) { dateFin = value; }
    void setResponsable(const QString &value) { responsable = value; }
    void setPriorite(const QString &value) { priorite = value; }

    // ========== MÉTIERS BASIQUES : GESTION DES COMMANDES ==========
    
    // a) Recherche et filtrage
    static QList<ProductionData> rechercherParDate(const QList<ProductionData> &liste, 
                                                     const QDate &dateDebut, 
                                                     const QDate &dateFin);
    
    static QList<ProductionData> rechercherParStatut(const QList<ProductionData> &liste, 
                                                       const QString &statut);
    
    static QList<ProductionData> rechercherParPriorite(const QList<ProductionData> &liste,
                                                         const QString &priorite);
    
    static QList<ProductionData> rechercherParResponsable(const QList<ProductionData> &liste,
                                                            const QString &responsable);
    
    // b) Suivi des délais et priorités
    static QList<ProductionData> obtenirProductionsUrgentes(const QList<ProductionData> &liste);
    
    static QList<ProductionData> obtenirProductionsEnRetard(const QList<ProductionData> &liste);
    
    static QList<ProductionData> obtenirProductionsARisque(const QList<ProductionData> &liste, 
                                                             int joursAlerte = 3);
    
    static int compterProductionsParStatut(const QList<ProductionData> &liste, 
                                            const QString &statut);
    
    // c) Analyse commerciale / Reporting
    static int calculerQuantiteTotale(const QList<ProductionData> &liste, 
                                       const QString &statut = "Terminé");
    
    static QMap<QString, int> statistiquesParProduit(const QList<ProductionData> &liste);
    
    static QMap<QString, int> statistiquesParStatut(const QList<ProductionData> &liste);
    
    static QMap<QString, int> statistiquesParResponsable(const QList<ProductionData> &liste);
    
    static double calculerTauxAchevementGlobal(const QList<ProductionData> &liste);
    
    static QMap<QString, double> calculerTauxAchevementParProduit(const QList<ProductionData> &liste);
    
    // d) Tri et organisation
    static void trierParPriorite(QList<ProductionData> &liste, bool decroissant = true);
    
    static void trierParQuantite(QList<ProductionData> &liste, bool decroissant = true);
    
    static void trierParDateDebut(QList<ProductionData> &liste, bool decroissant = false);
    
    static void trierParDateFin(QList<ProductionData> &liste, bool decroissant = false);
    
    // ========== MÉTIERS INNOVANTS : VALEUR AJOUTÉE ==========
    
    // a) Traçabilité / Suivi QR Code
    QString genererQRCodeData() const;
    
    QString genererQRCodeComplet() const;
    
    // b) Alerte prédictive / Retard
    bool estEnRetard() const;
    
    bool estARisque(int joursAlerte = 3) const;
    
    int joursAvantEcheance() const;
    
    QString getAlerteRetard() const;
    
    QString getNiveauUrgence() const;
    
    // c) Validation et contrôle qualité
    bool estValide() const;
    
    QStringList obtenirProblemes() const;

    // ========== MÉTHODES CRUD ==========
    bool ajouter();
    bool modifier();
    bool supprimer(int id);
    QSqlQueryModel* afficher();
    QSqlQueryModel* rechercher(const QString &terme);
    QSqlQueryModel* trierPar(const QString &colonne);

private:
    QString id;
    QString reference;
    QString produit;
    QString quantite;
    QString statut;
    QDate dateDebut;
    QDate dateFin;
    QString responsable;
    QString priorite;
    
    // Attributs pour CRUD
    double montantHT;
    QDate dateCreation;
    QDate dateLivraison;
    QString type;
    QString client;
    
    // Helper pour convertir priorité en valeur numérique
    static int prioriteToInt(const QString &priorite);

public:
    // Getters/Setters pour CRUD
    double getMontantHT() const { return montantHT; }
    void setMontantHT(double value) { montantHT = value; }
    QDate getDateCreation() const { return dateCreation; }
    void setDateCreation(const QDate &value) { dateCreation = value; }
    QDate getDateLivraison() const { return dateLivraison; }
    void setDateLivraison(const QDate &value) { dateLivraison = value; }
    QString getType() const { return type; }
    void setType(const QString &value) { type = value; }
    QString getClient() const { return client; }
    void setClient(const QString &value) { client = value; }
    
    // Setter pour ID (pour modifier)
    void setIdInt(int value) { id = QString::number(value); }
    void setId(int value) { id = QString::number(value); }
    int getIdInt() const { return id.toInt(); }
};

// Alias pour compatibilité
using Production = ProductionData;

#endif // PRODUCTION_H
