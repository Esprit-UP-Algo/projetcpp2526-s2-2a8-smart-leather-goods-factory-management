#ifndef PRODUCTIONDIALOG_H
#define PRODUCTIONDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QDateEdit>
#include <QString>
#include <QDate>

// ═══════════════════════════════════════════════════════════════════════════
// CLASSE: ProductionCommande - Données pour la vue complète
// ═══════════════════════════════════════════════════════════════════════════
class ProductionCommande
{
public:
    ProductionCommande();
    
    // Identification
    int idCommande;
    QString reference;
    QString priorite;
    QDate dateLivraisonPrevue;
    
    // Planification (prévu)
    QDate dateDebutPrevue;
    QDate dateFinPrevue;
    QString atelier;
    int ordrePassage;
    
    // Suivi de production (réel)
    QString etatProduction;  // Planifié, En cours, Bloqué, Terminé
    QString etapeActuelle;
    int avancement;  // en %
    bool retard;
    
    // Livraison
    QString societeLivraison;
    QString numeroSuiviColis;
    QDate dateExpeditionPrevue;
    QDate dateExpeditionReelle;
    QString statutLivraison;  // Non expédiée, En livraison, Livrée
    
    // Méthodes utilitaires
    QString getRetardText() const { return retard ? "Oui" : "Non"; }
    int getJoursRetard() const;
    QString getAvancementText() const { return QString::number(avancement) + " %"; }
};

// ═══════════════════════════════════════════════════════════════════════════
// CLASSE: ProductionDialog - Dialogue CRUD (Add/Edit/Delete)
// ═══════════════════════════════════════════════════════════════════════════
class ProductionDialog : public QDialog
{
    Q_OBJECT
public:
    enum DialogMode { AddMode, EditMode, DeleteMode };

    explicit ProductionDialog(QWidget *parent = nullptr, DialogMode mode = AddMode);
    ~ProductionDialog();

    void setProductionData(const QString &id, const QString &reference, const QString &produit,
                           const QString &quantite, const QString &statut, const QString &dateDebut,
                           const QString &dateFin, const QString &responsable, const QString &priorite);

    QString getId()          const;
    QString getReference()   const;
    QString getProduit()     const;
    QString getQuantite()    const;
    QString getStatut()      const;
    QString getDateDebut()   const;
    QString getDateFin()     const;
    QString getResponsable() const;
    QString getPriorite()    const;

private slots:
    void onSaveClicked();
    void onDeleteConfirmed();

private:
    void setupUI();

    DialogMode   m_mode;
    QLabel      *lblTitle, *lblDeleteWarning;
    QLineEdit   *txtId, *txtReference, *txtQuantite;
    QComboBox   *cmbProduit, *cmbStatut, *cmbResponsable, *cmbPriorite;
    QDateEdit   *dateDebut, *dateFin;
    QPushButton *btnSave, *btnCancel, *btnDelete;
};

#endif // PRODUCTIONDIALOG_H