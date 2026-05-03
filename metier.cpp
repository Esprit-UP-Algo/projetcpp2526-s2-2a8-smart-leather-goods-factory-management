#include "metier.h"
#include "connection.h"
#include <QSqlError>
#include <QDebug>

Metier::Metier()
    : id(0)
    , salaireBase(0.0)
    , actif(true)
{
}

bool Metier::ajouter()
{
    QSqlDatabase db = Connection::instance()->getDatabase();
    QSqlQuery query(db);

    query.prepare(
        "INSERT INTO CUIREA.METIERS "
        "(ID_METIER, CODE, LIBELLE, DESCRIPTION, DOMAINE, NIVEAU_QUALIFICATION, SALAIRE_BASE, ACTIF) "
        "VALUES (SEQ_METIER.NEXTVAL, :code, :libelle, :description, :domaine, :niveau, :salaire, :actif)"
    );
    query.bindValue(":code",        code.toUpper().trimmed());
    query.bindValue(":libelle",     libelle);
    query.bindValue(":description", description.isEmpty() ? QVariant(QVariant::String) : description);
    query.bindValue(":domaine",     domaine);
    query.bindValue(":niveau",      niveauQualification);
    query.bindValue(":salaire",     salaireBase);
    query.bindValue(":actif",       actif ? 1 : 0);

    if (!query.exec()) {
        qDebug() << "❌ Erreur ajout métier:" << query.lastError().text();
        return false;
    }
    db.commit();
    qDebug() << "✅ Métier ajouté:" << libelle;
    return true;
}

bool Metier::modifier()
{
    QSqlQuery query(Connection::instance()->getDatabase());
    query.prepare(
        "UPDATE CUIREA.METIERS SET "
        "CODE = :code, LIBELLE = :libelle, DESCRIPTION = :description, "
        "DOMAINE = :domaine, NIVEAU_QUALIFICATION = :niveau, "
        "SALAIRE_BASE = :salaire, ACTIF = :actif "
        "WHERE ID_METIER = :id"
    );
    query.bindValue(":id",          id);
    query.bindValue(":code",        code.toUpper().trimmed());
    query.bindValue(":libelle",     libelle);
    query.bindValue(":description", description.isEmpty() ? QVariant(QVariant::String) : description);
    query.bindValue(":domaine",     domaine);
    query.bindValue(":niveau",      niveauQualification);
    query.bindValue(":salaire",     salaireBase);
    query.bindValue(":actif",       actif ? 1 : 0);

    if (!query.exec()) {
        qDebug() << "❌ Erreur modification métier:" << query.lastError().text();
        return false;
    }
    if (query.numRowsAffected() == 0) {
        qDebug() << "⚠️ Aucune ligne modifiée pour ID_METIER=" << id;
        return false;
    }
    Connection::instance()->getDatabase().commit();
    qDebug() << "✅ Métier modifié ID=" << id;
    return true;
}

bool Metier::supprimer(int metierID)
{
    // Vérifier si des employés utilisent ce métier
    QSqlQuery check(Connection::instance()->getDatabase());
    check.prepare("SELECT COUNT(*) FROM CUIREA.EMPLOYES WHERE UPPER(POSTE) = "
                  "(SELECT UPPER(LIBELLE) FROM CUIREA.METIERS WHERE ID_METIER = :id)");
    check.bindValue(":id", metierID);
    if (check.exec() && check.next() && check.value(0).toInt() > 0) {
        qDebug() << "⚠️ Métier utilisé par des employés, suppression refusée";
        return false;
    }

    QSqlQuery query(Connection::instance()->getDatabase());
    query.prepare("DELETE FROM CUIREA.METIERS WHERE ID_METIER = :id");
    query.bindValue(":id", metierID);
    if (!query.exec()) {
        qDebug() << "❌ Erreur suppression métier:" << query.lastError().text();
        return false;
    }
    Connection::instance()->getDatabase().commit();
    return true;
}

QSqlQueryModel* Metier::afficher()
{
    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery(
        "SELECT ID_METIER, CODE, LIBELLE, DOMAINE, NIVEAU_QUALIFICATION, SALAIRE_BASE, ACTIF "
        "FROM CUIREA.METIERS ORDER BY LIBELLE",
        Connection::instance()->getDatabase()
    );

    if (model->lastError().isValid()) {
        qDebug() << "❌ Erreur affichage métiers:" << model->lastError().text();
        delete model;
        return nullptr;
    }

    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Code");
    model->setHeaderData(2, Qt::Horizontal, "Libellé");
    model->setHeaderData(3, Qt::Horizontal, "Domaine");
    model->setHeaderData(4, Qt::Horizontal, "Niveau");
    model->setHeaderData(5, Qt::Horizontal, "Salaire Base");
    model->setHeaderData(6, Qt::Horizontal, "Actif");
    return model;
}

