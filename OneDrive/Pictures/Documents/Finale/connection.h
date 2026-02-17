#ifndef CONNECTION_H
#define CONNECTION_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

class Connection
{
private:
    static Connection* instance;
    QSqlDatabase db;
    
    // Constructeur privé (Singleton)
    Connection();
    
    // Paramètres de connexion (À MODIFIER!)
    QString dataSourceName = "Source_Projet2A";  // Votre source ODBC
    QString username = "cuirea_user";             // Votre utilisateur
    QString password = "Cuirea2024";              // Votre mot de passe

public:
    // Empêcher la copie
    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;
    
    static Connection* getInstance();
    bool createconnect();
    QSqlDatabase getDatabase() const { return db; }
    ~Connection();
};

#endif // CONNECTION_H
