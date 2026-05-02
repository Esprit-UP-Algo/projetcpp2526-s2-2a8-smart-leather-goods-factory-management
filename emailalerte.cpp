#include "emailalerte.h"
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QDebug>

// ── Configuration Brevo ───────────────────────────────────────────────────────
// IMPORTANT: Ne jamais commiter de vraies clés API!
// Utilisez des variables d'environnement en production
const QString EmailAlerte::API_KEY      = "YOUR_BREVO_API_KEY_HERE"; // À remplacer par votre clé
const QString EmailAlerte::SENDER_EMAIL = "your-email@example.com";
const QString EmailAlerte::SENDER_NAME  = "CUIREA - Alerte Stock


EmailAlerte::EmailAlerte(QObject *parent)
    : QObject(parent)
    , m_manager(new QNetworkAccessManager(this))
{
    connect(m_manager, &QNetworkAccessManager::finished,
            this,      &EmailAlerte::onReplyFinished);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Envoi email alerte température (matière > ambiance)
// ─────────────────────────────────────────────────────────────────────────────
void EmailAlerte::sendTemperatureAlert(const QString &recipient,
                                       const QString &nomMatiere,
                                       double tempMatiere,
                                       double tempAmbiance,
                                       double difference)
{
    QString alertColor = "#D32F2F";
    
    QString body = QString(
        "<html>"
        "<body style='font-family:Arial,sans-serif;margin:0;padding:0;background:#f4f4f4;'>"
        "<div style='max-width:500px;margin:30px auto;background:white;"
        "border-radius:8px;overflow:hidden;box-shadow:0 2px 8px rgba(0,0,0,0.1);'>"
        "  <div style='background:%1;padding:20px;text-align:center;'>"
        "    <h1 style='color:white;margin:0;font-size:22px;'>⚠ ALERTE TEMPÉRATURE - %2</h1>"
        "    <p style='color:white;margin:5px 0 0;font-size:14px;'>Stock Matières Premières — CUIREA</p>"
        "  </div>"
        "  <div style='padding:25px;'>"
        "    <p style='font-size:15px;color:#333;'>Alerte détectée à <strong>%3</strong></p>"
        "    <table style='width:100%%;border-collapse:collapse;margin:20px 0;'>"
        "      <tr style='background:#fff3f3;'>"
        "        <td style='padding:12px;border:1px solid #ffcdd2;font-weight:bold;color:#555;'>🌡 Température %2</td>"
        "        <td style='padding:12px;border:1px solid #ffcdd2;font-size:18px;font-weight:bold;color:%1;'>%4 °C</td>"
        "      </tr>"
        "      <tr>"
        "        <td style='padding:12px;border:1px solid #e0e0e0;font-weight:bold;color:#555;'>🌡 Température ambiante</td>"
        "        <td style='padding:12px;border:1px solid #e0e0e0;color:#2E7D32;font-weight:bold;'>%5 °C</td>"
        "      </tr>"
        "      <tr style='background:#fff3f3;'>"
        "        <td style='padding:12px;border:1px solid #ffcdd2;font-weight:bold;color:#555;'>⚠ Différence</td>"
        "        <td style='padding:12px;border:1px solid #ffcdd2;font-size:18px;font-weight:bold;color:%1;'>+%6 °C</td>"
        "      </tr>"
        "    </table>"
        "    <div style='background:#fff3f3;border-left:4px solid %1;padding:15px;border-radius:4px;'>"
        "      <p style='margin:0;color:%1;font-weight:bold;'>Action requise :</p>"
        "      <p style='margin:5px 0 0;color:#555;'>Vérifier les conditions de stockage du %2</p>"
        "    </div>"
        "  </div>"
        "  <div style='background:#f5f5f5;padding:12px;text-align:center;'>"
        "    <p style='margin:0;color:#999;font-size:11px;'>"
        "    CUIREA Management System — Alerte automatique Arduino</p>"
        "  </div>"
        "</div>"
        "</body></html>"
    ).arg(alertColor)
     .arg(nomMatiere)
     .arg(QDateTime::currentDateTime().toString("HH:mm:ss"))
     .arg(tempMatiere, 0, 'f', 1)
     .arg(tempAmbiance, 0, 'f', 1)
     .arg(difference, 0, 'f', 1);

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
    root["subject"]     = QString("⚠️ ALERTE TEMPÉRATURE - %1").arg(nomMatiere);
    root["htmlContent"] = body;

    QJsonDocument doc(root);
    m_manager->post(request, doc.toJson());

    qDebug() << "📧 Email alerte température envoyé à" << recipient
             << "| Matière:" << nomMatiere << tempMatiere << "°C | Ambiance:" << tempAmbiance << "°C";
}

// ─────────────────────────────────────────────────────────────────────────────
//  Envoi email alerte livraison rejetée
// ─────────────────────────────────────────────────────────────────────────────
void EmailAlerte::sendDeliveryRejectedAlert(const QString &fournisseur,
                                            double quantiteCommandee,
                                            double quantiteMesuree)
{
    QString alertColor = "#D32F2F";
    double diff = qAbs(quantiteMesuree - quantiteCommandee);
    double diffPercent = (diff / quantiteCommandee) * 100.0;
    
    QString body = QString(
        "<html>"
        "<body style='font-family:Arial,sans-serif;margin:0;padding:0;background:#f4f4f4;'>"
        "<div style='max-width:500px;margin:30px auto;background:white;"
        "border-radius:8px;overflow:hidden;box-shadow:0 2px 8px rgba(0,0,0,0.1);'>"
        "  <div style='background:%1;padding:20px;text-align:center;'>"
        "    <h1 style='color:white;margin:0;font-size:22px;'>❌ LIVRAISON REJETÉE</h1>"
        "    <p style='color:white;margin:5px 0 0;font-size:14px;'>Validation Balance Arduino</p>"
        "  </div>"
        "  <div style='padding:25px;'>"
        "    <p style='font-size:15px;color:#333;'><strong>Fournisseur:</strong> %2</p>"
        "    <p style='font-size:13px;color:#666;'>La quantité mesurée ne correspond pas à la commande.</p>"
        "    <table style='width:100%%;border-collapse:collapse;margin:20px 0;'>"
        "      <tr>"
        "        <td style='padding:12px;border:1px solid #e0e0e0;font-weight:bold;color:#555;'>📦 Quantité Commandée</td>"
        "        <td style='padding:12px;border:1px solid #e0e0e0;color:#2E7D32;font-weight:bold;'>%3 kg</td>"
        "      </tr>"
        "      <tr style='background:#fff3f3;'>"
        "        <td style='padding:12px;border:1px solid #ffcdd2;font-weight:bold;color:#555;'>⚖ Quantité Mesurée</td>"
        "        <td style='padding:12px;border:1px solid #ffcdd2;font-size:18px;font-weight:bold;color:%1;'>%4 kg</td>"
        "      </tr>"
        "      <tr style='background:#fff3f3;'>"
        "        <td style='padding:12px;border:1px solid #ffcdd2;font-weight:bold;color:#555;'>⚠ Différence</td>"
        "        <td style='padding:12px;border:1px solid #ffcdd2;font-weight:bold;color:%1;'>%5 kg (%6%%)</td>"
        "      </tr>"
        "      <tr style='background:#f9f9f9;'>"
        "        <td style='padding:12px;border:1px solid #e0e0e0;font-weight:bold;color:#555;'>🕐 Date / Heure</td>"
        "        <td style='padding:12px;border:1px solid #e0e0e0;color:#333;'>%7</td>"
        "      </tr>"
        "    </table>"
        "    <div style='background:#fff3f3;border-left:4px solid %1;padding:15px;border-radius:4px;'>"
        "      <p style='margin:0;color:%1;font-weight:bold;'>⚠ Tolérance dépassée (>5%%) :</p>"
        "      <p style='margin:5px 0 0;color:#555;'>Contactez le fournisseur pour clarification. "
        "La livraison a été enregistrée comme rejetée.</p>"
        "    </div>"
        "  </div>"
        "  <div style='background:#f5f5f5;padding:12px;text-align:center;'>"
        "    <p style='margin:0;color:#999;font-size:11px;'>"
        "    CUIREA Management System — Validation automatique HX711</p>"
        "  </div>"
        "</div>"
        "</body></html>"
    ).arg(alertColor)
     .arg(fournisseur)
     .arg(quantiteCommandee, 0, 'f', 2)
     .arg(quantiteMesuree, 0, 'f', 2)
     .arg(diff, 0, 'f', 2)
     .arg(diffPercent, 0, 'f', 1)
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
    toObj["email"] = "admin@cuirea.com";
    toArray.append(toObj);

    QJsonObject root;
    root["sender"]      = sender;
    root["to"]          = toArray;
    root["subject"]     = QString("❌ Livraison Rejetée - %1 (Diff: %2%%)")
                          .arg(fournisseur)
                          .arg(diffPercent, 0, 'f', 1);
    root["htmlContent"] = body;

    QJsonDocument doc(root);
    m_manager->post(request, doc.toJson());

    qDebug() << "📧 Email livraison rejetée envoyé | Fournisseur:" << fournisseur
             << "| Diff:" << diffPercent << "%";
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
