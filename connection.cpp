#include "connection.h"
#include <QSqlError>
#include <QDebug>

// Initialisation du pointeur d'instance
Connection* Connection::p_instance = nullptr;

// Constructeur privé
Connection::Connection()
{
    // Initialisation de la base de données
    db = QSqlDatabase::addDatabase("QODBC");
}

// Méthode statique pour obtenir l'instance unique
Connection* Connection::instance()
{
    if (p_instance == nullptr) {
        p_instance = new Connection();
    }
    return p_instance;
}

// Méthode pour établir la connexion
bool Connection::createConnect()
{
    bool test = false;
    
    db.setDatabaseName("Projet2A"); // Nom de la source de données ODBC
    db.setUserName("cuirea"); // Nom de l'utilisateur
    db.setPassword("0000"); // Mot de passe
    
    if (db.open()) {
        test = true;
        qDebug() << "✅ Connexion à la base de données réussie";
        qDebug() << "Base de données: Projet2A";
        qDebug() << "Utilisateur: cuirea";
    } else {
        qDebug() << "❌ Erreur de connexion:" << db.lastError().text();
        qDebug() << "Type d'erreur:" << db.lastError().type();
        qDebug() << "Driver:" << db.lastError().driverText();
        qDebug() << "Database:" << db.lastError().databaseText();
    }
    
    return test;
}

// Fermer la connexion
void Connection::closeConnection()
{
    if (db.isOpen()) {
        db.close();
        qDebug() << "🔒 Connexion à la base de données fermée";
    }
}

// Destructeur privé
Connection::~Connection()
{
    closeConnection();
}
