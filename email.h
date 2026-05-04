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

    // Updated: added attachment file path
    bool sendEmail(const QString &recipient,
                   const QString &subject,
                   const QString &body,
                   const QString &attachmentPath = QString());

private:
    QNetworkAccessManager *manager;
};

#endif // MAIL_H