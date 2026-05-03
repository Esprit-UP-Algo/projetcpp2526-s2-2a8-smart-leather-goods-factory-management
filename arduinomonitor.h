#ifndef ARDUINOMONITOR_H
#define ARDUINOMONITOR_H

#include <QObject>
#include <QTimer>
#include <QSqlDatabase>
#include "arduino.h"
#include "emailalerte.h"

// ─────────────────────────────────────────────────────────────────────────────
//  ArduinoMonitor — Orchestre les 3 scénarios Arduino multi-cartes :
//
//  SCÉNARIO 1 : Surveillance température du stock cuir
//    • Carte : TEMP_BALANCE (DHT11 x2 + HX711 + LED RGB)
//    • Interroge toutes les TEMP_POLL_MS ms
//    • Si T_matière > T_ambiance → LED rouge + email + notification Qt
//    • Enregistre dans ARDUINO_TEMP_READINGS
//
//  SCÉNARIO 2 : Validation livraison fournisseur
//    • Carte : TEMP_BALANCE (balance HX711 partagée avec température)
//    • Déclenché via startDeliveryCheck()
//    • Compare poids mesuré vs commandé (tolérance ±WEIGHT_TOLERANCE_PCT %)
//    • Si OK → enregistrement BD + mise à jour QUANTITE_ACTUELLE
//    • Si NOK → LED rouge + email d'alerte
//
//  SCÉNARIO 3 : Expédition commandes
//    • Carte : LIVRAISON (keypad + servos + LCD)
//    • Géré directement dans MainWindow via m_arduinoLivraison
//    • ArduinoMonitor n'intervient pas dans ce scénario
//
//  SCÉNARIO 4 : Pointage RFID employés
//    • Carte : POINTAGE (MFRC522 + LCD I2C + servo)
//    • Géré directement dans MainWindow via m_arduinoPointage
//    • ArduinoMonitor n'intervient pas dans ce scénario
// ─────────────────────────────────────────────────────────────────────────────
class ArduinoMonitor : public QObject
{
    Q_OBJECT

public:
    // Seuils configurables
    static constexpr double TEMP_MAX_CELSIUS    = 28.0;  // °C max pour le cuir
    static constexpr double TEMP_MIN_CELSIUS    = 5.0;   // °C min
    static constexpr double WEIGHT_TOLERANCE_PCT = 5.0;  // ±5 % tolérance poids
    static constexpr int    TEMP_POLL_MS        = 10000; // 10 s entre lectures temp

    explicit ArduinoMonitor(QObject *parent = nullptr);
    ~ArduinoMonitor();

    // ── Injection des instances Arduino (appelé après découverte) ──────────
    // Carte température + balance (ROLE:TEMP_BALANCE)
    void setArduinoTempBalance(Arduino *arduino);

    // Compatibilité ancienne API (1 seule carte) — redirige vers setArduinoTempBalance
    void setArduino(Arduino *arduino);

    // ── Scénario 1 : Surveillance température ─────────────────────────────
    void startTemperatureMonitoring();
    void stopTemperatureMonitoring();

    // ── Scénario 2 : Validation livraison ─────────────────────────────────
    void startDeliveryCheck(int fournisseurId, int matiereId, double orderedQty);
    void cancelDeliveryCheck();

    // ── État ───────────────────────────────────────────────────────────────
    bool isConnected() const;
    bool isMonitoringTemperature() const { return m_tempTimer && m_tempTimer->isActive(); }
    bool isDeliveryCheckPending()  const { return m_deliveryPending; }

    // Accès aux instances (pour tests)
    Arduino* arduino()            { return m_arduinoTempBalance; }
    Arduino* arduinoTempBalance() { return m_arduinoTempBalance; }

signals:
    // Température
    void temperatureUpdated(double celsius);
    void temperatureAlert(double celsius, const QString &message);
    void temperatureNormal(double celsius);

    // Livraison
    void deliveryWeightRead(double measuredKg, double orderedKg, double diffPct);
    void deliveryValidated(int fournisseurId, int matiereId, double qty);
    void deliveryRejected(int fournisseurId, int matiereId,
                          double measuredKg, double orderedKg, double diffPct);

    // Connexion Arduino
    void arduinoConnected(const QString &port);
    void arduinoDisconnected();

    // Navigation UI
    void navigateToMatieres();

private slots:
    void onTempPollTimer();

public slots:
    void onTemperatureReceived(double tempMatiere, double tempAmbiance);

private slots:
    void onWeightStable(double kg);

private:
    void connectArduino();
    void saveTemperatureReading(double tempMatiere, double tempAmbiance, bool isAlert);
    void saveDeliveryRecord(double measuredKg, bool validated, const QString &note = QString());

    Arduino     *m_arduinoTempBalance = nullptr;  // carte TEMP_BALANCE
    EmailAlerte *m_emailAlerte        = nullptr;
    QTimer      *m_tempTimer          = nullptr;
    QTimer      *m_reconnectTimer     = nullptr;

    // État livraison en cours
    bool    m_deliveryPending         = false;
    int     m_deliveryFournisseurId   = -1;
    int     m_deliveryMatiereId       = -1;
    double  m_deliveryOrderedQty      = 0.0;
    double  m_lastMeasuredWeight      = 0.0;

    // Anti-spam alertes température
    bool    m_tempAlertSent           = false;
    double  m_lastTempAlertValue      = 0.0;
};

#endif // ARDUINOMONITOR_H
