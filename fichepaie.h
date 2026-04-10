#ifndef FICHEPAIE_H
#define FICHEPAIE_H

#include <QString>
#include <QDate>
#include <QDialog>
#include <QLineEdit>
#include <QDoubleSpinBox>

struct DonneesPaie {
    // Informations employé
    QString matricule;
    QString nom;
    QString prenom;
    QString cin;
    QString poste;
    QString departement;
    QDate dateEmbauche;
    
    // Salaire et primes
    double salaireBrut;
    double primeRendement;
    double primeAnciennete;
    double primeAutres;
    
    // Retenues et avances
    double retenues;
    double avances;
    
    // Période
    QString mois;
    int annee;
};

class FichePaieDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FichePaieDialog(const QString &matricule, const QString &nom, 
                            const QString &prenom, const QString &cin,
                            const QString &poste, const QString &departement,
                            const QDate &dateEmbauche, QWidget *parent = nullptr);
    
    DonneesPaie getDonneesPaie() const;

private slots:
    void calculerSalaire();
    void genererPDF();

private:
    void setupUI();
    void applyStyles();
    double calculerCNSS(double salaireBrut) const;
    double calculerIRPP(double salaireImposable) const;
    
    // Données employé
    QString m_matricule;
    QString m_nom;
    QString m_prenom;
    QString m_cin;
    QString m_poste;
    QString m_departement;
    QDate m_dateEmbauche;
    
    // Widgets de saisie
    QDoubleSpinBox *salaireBrutSpin;
    QDoubleSpinBox *primeRendementSpin;
    QDoubleSpinBox *primeAncienneteSpin;
    QDoubleSpinBox *primeAutresSpin;
    QDoubleSpinBox *retenuesSpin;
    QDoubleSpinBox *avancesSpin;
    
    // Widgets d'affichage
    QLineEdit *cnssEdit;
    QLineEdit *irppEdit;
    QLineEdit *totalGainsEdit;
    QLineEdit *totalRetenuesEdit;
    QLineEdit *netAPayerEdit;
};

class FichePaiePDF
{
public:
    static bool genererPDF(const DonneesPaie &donnees, const QString &cheminFichier);

private:
    static double calculerCNSS(double salaireBrut);
    static double calculerIRPP(double salaireImposable);
};

#endif // FICHEPAIE_H
