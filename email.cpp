#include "email.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QEventLoop>
#include <QFile>
#include <QFileInfo>

Mail::Mail(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
}

bool Mail::sendEmail(const QString &recipient,
                     const QString &subject,
                     const QString &body,
                     const QString &attachmentPath)
{
    QNetworkRequest request(QUrl("https://api.brevo.com/v3/smtp/email"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("api-key", ""); // your key

    QJsonObject senderObj;
    senderObj["email"] = "aporiaaaaa1@gmail.com";
    senderObj["name"] = "CUIREA";

    QJsonArray toArray;
    QJsonObject toObj;
    toObj["email"] = recipient;
    toArray.append(toObj);

    QJsonObject root;
    root["sender"] = senderObj;
    root["to"] = toArray;
    root["subject"] = subject;
    root["htmlContent"] = body;

    // =========================
    // 📎 ATTACHMENT PART
    // =========================
    if (!attachmentPath.isEmpty())
    {
        QFile file(attachmentPath);

        if (file.open(QIODevice::ReadOnly))
        {
            QByteArray fileData = file.readAll();
            file.close();

            QJsonObject attachmentObj;
            attachmentObj["name"] = QFileInfo(file).fileName();
            attachmentObj["content"] = QString(fileData.toBase64());

            QJsonArray attachmentsArray;
            attachmentsArray.append(attachmentObj);

            root["attachment"] = attachmentsArray;
        }
        else
        {
            qDebug() << "❌ Impossible d'ouvrir le fichier:" << attachmentPath;
        }
    }

    QJsonDocument doc(root);
    QByteArray data = doc.toJson();

    QNetworkReply *reply = manager->post(request, data);

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "❌ Erreur envoi email:" << reply->errorString();
        reply->deleteLater();
        return false;
    }

    qDebug() << "✅ Email envoyé à" << recipient;
    reply->deleteLater();
    return true;
}