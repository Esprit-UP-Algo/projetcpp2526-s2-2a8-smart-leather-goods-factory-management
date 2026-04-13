#include "tripo3dgenerator.h"
#include "envloader.h"
#include <QDebug>
#include <QDir>
#include <QCoreApplication>
#include <QFile>

// Utilise l'API Meshy (test mode gratuit ou cle reelle)
static const QString MESHY_BASE = "https://api.meshy.ai/openapi/v2/text-to-3d";

Tripo3DGenerator::Tripo3DGenerator(QObject *parent)
    : QObject(parent), m_polling(false)
{
    m_network = new QNetworkAccessManager(this);
    m_pollTimer = new QTimer(this);
    m_pollTimer->setInterval(3000);
    connect(m_pollTimer, &QTimer::timeout, this, &Tripo3DGenerator::pollTaskStatus);

    // Essayer MESHY_API_KEY d'abord, sinon cle test
    m_apiKey = EnvLoader::get("MESHY_API_KEY");
    if (m_apiKey.isEmpty())
        m_apiKey = "msy_dummy_api_key_for_test_mode_12345678";
}

void Tripo3DGenerator::generate(const QString &prompt)
{
    qDebug() << "[3D Gen] API Key:" << (m_apiKey.contains("dummy") ? "TEST MODE" : "REAL KEY");
    qDebug() << "[3D Gen] Prompt:" << prompt;

    emit progress(0, "Envoi a Meshy AI...");

    QJsonObject body;
    body["prompt"] = prompt;
    body["mode"] = "preview";

    QNetworkRequest req{QUrl(MESHY_BASE)};
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("Authorization", ("Bearer " + m_apiKey).toUtf8());

    QNetworkReply *reply = m_network->post(req, QJsonDocument(body).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onCreateTaskReply(reply);
    });
}

void Tripo3DGenerator::cancel()
{
    m_pollTimer->stop();
    m_polling = false;
}

void Tripo3DGenerator::onCreateTaskReply(QNetworkReply *reply)
{
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        emit error("Erreur reseau: " + reply->errorString());
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    QJsonObject root = doc.object();
    qDebug() << "[3D Gen] Create response:" << doc.toJson(QJsonDocument::Compact).left(200);

    m_taskId = root["result"].toString();
    if (m_taskId.isEmpty()) {
        emit error("Pas de task_id dans la reponse");
        return;
    }

    qDebug() << "[3D Gen] Task ID:" << m_taskId;
    emit progress(10, "Tache creee, generation en cours...");
    m_polling = true;
    m_pollTimer->start();
}

void Tripo3DGenerator::pollTaskStatus()
{
    if (!m_polling || m_taskId.isEmpty()) return;

    QNetworkRequest req{QUrl(MESHY_BASE + "/" + m_taskId)};
    req.setRawHeader("Authorization", ("Bearer " + m_apiKey).toUtf8());

    QNetworkReply *reply = m_network->get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onPollReply(reply);
    });
}

void Tripo3DGenerator::onPollReply(QNetworkReply *reply)
{
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
        emit progress(0, "Erreur poll: " + reply->errorString());
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    QJsonObject data = doc.object();
    QString status = data["status"].toString();
    int prog = data["progress"].toInt();

    qDebug() << "[3D Gen] Status:" << status << "Progress:" << prog;
    emit progress(prog, QString("Generation: %1%").arg(prog));

    if (status == "SUCCEEDED") {
        m_pollTimer->stop();
        m_polling = false;

        QString imageUrl = data["thumbnail_url"].toString();
        QString modelUrl = data["model_urls"].toObject()["glb"].toString();

        qDebug() << "[3D Gen] Thumbnail:" << imageUrl.left(80);
        qDebug() << "[3D Gen] Model GLB:" << modelUrl.left(80);

        if (!imageUrl.isEmpty()) {
            emit progress(95, "Telechargement de l'image...");
            QNetworkRequest imgReq{QUrl(imageUrl)};
            QNetworkReply *imgReply = m_network->get(imgReq);
            connect(imgReply, &QNetworkReply::finished, this, [this, imgReply, modelUrl]() {
                imgReply->deleteLater();
                if (imgReply->error() == QNetworkReply::NoError) {
                    QByteArray imgData = imgReply->readAll();
                    QString dir = QCoreApplication::applicationDirPath() + "/photos_articles";
                    QDir().mkpath(dir);
                    QString filename = dir + "/meshy_" + m_taskId.left(12) + ".png";
                    QFile f(filename);
                    if (f.open(QIODevice::WriteOnly)) {
                        f.write(imgData);
                        f.close();
                        m_savedImagePath = filename;
                        qDebug() << "[3D Gen] Image sauvegardee:" << filename;
                    }
                }
                emit finished(m_savedImagePath, modelUrl);
            });
        } else {
            emit finished("", modelUrl);
        }

    } else if (status == "FAILED") {
        m_pollTimer->stop();
        m_polling = false;
        QString errMsg = data["task_error"].toObject()["message"].toString();
        emit error("Generation echouee: " + errMsg);
    }
}
