#include "notification.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScreen>
#include <QApplication>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonArray>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QUrl>

// ===============================================================
//  NotificationWidget
// ===============================================================

int NotificationWidget::s_stackOffset = 0;
QList<NotificationWidget*> NotificationWidget::s_active;

void NotificationWidget::show(const QString &title, const QString &message, Type type,
                               const QString &action1Label, std::function<void()> action1,
                               const QString &action2Label, std::function<void()> action2,
                               int durationMs, bool aiGenerated)
{
    if (s_active.size() >= MAX_TOASTS && !s_active.isEmpty())
        s_active.first()->fadeOut();

    auto *w = new NotificationWidget(title, message, type,
                                     action1Label, action1,
                                     action2Label, action2,
                                     durationMs, aiGenerated);
    s_active.append(w);
    connect(w, &QObject::destroyed, [w]() { s_active.removeAll(w); });
}

void NotificationWidget::closeAll()
{
    for (auto *w : std::as_const(s_active)) w->fadeOut();
}

NotificationWidget::NotificationWidget(const QString &title, const QString &message,
                                       Type type,
                                       const QString &action1Label, std::function<void()> action1,
                                       const QString &action2Label, std::function<void()> action2,
                                       int durationMs, bool aiGenerated, QWidget *parent)
    : QWidget(parent, Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint)
    , m_durationMs(durationMs), m_elapsed(0), m_paused(false)
    , m_action1Btn(nullptr), m_action2Btn(nullptr)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);
    setFixedWidth(340);

    struct Theme { QString bg, border, icon, progressColor; };
    static const Theme themes[] = {
        {"#1A2E4A", "#378ADD", "\xF0\x9F\x94\xB5", "#85B7EB"}, // Info
        {"#7A4A1A", "#C4923A", "\xF0\x9F\x9F\xA0", "#F0C070"}, // Warning
        {"#6B2737", "#A0485A", "\xF0\x9F\x94\xB4", "#C4923A"}, // Critical
        {"#2D4228", "#7A9E6F", "\xF0\x9F\x9F\xA2", "#A0D090"}, // Success
    };
    const Theme &theme = themes[qBound(0, static_cast<int>(type), 3)];

    const QString borderColor = aiGenerated ? "#F5C518" : theme.border;
    const QString borderWidth = aiGenerated ? "6px"     : "5px";

    auto *container = new QWidget(this);
    container->setObjectName("container");
    container->setStyleSheet(
        QString("#container { background:%1; border-left:%2 solid %3;"
                " border-radius:10px; padding:12px 14px 8px 14px; }")
        .arg(theme.bg, borderWidth, borderColor));

    auto *shadow = new QGraphicsDropShadowEffect(container);
    shadow->setBlurRadius(aiGenerated ? 28 : 20);
    shadow->setOffset(0, aiGenerated ? 7 : 5);
    shadow->setColor(aiGenerated ? QColor(245, 197, 24, 160) : QColor(0, 0, 0, 130));
    container->setGraphicsEffect(shadow);

    auto *outerLay = new QVBoxLayout(this);
    outerLay->setContentsMargins(8, 8, 8, 8);
    outerLay->addWidget(container);

    auto *lay = new QVBoxLayout(container);
    lay->setSpacing(5);
    lay->setContentsMargins(0, 0, 0, 0);

    // Titre
    auto *titleRow = new QHBoxLayout();
    titleRow->setSpacing(6);

    m_icon = new QLabel(theme.icon, container);
    m_icon->setStyleSheet("font-size:15px;");
    m_icon->setFixedWidth(22);

    m_title = new QLabel(title, container);
    m_title->setStyleSheet("color:white; font-size:13px; font-weight:bold; background:transparent;");
    m_title->setWordWrap(true);

    titleRow->addWidget(m_icon);
    titleRow->addWidget(m_title, 1);

    if (aiGenerated) {
        auto *aiBadge = new QLabel("✨ IA", container);
        aiBadge->setStyleSheet(
            "background:#F5C518; color:#1A1A1A; font-size:9px; font-weight:bold;"
            " border-radius:4px; padding:2px 5px;");
        aiBadge->setFixedHeight(16);
        titleRow->addWidget(aiBadge);
    }

    m_closeBtn = new QPushButton("x", container);
    m_closeBtn->setFixedSize(20, 20);
    m_closeBtn->setStyleSheet(
        "QPushButton { background:rgba(255,255,255,0.15); color:white;"
        " border:none; border-radius:10px; font-size:10px; }"
        "QPushButton:hover { background:rgba(255,255,255,0.30); }");
    connect(m_closeBtn, &QPushButton::clicked, this, &NotificationWidget::fadeOut);
    titleRow->addWidget(m_closeBtn);
    lay->addLayout(titleRow);

    m_message = new QLabel(message, container);
    m_message->setStyleSheet("color:rgba(255,255,255,0.88); font-size:12px; background:transparent;");
    m_message->setWordWrap(true);
    lay->addWidget(m_message);

    // Boutons d'action
    if (!action1Label.isEmpty() || !action2Label.isEmpty()) {
        static const QString btnP =
            "QPushButton { background:rgba(255,255,255,0.20); color:white;"
            " border:1px solid rgba(255,255,255,0.35); border-radius:5px; padding:4px 10px; font-size:11px; }"
            "QPushButton:hover { background:rgba(255,255,255,0.35); }";
        static const QString btnS =
            "QPushButton { background:transparent; color:rgba(255,255,255,0.65);"
            " border:1px solid rgba(255,255,255,0.20); border-radius:5px; padding:4px 10px; font-size:11px; }"
            "QPushButton:hover { color:white; border-color:rgba(255,255,255,0.45); }";

        auto *actRow = new QHBoxLayout();
        actRow->setSpacing(6);

        auto makeBtn = [&](QPushButton *&btn, const QString &label,
                           const QString &style, std::function<void()> cb) {
            btn = new QPushButton(label, container);
            btn->setStyleSheet(style);
            connect(btn, &QPushButton::clicked, this, [this, cb]() {
                if (cb) cb();
                fadeOut();
            });
            actRow->addWidget(btn);
        };

        if (!action1Label.isEmpty()) makeBtn(m_action1Btn, action1Label, btnP, action1);
        if (!action2Label.isEmpty()) makeBtn(m_action2Btn, action2Label, btnS, action2);
        actRow->addStretch();
        lay->addLayout(actRow);
    }

    m_progress = new QProgressBar(container);
    m_progress->setRange(0, durationMs);
    m_progress->setValue(durationMs);
    m_progress->setTextVisible(false);
    m_progress->setFixedHeight(3);
    m_progress->setStyleSheet(
        QString("QProgressBar { background:rgba(255,255,255,0.15); border-radius:1px; border:none; }"
                "QProgressBar::chunk { background:%1; border-radius:1px; }")
        .arg(theme.progressColor));
    lay->addWidget(m_progress);

    // Position
    adjustSize();
    const QRect screen = QApplication::primaryScreen()->availableGeometry();
    const int finalX = screen.right() - width() - 20;
    const int finalY = screen.bottom() - height() - 20 - s_stackOffset;
    s_stackOffset += height() + 10;

    move(screen.right() + 10, finalY);
    setWindowOpacity(0.0);
    QWidget::show();

    // Animations
    m_slideIn = new QPropertyAnimation(this, "pos", this);
    m_slideIn->setDuration(350);
    m_slideIn->setStartValue(QPoint(screen.right() + 10, finalY));
    m_slideIn->setEndValue(QPoint(finalX, finalY));
    m_slideIn->setEasingCurve(QEasingCurve::OutCubic);
    m_slideIn->start(QAbstractAnimation::DeleteWhenStopped);

    m_fadeIn = new QPropertyAnimation(this, "opacity", this);
    m_fadeIn->setDuration(350);
    m_fadeIn->setStartValue(0.0);
    m_fadeIn->setEndValue(1.0);
    m_fadeIn->start(QAbstractAnimation::DeleteWhenStopped);

    // Timers
    m_autoClose = new QTimer(this);
    m_autoClose->setSingleShot(true);
    connect(m_autoClose, &QTimer::timeout, this, &NotificationWidget::fadeOut);
    m_autoClose->start(durationMs);

    m_progressTimer = new QTimer(this);
    connect(m_progressTimer, &QTimer::timeout, this, [this]() {
        if (m_paused) return;
        m_elapsed += 50;
        m_progress->setValue(m_durationMs - m_elapsed);
    });
    m_progressTimer->start(50);
}

