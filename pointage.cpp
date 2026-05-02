#include "pointage.h"
#include "connection.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QDateTime>
#include <QDebug>

static QSqlDatabase db()
{
    QString name = Connection::instance()->getDatabase().connectionName();
    return QSqlDatabase::database(name);
}

Pointage::Pointage()
    : m_id(-1)
    , m_dejaPointe(false)
{}

// ── Trouver employé par UID RFID ────────────────
int Pointage::trouverEmployeParRfid(const QString &uid)
{
    // Normaliser l'UID: majuscules, sans espaces
    QString uidNorm = uid.toUpper().trimmed().replace(" ", "");

    qDebug() << "=== RECHERCHE EMPLOYE ===";
    qDebug() << "UID recu:" << uid;
    qDebug() << "UID normalise:" << uidNorm;

    QSqlQuery query(db());
    query.prepare(
        "SELECT ID_EMPLOYE, NOM, PRENOM, UID_CARTE "
        "FROM CUIREA.EMPLOYES "
        "WHERE UPPER(TRIM(REPLACE(UID_CARTE, ' ', ''))) = :uid "
        "AND ACTIF = 1"
    );
    query.bindValue(":uid", uidNorm);

    if (query.exec()) {
        qDebug() << "Requete executee";
        if (query.next()) {
            m_id = query.value(0).toInt();
            m_nom = query.value(1).toString();
            m_prenom = query.value(2).toString();
            QString uidBdd = query.value(3).toString();
            qDebug() << "TROUVE! ID:" << m_id << "Nom:" << m_nom << "Prenom:" << m_prenom;
            qDebug() << "UID en BDD:" << uidBdd;
            return m_id;
        } else {
            qDebug() << "Aucun employe trouve avec cet UID";
            
            // Debug: lister tous les UID en base
            QSqlQuery debugQuery(db());
            debugQuery.exec("SELECT ID_EMPLOYE, NOM, UID_CARTE FROM CUIREA.EMPLOYES WHERE ACTIF = 1");
            qDebug() << "=== EMPLOYES ACTIFS EN BASE ===";
            while (debugQuery.next()) {
                qDebug() << "ID:" << debugQuery.value(0).toInt() 
                         << "Nom:" << debugQuery.value(1).toString()
                         << "UID:" << debugQuery.value(2).toString();
            }
        }
    } else {
        qDebug() << "Erreur SQL:" << query.lastError().text();
    }

    return -1;
}

// ── Vérifier si déjà pointé aujourd'hui ──────────
bool Pointage::dejaPoinceAujourdhui(int idEmploye)
{
    QSqlQuery query(db());
    query.prepare(
        "SELECT STATUT_POINTAGE, DATE_DERNIER_POINTAGE "
        "FROM CUIREA.EMPLOYES "
        "WHERE ID_EMPLOYE = :id "
        "AND TRUNC(DATE_DERNIER_POINTAGE) = TRUNC(SYSDATE)"
    );
    query.bindValue(":id", idEmploye);

    if (query.exec() && query.next()) {
        QString statut = query.value(0).toString();
        return (statut == "PRESENT");
    }
    return false;
}

