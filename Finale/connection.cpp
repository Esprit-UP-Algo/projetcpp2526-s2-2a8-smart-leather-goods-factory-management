#include "connection.h"

// Constructeur privé
Connection::Connection()
{
}

// Destructeur privé
Connection::~Connection()
{
    if (db.isOpen()) {
        db.close();
    }
}

// Méthode statique pour obtenir l'instance unique (Singleton)
Connection& Connection::getInstance()
{
    static Connection instance;
    return instance;
}

// Méthode pour créer la connexion
bool Connection::createConnection()
{
    // Lister toutes les sources de données disponibles
    qDebug() << "=== SOURCES DE DONNÉES ODBC DISPONIBLES ===";
    QStringList dataSources = QSqlDatabase::connectionNames();
    qDebug() << "Sources enregistrées:" << dataSources;
    
    db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName("Source_Projet2A");
    db.setUserName("malek");
    db.setPassword("malek123");
    
    qDebug() << "\n=== TENTATIVE DE CONNEXION ===";
    qDebug() << "Database Name:" << db.databaseName();
    qDebug() << "User Name:" << db.userName();
    qDebug() << "Driver:" << db.driverName();
    
    if (db.open()) {
        qDebug() << "✓ Connexion réussie à la base de données Source_Projet2A";
        return true;
    } else {
        qDebug() << "\n✗ ERREUR DE CONNEXION:";
        qDebug() << "   Error Type:" << db.lastError().type();
        qDebug() << "   Error Text:" << db.lastError().text();
        qDebug() << "   Native Error Code:" << db.lastError().nativeErrorCode();
        qDebug() << "\nVÉRIFIEZ:";
        qDebug() << "1. Que la source ODBC 'Source_Projet2A' existe dans le bon gestionnaire (32 ou 64-bit)";
        qDebug() << "2. Ouvrez C:\\Windows\\System32\\odbcad32.exe (64-bit)";
        qDebug() << "3. Vérifiez l'onglet 'System DSN' ou 'User DSN'";
        return false;
    }
}

// Méthode pour obtenir la base de données
QSqlDatabase Connection::getDatabase()
{
    return db;
}
