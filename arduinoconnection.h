#ifndef ARDUINOCONNECTION_H
#define ARDUINOCONNECTION_H

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QString>
#include <QList>

// ─────────────────────────────────────────────────────────────────────────────
//  ArduinoConnection — Gestion d'une connexion série vers un Arduino
//
//  Nouveauté multi-cartes :
//    • connectToPort(portName)  : connexion directe à un port connu
//    • scanAllPorts()           : retourne la liste de tous les ports Arduino
//    • connect_arduino()        : conservé pour compatibilité (1ère carte trouvée)
// ─────────────────────────────────────────────────────────────────────────────
class ArduinoConnection
{
public:
    ArduinoConnection();
    ~ArduinoConnection();

    // ── Connexion directe à un port nommé (multi-cartes) ──────────────────
    int connectToPort(const QString &portName);

    // ── Scan : retourne tous les ports Arduino disponibles ────────────────
    static QList<QString> scanAllPorts();

    // ── Compatibilité ancienne API (1ère carte trouvée) ───────────────────
    int connect_arduino();
    int close_arduino();

    QSerialPort* getSerial() const;
    QString getPortName() const;
    bool isAvailable() const;

private:
    // VID/PID connus : Uno officiel, clones CH340, CP2102, FTDI
    static const QList<QPair<quint16,quint16>>& knownIds();

    int openPort(const QString &portName);   // ouvre et configure le port série

    QSerialPort*  serial;
    QString       arduino_port_name;
    bool          arduino_is_available;
};

#endif // ARDUINOCONNECTION_H
