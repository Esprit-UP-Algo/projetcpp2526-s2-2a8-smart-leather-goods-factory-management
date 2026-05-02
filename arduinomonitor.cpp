#include "arduinomonitor.h"
#include "connection.h"
#include "emailalerte.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QDebug>

ArduinoMonitor::ArduinoMonitor(QObject *parent)
    : QObject(parent)
    , m_emailAlerte(new EmailAlerte(this))
    , m_tempTimer(new QTimer(this))
    , m_reconnectTimer(new QTimer(this))
{
    connect(m_tempTimer, &QTimer::timeout, this, &ArduinoMonitor::onTempPollTimer);

    m_reconnectTimer->setInterval(30000);
    connect(m_reconnectTimer, &QTimer::timeout, this, [this]() {
        if (m_arduinoTempBalance && !m_arduinoTempBalance->isConnected()) {
            qDebug() << "🔄 Tentative reconnexion TEMP_BALANCE...";
            connectArduino();
        }
    });
}

ArduinoMonitor::~ArduinoMonitor()
{
    stopTemperatureMonitoring();
    // Ne pas supprimer les instances Arduino — elles appartiennent à ArduinoManager
}

// ─────────────────────────────────────────────────────────────────────────────
//  Injection des instances Arduino
// ─────────────────────────────────────────────────────────────────────────────
void ArduinoMonitor::setArduinoTempBalance(Arduino *arduino)
{
    if (m_arduinoTempBalance)
        disconnect(m_arduinoTempBalance, nullptr, this, nullptr);

    m_arduinoTempBalance = arduino;

    if (m_arduinoTempBalance) {
        connect(m_arduinoTempBalance, &Arduino::temperatureReceived,
                this, &ArduinoMonitor::onTemperatureReceived);
        connect(m_arduinoTempBalance, &Arduino::weightStable,
                this, &ArduinoMonitor::onWeightStable);
        connect(m_arduinoTempBalance, &Arduino::errorOccurred, this, [](const QString &msg) {
            qDebug() << "⚠ TEMP_BALANCE erreur:" << msg;
        });
        qDebug() << "✅ ArduinoMonitor ← TEMP_BALANCE sur" << m_arduinoTempBalance->getPortName();
    }
}

