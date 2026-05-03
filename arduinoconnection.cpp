#include "arduinoconnection.h"
#include <QDebug>
#include <QProcessEnvironment>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>

// ─────────────────────────────────────────────────────────────────────────────
//  VID/PID connus : Uno officiel, clones CH340, CP2102, FTDI
// ─────────────────────────────────────────────────────────────────────────────
const QList<QPair<quint16,quint16>>& ArduinoConnection::knownIds()
{
    static const QList<QPair<quint16,quint16>> ids = {
        {0x2341, 0x0043},  // Arduino Uno officiel
        {0x2341, 0x0001},  // Arduino Uno officiel (ancien)
        {0x1A86, 0x7523},  // Clone CH340
        {0x1A86, 0x5523},  // Clone CH340G
        {0x10C4, 0xEA60},  // Clone CP2102
        {0x0403, 0x6001},  // FTDI FT232RL
        {0x0403, 0x6015},  // FTDI FT231X
    };
    return ids;
}

// ─────────────────────────────────────────────────────────────────────────────
ArduinoConnection::ArduinoConnection()
    : serial(new QSerialPort()),
      arduino_is_available(false),
      arduino_port_name("")
{}

ArduinoConnection::~ArduinoConnection()
{
    close_arduino();
    delete serial;
}

// ─────────────────────────────────────────────────────────────────────────────
//  scanAllPorts — retourne tous les ports qui correspondent à un Arduino connu
//
//  Priorité :
//    1. Variable ARDUINO_PORTS dans .env  (ex: ARDUINO_PORTS=COM6,COM7)
//    2. Détection automatique par VID/PID
//    3. Fallback : tous les ports COM disponibles (si aucun VID/PID reconnu)
// ─────────────────────────────────────────────────────────────────────────────
QList<QString> ArduinoConnection::scanAllPorts()
{
    QList<QString> result;

    // ── 1. Ports forcés via .env (ARDUINO_PORTS=COM6:LIVRAISON) ─────────────
    QStringList envCandidates = {
        ".env",
        QCoreApplication::applicationDirPath() + "/.env",
        QCoreApplication::applicationDirPath() + "/../.env",
        QCoreApplication::applicationDirPath() + "/../../.env"
    };
    QFile envFile;
    for (const QString &path : envCandidates) {
        envFile.setFileName(path);
        if (envFile.exists()) break;
    }
    if (envFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&envFile);
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (line.startsWith("ARDUINO_PORTS=")) {
                QString val = line.mid(14).trimmed();
                QStringList forced = val.split(',', Qt::SkipEmptyParts);
                for (const QString &p : forced) {
                    // Accepter "COM6" ou "COM6:LIVRAISON" — on prend juste le port
                    QString port = p.trimmed().split(':').value(0).trimmed();
                    if (!port.isEmpty()) {
                        qDebug() << "📌 Port forcé via .env:" << port;
                        result.append(port);
                    }
                }
                break;
            }
        }
        envFile.close();
    }
    if (!result.isEmpty()) return result;

    // ── 2. Détection automatique par VID/PID ─────────────────────────────────
    const auto &ids = knownIds();

    qDebug() << "=== Scan multi-cartes Arduino ===";
    foreach (const QSerialPortInfo &port, QSerialPortInfo::availablePorts()) {
        qDebug() << " Port:" << port.portName()
                 << "VID:" << QString("0x%1").arg(port.vendorIdentifier(), 4, 16, QChar('0')).toUpper()
                 << "PID:" << QString("0x%1").arg(port.productIdentifier(), 4, 16, QChar('0')).toUpper()
                 << "Desc:" << port.description();

        if (!port.hasVendorIdentifier() || !port.hasProductIdentifier()) continue;

        for (const auto &id : ids) {
            if (port.vendorIdentifier()  == id.first &&
                port.productIdentifier() == id.second) {
                qDebug() << "  ✅ Arduino détecté sur" << port.portName();
                result.append(port.portName());
                break;
            }
        }
    }

    // ── 3. Fallback : accepter tous les ports série si aucun VID/PID reconnu ─
    if (result.isEmpty()) {
        qDebug() << "⚠️ Aucun VID/PID reconnu — fallback: scan de tous les ports COM";
        foreach (const QSerialPortInfo &port, QSerialPortInfo::availablePorts()) {
            // Exclure les ports Bluetooth et virtuels sans description
            if (port.description().contains("Bluetooth", Qt::CaseInsensitive)) continue;
            if (port.portName().startsWith("COM") || port.portName().startsWith("ttyUSB")
                || port.portName().startsWith("ttyACM")) {
                qDebug() << "  🔍 Fallback inclus:" << port.portName() << "-" << port.description();
                result.append(port.portName());
            }
        }
    }

    qDebug() << "=== Total Arduino trouvés:" << result.size() << "===";
    return result;
}

// ─────────────────────────────────────────────────────────────────────────────
//  openPort — ouvre et configure un port série
// ─────────────────────────────────────────────────────────────────────────────
int ArduinoConnection::openPort(const QString &portName)
{
    serial->setPortName(portName);
    if (serial->open(QSerialPort::ReadWrite)) {
        serial->setBaudRate(QSerialPort::Baud9600);
        serial->setDataBits(QSerialPort::Data8);
        serial->setParity(QSerialPort::NoParity);
        serial->setStopBits(QSerialPort::OneStop);
        serial->setFlowControl(QSerialPort::NoFlowControl);
        arduino_port_name    = portName;
        arduino_is_available = true;
        return 0;   // ✅ connecté
    }
    qDebug() << "⚠️ Port trouvé mais impossible à ouvrir:" << serial->errorString();
    return 1;       // ❌ port trouvé mais pas ouvert
}

// ─────────────────────────────────────────────────────────────────────────────
//  connectToPort — connexion directe à un port nommé (multi-cartes)
// ─────────────────────────────────────────────────────────────────────────────
int ArduinoConnection::connectToPort(const QString &portName)
{
    if (serial->isOpen()) serial->close();
    arduino_is_available = false;
    arduino_port_name    = "";
    return openPort(portName);
}

// ─────────────────────────────────────────────────────────────────────────────
//  connect_arduino — compatibilité ancienne API (1ère carte trouvée)
// ─────────────────────────────────────────────────────────────────────────────
int ArduinoConnection::connect_arduino()
{
    QList<QString> ports = scanAllPorts();
    if (ports.isEmpty()) {
        qDebug() << "❌ Aucun Arduino détecté";
        return -1;
    }
    return openPort(ports.first());
}

// ─────────────────────────────────────────────────────────────────────────────
//  close_arduino
// ─────────────────────────────────────────────────────────────────────────────
int ArduinoConnection::close_arduino()
{
    if (serial && serial->isOpen()) {
        serial->close();
        arduino_is_available = false;
        return 0;   // ✅ fermé
    }
    return 1;       // ❌ déjà fermé
}

// ─────────────────────────────────────────────────────────────────────────────
//  Accesseurs
// ─────────────────────────────────────────────────────────────────────────────
QSerialPort* ArduinoConnection::getSerial() const { return serial; }
QString      ArduinoConnection::getPortName() const { return arduino_port_name; }
bool         ArduinoConnection::isAvailable() const { return arduino_is_available; }
