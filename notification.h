#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QProgressBar>
#include <QList>
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonObject>
#include <QSqlDatabase>
#include <QDate>
#include <QSet>
#include <functional>

// ═══════════════════════════════════════════════════════════════════════════════
//  NotificationWidget — Toast flottant bas à droite, empilable, avec actions
// ═══════════════════════════════════════════════════════════════════════════════
class NotificationWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ windowOpacity WRITE setWindowOpacity)

public:
    enum Type { Info, Warning, Critical, Success };

    static void show(const QString &title,
                     const QString &message,
                     Type type = Info,
                     const QString &action1Label = {},
                     std::function<void()> action1 = {},
                     const QString &action2Label = {},
                     std::function<void()> action2 = {},
                     int durationMs = 5000,
                     bool aiGenerated = false);

    static void closeAll();
    static int activeCount() { return s_active.size(); }
    static int s_stackOffset;

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    explicit NotificationWidget(const QString &title,
                                const QString &message,
                                Type type,
                                const QString &action1Label,
                                std::function<void()> action1,
                                const QString &action2Label,
                                std::function<void()> action2,
                                int durationMs,
                                bool aiGenerated = false,
                                QWidget *parent = nullptr);
    void fadeOut();

    QLabel       *m_icon;
    QLabel       *m_title;
    QLabel       *m_message;
    QPushButton  *m_closeBtn;
    QPushButton  *m_action1Btn;
    QPushButton  *m_action2Btn;
    QProgressBar *m_progress;

    QTimer             *m_autoClose;
    QTimer             *m_progressTimer;
    QPropertyAnimation *m_fadeIn;
    QPropertyAnimation *m_fadeOut;
    QPropertyAnimation *m_slideIn;

    int  m_durationMs;
    int  m_elapsed;
    bool m_paused;

    static constexpr int MAX_TOASTS = 5;
    static QList<NotificationWidget*> s_active;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  NotificationAI — Génère les notifications via l'API Google Gemini
// ═══════════════════════════════════════════════════════════════════════════════
class NotificationAI : public QObject
{
    Q_OBJECT

public:
    explicit NotificationAI(const QString &apiKey, QObject *parent = nullptr);

    void analyze(const QMap<QString, QString> &context);
    void analyzeText(const QString &situation);
    void setApiKey(const QString &key) { m_apiKey = key; }

signals:
    void notificationReady(const QString &title,
                           const QString &message,
                           NotificationWidget::Type type);
    void errorOccurred(const QString &error);

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    QString buildPrompt(const QMap<QString, QString> &context) const;
    NotificationWidget::Type parseType(const QString &t) const;

    QNetworkAccessManager *m_network;
    QString                m_apiKey;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  NotificationServer — Reçoit des webhooks HTTP depuis un backend externe
//  POST http://localhost:9876/notify
// ═══════════════════════════════════════════════════════════════════════════════
class NotificationServer : public QObject
{
    Q_OBJECT

public:
    explicit NotificationServer(quint16 port = 9876, QObject *parent = nullptr);
    bool start();

signals:
    void webhookReceived(const QJsonObject &payload);

private slots:
    void onNewConnection();
    void onDataReady();

private:
    void processRequest(const QByteArray &data);
    QTcpServer *m_server;
    quint16     m_port;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  NotificationPipeline — Orchestre Server + AI → Toast
// ═══════════════════════════════════════════════════════════════════════════════
class NotificationPipeline : public QObject
{
    Q_OBJECT

public:
    explicit NotificationPipeline(const QString &apiKey,
                                  QObject *parent = nullptr);
    bool start(quint16 port = 9876);
    void triggerFromApp(const QMap<QString, QString> &context);
    void triggerText(const QString &situation);
    NotificationAI *ai() { return m_ai; }

signals:
    void notificationTriggered(const QString &title,
                               const QString &message,
                               NotificationWidget::Type type);

private slots:
    void onWebhookReceived(const QJsonObject &payload);

private:
    NotificationServer *m_server;
    NotificationAI     *m_ai;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  NotificationWatcher — Surveille la BDD et déclenche automatiquement
// ═══════════════════════════════════════════════════════════════════════════════
class NotificationWatcher : public QObject
{
    Q_OBJECT

public:
    explicit NotificationWatcher(NotificationPipeline *pipeline,
                                 QSqlDatabase db,
                                 QObject *parent = nullptr);
    void start(int intervalMs = 120000);
    void stop();
    void checkNow();

private slots:
    void onTimerTick();

private:
    void checkCommandesEnRetard();
    void checkCommandesEnAttente();
    void checkCommandesSuspendues();
    void checkLivraisonsProches();

    NotificationPipeline *m_pipeline;
    QSqlDatabase          m_db;
    QTimer               *m_timer;
    QSet<QString>         m_notifiedRefs;
};

#endif // NOTIFICATION_H
