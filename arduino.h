#ifndef ARDUINO_H
#define ARDUINO_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QString>
#include <QByteArray>
#include <QTimer>

// ─────────────────────────────────────────────────────────────────────────────
//  Arduino — Gestion de la communication série avec la carte Arduino
//  Protocole texte ligne par ligne :
//    Arduino → Qt  :  "TEMP:25.3"  |  "WEIGHT:45.2"  |  "WEIGHT_STABLE:45.2"
//    Qt → Arduino  :  "LED_RED\n"  |  "LED_GREEN\n"   |  "LED_OFF\n"
//                     "READ_TEMP\n"|  "READ_WEIGHT\n"
// ─────────────────────────────────────────────────────────────────────────────
class Arduino : public QObject
{
    Q_OBJECT

public:
    explicit Arduino(QObject *parent = nullptr);
    ~Arduino();

    // Connexion
    int  connect_arduino();
    void close_arduino();
    bool isConnected() const;

    // Communication bas niveau
    int        write_to_arduino(const QByteArray &data);
    QByteArray read_from_arduino();

    // Commandes LED (1 seule LED rouge partagée)
    void ledRed();    // allumer → alerte
    void ledOff();    // éteindre → normal/OK

    // Demandes de lecture capteurs
    void requestTemperature();
    void requestWeight();

    // Info
    QString getPortName() const;

    // Démarrer/arrêter la lecture continue (signal readyRead)
    void startContinuousRead();
    void stopContinuousRead();

signals:
    // Émis quand une température valide est reçue
    void temperatureReceived(double celsius);
    // Émis quand un poids valide est reçu (lecture en cours)
    void weightReceived(double kg);
    // Émis quand le poids est stable (mesure finale)
    void weightStable(double kg);
    // Émis sur erreur de communication
    void errorOccurred(const QString &message);

private slots:
    void onSerialDataReady();

private:
    QString findArduinoPort();
    void    parseLine(const QString &line);

    QSerialPort *serial;
    bool         arduino_available;
    QString      arduino_port_name;
    QByteArray   m_readBuffer;   // buffer pour les lignes incomplètes

    static const quint16 ARDUINO_VENDOR_ID  = 9025;   // 0x2341
    static const quint16 ARDUINO_PRODUCT_ID = 67;     // Uno
};

#endif // ARDUINO_H
