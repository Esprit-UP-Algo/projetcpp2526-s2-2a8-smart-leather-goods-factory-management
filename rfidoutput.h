#ifndef RFIDOUTPUT_H
#define RFIDOUTPUT_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QDebug>
#include <QMessageBox>

class RfidOutput : public QObject
{
    Q_OBJECT

public:
    explicit RfidOutput(QObject *parent = nullptr);
    ~RfidOutput();

    // Initialise le port serie (a appeler au demarrage)
    bool initSerialPort(const QString &portName, int baudRate = 9600);

    // Utilise la connexion Oracle deja ouverte dans ton projet
    void setDatabase(QSqlDatabase db);

    // TEST : simuler un badge sans Arduino
    void simulerBadge(const QString &uid);

private slots:
    // Declenche automatiquement quand l'Arduino envoie un UID
    void onDataReceived();

private:
    QSerialPort *m_serial;
    QSqlDatabase m_db;

    // Traitement principal : recoit l'UID complet
    void processUID(const QString &uid);

    // Requete 1 : verifier si l'employe existe
    // Retourne true si trouve, et remplit id/nom/prenom
    bool verifierEmploye(const QString &uid, int &idEmploye,
                         QString &prenom, QString &nom);

    // Requete 2 : trouver le dernier pointage pour le toggle
    // Retourne 'E' ou 'S' (defaut 'S' pour que le premier soit 'E')
    QChar dernierPointage(int idEmploye);

    // Requete 3 : inserer le nouveau pointage
    bool insererPointage(int idEmploye, QChar type);

    // Envoie la reponse a l'Arduino via le port serie
    void envoyerReponse(const QString &message);

    // Mode simulation (sans Arduino)
    bool m_simulationMode = false;
};

#endif // RFIDOUTPUT_H