void NotificationWidget::fadeOut()
{
    if (m_autoClose)     m_autoClose->stop();
    if (m_progressTimer) m_progressTimer->stop();

    m_fadeOut = new QPropertyAnimation(this, "opacity", this);
    m_fadeOut->setDuration(400);
    m_fadeOut->setStartValue(windowOpacity());
    m_fadeOut->setEndValue(0.0);
    connect(m_fadeOut, &QPropertyAnimation::finished, this, [this]() {
        s_stackOffset = qMax(0, s_stackOffset - height() - 10);
        close();
    });
    m_fadeOut->start(QAbstractAnimation::DeleteWhenStopped);
}

void NotificationWidget::enterEvent(QEnterEvent *)
{
    m_paused = true;
    if (m_autoClose) m_autoClose->stop();
}

void NotificationWidget::leaveEvent(QEvent *)
{
    m_paused = false;
    const int remaining = m_durationMs - m_elapsed;
    if (remaining > 0 && m_autoClose) m_autoClose->start(remaining);
}

void NotificationWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) emit clicked();
    QWidget::mousePressEvent(event);
}

// ===============================================================
//  NotificationAI
// ===============================================================

NotificationAI::NotificationAI(const QString &apiKey, QObject *parent)
    : QObject(parent)
    , m_network(new QNetworkAccessManager(this))
    , m_apiKey(apiKey)
{
    connect(m_network, &QNetworkAccessManager::finished,
            this, &NotificationAI::onReplyFinished);
}

