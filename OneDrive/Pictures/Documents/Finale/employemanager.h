#ifndef EMPLOYEMANAGER_H
#define EMPLOYEMANAGER_H

#include <QObject>
#include <QList>
#include <QMap>
#include "employe.h"
#include "connection.h"

class EmployeManager : public QObject
{
    Q_OBJECT

public:
    explicit EmployeManager(QObject *parent = nullptr);
    ~EmployeManager();
    
    // Opérations CRUD
    bool ajouterEmploye(const Employe &employe);
    bool modifierEmploye(const Employe &employe);
    bool supprimerEmploye(int id);
    QList<Employe> getAllEmployes();
    Employe getEmployeById(int id);
    
    // Statistiques
    int getTotalEmployes();
    int getEmployesActifs();
    QMap<QString, int> getEmployesParDepartement();
};

#endif // EMPLOYEMANAGER_H
