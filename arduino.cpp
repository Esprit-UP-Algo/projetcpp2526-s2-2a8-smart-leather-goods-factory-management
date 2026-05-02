#include "arduino.h"
#include <QDebug>
#include <QSerialPort>

Arduino::Arduino(QObject *parent)
    : QObject(parent)
    , m_connection(new ArduinoConnection())
    , m_role("")
{
}

Arduino::~Arduino()
{
    close_arduino();
}

// ─── Connexion ────────────────────────────────────────────────────────────────
int Arduino::connect_arduino()
{
    return m_connection->connect_arduino();
}

int Arduino::connectToPort(const QString &portName)
{
    return m_connection->connectToPort(portName);
}

void Arduino::close_arduino()
{
    m_connection->close_arduino();
}

bool Arduino::isConnected() const
{
    QSerialPort* serial = m_connection->getSerial();
    return serial && serial->isOpen();
}

QString Arduino::getPortName() const
{
    QSerialPort* serial = m_connection->getSerial();
    return serial ? serial->portName() : QString();
}

// ─── Communication ────────────────────────────────────────────────────────────
int Arduino::write_to_arduino(QByteArray data)
{
    QSerialPort* serial = m_connection->getSerial();
    if (serial && serial->isWritable()) {
        serial->write(data);
        return 0;   // ✅ envoyé
    }
    qDebug() << "Erreur : impossible d'écrire sur le port !";
    return -1;      // ❌ erreur
}

QByteArray Arduino::read_from_arduino()
{
    QSerialPort* serial = m_connection->getSerial();
    if (serial && serial->isReadable()) {
        m_data = serial->readAll();
        return m_data;
    }
    return "";
}

void Arduino::startContinuousRead()
{
    QSerialPort* serial = m_connection->getSerial();
    if (serial) {
        connect(serial, &QSerialPort::readyRead, this, &Arduino::onDataReceived);
    }
}

// ─── Commandes Arduino ────────────────────────────────────────────────────────
void Arduino::requestTemperature() { write_to_arduino("GET_TEMP\n"); }
void Arduino::requestWeight()      { write_to_arduino("GET_WEIGHT\n"); }
void Arduino::tareScale()          { write_to_arduino("TARE\n"); }
void Arduino::ledRed()             { write_to_arduino("LED_RED\n"); }
void Arduino::ledGreen()           { write_to_arduino("LED_GREEN\n"); }
void Arduino::ledOff()             { write_to_arduino("LED_OFF\n"); }

// ─── Traitement données ───────────────────────────────────────────────────────
void Arduino::onDataReceived()
{
    QByteArray data = read_from_arduino();
    if (!data.isEmpty()) {
        QString message = QString::fromUtf8(data).trimmed();
        processArduinoData(message);
    }
}

void Arduino::processArduinoData(const QString &data)
{
    qDebug() << "[Arduino" << m_role << "] data:" << data;

    // Format: TEMP:25.5,22.3 (matière,ambiance)
    if (data.startsWith("TEMP:")) {
        QStringList parts = data.mid(5).split(',');
        if (parts.size() == 2) {
            bool ok1, ok2;
            double tempMatiere  = parts[0].toDouble(&ok1);
            double tempAmbiance = parts[1].toDouble(&ok2);
            if (ok1 && ok2)
                emit temperatureReceived(tempMatiere, tempAmbiance);
        }
    }
    // Format: WEIGHT:45.67
    else if (data.startsWith("WEIGHT:")) {
        bool ok;
        double weight = data.mid(7).toDouble(&ok);
        if (ok)
            emit weightStable(weight);
    }
    // Format: ERROR:message
    else if (data.startsWith("ERROR:")) {
        emit errorOccurred(data.mid(6));
    }
}

// ─── Accès à la connexion ─────────────────────────────────────────────────────
ArduinoConnection* Arduino::connection()
{
    return m_connection;
}
