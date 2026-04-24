#include "arduinomonitor.h"
#include "connection.h"
#include "emailalerte.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QDebug>

ArduinoMonitor::ArduinoMonitor(QObject *parent)
    : QObject(parent)
    , m_arduino(new Arduino(this))
    , m_emailAlerte(new EmailAlerte(this))
    , m_tempTimer(new QTimer(this))
    , m_reconnectTimer(new QTimer(this))
    , m_deliveryPending(false)
    , m_deliveryFournisseurId(-1)
    , m_deliveryMatiereId(-1)
    , m_deliveryOrderedQty(0.0)
    , m_lastMeasuredWeight(0.0)
    , m_tempAlertSent(false)
    , m_lastTempAlertValue(0.0)
{
    // Connecter les signaux Arduino
    connect(m_arduino, &Arduino::temperatureReceived,
            this,      &ArduinoMonitor::onTemperatureReceived);
    connect(m_arduino, &Arduino::weightStable,
            this,      &ArduinoMonitor::onWeightStable);
    connect(m_arduino, &Arduino::errorOccurred, this, [this](const QString &msg){
        qDebug() << "⚠ Arduino erreur:" << msg;
    });

    // Timer de polling température
    connect(m_tempTimer, &QTimer::timeout, this, &ArduinoMonitor::onTempPollTimer);

    // Timer de reconnexion automatique (toutes les 30 s si déconnecté)
    m_reconnectTimer->setInterval(30000);
    connect(m_reconnectTimer, &QTimer::timeout, this, [this](){
        if (!m_arduino->isConnected()) {
            qDebug() << "🔄 Tentative de reconnexion Arduino...";
            connectArduino();
        }
    });

    // Tentative de connexion initiale
    connectArduino();
}

ArduinoMonitor::~ArduinoMonitor()
{
    stopTemperatureMonitoring();
    m_arduino->close_arduino();
}

// ─────────────────────────────────────────────────────────────────────────────
//  Connexion Arduino
// ─────────────────────────────────────────────────────────────────────────────
void ArduinoMonitor::connectArduino()
{
    if (m_arduino->isConnected()) return;

    int ret = m_arduino->connect_arduino();
    if (ret == 0) {
        m_arduino->startContinuousRead();
        m_reconnectTimer->stop();
        emit arduinoConnected(m_arduino->getPortName());
    } else {
        qDebug() << "⚠ Arduino non disponible — reconnexion dans 30 s";
        m_reconnectTimer->start();
        emit arduinoDisconnected();
    }
}

bool ArduinoMonitor::isConnected() const
{
    return m_arduino->isConnected();
}

// ─────────────────────────────────────────────────────────────────────────────
//  SCÉNARIO 1 : Surveillance température
// ─────────────────────────────────────────────────────────────────────────────
void ArduinoMonitor::startTemperatureMonitoring()
{
    if (!m_arduino->isConnected()) {
        qDebug() << "⚠ Arduino non connecté — surveillance température en attente";
        return;
    }
    m_tempTimer->start(TEMP_POLL_MS);
    qDebug() << "🌡 Surveillance température démarrée (intervalle:" << TEMP_POLL_MS / 1000 << "s)";
}

void ArduinoMonitor::stopTemperatureMonitoring()
{
    m_tempTimer->stop();
    qDebug() << "🌡 Surveillance température arrêtée";
}

void ArduinoMonitor::onTempPollTimer()
{
    if (m_arduino->isConnected())
        m_arduino->requestTemperature();
}

