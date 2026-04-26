#include "pointage.h"
#include "connection.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QDateTime>
#include <QDebug>

// Helper : retourne toujours la vraie connexion active
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
    QString uidNorm = uid.toUpper().replace(" ", "");

    qDebug() << "Recherche employé pour UID:" << uid;
    qDebug() << "UID normalisé:" << uidNorm;

    QSqlQuery query(db());
    query.prepare(
        "SELECT ID_EMPLOYE, NOM, PRENOM "
        "FROM CUIREA.EMPLOYES "
        "WHERE REPLACE(UPPER(RFID_UID),' ','') = :uid "
        "AND ACTIF = 1"
    );
    query.bindValue(":uid", uidNorm);

    if (query.exec() && query.next()) {
        m_id     = query.value(0).toInt();
        m_nom    = query.value(1).toString();
        m_prenom = query.value(2).toString();
        qDebug() << "✅ Employé trouvé - ID:" << m_id
                 << "Nom:" << m_nom << "Prénom:" << m_prenom;
        return m_id;
    }

    qDebug() << "❌ Badge non trouvé:" << uidNorm;
    if (query.lastError().isValid())
        qDebug() << "❌ Erreur SQL:" << query.lastError().text();
    return -1;
}

// ── Vérifier si déjà pointé aujourd'hui ──────────
bool Pointage::dejaPoinceAujourdhui(int idEmploye)
{
    QSqlQuery query(db());
    query.prepare(
        "SELECT COUNT(*) FROM CUIREA.POINTAGE "
        "WHERE ID_EMPLOYE = :id "
        "AND TRUNC(DATE_POINTAGE) = TRUNC(SYSDATE)"
    );
    query.bindValue(":id", idEmploye);

    if (query.exec() && query.next())
        return query.value(0).toInt() > 0;
    return false;
}

// ── Marquer PRESENT ───────────────────────────────
bool Pointage::marquerPresent(const QString &uidRfid)
{
    m_dejaPointe = false;

    int idEmploye = trouverEmployeParRfid(uidRfid);
    if (idEmploye == -1) {
        qDebug() << "❌ Employé non trouvé pour UID:" << uidRfid;
        return false;
    }

    if (dejaPoinceAujourdhui(idEmploye)) {
        qDebug() << "⚠️ Déjà pointé:" << m_nom;
        m_dejaPointe = true;
        return true;
    }

    // ✅ Utiliser QSqlDatabase::database(name) — identique au bouton test
    QSqlDatabase database = db();

    if (!database.isOpen()) {
        qDebug() << "❌ Base de données non ouverte!";
        return false;
    }

    database.transaction();

    QSqlQuery query(database);
    bool ok = query.exec(
        "INSERT INTO CUIREA.POINTAGE "
        "(ID_POINTAGE, ID_EMPLOYE, DATE_POINTAGE, HEURE_ARRIVEE, STATUT) "
        "VALUES (SEQ_POINTAGE.NEXTVAL, " + QString::number(idEmploye) + ", "
        "TRUNC(SYSDATE), SYSTIMESTAMP, 'PRESENT')"
    );

    if (!ok) {
        qDebug() << "❌ Erreur INSERT:" << query.lastError().text();
        qDebug() << "❌ Code natif:" << query.lastError().nativeErrorCode();
        database.rollback();
        return false;
    }

    qDebug() << "INSERT OK, lignes:" << query.numRowsAffected();

    if (!database.commit()) {
        qDebug() << "❌ Erreur COMMIT:" << database.lastError().text();
        database.rollback();
        return false;
    }

    qDebug() << "✅ Pointage OK:" << m_nom << m_prenom << "(ID=" << idEmploye << ")";
    return true;
}

// ── Marquer ABSENTS tous les non-pointés ─────────
void Pointage::marquerAbsentsJournee()
{
    QSqlDatabase database = db();
    database.transaction();

    QSqlQuery query(database);
    bool ok = query.exec(
        "INSERT INTO CUIREA.POINTAGE "
        "(ID_POINTAGE, ID_EMPLOYE, DATE_POINTAGE, STATUT) "
        "SELECT SEQ_POINTAGE.NEXTVAL, E.ID_EMPLOYE, TRUNC(SYSDATE), 'ABSENT' "
        "FROM CUIREA.EMPLOYES E "
        "WHERE E.ACTIF = 1 "
        "AND E.ID_EMPLOYE NOT IN ("
        "   SELECT P.ID_EMPLOYE FROM CUIREA.POINTAGE P "
        "   WHERE TRUNC(P.DATE_POINTAGE) = TRUNC(SYSDATE)"
        ")"
    );

    if (ok) {
        database.commit();
        qDebug() << "✅ Absents marqués:" << query.numRowsAffected();
    } else {
        database.rollback();
        qDebug() << "❌ Erreur absents:" << query.lastError().text();
    }
}

// ── Calendrier mensuel d'un employé ──────────────
QSqlQueryModel* Pointage::getCalendrierEmploye(int idEmploye, int mois, int annee)
{
    QSqlQueryModel *model = new QSqlQueryModel();
    QString sql = QString(
        "SELECT "
        "  TO_CHAR(DATE_POINTAGE,'DD/MM/YYYY') AS JOUR, "
        "  TO_CHAR(HEURE_ARRIVEE,'HH24:MI')    AS HEURE, "
        "  STATUT "
        "FROM CUIREA.POINTAGE "
        "WHERE ID_EMPLOYE = %1 "
        "  AND EXTRACT(MONTH FROM DATE_POINTAGE) = %2 "
        "  AND EXTRACT(YEAR  FROM DATE_POINTAGE) = %3 "
        "ORDER BY DATE_POINTAGE"
    ).arg(idEmploye).arg(mois).arg(annee);

    model->setQuery(sql, db());
    model->setHeaderData(0, Qt::Horizontal, "Date");
    model->setHeaderData(1, Qt::Horizontal, "Heure arrivée");
    model->setHeaderData(2, Qt::Horizontal, "Statut");
    return model;
}

// ── Calcul salaire net ────────────────────────────
double Pointage::calculerSalaire(int idEmploye, int mois, int annee)
{
    QSqlQuery q1(db());
    q1.prepare("SELECT NVL(SALAIRE_BASE,1500) FROM CUIREA.EMPLOYES WHERE ID_EMPLOYE=:id");
    q1.bindValue(":id", idEmploye);
    double base = 1500;
    if (q1.exec() && q1.next()) base = q1.value(0).toDouble();

    QSqlQuery q2(db());
    q2.prepare(
        "SELECT COUNT(*) FROM CUIREA.POINTAGE "
        "WHERE ID_EMPLOYE=:id AND STATUT='ABSENT' "
        "AND EXTRACT(MONTH FROM DATE_POINTAGE)=:m "
        "AND EXTRACT(YEAR  FROM DATE_POINTAGE)=:y"
    );
    q2.bindValue(":id", idEmploye);
    q2.bindValue(":m",  mois);
    q2.bindValue(":y",  annee);

    int absences = 0;
    if (q2.exec() && q2.next()) absences = q2.value(0).toInt();

    double deduction = (base / 30.0) * absences;
    double net       = base - deduction;

    qDebug() << "Salaire base:" << base
             << "| Absences:" << absences
             << "| Déduction:" << deduction
             << "| Net:" << net;
    return net;
}