void NotificationAI::analyze(const QMap<QString, QString> &context)
{
    if (m_apiKey.isEmpty()) {
        emit errorOccurred("Cle API Google non configuree.");
        return;
    }

    const QString urlStr =
        QLatin1String("https://generativelanguage.googleapis.com/v1beta/models/"
                      "gemini-2.0-flash:generateContent?key=") + m_apiKey;

    QNetworkRequest request;
    request.setUrl(QUrl(urlStr));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject sysPart;
    sysPart["text"] = QLatin1String(
        "Tu es l'assistant IA de CUIREA Management. "
        "Reponds UNIQUEMENT en JSON valide, sans markdown. "
        "Format: {\"title\":\"...\",\"message\":\"...\",\"type\":\"Info|Warning|Critical|Success\"} "
        "Titre < 6 mots, message actionnable en francais. "
        "Critical=retard/urgence, Warning=suspendu/stock, Success=livre/paye, Info=general.");

    QJsonArray sysParts;
    sysParts.append(sysPart);
    QJsonObject sysInstruction;
    sysInstruction["parts"] = sysParts;

    QJsonObject part;
    part["text"] = buildPrompt(context);
    QJsonObject userMsg;
    userMsg["role"]  = QLatin1String("user");
    userMsg["parts"] = QJsonArray{ part };

    QJsonObject body;
    body["system_instruction"] = sysInstruction;
    body["contents"]           = QJsonArray{ userMsg };

    m_network->post(request, QJsonDocument(body).toJson());
}

void NotificationAI::analyzeText(const QString &situation)
{
    analyze({{"situation", situation}});
}

QString NotificationAI::buildPrompt(const QMap<QString, QString> &context) const
{
    QString prompt = "Analyse cette situation dans CUIREA Management"
                     " et genere une notification appropriee :\n\n";
    for (auto it = context.cbegin(); it != context.cend(); ++it)
        prompt += QString("- %1 : %2\n").arg(it.key(), it.value());
    prompt += "\nCriteres : Critical=retard/urgence, Warning=suspendu/stock,"
              " Success=termine/paiement, Info=general.";
    return prompt;
}

void NotificationAI::onReplyFinished(QNetworkReply *reply)
{
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        emit errorOccurred(reply->errorString());
        NotificationWidget::show("Erreur connexion", "Impossible de joindre l'assistant IA.",
                                 NotificationWidget::Warning, {}, {}, {}, {}, 5000, true);
        return;
    }

    const QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    const QString rawText =
        doc["candidates"][0]["content"]["parts"][0]["text"].toString().trimmed();

    const QJsonDocument parsed = QJsonDocument::fromJson(rawText.toUtf8());
    if (parsed.isNull() || !parsed.isObject()) {
        emit errorOccurred("Reponse JSON invalide : " + rawText);
        return;
    }

    const QJsonObject obj = parsed.object();
    const QString title   = obj["title"].toString("Notification CUIREA");
    const QString message = obj["message"].toString("Une mise a jour est disponible.");
    const auto    type    = parseType(obj["type"].toString("Info"));

    emit notificationReady(title, message, type);
    NotificationWidget::show(title, message, type, {}, {}, {}, {}, 5000, true);
}

