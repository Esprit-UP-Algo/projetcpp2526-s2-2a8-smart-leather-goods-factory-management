#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QTableWidget>
#include <QDebug>
#include <QDateTime>
#include <QDate>
#include <QPixmap>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Navigation
    void on_btnTableauBord_clicked();
    void on_btnProduits_clicked();

    // Actions CRUD
    void on_btnCreer_clicked();
    void on_btnModifier_clicked();
    void on_btnSupprimer_clicked();
    void on_btnConsulter_clicked();

    // Actions formulaire Créer
    void on_btnCreerSauvegarder_clicked();
    void on_btnCreerAnnuler_clicked();

    // Actions formulaire Modifier
    void on_btnModifierSauvegarder_clicked();
    void on_btnModifierAnnuler_clicked();

    // Actions formulaire Supprimer
    void on_btnSupprimerConfirmer_clicked();
    void on_btnSupprimerAnnuler_clicked();

    // Actions formulaire Consulter
    void on_btnConsulterRetour_clicked();

    // Actions recherche et filtrage
    void on_lineEditRecherche_textChanged(const QString &text);
    void on_comboBoxFiltreStatut_currentIndexChanged(int index);

    // Sélection dans le tableau
    void on_tableWidgetProduits_itemSelectionChanged();

    // Double-click → consultation rapide
    void on_tableWidgetProduits_itemDoubleClicked(QTableWidgetItem *item);

private:
    Ui::MainWindow *ui;
    QSqlDatabase db;
    QPixmap logoPixmap;          // logo CUIRÉA chargé une seule fois

    // Méthodes privées
    bool connecterBaseDeDonnees();
    void creerTableProduits();
    void chargerProduits();
    void chargerProduitsFiltres(const QString &recherche, const QString &statut);
    void afficherMessageErreur(const QString &titre, const QString &message);
    void afficherMessageSucces(const QString &titre, const QString &message);
    void viderFormulaireCreer();
    void viderFormulaireModifier();
    void chargerProduitPourModification(int idProduit);
    void chargerProduitPourSuppression(int idProduit);
    void chargerProduitPourConsultation(int idProduit);
    int  obtenirIdProduitSelectionne();
    bool validerFormulaire(bool isModification = false);
    QString genererReferenceAutomatique();

    // Mise à jour des statistiques du tableau de bord
    void miseAJourStatistiques();
};

#endif // MAINWINDOW_H
