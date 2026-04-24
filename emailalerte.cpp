#include "emailalerte.h"
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QDebug>

// ── Configuration Brevo ───────────────────────────────────────────────────────
const QString EmailAlerte::API_KEY      = "";
const QString EmailAlerte::SENDER_EMAIL = "omardridi123466@gmail.com";
const QString EmailAlerte::SENDER_NAME  = "CUIREA - Alerte Stock";

EmailAlerte::EmailAlerte(QObject *parent)
    : QObject(parent)
    , m_manager(new QNetworkAccessManager(this))
{
    connect(m_manager, &QNetworkAccessManager::finished,
            this,      &EmailAlerte::onReplyFinished);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Envoi email alerte température
// ─────────────────────────────────────────────────────────────────────────────
void EmailAlerte::sendTemperatureAlert(const QString &recipient,
                                       double celsius,
                                       double tempMax,
                                       double tempMin)
{
    // ── Construire le corps HTML ──────────────────────────────────────────────
    QString alertType = (celsius > tempMax) ? "ÉLEVÉE 🔥" : "BASSE 🧊";
    QString alertColor = "#D32F2F";

    QString body = QString(
        "<html>"
        "<body style='font-family:Arial,sans-serif;margin:0;padding:0;background:#f4f4f4;'>"
        "<div style='max-width:500px;margin:30px auto;background:white;"
        "border-radius:8px;overflow:hidden;box-shadow:0 2px 8px rgba(0,0,0,0.1);'>"

        "  <div style='background:%1;padding:20px;text-align:center;'>"
        "    <h1 style='color:white;margin:0;font-size:22px;'>⚠ ALERTE TEMPÉRATURE</h1>"
        "    <p style='color:white;margin:5px 0 0;font-size:14px;'>Stock Cuir — CUIREA</p>"
        "  </div>"

        "  <div style='padding:25px;'>"
        "    <p style='font-size:15px;color:#333;'>Une température <strong>%2</strong> "
        "a été détectée dans le département stock des matières premières.</p>"

        "    <table style='width:100%;border-collapse:collapse;margin:20px 0;'>"
        "      <tr style='background:#fff3f3;'>"
        "        <td style='padding:12px;border:1px solid #ffcdd2;font-weight:bold;color:#555;'>🌡 Température mesurée</td>"
        "        <td style='padding:12px;border:1px solid #ffcdd2;font-size:18px;font-weight:bold;color:%1;'>%3 °C</td>"
        "      </tr>"
        "      <tr>"
        "        <td style='padding:12px;border:1px solid #e0e0e0;font-weight:bold;color:#555;'>✅ Plage normale</td>"
        "        <td style='padding:12px;border:1px solid #e0e0e0;color:#2E7D32;font-weight:bold;'>%4°C — %5°C</td>"
        "      </tr>"
        "      <tr style='background:#f9f9f9;'>"
        "        <td style='padding:12px;border:1px solid #e0e0e0;font-weight:bold;color:#555;'>🕐 Date / Heure</td>"
        "        <td style='padding:12px;border:1px solid #e0e0e0;color:#333;'>%6</td>"
        "      </tr>"
        "    </table>"

        "    <div style='background:#fff3f3;border-left:4px solid %1;padding:15px;border-radius:4px;'>"
        "      <p style='margin:0;color:%1;font-weight:bold;'>Action requise :</p>"
        "      <p style='margin:5px 0 0;color:#555;'>Vérifiez immédiatement les conditions de "
        "stockage du cuir et prenez les mesures nécessaires.</p>"
        "    </div>"
        "  </div>"

        "  <div style='background:#f5f5f5;padding:12px;text-align:center;'>"
        "    <p style='margin:0;color:#999;font-size:11px;'>"
        "    CUIREA Management System — Alerte automatique Arduino</p>"
        "  </div>"
        "</div>"
        "</body></html>")
        .arg(alertColor)
        .arg(alertType)
        .arg(celsius, 0, 'f', 1)
        .arg(tempMin, 0, 'f', 0)
        .arg(tempMax, 0, 'f', 0)
        .arg(QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss"));

    // ── Construire la requête Brevo ───────────────────────────────────────────
    QNetworkRequest request(QUrl("https://api.brevo.com/v3/smtp/email"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("api-key", API_KEY.toUtf8());

    QJsonObject sender;
    sender["email"] = SENDER_EMAIL;
    sender["name"]  = SENDER_NAME;

    QJsonArray toArray;
    QJsonObject toObj;
    toObj["email"] = recipient;
    toArray.append(toObj);

    QJsonObject root;
    root["sender"]      = sender;
    root["to"]          = toArray;
    root["subject"]     = QString("🌡 ALERTE TEMPÉRATURE %1 — Stock Cuir CUIREA")
                              .arg(celsius > tempMax ? "ÉLEVÉE" : "BASSE");
    root["htmlContent"] = body;

    QJsonDocument doc(root);
    m_manager->post(request, doc.toJson());

    qDebug() << "📧 Email alerte température envoyé à" << recipient
             << "| T =" << celsius << "°C";
}

// ─────────────────────────────────────────────────────────────────────────────
//  Réponse Brevo
// ─────────────────────────────────────────────────────────────────────────────
void EmailAlerte::onReplyFinished(QNetworkReply *reply)
{
    QByteArray resp = reply->readAll();
    int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (reply->error() == QNetworkReply::NoError) {
        qDebug() << "✅ Email alerte envoyé avec succès | HTTP" << httpStatus;
        emit emailSent(reply->url().toString());
    } else {
        QString err = reply->errorString();
        qDebug() << "❌ Erreur envoi email alerte | HTTP" << httpStatus;
        qDebug() << "   Erreur Qt :" << err;
        qDebug() << "   Réponse Brevo :" << resp;
        emit emailFailed(err + " | " + resp);
    }
    reply->deleteLater();
}
