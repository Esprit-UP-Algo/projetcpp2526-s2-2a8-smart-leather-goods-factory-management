#include "matiere.h"
#include "connection.h"
#include <QSqlError>
#include <QDebug>

Matiere::Matiere()
    : id(0)
    , quantite(0.0)
    , seuil(0)
    , idFournisseur(1)
{
}

// ══════════════════════════════════════════════════════════════════════════════
// MÉTHODES CRUD - Opérations Base de Données
// ══════════════════════════════════════════════════════════════════════════════

bool Matiere::ajouter()
{
    QSqlQuery query(Connection::instance()->getDatabase());
    
    // Trouver le prochain ID disponible
    int newId = 0;
    
    // Méthode 1: Chercher le plus grand ID et ajouter 1
    QSqlQuery maxQuery(Connection::instance()->getDatabase());
    if (maxQuery.exec("SELECT NVL(MAX(ID_MATIERE), 0) FROM MATIERES_PREMIERES") && maxQuery.next()) {
        int maxId = maxQuery.value(0).toInt();
        newId = maxId + 1;
        qDebug() << "📊 MAX ID actuel:" << maxId << "→ Nouvel ID:" << newId;
    }
    
    // Vérification: cet ID existe-t-il déjà?
    QSqlQuery checkQuery(Connection::instance()->getDatabase());
    checkQuery.prepare("SELECT COUNT(*) FROM MATIERES_PREMIERES WHERE ID_MATIERE = :id");
    checkQuery.bindValue(":id", newId);
    
    if (checkQuery.exec() && checkQuery.next()) {
        int count = checkQuery.value(0).toInt();
        if (count > 0) {
            qDebug() << "⚠️ ID" << newId << "existe déjà! Recherche d'un ID libre...";
            
            // Chercher le premier gap dans les IDs
            QSqlQuery gapQuery(Connection::instance()->getDatabase());
            if (gapQuery.exec(
                "SELECT MIN(ID_MATIERE) + 1 "
                "FROM MATIERES_PREMIERES t1 "
                "WHERE NOT EXISTS (SELECT 1 FROM MATIERES_PREMIERES t2 WHERE t2.ID_MATIERE = t1.ID_MATIERE + 1) "
                "AND ID_MATIERE < (SELECT MAX(ID_MATIERE) FROM MATIERES_PREMIERES)") 
                && gapQuery.next() && !gapQuery.value(0).isNull()) {
                newId = gapQuery.value(0).toInt();
                qDebug() << "✅ Gap trouvé, utilisation de l'ID:" << newId;
            } else {
                // Dernier recours: utiliser la séquence
                QSqlQuery seqQuery(Connection::instance()->getDatabase());
                if (seqQuery.exec("SELECT SEQ_MATIERES.NEXTVAL FROM DUAL") && seqQuery.next()) {
                    newId = seqQuery.value(0).toInt();
                    qDebug() << "🔄 Utilisation de la séquence, ID:" << newId;
                }
            }
        }
    }
    
    if (newId <= 0) {
        qDebug() << "❌ Impossible de générer un ID valide";
        return false;
    }
    
    qDebug() << "✅ ID final sélectionné:" << newId;
    
    query.prepare("INSERT INTO MATIERES_PREMIERES (ID_MATIERE, NOM, TYPE_MATIERE, "
                  "QUANTITE_ACTUELLE, SEUIL, DATE_EXPIRATION, REFERENCE, PHOTO_URL, ID_FOURNISSEUR) "
                  "VALUES (:id, :nom, :type, :quantite, :seuil, "
                  ":dateExpiration, :reference, :photoUrl, :idFournisseur)");
    
    query.bindValue(":id", newId);
    query.bindValue(":nom", nom);
    query.bindValue(":type", type);
    query.bindValue(":quantite", quantite);
    query.bindValue(":seuil", seuil);
    query.bindValue(":dateExpiration", dateExpiration);
    query.bindValue(":reference", reference);
    query.bindValue(":photoUrl", photoUrl);
    query.bindValue(":idFournisseur", idFournisseur);
    
    qDebug() << "🔍 DEBUG - Valeurs avant INSERT:";
    qDebug() << "   ID:" << newId;
    qDebug() << "   nom:" << nom;
    qDebug() << "   type:" << type;
    qDebug() << "   quantite:" << quantite;
    qDebug() << "   seuil:" << seuil;
    qDebug() << "   idFournisseur:" << idFournisseur;
    
    if (!query.exec()) {
        qDebug() << "❌ Erreur ajout matière:" << query.lastError().text();
        qDebug() << "   Query:" << query.lastQuery();
        return false;
    }
    
    qDebug() << "✅ Matière ajoutée avec succès";
    return true;
}

