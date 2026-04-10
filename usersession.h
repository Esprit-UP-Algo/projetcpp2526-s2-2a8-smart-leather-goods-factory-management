#ifndef USERSESSION_H
#define USERSESSION_H

#include <QString>
#include <QDate>

class UserSession
{
public:
    enum Role {
        NONE,
        EMPLOYE,              // Consultation uniquement
        SERVICE_FOURNISSEUR,  // Gestion fournisseurs
        SERVICE_CLIENT,       // Gestion clients et suivi
        SERVICE_VENTE,        // Gestion ventes et clients
        RESPONSABLE_STOCK,    // Gestion complète stock et production
        RESPONSABLE_VENTE,    // Gestion complète ventes
        RH,                   // Gestion RH
        ADMIN                 // Accès complet
    };

    static UserSession& instance();
    
    // Authentification
    bool login(const QString &matricule, const QString &motDePasse);
    void logout();
    bool isLoggedIn() const { return m_isLoggedIn; }
    
    // Getters
    int getUserId() const { return m_userId; }
    QString getMatricule() const { return m_matricule; }
    QString getNomComplet() const { return m_nom + " " + m_prenom; }
    QString getNom() const { return m_nom; }
    QString getPrenom() const { return m_prenom; }
    QString getEmail() const { return m_email; }
    QString getPoste() const { return m_poste; }
    QString getDepartement() const { return m_departement; }
    Role getRole() const { return m_role; }
    QString getRoleString() const;
    
    // Vérification des permissions
    bool canAccessModule(const QString &moduleName) const;
    bool canCreate() const;
    bool canEdit() const;
    bool canDelete() const;
    bool canExport() const;
    bool canViewStatistics() const;
    
    // Modules accessibles
    bool canAccessEmployees() const;
    bool canAccessClients() const;
    bool canAccessFournisseurs() const;
    bool canAccessMatieres() const;
    bool canAccessProduction() const;
    bool canAccessArticles() const;

private:
    UserSession();
    ~UserSession();
    UserSession(const UserSession&) = delete;
    UserSession& operator=(const UserSession&) = delete;
    
    void updateLastLogin();
    Role parseRole(const QString &roleStr);
    
    bool m_isLoggedIn;
    int m_userId;
    QString m_matricule;
    QString m_nom;
    QString m_prenom;
    QString m_email;
    QString m_poste;
    QString m_departement;
    Role m_role;
};

#endif // USERSESSION_H