void ArduinoMonitor::onTemperatureReceived(double celsius)
{
    qDebug() << "🌡 Température stock cuir:" << celsius << "°C";
    saveTemperatureReading(celsius);
    emit temperatureUpdated(celsius);

    bool anomalie = (celsius > TEMP_MAX_CELSIUS || celsius < TEMP_MIN_CELSIUS);

    if (anomalie) {
        QString msg;
        if (celsius > TEMP_MAX_CELSIUS)
            msg = QString("⚠ TEMPÉRATURE ÉLEVÉE : %1°C (max autorisé : %2°C)")
                      .arg(celsius, 0, 'f', 1).arg(TEMP_MAX_CELSIUS);
        else
            msg = QString("⚠ TEMPÉRATURE BASSE : %1°C (min autorisé : %2°C)")
                      .arg(celsius, 0, 'f', 1).arg(TEMP_MIN_CELSIUS);

        emit temperatureAlert(celsius, msg);

        // Naviguer automatiquement vers Matières Premières
        emit navigateToMatieres();

        // LED rouge
        m_arduino->ledRed();

        // Email d'alerte (anti-spam : envoyer seulement si valeur change de >1°C)
        if (!m_tempAlertSent || qAbs(celsius - m_lastTempAlertValue) > 1.0) {
            m_emailAlerte->sendTemperatureAlert(
                "ahmedomardridi5@gmail.com",
                celsius,
                TEMP_MAX_CELSIUS,
                TEMP_MIN_CELSIUS);
            m_tempAlertSent      = true;
            m_lastTempAlertValue = celsius;
        }
    } else {
        // Température normale → LED rouge éteinte
        m_arduino->ledGreen();
        if (m_tempAlertSent) {
            m_tempAlertSent = false;
            emit temperatureNormal(celsius);
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  SCÉNARIO 2 : Validation livraison fournisseur
// ─────────────────────────────────────────────────────────────────────────────
void ArduinoMonitor::startDeliveryCheck(int fournisseurId, int matiereId, double orderedQty)
{
    if (!m_arduino->isConnected()) {
        qDebug() << "⚠ Arduino non connecté — impossible de lire la balance";
        return;
    }

    m_deliveryPending       = true;
    m_deliveryFournisseurId = fournisseurId;
    m_deliveryMatiereId     = matiereId;
    m_deliveryOrderedQty    = orderedQty;
    m_lastMeasuredWeight    = 0.0;

    qDebug() << "⚖ Démarrage vérification livraison — commandé:" << orderedQty << "kg";

    // Demander une lecture de poids
    m_arduino->requestWeight();
}

void ArduinoMonitor::cancelDeliveryCheck()
{
    m_deliveryPending = false;
    m_arduino->ledOff();
    qDebug() << "⚖ Vérification livraison annulée";
}

void ArduinoMonitor::onWeightStable(double kg)
{
    if (!m_deliveryPending) return;

    m_lastMeasuredWeight = kg;
    double ordered = m_deliveryOrderedQty;
    double diffPct = (ordered > 0.0) ? qAbs(kg - ordered) / ordered * 100.0 : 100.0;

    qDebug() << "⚖ Poids mesuré:" << kg << "kg | Commandé:" << ordered
             << "kg | Écart:" << diffPct << "%";

    emit deliveryWeightRead(kg, ordered, diffPct);

    if (diffPct <= WEIGHT_TOLERANCE_PCT) {
        // ── OK ──────────────────────────────────────────────────────────────
        m_arduino->ledGreen();
        m_deliveryPending = false;

        saveDeliveryRecord(kg, true);
        emit deliveryValidated(m_deliveryMatiereId, kg);

    } else {
        // ── NOK : LED rouge uniquement ───────────────────────────────────────
        m_arduino->ledRed();
        m_deliveryPending = false;

        qDebug() << "🔴 Livraison NOK — LED rouge allumée | écart:" << diffPct << "%";

        emit deliveryRejected(kg, ordered, diffPct);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Persistance BD
// ─────────────────────────────────────────────────────────────────────────────
void ArduinoMonitor::saveTemperatureReading(double celsius)
{
    QSqlQuery q(Connection::instance()->getDatabase());
    q.prepare("INSERT INTO ARDUINO_TEMP_READINGS "
              "(TEMPERATURE, TIMESTAMP, IS_ALERT) "
              "VALUES (:temp, :ts, :alert)");
    q.bindValue(":temp",  celsius);
    q.bindValue(":ts",    QDateTime::currentDateTime());
    q.bindValue(":alert", (celsius > TEMP_MAX_CELSIUS || celsius < TEMP_MIN_CELSIUS) ? 1 : 0);

    if (!q.exec())
        qDebug() << "❌ Erreur sauvegarde température:" << q.lastError().text();
}

void ArduinoMonitor::saveDeliveryRecord(double measuredKg, bool validated, const QString &note)
{
    QSqlQuery q(Connection::instance()->getDatabase());
    q.prepare("INSERT INTO ARDUINO_DELIVERIES "
              "(ID_FOURNISSEUR, ID_MATIERE, ORDERED_QTY, MEASURED_WEIGHT, "
              "VALIDATED, OPERATOR_NOTE, TIMESTAMP) "
              "VALUES (:fid, :mid, :ordered, :measured, :validated, :note, :ts)");
    q.bindValue(":fid",       m_deliveryFournisseurId);
    q.bindValue(":mid",       m_deliveryMatiereId);
    q.bindValue(":ordered",   m_deliveryOrderedQty);
    q.bindValue(":measured",  measuredKg);
    q.bindValue(":validated", validated ? 1 : 0);
    q.bindValue(":note",      note);
    q.bindValue(":ts",        QDateTime::currentDateTime());

    if (!q.exec())
        qDebug() << "❌ Erreur sauvegarde livraison:" << q.lastError().text();
    else
        qDebug() << "✅ Livraison enregistrée en BD";
}
