#ifndef ARDUINO_H
#define ARDUINO_H

#include "arduinoconnection.h"
#include <QObject>
#include <QByteArray>
#include <QString>

// ─────────────────────────────────────────────────────────────────────────────
//  Arduino — Encapsule une connexion série vers une carte Arduino
//
//  Nouveauté multi-cartes :
//    • m_role : rôle identifié via "ROLE:xxx" envoyé par l'Arduino au démarrage
//    • connectToPort(portName) : connexion directe à un port nommé
//    • role() / setRole()      : accès au rôle détecté
//
//  Rôles connus :
//    "TEMP_BALANCE"  → carte température + balance HX711
//    "LIVRAISON"     → carte expédition (keypad + servos + LCD)
//    "POINTAGE"      → carte pointage RFID (MFRC522 + LCD I2C + servo)
// ─────────────────────────────────────────────────────────────────────────────
class Arduino : public QObject
{
    Q_OBJECT

public:
    explicit Arduino(QObject *parent = nullptr);
    ~Arduino();

    // ── Connexion ──────────────────────────────────────────────────────────
    int  connect_arduino();                      // 1ère carte trouvée (compat.)
    int  connectToPort(const QString &portName); // connexion directe (multi-cartes)
    void close_arduino();
    bool isConnected() const;
    QString getPortName() const;

    // ── Rôle (identifié via "ROLE:xxx") ───────────────────────────────────
    QString role() const { return m_role; }
    void    setRole(const QString &r) { m_role = r; }

    // ── Communication ──────────────────────────────────────────────────────
    int        write_to_arduino(QByteArray data);
    QByteArray read_from_arduino();
    void       startContinuousRead();

    // ── Commandes Arduino ──────────────────────────────────────────────────
    void requestTemperature();
    void requestWeight();
    void tareScale();
    void ledRed();
    void ledGreen();
    void ledOff();

    // ── Accès à la connexion ───────────────────────────────────────────────
    ArduinoConnection* connection();

signals:
    void temperatureReceived(double tempMatiere, double tempAmbiance);
    void weightStable(double kg);
    void errorOccurred(const QString &message);

private slots:
    void onDataReceived();

private:
    void processArduinoData(const QString &data);

    ArduinoConnection* m_connection;
    QByteArray         m_data;
    QByteArray         m_readBuffer;   // buffer d'accumulation pour les lectures fragmentées
    QString            m_role;   // "TEMP_BALANCE" | "LIVRAISON" | "POINTAGE"
};

#endif // ARDUINO_H
