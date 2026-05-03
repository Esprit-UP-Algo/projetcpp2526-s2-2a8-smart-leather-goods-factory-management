#include "arduinomanager.h"
#include "arduinoconnection.h"
#include <QSerialPort>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <QThread>

ArduinoManager::ArduinoManager(QObject *parent)
    : QObject(parent)
    , m_timeoutTimer(new QTimer(this))
{
    m_timeoutTimer->setSingleShot(true);
    connect(m_timeoutTimer, &QTimer::timeout, this, &ArduinoManager::onRoleTimeout);
}

ArduinoManager::~ArduinoManager()
{
    // Fermer les ports temporaires encore ouverts
    for (auto *port : m_pendingPorts)
        if (port->isOpen()) port->close();
    qDeleteAll(m_pendingPorts);
}

// ─────────────────────────────────────────────────────────────────────────────
//  loadForcedRoles — lit ARDUINO_PORTS=COM6:LIVRAISON,COM7:TEMP_BALANCE depuis .env
//  Retourne un map port → rôle. Si pas de rôle spécifié, rôle = ""
// ─────────────────────────────────────────────────────────────────────────────
static QMap<QString,QString> loadForcedRoles()
{
    QMap<QString,QString> result;

    // Chercher .env dans le répertoire courant ET dans le répertoire de l'exécutable
    QStringList candidates = {
        ".env",
        QCoreApplication::applicationDirPath() + "/.env",
        QCoreApplication::applicationDirPath() + "/../.env",
        QCoreApplication::applicationDirPath() + "/../../.env"
    };

    QFile f;
    for (const QString &path : candidates) {
        f.setFileName(path);
        if (f.exists()) {
            qDebug() << "📂 .env trouvé:" << path;
            break;
        }
    }

    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "⚠️ .env introuvable — pas de rôles forcés";
        return result;
    }
    QTextStream in(&f);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.startsWith("ARDUINO_PORTS=")) continue;
        QStringList entries = line.mid(14).split(',', Qt::SkipEmptyParts);
        for (const QString &entry : entries) {
            QStringList parts = entry.trimmed().split(':');
            QString port = parts.value(0).trimmed();
            QString role = parts.value(1).trimmed();
            if (!port.isEmpty()) {
                result[port] = role;
                qDebug() << "📌 Rôle forcé:" << port << "→" << role;
            }
        }
        break;
    }
    f.close();
    return result;
}

// ─────────────────────────────────────────────────────────────────────────────
//  scanAndIdentify — point d'entrée public
// ─────────────────────────────────────────────────────────────────────────────
void ArduinoManager::scanAndIdentify()
{
    QMap<QString,QString> forcedRoles = loadForcedRoles();
    QList<QString> ports = ArduinoConnection::scanAllPorts();
    m_totalPorts = ports.size();
    m_identified = 0;

    if (m_totalPorts == 0) {
        qDebug() << "⚠️ ArduinoManager: aucun Arduino détecté";
        emit discoveryComplete();
        return;
    }

    qDebug() << "🔍 ArduinoManager: identification de" << m_totalPorts << "carte(s)...";

    for (const QString &portName : ports) {
        // Si le rôle est forcé dans .env → connexion directe, pas de double reset
        if (forcedRoles.contains(portName) && !forcedRoles[portName].isEmpty()) {
            qDebug() << "📌 Rôle forcé pour" << portName << "→" << forcedRoles[portName];
            assignRole(forcedRoles[portName], portName);
        } else {
            // Sinon attendre le message ROLE: de l'Arduino
            tryIdentifyPort(portName);
        }
    }

    // Timeout global uniquement si des ports sont encore en attente
    if (!m_pendingPorts.isEmpty())
        m_timeoutTimer->start(ROLE_TIMEOUT_MS);
    else
        checkAllIdentified();
}

// ─────────────────────────────────────────────────────────────────────────────
//  tryIdentifyPort — ouvre un port temporaire et écoute le ROLE:
// ─────────────────────────────────────────────────────────────────────────────
void ArduinoManager::tryIdentifyPort(const QString &portName)
{
    QSerialPort *port = new QSerialPort(this);
    port->setPortName(portName);
    port->setBaudRate(QSerialPort::Baud9600);
    port->setDataBits(QSerialPort::Data8);
    port->setParity(QSerialPort::NoParity);
    port->setStopBits(QSerialPort::OneStop);
    port->setFlowControl(QSerialPort::NoFlowControl);

    if (!port->open(QSerialPort::ReadWrite)) {
        qDebug() << "⚠️ Impossible d'ouvrir" << portName << ":" << port->errorString();
        delete port;
        m_totalPorts--;
        checkAllIdentified();
        return;
    }

    m_pendingPorts[portName]   = port;
    m_pendingBuffers[portName] = QByteArray();

    // Écouter les données reçues
    connect(port, &QSerialPort::readyRead, this, [this, portName]() {
        if (!m_pendingPorts.contains(portName)) return;

        QSerialPort *p = m_pendingPorts[portName];
        m_pendingBuffers[portName] += p->readAll();

        // Chercher une ligne complète
        QByteArray &buf = m_pendingBuffers[portName];
        while (buf.contains('\n')) {
            int idx = buf.indexOf('\n');
            QString line = QString::fromUtf8(buf.left(idx)).trimmed();
            buf.remove(0, idx + 1);

            qDebug() << "📡" << portName << "→" << line;

            if (line.startsWith("ROLE:")) {
                QString role = line.mid(5).trimmed();
                assignRole(role, portName);
                return;
            }
        }
    });

    qDebug() << "⏳ En attente ROLE: sur" << portName;
}

