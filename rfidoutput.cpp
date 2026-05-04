#include "rfidoutput.h"

// ─────────────────────────────────────────────
// Constructeur / Destructeur
// ─────────────────────────────────────────────

RfidOutput::RfidOutput(QObject *parent)
    : QObject(parent),
      m_serial(new QSerialPort(this))
{
    // Connecte le signal readyRead au slot onDataReceived
    // Des que l'Arduino envoie des donnees, onDataReceived est appele
    connect(m_serial, &QSerialPort::readyRead,
            this,     &RfidOutput::onDataReceived);
}

RfidOutput::~RfidOutput()
{
    if (m_serial->isOpen())
        m_serial->close();
}

// ─────────────────────────────────────────────
// Initialisation du port serie
// ─────────────────────────────────────────────

bool RfidOutput::initSerialPort(const QString &portName, int baudRate)
{
    m_serial->setPortName(portName);           // ex: "COM3" ou "/dev/ttyUSB0"
    m_serial->setBaudRate(baudRate);           // 9600 bauds
    m_serial->setDataBits(QSerialPort::Data8);
    m_serial->setParity(QSerialPort::NoParity);
    m_serial->setStopBits(QSerialPort::OneStop);
    m_serial->setFlowControl(QSerialPort::NoFlowControl);

    if (!m_serial->open(QIODevice::ReadWrite)) {
        qWarning() << "[RFID] Impossible d'ouvrir le port:" << portName
                   << m_serial->errorString();
        return false;
    }

    qDebug() << "[RFID] Port serie ouvert:" << portName << "@" << baudRate << "bauds";
    return true;
}

// ─────────────────────────────────────────────
// Injecter la connexion Oracle existante
// ─────────────────────────────────────────────

void RfidOutput::setDatabase(QSqlDatabase db)
{
    m_db = db;
}


// ─────────────────────────────────────────────
// Slot : donnees recues depuis l'Arduino
// ─────────────────────────────────────────────

void RfidOutput::onDataReceived()
{
    // L'Arduino envoie "UID:A1B2C3D4\n"
    // On attend le \n pour avoir le message complet
    while (m_serial->canReadLine()) {
        QByteArray raw  = m_serial->readLine();
        QString    line = QString(raw).trimmed();  // supprime \r\n

        qDebug() << "[RFID] Recu:" << line;

        // Verifie le format "UID:XXXXXXXX"
        if (line.startsWith("UID:")) {
            QString uid = line.mid(4).trimmed();  // extrait "A1B2C3D4"
            if (!uid.isEmpty()) {
                processUID(uid);
            }
        }
    }
}

// ─────────────────────────────────────────────
// Traitement principal de l'UID recu
// ─────────────────────────────────────────────

void RfidOutput::processUID(const QString &uid)
{
    qDebug() << "[RFID] Traitement UID:" << uid;

    int     idEmploye = -1;
    QString prenom, nom;

    // ── REQUETE 1 : Verifier l'employe ──────────────────────
    if (!verifierEmploye(uid, idEmploye, prenom, nom)) {
        qDebug() << "[RFID] Carte inconnue -> DENIED";
        envoyerReponse("DENIED");
        return;
    }

    qDebug() << "[RFID] Employe trouve:" << prenom << nom
             << "(ID:" << idEmploye << ")";

    // ── REQUETE 2 : Toggle — dernier pointage ───────────────
    QChar dernierType = dernierPointage(idEmploye);
    QChar nouveauType = (dernierType == 'E') ? 'S' : 'E';

    qDebug() << "[RFID] Dernier type:" << dernierType
             << "-> Nouveau type:" << nouveauType;

    // ── REQUETE 3 : Inserer le nouveau pointage ─────────────
    if (!insererPointage(idEmploye, nouveauType)) {
        qWarning() << "[RFID] Echec INSERT pointage";
        envoyerReponse("DENIED");   // securite : on refuse si l'enregistrement echoue
        return;
    }

    // ── Envoyer GRANTED a l'Arduino ─────────────────────────
    // Format : "GRANTED:Prenom:E" ou "GRANTED:Prenom:S"
    QString message = QString("GRANTED:%1:%2").arg(prenom).arg(nouveauType);
    envoyerReponse(message);
}

// ─────────────────────────────────────────────
// REQUETE 1 — SELECT dans EMPLOYE
// ─────────────────────────────────────────────

