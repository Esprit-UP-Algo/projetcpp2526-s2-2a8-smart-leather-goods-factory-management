#include "employemanager.h"
#include <QDebug>

EmployeManager::EmployeManager(QObject *parent)
    : QObject(parent)
{
    // Mode statique - pas de connexion BD
}

EmployeManager::~EmployeManager()
{
}

bool EmployeManager::ajouterEmploye(const Employe &)
{
    // Mode statique - simulation
    qDebug() << "Mode statique: Employé ajouté (simulation)";
    return true;
}

bool EmployeManager::modifierEmploye(const Employe &)
{
    // Mode statique - simulation
    qDebug() << "Mode statique: Employé modifié (simulation)";
    return true;
}

bool EmployeManager::supprimerEmploye(int)
{
    // Mode statique - simulation
    qDebug() << "Mode statique: Employé supprimé (simulation)";
    return true;
}

QList<Employe> EmployeManager::getAllEmployes()
{
    // Retourner une liste vide en mode statique
    return QList<Employe>();
}

Employe EmployeManager::getEmployeById(int)
{
    // Retourner un employé vide en mode statique
    return Employe();
}

int EmployeManager::getTotalEmployes()
{
    return 0;
}

int EmployeManager::getEmployesActifs()
{
    return 0;
}

QMap<QString, int> EmployeManager::getEmployesParDepartement()
{
    return QMap<QString, int>();
}
