#include "map.h"
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

Map::Map(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
}
void Map::geocodeAddress(const QString &address)
{
    QString apiKey = "56677253ce37437ea0e166d0f5f0197d";

    QString baseUrl = "https://api.opencagedata.com/geocode/v1/json";

    QUrl url(baseUrl);

    QUrlQuery query;
    query.addQueryItem("q", address);
    query.addQueryItem("key", apiKey);

    url.setQuery(query);

    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", "MyApp");

    QNetworkReply *reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, this, [=]() {

        QByteArray response = reply->readAll();
        qDebug() << "RAW RESPONSE:" << response;

        QJsonDocument doc = QJsonDocument::fromJson(response);
        QJsonObject root = doc.object();

        QJsonArray results = root["results"].toArray();

        if (!results.isEmpty()) {
            QJsonObject geometry = results[0].toObject()["geometry"].toObject();

            double lat = geometry["lat"].toDouble();
            double lon = geometry["lng"].toDouble();

            emit coordinatesReady(lat, lon);
        }

        reply->deleteLater();
    });
}
