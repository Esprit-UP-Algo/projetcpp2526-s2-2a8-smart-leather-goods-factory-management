#ifndef MAIL_H
#define MAIL_H

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>

class Mail : public QObject
{
    Q_OBJECT
public:
    explicit Mail(QObject *parent = nullptr);

    // Fonction pour envoyer un email via Brevo
    bool sendEmail(const QString &recipient, const QString &subject, const QString &body);

private:
    QNetworkAccessManager *manager;
};

#endif // MAIL_H
