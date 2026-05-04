#ifndef POINTAGE_H
#define POINTAGE_H

#include <QString>
#include <QDate>
#include <QSqlQueryModel>
#include <QMap>

class Pointage
{
public:
    Pointage();
    
    // Appelé quand Arduino envoie un UID
    bool marquerPresent(const QString &uidCarte);
    
    // Appelé en fin de journée (ex: timer 23h59)
    void marquerAbsentsJournee();
    
    // Calendrier mensuel d'un employé
    QSqlQueryModel* getCalendrierEmploye(int idEmploye, int mois, int annee);
    
    // Calcul salaire net après déductions absences
    double calculerSalaire(int idEmploye, int mois, int annee);
    
    // Résultats du dernier pointage
    QString getDernierNom()    const { return m_nom; }
    QString getDernierPrenom() const { return m_prenom; }
    int     getDernierId()     const { return m_id; }
    bool    estDejaPointe()    const { return m_dejaPointe; }

private:
    int     m_id;
    QString m_nom;
    QString m_prenom;
    bool    m_dejaPointe;
    
    int  trouverEmployeParRfid(const QString &uid);
    bool dejaPoinceAujourdhui(int idEmploye);
};

#endif // POINTAGE_H
