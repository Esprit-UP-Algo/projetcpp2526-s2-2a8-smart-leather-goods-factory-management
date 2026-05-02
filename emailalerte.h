#ifndef EMAILALERTE_H
#define EMAILALERTE_H

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>

// ─────────────────────────────────────────────────────────────────────────────
//  EmailAlerte — Envoi d'emails d'alerte Arduino via Brevo API
//  Utilisé pour :
//    • Alerte température anormale (stock cuir)
//    • Alerte différence température matière/ambiance
//    • Alerte livraison rejetée
// ─────────────────────────────────────────────────────────────────────────────
class EmailAlerte : public QObject
{
    Q_OBJECT

public:
    explicit EmailAlerte(QObject *parent = nullptr);

    // Envoie un email d'alerte température (matière > ambiance)
    // recipient     : adresse email du responsable stock
    // nomMatiere    : nom de la matière première (ex: "Cuir")
    // tempMatiere   : température matière mesurée
    // tempAmbiance  : température ambiante mesurée
    // difference    : différence (tempMatiere - tempAmbiance)
    void sendTemperatureAlert(const QString &recipient,
                              const QString &nomMatiere,
                              double tempMatiere,
                              double tempAmbiance,
                              double difference);

    // Envoie un email d'alerte livraison rejetée
    void sendDeliveryRejectedAlert(const QString &fournisseur, 
                                   double quantiteCommandee, 
                                   double quantiteMesuree);

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