// Compatibilité ancienne API
void ArduinoMonitor::setArduino(Arduino *arduino)
{
    setArduinoTempBalance(arduino);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Connexion Arduino
// ─────────────────────────────────────────────────────────────────────────────
void ArduinoMonitor::connectArduino()
{
    if (!m_arduinoTempBalance) {
        qDebug() << "⚠ TEMP_BALANCE non défini dans ArduinoMonitor";
        return;
    }
    if (m_arduinoTempBalance->isConnected()) return;

    int ret = m_arduinoTempBalance->connect_arduino();
    if (ret == 0) {
        m_arduinoTempBalance->startContinuousRead();
        m_reconnectTimer->stop();
        emit arduinoConnected(m_arduinoTempBalance->getPortName());
    } else {
        qDebug() << "⚠ TEMP_BALANCE non disponible — reconnexion dans 30 s";
        m_reconnectTimer->start();
        emit arduinoDisconnected();
    }
}

bool ArduinoMonitor::isConnected() const
{
    return m_arduinoTempBalance && m_arduinoTempBalance->isConnected();
}

// ─────────────────────────────────────────────────────────────────────────────
//  SCÉNARIO 1 : Surveillance température
// ─────────────────────────────────────────────────────────────────────────────
void ArduinoMonitor::startTemperatureMonitoring()
{
    if (!m_arduinoTempBalance) {
        qDebug() << "⚠ TEMP_BALANCE non défini — surveillance impossible";
        return;
    }
    if (!m_arduinoTempBalance->isConnected()) {
        qDebug() << "⚠ TEMP_BALANCE non connecté — surveillance en attente";
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
    if (m_arduinoTempBalance && m_arduinoTempBalance->isConnected())
        m_arduinoTempBalance->requestTemperature();
}

void ArduinoMonitor::onTemperatureReceived(double tempMatiere, double tempAmbiance)
{
    qDebug() << "🌡 Matière:" << tempMatiere << "°C | Ambiance:" << tempAmbiance << "°C";
    emit temperatureUpdated(tempMatiere);

    bool anomalie = (tempMatiere > tempAmbiance);

    if (anomalie) {
        QString nomMatiere = "Cuir";
        QSqlQuery q(Connection::instance()->getDatabase());
        q.prepare("SELECT NOM FROM MATIERES_PREMIERES WHERE ID_MATIERE = 3");
        if (q.exec() && q.next())
            nomMatiere = q.value(0).toString();

        saveTemperatureReading(tempMatiere, tempAmbiance, true);

        QString msg = QString("⚠ ALERTE TEMPÉRATURE - %1: %2°C > Ambiance: %3°C (Δ: +%4°C)")
                      .arg(nomMatiere)
                      .arg(tempMatiere,  0, 'f', 1)
                      .arg(tempAmbiance, 0, 'f', 1)
                      .arg(tempMatiere - tempAmbiance, 0, 'f', 1);

        emit temperatureAlert(tempMatiere, msg);

        if (!m_tempAlertSent || qAbs(tempMatiere - m_lastTempAlertValue) > 1.0) {
            m_emailAlerte->sendTemperatureAlert("ahmedomardridi5@gmail.com",
                                                nomMatiere,
                                                tempMatiere,
                                                tempAmbiance,
                                                tempMatiere - tempAmbiance);
            m_tempAlertSent      = true;
            m_lastTempAlertValue = tempMatiere;
        }
    } else {
        saveTemperatureReading(tempMatiere, tempAmbiance, false);
        if (m_tempAlertSent) {
            m_tempAlertSent = false;
            emit temperatureNormal(tempMatiere);
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  SCÉNARIO 2 : Validation livraison fournisseur
// ─────────────────────────────────────────────────────────────────────────────
void ArduinoMonitor::startDeliveryCheck(int fournisseurId, int matiereId, double orderedQty)
{
    if (!m_arduinoTempBalance || !m_arduinoTempBalance->isConnected()) {
        qDebug() << "⚠ TEMP_BALANCE non connecté — impossible de lire la balance";
        return;
    }

    m_deliveryPending       = true;
    m_deliveryFournisseurId = fournisseurId;
    m_deliveryMatiereId     = matiereId;
    m_deliveryOrderedQty    = orderedQty;
    m_lastMeasuredWeight    = 0.0;

    qDebug() << "⚖ Démarrage vérification livraison — commandé:" << orderedQty << "kg";
    m_arduinoTempBalance->requestWeight();
}

void ArduinoMonitor::cancelDeliveryCheck()
{
    m_deliveryPending = false;
    if (m_arduinoTempBalance) m_arduinoTempBalance->ledOff();
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
        m_deliveryPending = false;
        saveDeliveryRecord(kg, true);
        emit deliveryValidated(m_deliveryFournisseurId, m_deliveryMatiereId, kg);
    } else {
        m_deliveryPending = false;
        if (m_arduinoTempBalance) m_arduinoTempBalance->ledRed();
        saveDeliveryRecord(kg, false);
        qDebug() << "🔴 Livraison NOK | mesuré:" << kg << "kg | commandé:" << ordered
                 << "kg | écart:" << diffPct << "%";
        emit deliveryRejected(m_deliveryFournisseurId, m_deliveryMatiereId, kg, ordered, diffPct);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Persistance BD
// ─────────────────────────────────────────────────────────────────────────────
void ArduinoMonitor::saveTemperatureReading(double tempMatiere, double tempAmbiance, bool isAlert)
{
    QSqlQuery q(Connection::instance()->getDatabase());
    q.prepare("INSERT INTO ARDUINO_TEMP_READINGS "
              "(ID_MATIERE, TEMP_MATIERE, TEMP_AMBIANTE, IS_ALERT) "
              "VALUES (3, :tmat, :tamb, :alert)");
    q.bindValue(":tmat",  tempMatiere);
    q.bindValue(":tamb",  tempAmbiance);
    q.bindValue(":alert", isAlert ? 1 : 0);

    if (!q.exec())
        qDebug() << "❌ Erreur sauvegarde température:" << q.lastError().text();
    else
        qDebug() << "✅ Température sauvegardée (ID_MATIERE=3)";
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