bool RfidOutput::verifierEmploye(const QString &uid,
                                  int     &idEmploye,
                                  QString &prenom,
                                  QString &nom)
{
    if (!m_db.isOpen()) {
        qWarning() << "[RFID] Base de donnees non connectee";
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare("SELECT ID_EMPLOYE, NOM, PRENOM "
                  "FROM CUIREA.EMPLOYES "
                  "WHERE UID_CARTE = :uid");
    query.bindValue(":uid", uid);

    if (!query.exec()) {
        qWarning() << "[RFID] Erreur SELECT EMPLOYE:"
                   << query.lastError().text();
        return false;
    }

    if (!query.next()) {
        // Aucun employe avec cet UID
        return false;
    }

    idEmploye = query.value("ID_EMPLOYE").toInt();
    nom       = query.value("NOM").toString();
    prenom    = query.value("PRENOM").toString();

    return true;
}


// ─────────────────────────────────────────────
// REQUETE 2 — SELECT dernier pointage (toggle)
// ─────────────────────────────────────────────

QChar RfidOutput::dernierPointage(int idEmploye)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT TYPE_POINTAGE "
                  "FROM POINTAGE "
                  "WHERE ID_EMPLOYE = :id "
                  "ORDER BY DATE_POINTAGE DESC, HEURE_POINTAGE DESC "
                  "FETCH FIRST 1 ROW ONLY");
    query.bindValue(":id", idEmploye);

    if (!query.exec()) {
        qWarning() << "[RFID] Erreur SELECT POINTAGE:"
                   << query.lastError().text();
        // Par defaut : on suppose "sortie" -> prochain sera "entree"
        return 'S';
    }

    if (!query.next()) {
        // Aucun pointage precedent -> c'est une entree
        return 'S';   // toggle : S -> prochain sera E
    }

    QString type = query.value("TYPE_POINTAGE").toString();
    return type.isEmpty() ? QChar('S') : type.at(0);
}

// ─────────────────────────────────────────────
// REQUETE 3 — INSERT INTO POINTAGE
// ─────────────────────────────────────────────

bool RfidOutput::insererPointage(int idEmploye, QChar type)
{
    QSqlQuery query(m_db);

    // SYSDATE gere date ET heure cote Oracle
    // TO_CHAR(...,'HH24:MI:SS') donne l'heure formatee en texte
    query.prepare("INSERT INTO POINTAGE "
                  "(ID_EMPLOYE, DATE_POINTAGE, HEURE_POINTAGE, TYPE_POINTAGE) "
                  "VALUES (:id, SYSDATE, TO_CHAR(SYSDATE,'HH24:MI:SS'), :type)");

    query.bindValue(":id",   idEmploye);
    query.bindValue(":type", QString(type));

    if (!query.exec()) {
        qWarning() << "[RFID] Erreur INSERT POINTAGE:"
                   << query.lastError().text();
        return false;
    }

    qDebug() << "[RFID] Pointage enregistre — type:" << type;
    return true;
}

// ─────────────────────────────────────────────
// Envoyer la reponse a l'Arduino
// ─────────────────────────────────────────────

void RfidOutput::envoyerReponse(const QString &message)
{
    // Mode simulation : afficher une boite de dialogue au lieu d'envoyer a l'Arduino
    if (m_simulationMode) {
        QString titre, texte;
        if (message.startsWith("GRANTED:")) {
            // Format: "GRANTED:Prenom:E" ou "GRANTED:Prenom:S"
            QStringList parts = message.split(':');
            QString prenom = parts.value(1);
            QString type = parts.value(2);
            
            if (type == "E") {
                titre = "ENTREE AUTORISEE";
                texte = QString("Bienvenue %1 !\n\n"
                               "Pointage ENTREE enregistre.\n"
                               "LED verte allumee\n"
                               "Servo ouvre la porte").arg(prenom);
            } else {
                titre = "SORTIE AUTORISEE";
                texte = QString("Au revoir %1 !\n\n"
                               "Pointage SORTIE enregistre.\n"
                               "LED verte allumee\n"
                               "Servo ouvre la porte").arg(prenom);
            }
            QMessageBox::information(nullptr, titre, texte);
        } else if (message == "DENIED") {
            titre = "ACCES REFUSE";
            texte = "Carte inconnue !\n\n"
                   "LED rouge allumee\n"
                   "3 bips d'erreur\n"
                   "Porte reste fermee";
            QMessageBox::critical(nullptr, titre, texte);
        }
        qDebug() << "[RFID SIMULATION] Reponse:" << message;
        return;
    }

    if (!m_serial->isOpen()) {
        qWarning() << "[RFID] Port serie ferme, impossible d'envoyer";
        return;
    }

    // Ajoute \n pour que l'Arduino puisse detecter la fin du message
    QString full = message + "\n";
    m_serial->write(full.toUtf8());

    qDebug() << "[RFID] Envoye a Arduino:" << message;
}

// ─────────────────────────────────────────────
// TEST : Simuler un badge sans Arduino
// ─────────────────────────────────────────────

void RfidOutput::simulerBadge(const QString &uid)
{
    m_simulationMode = true;
    qDebug() << "[RFID SIMULATION] Badge simule avec UID:" << uid;
    processUID(uid);
    m_simulationMode = false;
}
