#include "connection.h"

Connection* Connection::instance = nullptr;

Connection::Connection() {}

Connection::~Connection() {
    if (db.isOpen()) {
        db.close();
    }
    instance = nullptr;
}

Connection* Connection::getInstance() {
    if (instance == nullptr) {
        instance = new Connection();
    }
    return instance;
}

bool Connection::createconnect() {
    // Vérifier les drivers disponibles
    qDebug() << "=== Drivers SQL disponibles ===";
    QStringList drivers = QSqlDatabase::drivers();
    for (const QString &driver : drivers) {
        qDebug() << "  -" << driver;
    }
    
    // Essayer d'abord QOCI (connexion directe Oracle)
    if (QSqlDatabase::isDriverAvailable("QOCI")) {
        qDebug() << "✅ Tentative avec driver QOCI (Oracle direct)";
        
        db = QSqlDatabase::addDatabase("QOCI");
        db.setHostName("localhost");
        db.setPort(1521);
        db.setDatabaseName("XE");  // Service name
        db.setUserName(username);
        db.setPassword(password);
        
        qDebug() << "Paramètres QOCI:";
        qDebug() << "  - Host: localhost";
        qDebug() << "  - Port: 1521";
        qDebug() << "  - Service: XE";
        qDebug() << "  - User:" << username;
        
        if (db.open()) {
            qDebug() << "✅ Connexion QOCI réussie !";
            return true;
        } else {
            qDebug() << "❌ Erreur QOCI:" << db.lastError().text();
        }
    }
    
    // Si QOCI échoue, essayer QODBC
    if (QSqlDatabase::isDriverAvailable("QODBC")) {
        qDebug() << "✅ Tentative avec driver QODBC";
        
        // Fermer la connexion précédente si elle existe
        if (db.isOpen()) {
            db.close();
        }
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
        
        db = QSqlDatabase::addDatabase("QODBC");
        
        // Essayer avec une chaîne de connexion complète
        QString connectionString = QString("DRIVER={Oracle in XE};DBQ=XE;UID=%1;PWD=%2;")
                                    .arg(username)
                                    .arg(password);
        
        db.setDatabaseName(connectionString);
        
        qDebug() << "Paramètres QODBC:";
        qDebug() << "  - Connection String:" << connectionString.replace(password, "****");
        
        if (db.open()) {
            qDebug() << "✅ Connexion QODBC réussie !";
            return true;
        } else {
            qDebug() << "❌ Erreur QODBC:" << db.lastError().text();
        }
    }
    
    // Si tout échoue, essayer QODBC avec DSN
    if (QSqlDatabase::isDriverAvailable("QODBC")) {
        qDebug() << "✅ Tentative avec QODBC DSN";
        
        if (db.isOpen()) {
            db.close();
        }
        QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
        
        db = QSqlDatabase::addDatabase("QODBC");
        db.setDatabaseName(dataSourceName);
        db.setUserName(username);
        db.setPassword(password);
        
        qDebug() << "Paramètres QODBC DSN:";
        qDebug() << "  - DSN:" << dataSourceName;
        qDebug() << "  - User:" << username;
        
        if (db.open()) {
            qDebug() << "✅ Connexion QODBC DSN réussie !";
            return true;
        } else {
            qDebug() << "❌ Erreur QODBC DSN:" << db.lastError().text();
        }
    }
    
    qDebug() << "❌ Toutes les tentatives de connexion ont échoué";
    return false;
}
