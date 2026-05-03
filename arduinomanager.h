#ifndef ARDUINOMANAGER_H
#define ARDUINOMANAGER_H

#include <QObject>
#include <QTimer>
#include <QMap>
#include "arduino.h"

// ─────────────────────────────────────────────────────────────────────────────
//  ArduinoManager — Découverte et gestion de 3 cartes Arduino simultanées
//
//  Principe (Option A — identification automatique par message) :
//    1. scanAndIdentify() ouvre chaque port Arduino détecté
//    2. Attend le message "ROLE:xxx" envoyé par chaque carte au démarrage
//    3. Assigne chaque port à la bonne instance Arduino selon son rôle
//
//  Rôles attendus (définis dans les sketches .ino) :
//    "ROLE:TEMP_BALANCE"  → m_arduinoTempBalance
//    "ROLE:LIVRAISON"     → m_arduinoLivraison
//    "ROLE:POINTAGE"      → m_arduinoPointage
//
//  Usage :
//    ArduinoManager *mgr = new ArduinoManager(this);
//    connect(mgr, &ArduinoManager::discoveryComplete, this, &MainWindow::onArduinoReady);
//    mgr->scanAndIdentify();
// ─────────────────────────────────────────────────────────────────────────────
class ArduinoManager : public QObject
{
    Q_OBJECT

public:
    // Délai d'attente max pour recevoir le ROLE: (ms)
    static constexpr int ROLE_TIMEOUT_MS = 8000;  // Augmenté à 8 secondes pour cartes lentes

    explicit ArduinoManager(QObject *parent = nullptr);
    ~ArduinoManager();

    // Lance la découverte asynchrone de toutes les cartes
    void scanAndIdentify();

    // Accès aux instances identifiées (nullptr si non trouvée)
    Arduino* arduinoTempBalance() const { return m_arduinoTempBalance; }
    Arduino* arduinoLivraison()   const { return m_arduinoLivraison;   }
    Arduino* arduinoPointage()    const { return m_arduinoPointage;    }

    // Résumé de l'état de connexion
    QString statusSummary() const;

signals:
    // Émis quand toutes les cartes ont été identifiées (ou timeout)
    void discoveryComplete();

    // Émis pour chaque carte identifiée avec succès
    void cardIdentified(const QString &role, const QString &port);

    // Émis si une carte attendue n'a pas été trouvée
    void cardMissing(const QString &role);

private slots:
    void onRoleTimeout();

private:
    void tryIdentifyPort(const QString &portName);
    void assignRole(const QString &role, const QString &portName);
    void checkAllIdentified();

    Arduino* m_arduinoTempBalance = nullptr;
    Arduino* m_arduinoLivraison   = nullptr;
    Arduino* m_arduinoPointage    = nullptr;

    // Ports en cours d'identification : port → QSerialPort temporaire
    QMap<QString, QSerialPort*> m_pendingPorts;
    QMap<QString, QByteArray>   m_pendingBuffers;

    QTimer  *m_timeoutTimer = nullptr;
    int      m_totalPorts   = 0;
    int      m_identified   = 0;
};

#endif // ARDUINOMANAGER_H
