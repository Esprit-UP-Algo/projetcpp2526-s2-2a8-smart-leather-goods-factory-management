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
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonObject>
#include <QSqlDatabase>
#include <QDate>
#include <QSet>
#include <QDateTime>
#include <QScrollArea>
#include <QNetworkAccessManager>
#include <QNetworkReply>
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
    static int  activeCount() { return s_active.size(); }
    static void repositionAll();
    static void setToastsEnabled(bool enabled) { s_toastsEnabled = enabled; }

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void showEvent(QShowEvent *event) override;

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
    static bool s_toastsEnabled;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  NotificationHistoryItem — Une entrée dans l'historique
// ═══════════════════════════════════════════════════════════════════════════════
struct NotificationHistoryItem {
    QString            title;
    QString            message;
    NotificationWidget::Type type;
    QDateTime          timestamp;
    bool               read     = false;
    bool               aiGenerated = false;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  NotificationHistory — Stocke toutes les notifications (singleton)
// ═══════════════════════════════════════════════════════════════════════════════
class NotificationHistory : public QObject
{
    Q_OBJECT

public:
    static NotificationHistory &instance();

    void add(const NotificationHistoryItem &item);
    void markAllRead();
    int  unreadCount() const;
    const QList<NotificationHistoryItem> &items() const { return m_items; }

signals:
    void changed();

private:
    NotificationHistory() = default;
    QList<NotificationHistoryItem> m_items;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  NotificationBell — Icône cloche avec badge, cliquable dans la topbar
// ═══════════════════════════════════════════════════════════════════════════════
class NotificationBell : public QWidget
{
    Q_OBJECT

public:
    explicit NotificationBell(QWidget *parent = nullptr);
    void refresh();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    void openPanel();
    bool m_hovered = false;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  NotificationPanel — Fenêtre dropdown listant l'historique
// ═══════════════════════════════════════════════════════════════════════════════
class NotificationPanel : public QWidget
{
    Q_OBJECT

public:
    explicit NotificationPanel(QWidget *parent = nullptr);
    void populate();

protected:
    void focusOutEvent(QFocusEvent *event) override;

    void showSummary(const QString &text);
private:
    QScrollArea *m_scroll;
    QWidget     *m_container;
    QPushButton *m_markAllBtn;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  NotificationSound — Joue un son selon le type (6)
// ═══════════════════════════════════════════════════════════════════════════════
class NotificationSound : public QObject
{
    Q_OBJECT

public:
    static void play(NotificationWidget::Type type);

private:
    // Génère un bip via QAudioSink (PCM 44100Hz)
    static void beep(int frequencyHz, int durationMs, float volume = 0.4f);
    static void playSequence(const QList<QPair<int,int>> &notes);
};

// ═══════════════════════════════════════════════════════════════════════════════
//  NotificationAI — Appelle Groq pour résumer un message technique (7)
// ═══════════════════════════════════════════════════════════════════════════════
class NotificationAI : public QObject
{
    Q_OBJECT

public:
    // apiKey  : ta clé Groq  (ex: "gsk_xxxx")
    // model   : défaut "llama3-8b-8192" — rapide et gratuit sur Groq
    explicit NotificationAI(const QString &apiKey,
                             const QString &model = "llama-3.1-8b-instant",
                             QObject *parent = nullptr);

    // Envoie le contexte brut à Groq, rappelle le callback avec le résumé
    // context : "CMD-2024-0031 · statut=En_Attente · jours=7 · priorite=Urgente"
    // onResult(title, message, type)
    void summarize(const QString &context,
                   std::function<void(const QString &title,
                                      const QString &message,
                                      NotificationWidget::Type type)> onResult);

    void generateSummary(std::function<void(const QString &summary)> onResult);

    static void setGlobalInstance(NotificationAI *ai) { s_instance = ai; }
    static NotificationAI *globalInstance()           { return s_instance; }

private slots:
    void onReply(QNetworkReply *reply);

private:
    QNetworkReply *postGroq(const QString &system, const QString &user, int maxTokens);
    QString                m_apiKey;
    QString                m_model;
    QNetworkAccessManager *m_nam;

    static NotificationAI *s_instance;

    struct PendingRequest {
        std::function<void(const QString&, const QString&, NotificationWidget::Type)> callback;
        std::function<void(const QString&)> summaryCallback;
    };
    QHash<QNetworkReply*, PendingRequest> m_pending;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  NotificationServer — Reçoit des webhooks HTTP POST localhost:9876/notify
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
//  NotificationWatcher — Surveille la BDD et déclenche automatiquement
// ═══════════════════════════════════════════════════════════════════════════════
class NotificationWatcher : public QObject
{
    Q_OBJECT

public:
    explicit NotificationWatcher(QSqlDatabase db,
                                 QObject *parent = nullptr);

    // Optionnel : injecter l'IA pour les alertes complexes
    void setAI(NotificationAI *ai) { m_ai = ai; }

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

    // Affiche via IA si disponible, sinon affiche directement
    void showWithAI(const QString &context,
                    const QString &fallbackTitle,
                    const QString &fallbackMessage,
                    NotificationWidget::Type type);

    QSqlDatabase   m_db;
    QTimer        *m_timer;
    QSet<QString>  m_notifiedRefs;
    NotificationAI *m_ai = nullptr;
};

#endif // NOTIFICATION_H