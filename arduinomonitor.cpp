#include "arduinomonitor.h"
#include "connection.h"
#include "emailalerte.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QDebug>

ArduinoMonitor::ArduinoMonitor(QObject *parent)
    : QObject(parent)
    , m_arduino(nullptr) // Sera défini par setArduino()
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
    // Timer de polling température
    connect(m_tempTimer, &QTimer::timeout, this, &ArduinoMonitor::onTempPollTimer);

    // Timer de reconnexion automatique (toutes les 30 s si déconnecté)
    m_reconnectTimer->setInterval(30000);
    connect(m_reconnectTimer, &QTimer::timeout, this, [this](){
        if (m_arduino && !m_arduino->isConnected()) {
            qDebug() << "🔄 Tentative de reconnexion Arduino...";
            connectArduino();
        }
    });
}

void ArduinoMonitor::setArduino(Arduino *arduino)
{
    if (m_arduino) {
        // Déconnecter l'ancienne instance
        disconnect(m_arduino, nullptr, this, nullptr);
    }
    
    m_arduino = arduino;
    
    if (m_arduino) {
        // Connecter les signaux Arduino
        connect(m_arduino, &Arduino::temperatureReceived,
                this,      &ArduinoMonitor::onTemperatureReceived);
        connect(m_arduino, &Arduino::weightStable,
                this,      &ArduinoMonitor::onWeightStable);
        connect(m_arduino, &Arduino::errorOccurred, this, [this](const QString &msg){
            qDebug() << "⚠ Arduino erreur:" << msg;
        });
        
        qDebug() << "✅ ArduinoMonitor utilise l'instance Arduino partagée";
    }
}

ArduinoMonitor::~ArduinoMonitor()
{
    stopTemperatureMonitoring();
    // Ne pas fermer m_arduino car il est partagé avec MainWindow
}

// ─────────────────────────────────────────────────────────────────────────────
//  Connexion Arduino
// ─────────────────────────────────────────────────────────────────────────────
void ArduinoMonitor::connectArduino()
{
    if (!m_arduino) {
        qDebug() << "⚠ Arduino non défini dans ArduinoMonitor";
        return;
    }
    
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
    if (!m_arduino) {
        qDebug() << "⚠ Arduino non défini — surveillance température impossible";
        return;
    }
    
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

void ArduinoMonitor::onTemperatureReceived(double tempMatiere, double tempAmbiance)
{
    qDebug() << "🌡 Température matière:" << tempMatiere << "°C | Ambiance:" << tempAmbiance << "°C";
    emit temperatureUpdated(tempMatiere);

    // SCÉNARIO: T_matière > T_ambiance → ALERTE
    bool anomalie = (tempMatiere > tempAmbiance);

    if (anomalie) {
        // Récupérer le nom de la matière depuis la BDD (ID_MATIERE=3)
        QString nomMatiere = "Cuir"; // Valeur par défaut
        QSqlQuery q(Connection::instance()->getDatabase());
        q.prepare("SELECT NOM FROM MATIERES_PREMIERES WHERE ID_MATIERE = 3");
        if (q.exec() && q.next()) {
            nomMatiere = q.value(0).toString();
            qDebug() << "✅ Nom matière récupéré:" << nomMatiere;
        } else {
            qDebug() << "⚠ Impossible de récupérer nom matière, utilisation valeur par défaut:" << nomMatiere;
            qDebug() << "   Erreur SQL:" << q.lastError().text();
        }
        
        // Sauvegarder avec flag alerte
        saveTemperatureReading(tempMatiere, tempAmbiance, true);

        QString msg = QString("⚠ ALERTE TEMPÉRATURE - %1: %2°C > Ambiance: %3°C (Δ: +%4°C)")
                      .arg(nomMatiere)
                      .arg(tempMatiere, 0, 'f', 1)
                      .arg(tempAmbiance, 0, 'f', 1)
                      .arg(tempMatiere - tempAmbiance, 0, 'f', 1);

        emit temperatureAlert(tempMatiere, msg);

        // LED gérée automatiquement par l'Arduino (pas de commande Qt)
        // L'Arduino allume la LED quand T_matière > T_ambiance

        // Email d'alerte (anti-spam : envoyer seulement si valeur change de >1°C)
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
        // Température normale → sauvegarde sans alerte
        saveTemperatureReading(tempMatiere, tempAmbiance, false);
        // LED gérée automatiquement par l'Arduino
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
        // ── OK : poids conforme → enregistrer en BD ─────────────────────────
        m_deliveryPending = false;
        saveDeliveryRecord(kg, true);
        emit deliveryValidated(m_deliveryFournisseurId, m_deliveryMatiereId, kg);
    } else {
        // ── NOK : poids non conforme → LED rouge + enregistrement BD ────────
        m_deliveryPending = false;
        m_arduino->ledRed();
        saveDeliveryRecord(kg, false);  // validated=0 → NOK

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
    q.bindValue(":tmat",   tempMatiere);
    q.bindValue(":tamb",   tempAmbiance);
    q.bindValue(":alert",  isAlert ? 1 : 0);

    if (!q.exec()) {
        qDebug() << "❌ Erreur sauvegarde température:" << q.lastError().text();
        qDebug() << "   SQL:" << q.lastQuery();
    } else {
        qDebug() << "✅ Température sauvegardée en BDD (ID_MATIERE=3)";
    }
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
