#include "client.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDate>

// ========================
// Constructeurs
// ========================

Client::Client()
    : id_client(0), nom(""), prenom(""), sexe(""), cin(""),
    pays(""), ville(""), adresse(""), email(""),
    date_inscription(QDate::currentDate().toString("yyyy-MM-dd")),
    id_employe(0) {}

Client::Client(int id_client, const QString& nom, const QString& prenom,
               const QString& sexe, const QString& cin,
               const QString& pays, const QString& ville,
               const QString& adresse, const QString& email,
               const QString& date_inscription, int id_employe)
    : id_client(id_client), nom(nom), prenom(prenom), sexe(sexe), cin(cin),
    pays(pays), ville(ville), adresse(adresse), email(email),
    date_inscription(date_inscription), id_employe(id_employe) {}

// ========================
// Destructeur
// ========================

Client::~Client() {}

// ========================
// Getters
// ========================

int     Client::getId_client()        const { return id_client; }
QString Client::getNom()              const { return nom; }
QString Client::getPrenom()           const { return prenom; }
QString Client::getSexe()             const { return sexe; }
QString Client::getCin()              const { return cin; }
QString Client::getPays()             const { return pays; }
QString Client::getVille()            const { return ville; }
QString Client::getAdresse()          const { return adresse; }
QString Client::getEmail()            const { return email; }
QString Client::getDate_inscription() const { return date_inscription; }
int     Client::getId_employe()       const { return id_employe; }

// ========================
// Setters
// ========================

void Client::setId_client(int id)                   { id_client = id; }
void Client::setNom(const QString& n)               { nom = n; }
void Client::setPrenom(const QString& p)            { prenom = p; }
void Client::setSexe(const QString& s)              { sexe = s; }
void Client::setCin(const QString& c)               { cin = c; }
void Client::setPays(const QString& p)              { pays = p; }
void Client::setVille(const QString& v)             { ville = v; }
void Client::setAdresse(const QString& a)           { adresse = a; }
void Client::setEmail(const QString& e)             { email = e; }
void Client::setDate_inscription(const QString& d)  { date_inscription = d; }
void Client::setId_employe(int id)                  { id_employe = id; }

// ========================
// CRUD — Afficher
// ========================

QSqlQueryModel* Client::afficherClients()
{
    QSqlQueryModel* model = new QSqlQueryModel();

    model->setQuery(
        "SELECT id_client, nom, prenom, sexe, cin, pays, ville, adresse, email, "
        "TO_CHAR(date_inscription, 'YYYY-MM-DD') AS date_inscription "
        "FROM Clients "
        "ORDER BY id_client"
        );

    if (model->lastError().isValid()) {
        qDebug() << "[Client::afficherClients] Erreur SQL :" << model->lastError().text();
    }

    return model;
}

// ========================
// CRUD — Ajouter
// ========================

bool Client::ajouter()
{
    QSqlQuery query;

    // ── Récupère le prochain id via la séquence ──────────────
    // Remplacez "SEQ_CLIENTS" par le vrai nom de votre séquence si différent
    QSqlQuery seqQuery;
    if (!seqQuery.exec("SELECT SEQ_CLIENTS.NEXTVAL FROM DUAL")) {
        qDebug() << "[Client::ajouter] Impossible de lire la séquence :"
                 << seqQuery.lastError().text();
        return false;
    }
    seqQuery.next();
    int newId = seqQuery.value(0).toInt();
    qDebug() << "[Client::ajouter] Nouvel id_client =" << newId;

    // ── INSERT ───────────────────────────────────────────────
    query.prepare(
        "INSERT INTO Clients "
        "(id_client, nom, prenom, sexe, cin, pays, ville, adresse, email, date_inscription, id_employe) "
        "VALUES "
        "(:id_client, :nom, :prenom, :sexe, :cin, :pays, :ville, :adresse, :email, "
        " TO_DATE(:date_inscription, 'YYYY-MM-DD'), :id_employe)"
        );

    query.bindValue(":id_client",         newId);
    query.bindValue(":nom",               nom);
    query.bindValue(":prenom",            prenom.isEmpty()   ? QVariant(QVariant::String) : prenom);
    query.bindValue(":sexe",              sexe.isEmpty()     ? QVariant(QVariant::String) : sexe);
    query.bindValue(":cin",               cin.isEmpty()      ? QVariant(QVariant::String) : cin);
    query.bindValue(":pays",              pays.isEmpty()     ? QVariant(QVariant::String) : pays);
    query.bindValue(":ville",             ville.isEmpty()    ? QVariant(QVariant::String) : ville);
    query.bindValue(":adresse",           adresse.isEmpty()  ? QVariant(QVariant::String) : adresse);
    query.bindValue(":email",             email.isEmpty()    ? QVariant(QVariant::String) : email);
    query.bindValue(":date_inscription",  date_inscription.isEmpty()
                                             ? QDate::currentDate().toString("yyyy-MM-dd")
                                             : date_inscription);
    query.bindValue(":id_employe",        id_employe > 0
                                       ? QVariant(id_employe)
                                       : QVariant(QVariant::Int));

    if (!query.exec()) {
        qDebug() << "[Client::ajouter] Erreur SQL :" << query.lastError().text()
        << "\n  Query :"  << query.lastQuery();
        return false;
    }

    id_client = newId; // met à jour l'instance avec le vrai id
    qDebug() << "[Client::ajouter] Client inséré avec succès, id =" << id_client;
    return true;
}

// ========================
// CRUD — Modifier
// ========================

bool Client::modifier()
{
    if (id_client <= 0) {
        qDebug() << "[Client::modifier] id_client invalide.";
        return false;
    }

    QSqlQuery query;

    query.prepare(
        "UPDATE Clients SET "
        "nom              = :nom, "
        "prenom           = :prenom, "
        "sexe             = :sexe, "
        "cin              = :cin, "
        "pays             = :pays, "
        "ville            = :ville, "
        "adresse          = :adresse, "
        "email            = :email, "
        "date_inscription = TO_DATE(:date_inscription, 'YYYY-MM-DD'), "
        "id_employe       = :id_employe "
        "WHERE id_client  = :id_client"
        );

    query.bindValue(":nom",               nom);
    query.bindValue(":prenom",            prenom);
    query.bindValue(":sexe",              sexe);
    query.bindValue(":cin",               cin);
    query.bindValue(":pays",              pays);
    query.bindValue(":ville",             ville);
    query.bindValue(":adresse",           adresse);
    query.bindValue(":email",             email);
    query.bindValue(":date_inscription",  date_inscription.isEmpty()
                                             ? QDate::currentDate().toString("yyyy-MM-dd")
                                             : date_inscription);
    query.bindValue(":id_employe",        id_employe > 0 ? id_employe : QVariant(QVariant::Int));
    query.bindValue(":id_client",         id_client);

    if (!query.exec()) {
        qDebug() << "[Client::modifier] Erreur SQL :" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

// ========================
// CRUD — Supprimer
// ========================

bool Client::supprimer(int id)
{
    if (id <= 0) {
        qDebug() << "[Client::supprimer] id_client invalide.";
        return false;
    }

    QSqlQuery query;
    query.prepare("DELETE FROM Clients WHERE id_client = :id_client");
    query.bindValue(":id_client", id);

    if (!query.exec()) {
        qDebug() << "[Client::supprimer] Erreur SQL :" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}