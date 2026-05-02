#ifndef ARDUINO_H
#define ARDUINO_H

#include "arduinoconnection.h"
#include <QObject>
#include <QByteArray>

class Arduino : public QObject
{
    Q_OBJECT

public:
    explicit Arduino(QObject *parent = nullptr);
    ~Arduino();

    // Connexion
    int connect_arduino();
    void close_arduino();
    bool isConnected() const;
    QString getPortName() const;

    // Communication
    int write_to_arduino(QByteArray data);
    QByteArray read_from_arduino();
    void startContinuousRead();

    // Commandes Arduino
    void requestTemperature();
    void requestWeight();
    void tareScale();  // Remettre la balance à zéro
    void ledRed();
    void ledGreen();
    void ledOff();

    // Accès à la connexion
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
};

#endif // ARDUINO_H