QSqlQueryModel* Metier::rechercher(const QString &terme)
{
    QSqlQueryModel *model = new QSqlQueryModel();
    QString sql = QString(
        "SELECT ID_METIER, CODE, LIBELLE, DOMAINE, NIVEAU_QUALIFICATION, SALAIRE_BASE, ACTIF "
        "FROM CUIREA.METIERS "
        "WHERE UPPER(CODE) LIKE UPPER('%%1%') "
        "OR UPPER(LIBELLE) LIKE UPPER('%%1%') "
        "OR UPPER(DOMAINE) LIKE UPPER('%%1%') "
        "OR UPPER(NIVEAU_QUALIFICATION) LIKE UPPER('%%1%') "
        "ORDER BY LIBELLE"
    ).arg(terme);

    model->setQuery(sql, Connection::instance()->getDatabase());

    if (model->lastError().isValid()) {
        qDebug() << "❌ Erreur recherche métiers:" << model->lastError().text();
        delete model;
        return nullptr;
    }

    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Code");
    model->setHeaderData(2, Qt::Horizontal, "Libellé");
    model->setHeaderData(3, Qt::Horizontal, "Domaine");
    model->setHeaderData(4, Qt::Horizontal, "Niveau");
    model->setHeaderData(5, Qt::Horizontal, "Salaire Base");
    model->setHeaderData(6, Qt::Horizontal, "Actif");
    return model;
}

QSqlQueryModel* Metier::trierPar(const QString &colonne)
{
    // Whitelist to prevent SQL injection
    static const QStringList allowed = {"LIBELLE","CODE","DOMAINE","NIVEAU_QUALIFICATION","SALAIRE_BASE"};
    QString col = colonne.toUpper();
    if (!allowed.contains(col)) col = "LIBELLE";

    QSqlQueryModel *model = new QSqlQueryModel();
    model->setQuery(
        QString("SELECT ID_METIER, CODE, LIBELLE, DOMAINE, NIVEAU_QUALIFICATION, SALAIRE_BASE, ACTIF "
                "FROM CUIREA.METIERS ORDER BY %1").arg(col),
        Connection::instance()->getDatabase()
    );

    if (model->lastError().isValid()) {
        qDebug() << "❌ Erreur tri métiers:" << model->lastError().text();
    }

    model->setHeaderData(0, Qt::Horizontal, "ID");
    model->setHeaderData(1, Qt::Horizontal, "Code");
    model->setHeaderData(2, Qt::Horizontal, "Libellé");
    model->setHeaderData(3, Qt::Horizontal, "Domaine");
    model->setHeaderData(4, Qt::Horizontal, "Niveau");
    model->setHeaderData(5, Qt::Horizontal, "Salaire Base");
    model->setHeaderData(6, Qt::Horizontal, "Actif");
    return model;
}

int Metier::getTotalMetiers()
{
    QSqlQuery q(Connection::instance()->getDatabase());
    q.prepare("SELECT COUNT(*) FROM CUIREA.METIERS");
    if (q.exec() && q.next()) return q.value(0).toInt();
    return 0;
}

int Metier::getNombreDomaines()
{
    QSqlQuery q(Connection::instance()->getDatabase());
    q.prepare("SELECT COUNT(DISTINCT DOMAINE) FROM CUIREA.METIERS");
    if (q.exec() && q.next()) return q.value(0).toInt();
    return 0;
}

QMap<QString,int> Metier::getEffectifParDomaine()
{
    QMap<QString,int> stats;
    QSqlQuery q(Connection::instance()->getDatabase());
    q.prepare("SELECT DOMAINE, COUNT(*) FROM CUIREA.METIERS GROUP BY DOMAINE ORDER BY COUNT(*) DESC");
    if (q.exec()) {
        while (q.next())
            stats[q.value(0).toString()] = q.value(1).toInt();
    }
    return stats;
}

QMap<QString,int> Metier::getEffectifParNiveau()
{
    QMap<QString,int> stats;
    QSqlQuery q(Connection::instance()->getDatabase());
    q.prepare("SELECT NIVEAU_QUALIFICATION, COUNT(*) FROM CUIREA.METIERS "
              "GROUP BY NIVEAU_QUALIFICATION ORDER BY NIVEAU_QUALIFICATION");
    if (q.exec()) {
        while (q.next())
            stats[q.value(0).toString()] = q.value(1).toInt();
    }
    return stats;
}