// ── Marquer PRESENT ───────────────────────────────
bool Pointage::marquerPresent(const QString &uidRfid)
{
    m_dejaPointe = false;

    int idEmploye = trouverEmployeParRfid(uidRfid);
    if (idEmploye == -1) {
        qDebug() << "Badge non reconnu:" << uidRfid;
        return false;
    }

    // Verifier si deja pointe aujourd'hui
    if (dejaPoinceAujourdhui(idEmploye)) {
        qDebug() << "Deja pointe aujourd'hui:" << m_nom << m_prenom;
        m_dejaPointe = true;
        
        // Marquer comme SORTI (deuxieme badge = sortie)
        QSqlQuery query(db());
        query.prepare(
            "UPDATE CUIREA.EMPLOYES SET "
            "HEURE_DEPART = SYSTIMESTAMP, "
            "STATUT_POINTAGE = 'SORTI' "
            "WHERE ID_EMPLOYE = :id"
        );
        query.bindValue(":id", idEmploye);
        query.exec();
        
        // Mettre a jour l'historique avec heure de depart
        QSqlQuery histQuery(db());
        histQuery.prepare(
            "UPDATE CUIREA.POINTAGE_HISTORIQUE SET "
            "HEURE_DEPART = SYSTIMESTAMP, STATUT = 'SORTI' "
            "WHERE ID_EMPLOYE = :id AND TRUNC(DATE_POINTAGE) = TRUNC(SYSDATE)"
        );
        histQuery.bindValue(":id", idEmploye);
        histQuery.exec();
        
        db().commit();
        return true;
    }

    // Premier pointage du jour
    QSqlDatabase database = db();
    database.transaction();

    // 1. Mettre a jour EMPLOYES
    QSqlQuery query(database);
    query.prepare(
        "UPDATE CUIREA.EMPLOYES SET "
        "DATE_DERNIER_POINTAGE = TRUNC(SYSDATE), "
        "HEURE_ARRIVEE = SYSTIMESTAMP, "
        "HEURE_DEPART = NULL, "
        "STATUT_POINTAGE = 'PRESENT' "
        "WHERE ID_EMPLOYE = :id"
    );
    query.bindValue(":id", idEmploye);

    if (!query.exec()) {
        qDebug() << "Erreur UPDATE EMPLOYES:" << query.lastError().text();
        database.rollback();
        return false;
    }

    // 2. Inserer dans POINTAGE_HISTORIQUE
    QSqlQuery histQuery(database);
    histQuery.prepare(
        "INSERT INTO CUIREA.POINTAGE_HISTORIQUE "
        "(ID_POINTAGE, ID_EMPLOYE, DATE_POINTAGE, HEURE_ARRIVEE, STATUT) "
        "VALUES (CUIREA.SEQ_POINTAGE_HIST.NEXTVAL, :id, TRUNC(SYSDATE), SYSTIMESTAMP, 'PRESENT')"
    );
    histQuery.bindValue(":id", idEmploye);
    
    if (!histQuery.exec()) {
        qDebug() << "Erreur INSERT HISTORIQUE:" << histQuery.lastError().text();
        // Continue quand meme, l'historique n'est pas critique
    }

    if (!database.commit()) {
        qDebug() << "Erreur COMMIT:" << database.lastError().text();
        database.rollback();
        return false;
    }

    qDebug() << "Pointage OK:" << m_prenom << m_nom;
    return true;
}

// ── Marquer ABSENTS tous les non-pointés ─────────
void Pointage::marquerAbsentsJournee()
{
    QSqlDatabase database = db();
    database.transaction();

    QSqlQuery query(database);
    bool ok = query.exec(
        "UPDATE CUIREA.EMPLOYES SET "
        "STATUT_POINTAGE = 'ABSENT', "
        "DATE_DERNIER_POINTAGE = TRUNC(SYSDATE) "
        "WHERE ACTIF = 1 "
        "AND (DATE_DERNIER_POINTAGE IS NULL OR TRUNC(DATE_DERNIER_POINTAGE) < TRUNC(SYSDATE))"
    );

    if (ok) {
        database.commit();
        qDebug() << "Absents marques:" << query.numRowsAffected();
    } else {
        database.rollback();
        qDebug() << "Erreur absents:" << query.lastError().text();
    }
}

// ── Calendrier mensuel d'un employé ──────────────
QSqlQueryModel* Pointage::getCalendrierEmploye(int idEmploye, int mois, int annee)
{
    QSqlQueryModel *model = new QSqlQueryModel();
    
    QSqlQuery query(db());
    query.prepare(
        "SELECT "
        "TO_CHAR(DATE_POINTAGE, 'DD/MM/YYYY') AS \"Date\", "
        "TO_CHAR(HEURE_ARRIVEE, 'HH24:MI:SS') AS \"Arrivée\", "
        "TO_CHAR(HEURE_DEPART, 'HH24:MI:SS') AS \"Départ\", "
        "STATUT AS \"Statut\" "
        "FROM CUIREA.POINTAGE_HISTORIQUE "
        "WHERE ID_EMPLOYE = :id "
        "AND EXTRACT(MONTH FROM DATE_POINTAGE) = :mois "
        "AND EXTRACT(YEAR FROM DATE_POINTAGE) = :annee "
        "ORDER BY DATE_POINTAGE DESC"
    );
    query.bindValue(":id", idEmploye);
    query.bindValue(":mois", mois);
    query.bindValue(":annee", annee);
    
    if (query.exec()) {
        model->setQuery(std::move(query));
    } else {
        qDebug() << "Erreur calendrier:" << query.lastError().text();
    }
    
    return model;
}

// ── Calcul salaire net ────────────────────────────
double Pointage::calculerSalaire(int idEmploye, int mois, int annee)
{
    Q_UNUSED(mois);
    Q_UNUSED(annee);
    
    QSqlQuery q1(db());
    q1.prepare("SELECT NVL(SALAIRE_BASE, 1500) FROM CUIREA.EMPLOYES WHERE ID_EMPLOYE = :id");
    q1.bindValue(":id", idEmploye);
    
    double base = 1500;
    if (q1.exec() && q1.next()) 
        base = q1.value(0).toDouble();

    return base;
}
