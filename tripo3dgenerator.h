#ifndef TRIPO3DGENERATOR_H
#define TRIPO3DGENERATOR_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QString>

class Tripo3DGenerator : public QObject
{
    Q_OBJECT
public:
    explicit Tripo3DGenerator(QObject *parent = nullptr);

    void generate(const QString &prompt);
    void cancel();

signals:
    void progress(int percent, const QString &status);
    void finished(const QString &imageUrl, const QString &modelUrl);
    void error(const QString &message);

private slots:
    void onCreateTaskReply(QNetworkReply *reply);
    void pollTaskStatus();
    void onPollReply(QNetworkReply *reply);

private:
    QNetworkAccessManager *m_network;
    QTimer *m_pollTimer;
    QString m_apiKey;
    QString m_taskId;
    QString m_savedImagePath;
    bool m_polling;
};

#endif // TRIPO3DGENERATOR_H