bool Matiere::modifier()
{
    QSqlQuery query(Connection::instance()->getDatabase());
    query.prepare("UPDATE MATIERES_PREMIERES SET "
                  "NOM = :nom, TYPE_MATIERE = :type, QUANTITE_ACTUELLE = :quantite, "
                  "SEUIL = :seuil, DATE_EXPIRATION = :dateExpiration, "
                  "REFERENCE = :reference, PHOTO_URL = :photoUrl "
                  "WHERE ID_MATIERE = :id");
    
    query.bindValue(":id", id);
    query.bindValue(":nom", nom);
    query.bindValue(":type", type);
    query.bindValue(":quantite", quantite);
    query.bindValue(":seuil", seuil);
    query.bindValue(":dateExpiration", dateExpiration);
    query.bindValue(":reference", reference);
    query.bindValue(":photoUrl", photoUrl);
    
    if (!query.exec()) {
        qDebug() << "❌ Erreur modification matière:" << query.lastError().text();
        return false;
    }
    
    qDebug() << "✅ Matière modifiée avec succès";
    return true;
}

bool Matiere::supprimer(int id)
{
    QSqlQuery query(Connection::instance()->getDatabase());
    query.prepare("DELETE FROM MATIERES_PREMIERES WHERE ID_MATIERE = :id");
    query.bindValue(":id", id);
    
    if (!query.exec()) {
        qDebug() << "❌ Erreur suppression matière:" << query.lastError().text();
        return false;
    }
    
    qDebug() << "✅ Matière supprimée avec succès";
    return true;
}

QSqlQueryModel* Matiere::afficher()
{
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT ID_MATIERE, NOM, REFERENCE, TYPE_MATIERE, QUANTITE_ACTUELLE, "
                    "SEUIL, DATE_EXPIRATION, PHOTO_URL FROM MATIERES_PREMIERES ORDER BY NOM",
                    Connection::instance()->getDatabase());
    
    if (model->lastError().isValid()) {
        qDebug() << "❌ Erreur affichage matières:" << model->lastError().text();
        delete model;
        return nullptr;
    }
    
    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Nom");
    model->setHeaderData(2, Qt::Horizontal, "Référence");
    model->setHeaderData(3, Qt::Horizontal, "Type");
    model->setHeaderData(4, Qt::Horizontal, "Quantité");
    model->setHeaderData(5, Qt::Horizontal, "Seuil");
    model->setHeaderData(6, Qt::Horizontal, "Date Expiration");
    model->setHeaderData(7, Qt::Horizontal, "Photo");
    
    return model;
}

QSqlQueryModel* Matiere::rechercher(const QString &terme)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    
    QString queryStr = QString(
        "SELECT ID_MATIERE, NOM, REFERENCE, TYPE_MATIERE, QUANTITE_ACTUELLE, "
        "SEUIL, DATE_EXPIRATION, PHOTO_URL FROM MATIERES_PREMIERES "
        "WHERE UPPER(NOM) LIKE UPPER('%%1%') "
        "OR UPPER(REFERENCE) LIKE UPPER('%%1%') "
        "OR UPPER(TYPE_MATIERE) LIKE UPPER('%%1%') "
        "ORDER BY NOM"
    ).arg(terme);
    
    model->setQuery(queryStr, Connection::instance()->getDatabase());
    
    if (model->lastError().isValid()) {
        qDebug() << "❌ Erreur recherche matières:" << model->lastError().text();
        delete model;
        return nullptr;
    }
    
    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Nom");
    model->setHeaderData(2, Qt::Horizontal, "Référence");
    model->setHeaderData(3, Qt::Horizontal, "Type");
    model->setHeaderData(4, Qt::Horizontal, "Quantité");
    model->setHeaderData(5, Qt::Horizontal, "Seuil");
    model->setHeaderData(6, Qt::Horizontal, "Date Expiration");
    model->setHeaderData(7, Qt::Horizontal, "Photo");
    
    return model;
}

QSqlQueryModel* Matiere::trierPar(const QString &colonne)
{
    QSqlQueryModel* model = new QSqlQueryModel();
    
    QString queryStr = QString(
        "SELECT ID_MATIERE, NOM, REFERENCE, TYPE_MATIERE, QUANTITE_ACTUELLE, "
        "SEUIL, DATE_EXPIRATION, PHOTO_URL FROM MATIERES_PREMIERES ORDER BY %1"
    ).arg(colonne);
    
    model->setQuery(queryStr, Connection::instance()->getDatabase());
    
    if (model->lastError().isValid()) {
        qDebug() << "❌ Erreur tri matières:" << model->lastError().text();
        delete model;
        return nullptr;
    }
    
    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Nom");
    model->setHeaderData(2, Qt::Horizontal, "Référence");
    model->setHeaderData(3, Qt::Horizontal, "Type");
    model->setHeaderData(4, Qt::Horizontal, "Quantité");
    model->setHeaderData(5, Qt::Horizontal, "Seuil");
    model->setHeaderData(6, Qt::Horizontal, "Date Expiration");
    model->setHeaderData(7, Qt::Horizontal, "Photo");
    
    return model;
}