// ─────────────────────────────────────────────────────────────────────────────
//  assignRole — crée l'instance Arduino définitive pour ce rôle
// ─────────────────────────────────────────────────────────────────────────────
void ArduinoManager::assignRole(const QString &role, const QString &portName)
{
    // Fermer et supprimer le port temporaire
    if (m_pendingPorts.contains(portName)) {
        QSerialPort *tmp = m_pendingPorts.take(portName);
        tmp->close();
        tmp->deleteLater();
        m_pendingBuffers.remove(portName);
    }

    // Créer l'instance Arduino définitive
    Arduino *arduino = new Arduino(this);
    arduino->setRole(role);

    // Retry avec délai : l'Arduino fait un reset quand le port s'ouvre
    // On tente jusqu'à 3 fois avec 500 ms entre chaque essai
    int ret = -1;
    for (int attempt = 0; attempt < 3 && ret != 0; ++attempt) {
        if (attempt > 0) {
            qDebug() << "🔄 Retry connexion" << portName << "(tentative" << attempt + 1 << "/3)...";
            QThread::msleep(500);
        }
        ret = arduino->connectToPort(portName);
    }

    if (ret != 0) {
        qDebug() << "❌ Impossible de reconnecter" << portName << "pour rôle" << role;
        delete arduino;
        m_identified++;
        checkAllIdentified();
        return;
    }

    // startContinuousRead uniquement pour TEMP_BALANCE
    // LIVRAISON et POINTAGE : MainWindow connecte son propre readyRead
    if (role == "TEMP_BALANCE") {
        arduino->startContinuousRead();
    }

    // Assigner selon le rôle
    if (role == "TEMP_BALANCE") {
        m_arduinoTempBalance = arduino;
        qDebug() << "✅ TEMP_BALANCE →" << portName;
    } else if (role == "LIVRAISON") {
        m_arduinoLivraison = arduino;
        qDebug() << "✅ LIVRAISON →" << portName;
    } else if (role == "POINTAGE") {
        m_arduinoPointage = arduino;
        qDebug() << "✅ POINTAGE →" << portName;
    } else {
        qDebug() << "⚠️ Rôle inconnu:" << role << "sur" << portName;
        delete arduino;
    }

    emit cardIdentified(role, portName);
    m_identified++;
    checkAllIdentified();
}

// ─────────────────────────────────────────────────────────────────────────────
//  checkAllIdentified — émet discoveryComplete si tout est traité
// ─────────────────────────────────────────────────────────────────────────────
void ArduinoManager::checkAllIdentified()
{
    if (m_identified >= m_totalPorts) {
        m_timeoutTimer->stop();

        // Signaler les cartes manquantes
        if (!m_arduinoTempBalance) emit cardMissing("TEMP_BALANCE");
        if (!m_arduinoLivraison)   emit cardMissing("LIVRAISON");
        if (!m_arduinoPointage)    emit cardMissing("POINTAGE");

        qDebug() << "🏁 Découverte terminée:" << statusSummary();
        emit discoveryComplete();
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  onRoleTimeout — timeout global : forcer la fin de la découverte
// ─────────────────────────────────────────────────────────────────────────────
void ArduinoManager::onRoleTimeout()
{
    qDebug() << "⏰ Timeout identification Arduino — ports non identifiés:";
    for (const QString &portName : m_pendingPorts.keys()) {
        qDebug() << "  ❓" << portName << "(pas de ROLE: reçu)";
        QSerialPort *tmp = m_pendingPorts.take(portName);
        tmp->close();
        tmp->deleteLater();
        m_pendingBuffers.remove(portName);
    }

    if (!m_arduinoTempBalance) emit cardMissing("TEMP_BALANCE");
    if (!m_arduinoLivraison)   emit cardMissing("LIVRAISON");
    if (!m_arduinoPointage)    emit cardMissing("POINTAGE");

    qDebug() << "🏁 Découverte (timeout):" << statusSummary();
    emit discoveryComplete();
}

// ─────────────────────────────────────────────────────────────────────────────
//  statusSummary
// ─────────────────────────────────────────────────────────────────────────────
QString ArduinoManager::statusSummary() const
{
    return QString("TEMP_BALANCE=%1 | LIVRAISON=%2 | POINTAGE=%3")
        .arg(m_arduinoTempBalance ? m_arduinoTempBalance->getPortName() : "❌")
        .arg(m_arduinoLivraison   ? m_arduinoLivraison->getPortName()   : "❌")
        .arg(m_arduinoPointage    ? m_arduinoPointage->getPortName()    : "❌");
}
