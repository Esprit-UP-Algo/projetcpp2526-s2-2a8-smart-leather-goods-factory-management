#include "arduinoconnection.h"
#include <QDebug>

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
// ─────────────────────────────────────────────────────────────────────────────
QList<QString> ArduinoConnection::scanAllPorts()
{
    QList<QString> result;
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
