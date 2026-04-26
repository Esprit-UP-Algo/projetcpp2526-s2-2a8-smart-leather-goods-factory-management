#include "client.h"
#include "qtablewidget.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDate>
#include <QFile>
#include <QTextStream>
#include <QPdfWriter>
#include <QPrinter>
#include <QSqlRecord>
#include <QTextDocument>

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

    // ── Récupère le prochain id client ──────────────
    QSqlQuery seqQuery;
    if (!seqQuery.exec("SELECT SEQ_CLIENTS.NEXTVAL FROM DUAL")) {
        qDebug() << "[Client::ajouter] Erreur séquence :"
                 << seqQuery.lastError().text();
        return false;
    }
    seqQuery.next();
    int newId = seqQuery.value(0).toInt();

    // ── INSERT CLIENT ───────────────────────────────
    query.prepare(
        "INSERT INTO Clients "
        "(id_client, nom, prenom, sexe, cin, pays, ville, adresse, email, date_inscription, id_employe) "
        "VALUES "
        "(:id_client, :nom, :prenom, :sexe, :cin, :pays, :ville, :adresse, :email, "
        "TO_DATE(:date_inscription, 'YYYY-MM-DD'), :id_employe)"
        );

    query.bindValue(":id_client", newId);
    query.bindValue(":nom", nom);
    query.bindValue(":prenom", prenom.isEmpty() ? QVariant(QMetaType(QMetaType::QString)) : prenom);
    query.bindValue(":sexe", sexe.isEmpty() ? QVariant(QMetaType(QMetaType::QString)) : sexe);
    query.bindValue(":cin", cin.isEmpty() ? QVariant(QMetaType(QMetaType::QString)) : cin);
    query.bindValue(":pays", pays.isEmpty() ? QVariant(QMetaType(QMetaType::QString)) : pays);
    query.bindValue(":ville", ville.isEmpty() ? QVariant(QMetaType(QMetaType::QString)) : ville);
    query.bindValue(":adresse", adresse.isEmpty() ? QVariant(QMetaType(QMetaType::QString)) : adresse);
    query.bindValue(":email", email.isEmpty() ? QVariant(QMetaType(QMetaType::QString)) : email);
    query.bindValue(":date_inscription",
                    date_inscription.isEmpty()
                        ? QDate::currentDate().toString("yyyy-MM-dd")
                        : date_inscription);
    query.bindValue(":id_employe",
                    id_employe > 0 ? QVariant(id_employe)
                                   : QVariant(QMetaType(QMetaType::Int)));

    if (!query.exec()) {
        qDebug() << "[Client::ajouter] Erreur SQL :"
                 << query.lastError().text();
        return false;
    }

    // ── INSERT HISTORIQUE ───────────────────────────
    QSqlQuery histQuery;

    QSqlQuery seqHist;
    if (seqHist.exec("SELECT SEQ_HISTORIQUE.NEXTVAL FROM DUAL") && seqHist.next()) {

        histQuery.prepare(
            "INSERT INTO HISTORIQUE_CLIENT (ID_HIST, EMAIL, DATE_INSCRIPTION) "
            "VALUES (:id, :email, SYSDATE)"
            );

        histQuery.bindValue(":id", seqHist.value(0).toInt());
        histQuery.bindValue(":email", email);

        if (!histQuery.exec()) {
            qDebug() << "[Historique] Erreur :"
                     << histQuery.lastError().text();
        }
    }

    id_client = newId;
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

    // ── 1. Récupérer ancien email ───────────────────
    QString oldEmail;
    QSqlQuery getOld;
    getOld.prepare("SELECT email FROM Clients WHERE id_client = :id");
    getOld.bindValue(":id", id_client);

    if (getOld.exec() && getOld.next()) {
        oldEmail = getOld.value(0).toString();
    }

    // ── 2. UPDATE CLIENT ────────────────────────────
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

    query.bindValue(":nom", nom);
    query.bindValue(":prenom", prenom);
    query.bindValue(":sexe", sexe);
    query.bindValue(":cin", cin);
    query.bindValue(":pays", pays);
    query.bindValue(":ville", ville);
    query.bindValue(":adresse", adresse);
    query.bindValue(":email", email);
    query.bindValue(":date_inscription",
                    date_inscription.isEmpty()
                        ? QDate::currentDate().toString("yyyy-MM-dd")
                        : date_inscription);
    query.bindValue(":id_employe",
                    id_employe > 0 ? id_employe
                                   : QVariant(QMetaType(QMetaType::Int)));
    query.bindValue(":id_client", id_client);

    if (!query.exec()) {
        qDebug() << "[Client::modifier] Erreur SQL :" << query.lastError().text();
        return false;
    }

    // ── 3. Si email changé → update historique ──────
    if (oldEmail != email) {
        QSqlQuery histUpdate;
        histUpdate.prepare(
            "UPDATE HISTORIQUE_CLIENT "
            "SET EMAIL = :newEmail "
            "WHERE EMAIL = :oldEmail"
            );

        histUpdate.bindValue(":newEmail", email);
        histUpdate.bindValue(":oldEmail", oldEmail);

        if (!histUpdate.exec()) {
            qDebug() << "[Historique update] Erreur :"
                     << histUpdate.lastError().text();
        }
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




QList<Client> Client::getAll()
{
    QList<Client> list;
    QSqlQuery q;
    q.exec("SELECT id_client, nom, prenom, sexe, cin, pays, ville, adresse, email, "
           "TO_CHAR(date_inscription, 'YYYY-MM-DD'), id_employe FROM CLIENTS ORDER BY id_client");

    while (q.next()) {
        Client c;
        c.setId_client        (q.value(0).toInt());
        c.setNom              (q.value(1).toString());
        c.setPrenom           (q.value(2).toString());
        c.setSexe             (q.value(3).toString());
        c.setCin              (q.value(4).toString());
        c.setPays             (q.value(5).toString());
        c.setVille            (q.value(6).toString());
        c.setAdresse          (q.value(7).toString());
        c.setEmail            (q.value(8).toString());
        c.setDate_inscription (q.value(9).toString());
        c.setId_employe       (q.value(10).toInt());
        list << c;
    }
    return list;
}



//stat



QMap<QString, int> Client::statistiquesParVille()
{
    QMap<QString, int> stats;

    QSqlQuery query;
    query.prepare("SELECT ville, COUNT(*) FROM CLIENTS GROUP BY ville");

    if(query.exec())
    {
        while(query.next())
        {
            QString ville = query.value(0).toString();
            int count = query.value(1).toInt();

            stats[ville] = count;
        }
    }

    return stats;
}


//export




bool Client::exporterCommandesParClient(const QString& mailClient, const QString& fileName)
{
    if (mailClient.isEmpty() || fileName.isEmpty())
        return false;

    // 🔹 SQL Query
    QSqlQuery query;
    query.prepare("SELECT ID_COMMANDE, DATE_CREATION, MONTANT, STATUT "
                  "FROM CUIREA.COMMANDES WHERE MAIL_CLIENT = :mail");
    query.bindValue(":mail", mailClient);

    if (!query.exec()) {
        qDebug() << "Erreur SQL:" << query.lastError();
        return false;
    }

    // 🔹 Build HTML
    QString html;
    html += "<h2>Liste des Commandes du client: " + mailClient + "</h2>";
    html += "<table border='1' cellspacing='0' cellpadding='6' style='border-collapse:collapse;'>";

    // Headers
    html += "<tr style='background-color:#f2f2f2;'>";
    html += "<th>ID</th><th>Date</th><th>Montant</th><th>Statut</th>";
    html += "</tr>";

    // Data
    bool hasData = false;
    while (query.next()) {
        hasData = true;
        html += "<tr>";
        html += "<td>" + query.value(0).toString() + "</td>";
        html += "<td>" + query.value(1).toString() + "</td>";
        html += "<td>" + query.value(2).toString() + "</td>";
        html += "<td>" + query.value(3).toString() + "</td>";
        html += "</tr>";
    }

    html += "</table>";

    if (!hasData) {
        qDebug() << "Aucune commande trouvée pour ce client.";
        return false;
    }

    // 🔹 EXPORT PDF
    if (fileName.endsWith(".pdf", Qt::CaseInsensitive)) {
        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);

        QTextDocument doc;
        doc.setHtml(html);
        doc.print(&printer);
    }

    // 🔹 EXPORT DOC (Word compatible)
    else if (fileName.endsWith(".doc", Qt::CaseInsensitive)) {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return false;

        QTextStream out(&file);
        out << html;
        file.close();
    }

    // 🔹 EXPORT CSV
    else if (fileName.endsWith(".csv", Qt::CaseInsensitive)) {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return false;

        QTextStream out(&file);

        // Headers
        out << "ID,Date,Montant,Statut\n";

        // Re-execute query for CSV
        QSqlQuery queryCsv;
        queryCsv.prepare("SELECT ID_COMMANDE, DATE_CREATION, MONTANT, STATUT "
                         "FROM CUIREA.COMMANDES WHERE MAIL_CLIENT = :mail");
        queryCsv.bindValue(":mail", mailClient);

        if (!queryCsv.exec())
            return false;

        while (queryCsv.next()) {
            out << queryCsv.value(0).toString() << ","
                << queryCsv.value(1).toString() << ","
                << queryCsv.value(2).toString() << ","
                << queryCsv.value(3).toString() << "\n";
        }

        file.close();
    }

    else {
        return false;
    }

    return true;
}

//afficher historique



QSqlQueryModel* Client::afficherHistorique()
{
    QSqlQueryModel *model = new QSqlQueryModel();

    model->setQuery(
        "SELECT EMAIL, DATE_INSCRIPTION "
        "FROM HISTORIQUE_CLIENT "
        "ORDER BY DATE_INSCRIPTION DESC"
        );

    return model;
}


//taux
QMap<QString, int> Client::statistiquesParMois()
{
    QMap<QString, int> stats;

    QSqlQuery query;
    query.prepare(
        "SELECT TO_CHAR(date_inscription, 'YYYY-MM') AS mois, COUNT(*) "
        "FROM clients "
        "GROUP BY TO_CHAR(date_inscription, 'YYYY-MM') "
        "ORDER BY mois"
        );

    if(query.exec())
    {
        while(query.next())
        {
            QString mois = query.value(0).toString();
            int count = query.value(1).toInt();
            stats[mois] = count;
        }
    }

    return stats;
}
