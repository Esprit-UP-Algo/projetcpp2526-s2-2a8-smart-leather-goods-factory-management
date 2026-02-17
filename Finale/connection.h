#ifndef CONNECTION_H
#define CONNECTION_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

/* Classe Connection
 * Implémentation du pattern Singleton pour gérer une connexion unique à la base de données.
 * Utilise QSqlDatabase pour établir et fermer la connexion.
 * L'instance est accessible via la méthode statique getInstance().
 * La méthode createConnection() configure et ouvre la base de données.
 */
class Connection
{
public:
    // Méthode statique qui retourne une référence unique
    static Connection& getInstance();
    
    // Méthode utilisée pour établir la connexion à la base
    bool createConnection();
    
    // Méthode pour obtenir la base de données
    QSqlDatabase getDatabase();

private:
    // Attribut Connexion persistante
    QSqlDatabase db;
    
    // Constructeur privé pour éviter la création d'autres instances
    Connection();
    
    // Destructeur privé pour fermer proprement la connexion
    ~Connection();
    
    // Interdire la copie et l'affectation pour garantir l'unicité de l'instance
    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;
};

#endif // CONNECTION_H
