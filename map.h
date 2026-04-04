#ifndef MAP_H
#define MAP_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class Map : public QObject
{
    Q_OBJECT

public:
    explicit Map(QObject *parent = nullptr);

    void geocodeAddress(const QString &address);

signals:
    void coordinatesReady(double lat, double lon);

private:
    QNetworkAccessManager *manager;
};

#endif // MAP_H
