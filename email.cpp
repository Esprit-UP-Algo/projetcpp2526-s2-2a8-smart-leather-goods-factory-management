#include "email.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QEventLoop>

Mail::Mail(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
}

bool Mail::sendEmail(const QString &recipient, const QString &subject, const QString &body)
{
    QNetworkRequest request(QUrl("https://api.brevo.com/v3/smtp/email"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("api-key", "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"); // remplacer par votre clé

    QJsonObject senderObj;
    senderObj["email"] = "aporiaaaaa1@gmail.com"; // email vérifié dans Brevo
    senderObj["name"] = "CUIREA";

    QJsonObject toObj;
    toObj["email"] = recipient;

    QJsonArray toArray;
    toArray.append(toObj);

    QJsonObject root;
    root["sender"] = senderObj;
    root["to"] = toArray;
    root["subject"] = subject;
    root["htmlContent"] = body;

    QJsonDocument doc(root);
    QByteArray data = doc.toJson();

    QNetworkReply *reply = manager->post(request, data);

    // Attente de la réponse (bloquant)
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "❌ Erreur envoi email:" << reply->errorString();
        reply->deleteLater();
        return false;
    }

    qDebug() << "✅ Email envoyé à" << recipient;
    reply->deleteLater();
    return true;
}
