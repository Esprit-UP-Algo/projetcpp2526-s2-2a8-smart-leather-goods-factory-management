#include "connection.h"

Connection::Connection()
{
}

bool Connection::createconnect()
{
    bool test = false;
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("MatieresPremieresDB"); // Nom de la source de données ODBC
    db.setUserName("SYSTEM"); // Nom d'utilisateur Oracle
    db.setPassword("ta9chira21&A"); // Mot de passe - À REMPLIR!
    
    if (db.open())
        test = true;
    
    return test;
}

bool Connection::createconnectProduction()
{
    bool test = false;
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC", "ProductionConnection");
    db.setDatabaseName("Productionbd"); // Nom de la source de données ODBC
    db.setUserName("LYNA"); // Nom d'utilisateur Oracle
    db.setPassword("000"); // Mot de passe
    
    if (db.open())
        test = true;
    
    return test;
}
