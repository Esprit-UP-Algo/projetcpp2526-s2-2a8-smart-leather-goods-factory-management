#include "arduino.h"
#include <QDebug>

Arduino::Arduino(QObject *parent)
    : QObject(parent)
    , serial(new QSerialPort(this))
    , arduino_available(false)
    , arduino_port_name("")
{
}

Arduino::~Arduino()
{
    close_arduino();
}

// ─────────────────────────────────────────────────────────────────────────────
//  Détection automatique du port Arduino
// ─────────────────────────────────────────────────────────────────────────────
QString Arduino::findArduinoPort()
{
    // 1) Chercher par VendorID + ProductID (Arduino Uno)
    for (const QSerialPortInfo &info : QSerialPortInfo::availablePorts()) {
        if (info.hasVendorIdentifier() && info.hasProductIdentifier()) {
            if (info.vendorIdentifier() == ARDUINO_VENDOR_ID &&
                info.productIdentifier() == ARDUINO_PRODUCT_ID) {
                qDebug() << "✅ Arduino Uno détecté sur" << info.portName();
                return info.portName();
            }
        }
    }

    // 2) Chercher par VendorID seul (autres modèles Arduino)
    for (const QSerialPortInfo &info : QSerialPortInfo::availablePorts()) {
        if (info.hasVendorIdentifier() && info.vendorIdentifier() == ARDUINO_VENDOR_ID) {
            qDebug() << "✅ Arduino (autre modèle) détecté sur" << info.portName();
            return info.portName();
        }
    }

    // 3) Fallback : ports COM courants
    QStringList commonPorts = {"COM3", "COM4", "COM5", "COM6", "COM7", "COM8"};
    for (const QSerialPortInfo &info : QSerialPortInfo::availablePorts()) {
        if (commonPorts.contains(info.portName())) {
            qDebug() << "⚠ Arduino supposé sur" << info.portName() << "(fallback)";
            return info.portName();
        }
    }

    return "";
}

// ─────────────────────────────────────────────────────────────────────────────
//  Connexion
// ─────────────────────────────────────────────────────────────────────────────
int Arduino::connect_arduino()
{
    arduino_port_name = findArduinoPort();

    if (arduino_port_name.isEmpty()) {
        qDebug() << "❌ Aucun Arduino détecté";
        emit errorOccurred("Aucun Arduino détecté sur les ports disponibles.");
        return -1;
    }

    serial->setPortName(arduino_port_name);
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    if (serial->open(QIODevice::ReadWrite)) {
        arduino_available = true;
        qDebug() << "✅ Arduino connecté sur" << arduino_port_name;
        return 0;
    } else {
        qDebug() << "❌ Erreur connexion:" << serial->errorString();
        emit errorOccurred("Impossible d'ouvrir " + arduino_port_name + ": " + serial->errorString());
        return -1;
    }
}

void Arduino::close_arduino()
{
    if (serial && serial->isOpen()) {
        serial->close();
        arduino_available = false;
        qDebug() << "🔌 Arduino déconnecté";
    }
}

bool Arduino::isConnected() const
{
    return arduino_available && serial && serial->isOpen();
}

// ─────────────────────────────────────────────────────────────────────────────
//  Communication bas niveau
// ─────────────────────────────────────────────────────────────────────────────
int Arduino::write_to_arduino(const QByteArray &data)
{
    if (!serial->isOpen()) return -1;
    qint64 written = serial->write(data);
    serial->flush();
    return static_cast<int>(written);
}

QByteArray Arduino::read_from_arduino()
{
    if (serial->isOpen() && serial->waitForReadyRead(200))
        return serial->readAll();
    return QByteArray();
}

// ─────────────────────────────────────────────────────────────────────────────
//  Commandes LED
// ─────────────────────────────────────────────────────────────────────────────
void Arduino::ledRed()
{
    write_to_arduino("LED_RED\n");
    qDebug() << "🔴 LED rouge activée";
}

void Arduino::ledGreen()
{
    write_to_arduino("LED_GREEN\n");
    qDebug() << "🟢 LED verte activée";
}

void Arduino::ledOff()
{
    write_to_arduino("LED_OFF\n");
    qDebug() << "⚫ LED éteinte";
}

// ─────────────────────────────────────────────────────────────────────────────
//  Demandes de lecture capteurs
// ─────────────────────────────────────────────────────────────────────────────
void Arduino::requestTemperature()
{
    write_to_arduino("READ_TEMP\n");
}

void Arduino::requestWeight()
{
    write_to_arduino("READ_WEIGHT\n");
}

// ─────────────────────────────────────────────────────────────────────────────
//  Lecture continue (signal readyRead)
// ─────────────────────────────────────────────────────────────────────────────
void Arduino::startContinuousRead()
{
    connect(serial, &QSerialPort::readyRead, this, &Arduino::onSerialDataReady);
    qDebug() << "📡 Lecture continue Arduino démarrée";
}

void Arduino::stopContinuousRead()
{
    disconnect(serial, &QSerialPort::readyRead, this, &Arduino::onSerialDataReady);
    qDebug() << "📡 Lecture continue Arduino arrêtée";
}

// ─────────────────────────────────────────────────────────────────────────────
//  Slot interne : données disponibles sur le port série
// ─────────────────────────────────────────────────────────────────────────────
void Arduino::onSerialDataReady()
{
    m_readBuffer += serial->readAll();

    // Traiter toutes les lignes complètes (terminées par '\n')
    while (m_readBuffer.contains('\n')) {
        int idx = m_readBuffer.indexOf('\n');
        QByteArray lineBytes = m_readBuffer.left(idx).trimmed();
        m_readBuffer = m_readBuffer.mid(idx + 1);

        if (!lineBytes.isEmpty())
            parseLine(QString::fromUtf8(lineBytes));
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Parseur de protocole
//  Formats attendus :
//    TEMP:25.3          → température en °C
//    WEIGHT:45.2        → poids en kg (lecture en cours)
//    WEIGHT_STABLE:45.2 → poids stable (mesure finale)
// ─────────────────────────────────────────────────────────────────────────────
void Arduino::parseLine(const QString &line)
{
    qDebug() << "📥 Arduino →" << line;

    if (line.startsWith("TEMP:")) {
        bool ok;
        double temp = line.mid(5).toDouble(&ok);
        if (ok) {
            qDebug() << "🌡 Température reçue:" << temp << "°C";
            emit temperatureReceived(temp);
        }
    }
    else if (line.startsWith("WEIGHT_STABLE:")) {
        bool ok;
        double weight = line.mid(14).toDouble(&ok);
        if (ok) {
            qDebug() << "⚖ Poids stable reçu:" << weight << "kg";
            emit weightStable(weight);
        }
    }
    else if (line.startsWith("WEIGHT:")) {
        bool ok;
        double weight = line.mid(7).toDouble(&ok);
        if (ok) {
            qDebug() << "⚖ Poids reçu:" << weight << "kg";
            emit weightReceived(weight);
        }
    }
    else {
        qDebug() << "⚠ Ligne Arduino non reconnue:" << line;
    }
}

QString Arduino::getPortName() const
{
    return arduino_port_name;
}
