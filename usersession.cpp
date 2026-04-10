#include "usersession.h"
#include "connection.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QCryptographicHash>

UserSession::UserSession()
    : m_isLoggedIn(false)
    , m_userId(-1)
    , m_role(NONE)
{
}

UserSession::~UserSession()
{
}

UserSession& UserSession::instance()
{
    static UserSession instance;
    return instance;
}

bool UserSession::login(const QString &matricule, const QString &motDePasse)
{
    QSqlQuery query(Connection::instance()->getDatabase());
    
    query.prepare("SELECT ID_EMPLOYE, MATRICULE, NOM, PRENOM, EMAIL, POSTE, "
                  "DEPARTEMENT, ROLE_SYSTEME, ACTIF "
                  "FROM CUIREA.EMPLOYES "
                  "WHERE MATRICULE = :matricule AND MOT_DE_PASSE = :password");
    
    query.bindValue(":matricule", matricule.toUpper());
    query.bindValue(":password", motDePasse); // En production, utiliser un hash
    
    if (!query.exec()) {
        qDebug() << "Erreur login:" << query.lastError().text();
        return false;
    }
    
    if (query.next()) {
        // Vérifier si le compte est actif
        int actif = query.value(8).toInt();
        if (actif == 0) {
            qDebug() << "Compte désactivé";
            return false;
        }
        
        // Charger les informations utilisateur
        m_userId = query.value(0).toInt();
        m_matricule = query.value(1).toString();
        m_nom = query.value(2).toString();
        m_prenom = query.value(3).toString();
        m_email = query.value(4).toString();
        m_poste = query.value(5).toString();
        m_departement = query.value(6).toString();
        m_role = parseRole(query.value(7).toString());
        m_isLoggedIn = true;
        
        // Mettre à jour la dernière connexion
        updateLastLogin();
        
        qDebug() << "Login réussi:" << getNomComplet() << "- Rôle:" << getRoleString();
        return true;
    }
    
    qDebug() << "Identifiants incorrects";
    return false;
}

void UserSession::logout()
{
    m_isLoggedIn = false;
    m_userId = -1;
    m_matricule.clear();
    m_nom.clear();
    m_prenom.clear();
    m_email.clear();
    m_poste.clear();
    m_departement.clear();
    m_role = NONE;
    
    qDebug() << "Déconnexion réussie";
}

void UserSession::updateLastLogin()
{
    QSqlQuery query(Connection::instance()->getDatabase());
    query.prepare("UPDATE CUIREA.EMPLOYES SET DERNIERE_CONNEXION = SYSDATE "
                  "WHERE ID_EMPLOYE = :id");
    query.bindValue(":id", m_userId);
    query.exec();
}

UserSession::Role UserSession::parseRole(const QString &roleStr)
{
    QString role = roleStr.toUpper().replace(" ", "_");
    if (role == "ADMIN") return ADMIN;
    if (role == "RH") return RH;
    if (role == "RESPONSABLE_VENTE") return RESPONSABLE_VENTE;
    if (role == "SERVICE_VENTE") return SERVICE_VENTE;
    if (role == "SERVICE_CLIENT") return SERVICE_CLIENT;
    if (role == "RESPONSABLE_STOCK") return RESPONSABLE_STOCK;
    if (role == "SERVICE_FOURNISSEUR") return SERVICE_FOURNISSEUR;
    return EMPLOYE;
}

QString UserSession::getRoleString() const
{
    switch (m_role) {
        case ADMIN: return "Administrateur";
        case RH: return "Ressources Humaines";
        case RESPONSABLE_VENTE: return "Responsable Vente";
        case SERVICE_VENTE: return "Service Vente";
        case SERVICE_CLIENT: return "Service Client";
        case RESPONSABLE_STOCK: return "Responsable Stock";
        case SERVICE_FOURNISSEUR: return "Service Fournisseur";
        case EMPLOYE: return "Employé";
        default: return "Non défini";
    }
}

// Permissions par rôle
bool UserSession::canCreate() const
{
    // Tous sauf EMPLOYE peuvent créer
    return m_role >= SERVICE_FOURNISSEUR;
}

bool UserSession::canEdit() const
{
    // Tous sauf EMPLOYE peuvent modifier
    return m_role >= SERVICE_FOURNISSEUR;
}

bool UserSession::canDelete() const
{
    // Seulement les responsables, RH et ADMIN
    return m_role >= RESPONSABLE_STOCK;
}

bool UserSession::canExport() const
{
    // Tous sauf EMPLOYE
    return m_role >= SERVICE_FOURNISSEUR;
}

bool UserSession::canViewStatistics() const
{
    // Responsables, RH et ADMIN
    return m_role >= RESPONSABLE_STOCK;
}

// Accès aux modules
bool UserSession::canAccessEmployees() const
{
    return m_role == RH || m_role == ADMIN;
}

bool UserSession::canAccessClients() const
{
    // Service client, service vente, responsable vente, admin
    return m_role == SERVICE_CLIENT || 
           m_role == SERVICE_VENTE || 
           m_role == RESPONSABLE_VENTE || 
           m_role == ADMIN;
}

bool UserSession::canAccessFournisseurs() const
{
    // Service fournisseur, responsable stock, admin
    return m_role == SERVICE_FOURNISSEUR || 
           m_role == RESPONSABLE_STOCK || 
           m_role == ADMIN;
}

bool UserSession::canAccessMatieres() const
{
    // Service fournisseur, responsable stock, admin
    return m_role == SERVICE_FOURNISSEUR || 
           m_role == RESPONSABLE_STOCK || 
           m_role == ADMIN;
}

bool UserSession::canAccessProduction() const
{
    // Responsable stock et admin
    return m_role == RESPONSABLE_STOCK || m_role == ADMIN;
}

bool UserSession::canAccessArticles() const
{
    // Service vente, responsable vente, responsable stock, admin
    return m_role == SERVICE_VENTE || 
           m_role == RESPONSABLE_VENTE || 
           m_role == RESPONSABLE_STOCK || 
           m_role == ADMIN;
}

bool UserSession::canAccessModule(const QString &moduleName) const
{
    if (m_role == ADMIN) return true;
    
    if (moduleName == "employees") return canAccessEmployees();
    if (moduleName == "clients") return canAccessClients();
    if (moduleName == "fournisseurs") return canAccessFournisseurs();
    if (moduleName == "matieres") return canAccessMatieres();
    if (moduleName == "production") return canAccessProduction();
    if (moduleName == "articles") return canAccessArticles();
    
    return false;
}