NotificationWidget::Type NotificationAI::parseType(const QString &t) const
{
    if (t == "Critical") return NotificationWidget::Critical;
    if (t == "Warning")  return NotificationWidget::Warning;
    if (t == "Success")  return NotificationWidget::Success;
    return NotificationWidget::Info;
}

// ===============================================================
//  NotificationServer
// ===============================================================

NotificationServer::NotificationServer(quint16 port, QObject *parent)
    : QObject(parent), m_server(new QTcpServer(this)), m_port(port)
{
    connect(m_server, &QTcpServer::newConnection,
            this, &NotificationServer::onNewConnection);
}

bool NotificationServer::start()
{
    const bool ok = m_server->listen(QHostAddress::LocalHost, m_port);
    if (ok) qDebug() << "[NotificationServer] Ecoute sur localhost:" << m_port;
    else    qWarning() << "[NotificationServer] Erreur:" << m_server->errorString();
    return ok;
}

void NotificationServer::onNewConnection()
{
    QTcpSocket *socket = m_server->nextPendingConnection();
    connect(socket, &QTcpSocket::readyRead,    this,   &NotificationServer::onDataReady);
    connect(socket, &QTcpSocket::disconnected, socket, &QObject::deleteLater);
}

void NotificationServer::onDataReady()
{
    auto *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) return;
    processRequest(socket->readAll());
    socket->write("HTTP/1.1 200 OK\r\nContent-Length: 0\r\nAccess-Control-Allow-Origin: *\r\n\r\n");
    socket->disconnectFromHost();
}

void NotificationServer::processRequest(const QByteArray &data)
{
    const int bodyStart = data.indexOf("\r\n\r\n");
    if (bodyStart == -1) return;
    const QJsonObject obj =
        QJsonDocument::fromJson(data.mid(bodyStart + 4).trimmed()).object();
    if (!obj.isEmpty()) emit webhookReceived(obj);
}

// ===============================================================
//  NotificationPipeline
// ===============================================================

NotificationPipeline::NotificationPipeline(const QString &apiKey, QObject *parent)
    : QObject(parent)
    , m_server(new NotificationServer(9876, this))
    , m_ai(new NotificationAI(apiKey, this))
{
    connect(m_server, &NotificationServer::webhookReceived,
            this,     &NotificationPipeline::onWebhookReceived);
    connect(m_ai, &NotificationAI::notificationReady,
            this, &NotificationPipeline::notificationTriggered);
}

bool NotificationPipeline::start(quint16 port)
{
    Q_UNUSED(port)
    return m_server->start();
}

void NotificationPipeline::triggerFromApp(const QMap<QString, QString> &context)
{
    m_ai->analyze(context);
}

void NotificationPipeline::triggerText(const QString &situation)
{
    m_ai->analyzeText(situation);
}

void NotificationPipeline::onWebhookReceived(const QJsonObject &payload)
{
    QMap<QString, QString> ctx;
    for (auto it = payload.begin(); it != payload.end(); ++it)
        ctx[it.key()] = it.value().toString();
    m_ai->analyze(ctx);
}

// ===============================================================
//  NotificationWatcher
// ===============================================================

NotificationWatcher::NotificationWatcher(NotificationPipeline *pipeline,
                                         QSqlDatabase db, QObject *parent)
    : QObject(parent), m_pipeline(pipeline), m_db(db)
    , m_timer(new QTimer(this))
{
    connect(m_timer, &QTimer::timeout, this, &NotificationWatcher::onTimerTick);
}

void NotificationWatcher::start(int intervalMs) { checkNow(); m_timer->start(intervalMs); }
void NotificationWatcher::stop()                { m_timer->stop(); }
void NotificationWatcher::checkNow()            { onTimerTick(); }

void NotificationWatcher::onTimerTick()
{
    if (!m_db.isOpen()) return;
    checkCommandesEnRetard();
    checkCommandesEnAttente();
    checkCommandesSuspendues();
    checkLivraisonsProches();
}

// Helper : évite la répétition du pattern "déjà notifié ?"
static bool shouldNotify(QSet<QString> &notified, const QString &key)
{
    if (notified.contains(key)) return false;
    notified.insert(key);
    return true;
}

