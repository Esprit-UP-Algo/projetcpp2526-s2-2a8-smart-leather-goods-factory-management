#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void initialiserTableau();
    void calculerStatistiques();
    void ameliorerAffichageKPI();  // NOUVELLE MÉTHODE
    void filtrerParStatut(int index);
    void rechercherCommandes(const QString &text);
    void dupliquerCommande();
    void viderFormulaireCreer();
    bool validerFormulaireCreer();
    void ajouterNouvelleCommande();
    void chargerDonneesModifier();
    bool validerFormulaireModifier();
    void mettreAJourCommande();
    void chargerDonneesSupprimer();
    void supprimerCommande();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
