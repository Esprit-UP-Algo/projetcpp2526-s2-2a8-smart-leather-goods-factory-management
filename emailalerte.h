#ifndef EMAILALERTE_H
#define EMAILALERTE_H

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>

// ─────────────────────────────────────────────────────────────────────────────
//  EmailAlerte — Envoi d'emails d'alerte Arduino via Brevo API
//  Utilisé exclusivement pour :
//    • Alerte température anormale (stock cuir)
// ─────────────────────────────────────────────────────────────────────────────
class EmailAlerte : public QObject
{
    Q_OBJECT

public:
    explicit EmailAlerte(QObject *parent = nullptr);

    // Envoie un email d'alerte température
    // recipient : adresse email du responsable stock
    // celsius   : température mesurée
    // tempMax   : seuil max configuré
    // tempMin   : seuil min configuré
    void sendTemperatureAlert(const QString &recipient,
                              double celsius,
                              double tempMax,
                              double tempMin);

signals:
    void emailSent(const QString &recipient);
    void emailFailed(const QString &error);

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *m_manager;

    static const QString API_KEY;
    static const QString SENDER_EMAIL;
    static const QString SENDER_NAME;
};

#endif // EMAILALERTE_H