void NotificationWatcher::checkCommandesEnRetard()
{
    QSqlQuery q(m_db);
    q.prepare("SELECT reference, produit, montant_ht, date_livraison, mail_client "
              "FROM commandes "
              "WHERE priorite = 'Urgente' "
              "  AND statut NOT IN ('Termine','Annule') "
              "  AND date_livraison < :today");
    q.bindValue(":today", QDate::currentDate().toString("yyyy-MM-dd"));
    if (!q.exec()) { qWarning() << q.lastError().text(); return; }

    while (q.next()) {
        const QString ref = q.value("reference").toString();
        if (!shouldNotify(m_notifiedRefs, "retard_" + ref)) continue;

        const int jours = QDate::fromString(q.value("date_livraison").toString(), "yyyy-MM-dd")
                              .daysTo(QDate::currentDate());
        m_pipeline->triggerFromApp({
            {"commande",     ref},
            {"produit",      q.value("produit").toString()},
            {"montant",      q.value("montant_ht").toString() + " DT"},
            {"statut",       "retard livraison"},
            {"priorite",     "Urgente"},
            {"jours_retard", QString::number(jours)},
            {"client",       q.value("mail_client").toString()}
        });
    }
}

void NotificationWatcher::checkCommandesEnAttente()
{
    QSqlQuery q(m_db);
    q.prepare("SELECT reference, produit, montant_ht, date_creation FROM commandes "
              "WHERE statut = 'En Attente' AND date_creation < :limite");
    q.bindValue(":limite", QDate::currentDate().addDays(-7).toString("yyyy-MM-dd"));
    if (!q.exec()) { qWarning() << q.lastError().text(); return; }

    while (q.next()) {
        const QString ref = q.value("reference").toString();
        if (!shouldNotify(m_notifiedRefs, "attente_" + ref)) continue;

        const int jours = QDate::fromString(q.value("date_creation").toString(), "yyyy-MM-dd")
                              .daysTo(QDate::currentDate());
        m_pipeline->triggerFromApp({
            {"commande", ref},
            {"produit",  q.value("produit").toString()},
            {"montant",  q.value("montant_ht").toString() + " DT"},
            {"statut",   "En Attente depuis " + QString::number(jours) + " jours"},
            {"probleme", "commande bloquee sans traitement"}
        });
    }
}

void NotificationWatcher::checkCommandesSuspendues()
{
    QSqlQuery q(m_db);
    q.prepare("SELECT reference, produit, montant_ht, mail_client FROM commandes "
              "WHERE statut = 'Suspendu' AND date_creation < :limite");
    q.bindValue(":limite", QDate::currentDate().addDays(-3).toString("yyyy-MM-dd"));
    if (!q.exec()) { qWarning() << q.lastError().text(); return; }

    while (q.next()) {
        const QString ref = q.value("reference").toString();
        if (!shouldNotify(m_notifiedRefs, "suspendu_" + ref)) continue;

        m_pipeline->triggerFromApp({
            {"commande", ref},
            {"produit",  q.value("produit").toString()},
            {"montant",  q.value("montant_ht").toString() + " DT"},
            {"statut",   "Suspendu depuis 3+ jours"},
            {"client",   q.value("mail_client").toString()},
            {"probleme", "commande suspendue necessite verification"}
        });
    }
}

void NotificationWatcher::checkLivraisonsProches()
{
    QSqlQuery q(m_db);
    q.prepare("SELECT reference, produit, montant_ht, date_livraison, mail_client FROM commandes "
              "WHERE statut NOT IN ('Termine','Annule') AND date_livraison = :demain");
    q.bindValue(":demain", QDate::currentDate().addDays(1).toString("yyyy-MM-dd"));
    if (!q.exec()) { qWarning() << q.lastError().text(); return; }

    while (q.next()) {
        const QString ref = q.value("reference").toString();
        if (!shouldNotify(m_notifiedRefs, "livraison24h_" + ref)) continue;

        m_pipeline->triggerFromApp({
            {"commande",       ref},
            {"produit",        q.value("produit").toString()},
            {"montant",        q.value("montant_ht").toString() + " DT"},
            {"statut",         "livraison prevue demain"},
            {"date_livraison", q.value("date_livraison").toString()},
            {"client",         q.value("mail_client").toString()},
            {"alerte",         "livraison dans moins de 24 heures"}
        });
    }
}
