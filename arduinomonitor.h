#ifndef ARDUINOMONITOR_H
#define ARDUINOMONITOR_H

#include <QObject>
#include <QTimer>
#include <QSqlDatabase>
#include "arduino.h"
#include "emailalerte.h"

// ─────────────────────────────────────────────────────────────────────────────
//  ArduinoMonitor — Orchestre les deux scénarios Arduino :
//
//  SCÉNARIO 1 : Surveillance température du stock cuir
//    • Interroge l'Arduino toutes les TEMP_POLL_MS ms
//    • Si température > TEMP_MAX_CELSIUS ou < TEMP_MIN_CELSIUS
//      → LED rouge + email Brevo + notification Qt (onglet Matières Premières)
//    • Température normale → LED verte
//    • Enregistre chaque lecture dans ARDUINO_TEMP_READINGS
//
//  SCÉNARIO 2 : Validation livraison fournisseur
//    • Déclenché manuellement via startDeliveryCheck()
//    • Lit le poids stable de la balance HX711
//    • Compare avec la quantité commandée (tolérance ±WEIGHT_TOLERANCE_PCT %)
//    • Si OK  → LED verte + enregistrement BD + mise à jour QUANTITE_ACTUELLE
//    • Si NOK → LED rouge + email d'alerte (pas de notification Qt, pas de validation manuelle)
// ─────────────────────────────────────────────────────────────────────────────
class ArduinoMonitor : public QObject
{
    Q_OBJECT

public:
    // Seuils configurables
    static constexpr double TEMP_MAX_CELSIUS      = 28.0;  // °C max pour le cuir (alerte si T > 28°C)
    static constexpr double TEMP_MIN_CELSIUS       = 5.0;   // °C min
    static constexpr double WEIGHT_TOLERANCE_PCT   = 5.0;   // ±5 % tolérance poids
    static constexpr int    TEMP_POLL_MS           = 10000; // 10 s entre lectures temp

    explicit ArduinoMonitor(QObject *parent = nullptr);
    ~ArduinoMonitor();

    // Démarrer/arrêter la surveillance température
    void startTemperatureMonitoring();
    void stopTemperatureMonitoring();

    // Lancer une vérification de livraison
    // fournisseurId : ID du fournisseur qui livre
    // matiereId     : ID de la matière première livrée
    // orderedQty    : quantité commandée (kg)
    void startDeliveryCheck(int fournisseurId, int matiereId, double orderedQty);

    // Annuler une vérification en cours
    void cancelDeliveryCheck();

    bool isConnected() const;
    bool isMonitoringTemperature() const { return m_tempTimer && m_tempTimer->isActive(); }
    bool isDeliveryCheckPending()  const { return m_deliveryPending; }

    // Accès à l'instance Arduino (pour tests)
    Arduino *arduino() { return m_arduino; }

signals:
    // Température
    void temperatureUpdated(double celsius);
    void temperatureAlert(double celsius, const QString &message);
    void temperatureNormal(double celsius);

    // Livraison
    void deliveryWeightRead(double measuredKg, double orderedKg, double diffPct);
    void deliveryValidated(int fournisseurId, int matiereId, double qty);   // OK → enregistrer en BD
    void deliveryRejected(double measuredKg, double orderedKg, double diffPct);

    // Connexion Arduino
    void arduinoConnected(const QString &port);
    void arduinoDisconnected();

    // Navigation UI — température uniquement (connecté dans MainWindow)
    void navigateToMatieres();

private slots:
    void onTempPollTimer();
    void onTemperatureReceived(double celsius);
    void onWeightStable(double kg);

private:
    void connectArduino();
    void saveTemperatureReading(double celsius);
    void saveDeliveryRecord(double measuredKg, bool validated, const QString &note = QString());

    Arduino *m_arduino;
    EmailAlerte *m_emailAlerte;
    QTimer  *m_tempTimer;
    QTimer  *m_reconnectTimer;

    // État livraison en cours
    bool    m_deliveryPending;
    int     m_deliveryFournisseurId;
    int     m_deliveryMatiereId;
    double  m_deliveryOrderedQty;
    double  m_lastMeasuredWeight;

    // Anti-spam alertes température
    bool    m_tempAlertSent;
    double  m_lastTempAlertValue;
};

#endif // ARDUINOMONITOR_H
