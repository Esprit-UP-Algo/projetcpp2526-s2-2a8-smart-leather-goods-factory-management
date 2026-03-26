#ifndef CLIENT_H
#define CLIENT_H

#include <QString>
#include <QSqlQueryModel>
#include <QMap>

class Client {
private:
    int id_client;
    QString nom;
    QString prenom;
    QString sexe;
    QString cin;
    QString pays;
    QString ville;
    QString adresse;
    QString email;
    QString date_inscription; // Format: "YYYY-MM-DD"
    int id_employe;

public:
    // Constructeurs
    Client();
    Client(int id_client, const QString& nom, const QString& prenom,
           const QString& sexe, const QString& cin,
           const QString& pays, const QString& ville,
           const QString& adresse, const QString& email,
           const QString& date_inscription, int id_employe);

    // Destructeur
    ~Client();

    // Getters
    int getId_client() const;
    QString getNom() const;
    QString getPrenom() const;
    QString getSexe() const;
    QString getCin() const;
    QString getPays() const;
    QString getVille() const;
    QString getAdresse() const;
    QString getEmail() const;
    QString getDate_inscription() const;
    int getId_employe() const;

    // Setters
    void setId_client(int id);
    void setNom(const QString& nom);
    void setPrenom(const QString& prenom);
    void setSexe(const QString& sexe);
    void setCin(const QString& cin);
    void setPays(const QString& pays);
    void setVille(const QString& ville);
    void setAdresse(const QString& adresse);
    void setEmail(const QString& email);
    void setDate_inscription(const QString& date);
    void setId_employe(int id);

    // ========================
    // Méthodes CRUD (Qt/SQL)
    // ========================
    QSqlQueryModel* afficherClients();  // SELECT tous les clients → colonnes: id, nom, prenom, sexe, cin, pays, ville, adresse, email, date_inscription
    bool ajouter();                     // INSERT avec les attributs de l'instance courante
    bool modifier();                    // UPDATE basé sur id_client de l'instance courante
    bool supprimer(int id);

    QList<Client> getAll();//this
    QMap<QString, int> statistiquesParVille();//this
};

#endif // CLIENT_H
