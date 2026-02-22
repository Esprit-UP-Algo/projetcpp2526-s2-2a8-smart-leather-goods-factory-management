#include "client.h"
#include <QSqlQuery>
#include <QSqlQueryModel>


bool Client::ajouter()
{
    QSqlQuery query;
    query.prepare("INSERT INTO client (id_client, nom, prenom, sexe, cin, pays, ville, adresse, email, dateinscrit) "
                  "VALUES (:id_client, :nom, :prenom, :sexe, :cin, :pays, :ville, :adresse, :email, :dateinscrit)");

    query.bindValue(":id_client", id_client);
    query.bindValue(":nom", nom);
    query.bindValue(":prenom", prenom);
    query.bindValue(":sexe", sexe);
    query.bindValue(":cin", cin);
    query.bindValue(":pays", pays);
    query.bindValue(":ville", ville);
    query.bindValue(":adresse", adresse);
    query.bindValue(":email", email);
    query.bindValue(":dateinscrit", dateInscrit);

    return query.exec();
}

bool Client::modifier()
{
    QSqlQuery query;
    query.prepare("UPDATE client SET "
                  "nom = :nom, prenom = :prenom, sexe = :sexe, cin = :cin, "
                  "pays = :pays, ville = :ville, adresse = :adresse, email = :email, "
                  "date_inscription = :dateinscrit "
                  "WHERE Id_Client = :id_client");
    query.bindValue(":id_client", id_client);
    query.bindValue(":nom", nom);
    query.bindValue(":prenom", prenom);
    query.bindValue(":sexe", sexe);
    query.bindValue(":cin", cin);
    query.bindValue(":pays", pays);
    query.bindValue(":ville", ville);
    query.bindValue(":adresse", adresse);
    query.bindValue(":email", email);
    query.bindValue(":dateInscrit", dateInscrit);


    return query.exec();
}

bool Client::supprimer(int id_client)
{
    QSqlQuery query;
    query.prepare("DELETE FROM client WHERE id_client = :id_client");
    query.bindValue(":id_client", id_client);

    return query.exec();
}

QSqlQueryModel* Client::afficherClients()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM client");
    return model;
}
