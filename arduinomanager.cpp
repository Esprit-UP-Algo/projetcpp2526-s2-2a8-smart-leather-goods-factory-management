#include "arduinomanager.h"
#include "arduinoconnection.h"
#include <QSerialPort>
#include <QDebug>

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
//  scanAndIdentify — point d'entrée public
// ─────────────────────────────────────────────────────────────────────────────
void ArduinoManager::scanAndIdentify()
{
    QList<QString> ports = ArduinoConnection::scanAllPorts();
    m_totalPorts = ports.size();
    m_identified = 0;

    if (m_totalPorts == 0) {
        qDebug() << "⚠️ ArduinoManager: aucun Arduino détecté";
        emit discoveryComplete();
        return;
    }

    qDebug() << "🔍 ArduinoManager: identification de" << m_totalPorts << "carte(s)...";

    // Ouvrir chaque port et attendre le message ROLE:
    for (const QString &portName : ports)
        tryIdentifyPort(portName);

    // Timeout global : si après ROLE_TIMEOUT_MS on n'a pas tout identifié
    m_timeoutTimer->start(ROLE_TIMEOUT_MS);
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
    int ret = arduino->connectToPort(portName);

    if (ret != 0) {
        qDebug() << "❌ Impossible de reconnecter" << portName << "pour rôle" << role;
        delete arduino;
        m_identified++;
        checkAllIdentified();
        return;
    }

    arduino->startContinuousRead();

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
