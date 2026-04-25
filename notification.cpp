#include "notification.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScreen>
#include <QApplication>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>
#include <QShowEvent>
#include <QJsonDocument>
#include <QJsonArray>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QPainter>
#include <QFrame>
#include <QScrollBar>
#include <QNetworkRequest>
#include <QUrl>
#include <QAudioSink>
#include <QAudioFormat>
#include <QBuffer>
#include <QtMath>
#include <QPointer>
#include <QRegularExpression>

// ── Statics ──────────────────────────────────────────────────────────────────
QList<NotificationWidget*> NotificationWidget::s_active;
bool                       NotificationWidget::s_toastsEnabled = false;
NotificationAI*            NotificationAI::s_instance          = nullptr;

// ===============================================================
//  NotificationWidget
// ===============================================================

void NotificationWidget::repositionAll()
{
    const QRect screen = QApplication::primaryScreen()->availableGeometry();
    int offsetY = 20;
    for (int i = s_active.size() - 1; i >= 0; --i) {
        auto *w = s_active[i];
        w->move(screen.right() - w->width() - 20,
                screen.bottom() - offsetY - w->height());
        offsetY += w->height() + 10;
    }
}

void NotificationWidget::show(const QString &title, const QString &message, Type type,
                               const QString &action1Label, std::function<void()> action1,
                               const QString &action2Label, std::function<void()> action2,
                               int durationMs, bool aiGenerated)
{
    NotificationHistory::instance().add({title, message, type,
                                         QDateTime::currentDateTime(), false, aiGenerated});
    if (!s_toastsEnabled) return;

    if (s_active.size() >= MAX_TOASTS && !s_active.isEmpty())
        s_active.first()->fadeOut();

    NotificationSound::play(type);
    s_active.append(new NotificationWidget(title, message, type,
                                           action1Label, action1,
                                           action2Label, action2,
                                           durationMs, aiGenerated));
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

    struct Theme { const char *bg, *border, *icon, *progressColor; };
    static constexpr Theme themes[] = {
        {"#1A2E4A", "#378ADD", "\xF0\x9F\x94\xB5", "#85B7EB"},
        {"#7A4A1A", "#C4923A", "\xF0\x9F\x9F\xA0", "#F0C070"},
        {"#6B2737", "#A0485A", "\xF0\x9F\x94\xB4", "#C4923A"},
        {"#2D4228", "#7A9E6F", "\xF0\x9F\x9F\xA2", "#A0D090"},
    };
    const Theme &t = themes[qBound(0, static_cast<int>(type), 3)];

    auto *container = new QWidget(this);
    container->setObjectName("container");
    container->setStyleSheet(
        QString("#container { background:%1; border-left:%2 solid %3;"
                " border-radius:10px; padding:12px 14px 8px 14px; }")
        .arg(t.bg, aiGenerated ? "6px" : "5px", aiGenerated ? "#F5C518" : t.border));

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

    auto *titleRow = new QHBoxLayout();
    titleRow->setSpacing(6);

    m_icon = new QLabel(t.icon, container);
    m_icon->setStyleSheet("font-size:15px;");
    m_icon->setFixedWidth(22);

    m_title = new QLabel(title, container);
    m_title->setStyleSheet("color:white; font-size:13px; font-weight:bold; background:transparent;");
    m_title->setWordWrap(true);

    titleRow->addWidget(m_icon);
    titleRow->addWidget(m_title, 1);

    if (aiGenerated) {
        auto *badge = new QLabel("✨ IA", container);
        badge->setStyleSheet("background:#F5C518; color:#1A1A1A; font-size:9px; font-weight:bold;"
                             " border-radius:4px; padding:2px 5px;");
        badge->setFixedHeight(16);
        titleRow->addWidget(badge);
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
            connect(btn, &QPushButton::clicked, this, [this, cb]() { if (cb) cb(); fadeOut(); });
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
                "QProgressBar::chunk { background:%1; border-radius:1px; }").arg(t.progressColor));
    lay->addWidget(m_progress);

    setWindowOpacity(0.0);

    m_autoClose = new QTimer(this);
    m_autoClose->setSingleShot(true);
    connect(m_autoClose, &QTimer::timeout, this, &NotificationWidget::fadeOut);

    m_progressTimer = new QTimer(this);
    connect(m_progressTimer, &QTimer::timeout, this, [this]() {
        if (!m_paused) m_progress->setValue(m_durationMs - (m_elapsed += 50));
    });

    const QRect screen = QApplication::primaryScreen()->availableGeometry();
    move(screen.right() + 10, screen.bottom());
    QWidget::show();
}

void NotificationWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    ensurePolished();
    if (layout()) layout()->activate();
    adjustSize();

    const QRect screen = QApplication::primaryScreen()->availableGeometry();
    const int finalX   = screen.right() - width() - 20;

    int offsetY = 20;
    for (auto *w : std::as_const(s_active))
        if (w != this) offsetY += w->height() + 10;
    const int finalY = screen.bottom() - offsetY - height();
    move(screen.right() + 10, finalY);

    auto makeAnim = [this](const QByteArray &prop, const QVariant &from, const QVariant &to,
                           int ms, QEasingCurve::Type curve = QEasingCurve::Linear) {
        auto *a = new QPropertyAnimation(this, prop, this);
        a->setDuration(ms); a->setStartValue(from); a->setEndValue(to);
        a->setEasingCurve(curve);
        a->start(QAbstractAnimation::DeleteWhenStopped);
        return a;
    };
    m_slideIn = makeAnim("pos",   QPoint(screen.right()+10, finalY), QPoint(finalX, finalY), 350, QEasingCurve::OutCubic);
    m_fadeIn  = makeAnim("opacity", 0.0, 1.0, 350);

    m_autoClose->start(m_durationMs);
    m_progressTimer->start(50);
}

void NotificationWidget::fadeOut()
{
    if (m_autoClose)     m_autoClose->stop();
    if (m_progressTimer) m_progressTimer->stop();

    auto *anim = new QPropertyAnimation(this, "opacity", this);
    anim->setDuration(400);
    anim->setStartValue(windowOpacity());
    anim->setEndValue(0.0);
    connect(anim, &QPropertyAnimation::finished, this, [this]() {
        s_active.removeAll(this);
        repositionAll();
        close();
    });
    anim->start(QAbstractAnimation::DeleteWhenStopped);
    m_fadeOut = anim;
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
//  NotificationHistory
// ===============================================================

NotificationHistory &NotificationHistory::instance()
{
    static NotificationHistory inst;
    return inst;
}

void NotificationHistory::add(const NotificationHistoryItem &item)
{
    m_items.prepend(item);
    if (m_items.size() > 100) m_items.removeLast();
    emit changed();
}

void NotificationHistory::markAllRead()
{
    for (auto &item : m_items) item.read = true;
    emit changed();
}

int NotificationHistory::unreadCount() const
{
    return static_cast<int>(std::count_if(m_items.cbegin(), m_items.cend(),
                                          [](const NotificationHistoryItem &i) { return !i.read; }));
}

// ===============================================================
//  NotificationBell
// ===============================================================

NotificationBell::NotificationBell(QWidget *parent) : QWidget(parent)
{
    setFixedSize(42, 42);
    setCursor(Qt::PointingHandCursor);
    setToolTip("Notifications");
    setAttribute(Qt::WA_TranslucentBackground);
    connect(&NotificationHistory::instance(), &NotificationHistory::changed,
            this, &NotificationBell::refresh);
}

void NotificationBell::refresh() { update(); }

void NotificationBell::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    p.setBrush(m_hovered ? QColor(70, 42, 22, 240) : QColor(44, 26, 14, 220));
    p.setPen(QPen(QColor(196, 121, 90, 120), 1));
    p.drawEllipse(rect().adjusted(2, 2, -2, -2));

    const int unread = NotificationHistory::instance().unreadCount();
    QFont iconFont("Segoe UI Emoji", 16);
    iconFont.setStyleStrategy(QFont::PreferAntialias);
    p.setFont(iconFont);
    p.setPen(unread > 0 ? QColor("#FFD580") : QColor("#E8C9A0"));
    p.drawText(rect().adjusted(0, -1, 0, -1), Qt::AlignCenter, "🔔");

    if (unread > 0) {
        const QRectF badge(24, 3, 15, 15);
        p.setBrush(QColor("#E53935"));
        p.setPen(QPen(QColor(44, 26, 14), 1.5));
        p.drawEllipse(badge);
        p.setPen(Qt::white);
        p.setFont(QFont("Arial", unread > 9 ? 6 : 7, QFont::Bold));
        p.drawText(badge, Qt::AlignCenter, unread > 9 ? "9+" : QString::number(unread));
    }
}

void NotificationBell::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) openPanel();
    QWidget::mousePressEvent(event);
}

void NotificationBell::enterEvent(QEnterEvent *) { m_hovered = true;  update(); }
void NotificationBell::leaveEvent(QEvent *)       { m_hovered = false; update(); }

void NotificationBell::openPanel()
{
    NotificationHistory::instance().markAllRead();

    auto *panel = new NotificationPanel(window());
    panel->adjustSize();

    const QRect  screen    = QApplication::primaryScreen()->availableGeometry();
    const QPoint globalPos = mapToGlobal(QPoint(width(), height() + 6));

    int x = qBound(screen.left() + 8, globalPos.x() - panel->width(), screen.right() - panel->width() - 8);
    int y = globalPos.y();
    if (y + panel->height() > screen.bottom() - 8)
        y = mapToGlobal(QPoint(0, 0)).y() - panel->height() - 6;

    panel->move(x, y);
    panel->show();
    panel->raise();
}

// ===============================================================
//  NotificationPanel
// ===============================================================

NotificationPanel::NotificationPanel(QWidget *parent)
    : QWidget(parent, Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setFixedWidth(400);
    setStyleSheet("NotificationPanel { background:#1A0F07;"
                  " border:1px solid rgba(196,121,90,0.5); border-radius:12px; }");

    auto *lay = new QVBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);

    // Header
    auto *header = new QWidget(this);
    header->setFixedHeight(46);
    header->setStyleSheet("background:#2C1A0E; border-radius:12px 12px 0 0;");
    auto *hlay = new QHBoxLayout(header);
    hlay->setContentsMargins(12, 0, 10, 0);
    hlay->setSpacing(5);

    auto *titleLbl = new QLabel("🔔 Notifications", header);
    titleLbl->setStyleSheet("color:#F5EDE0; font-size:13px; font-weight:bold; background:transparent;");

    const int unread = NotificationHistory::instance().unreadCount();
    auto *countLbl = new QLabel(unread > 0 ? QString::number(unread) + " non lues" : "A jour", header);
    countLbl->setStyleSheet(unread > 0
        ? "color:#FFD580; font-size:10px; background:transparent;"
        : "color:rgba(245,237,224,0.3); font-size:10px; background:transparent;");

    static const QString btnStyle =
        "QPushButton { background:rgba(196,121,90,0.15); color:#E8A87C; font-size:10px;"
        " border:1px solid rgba(196,121,90,0.3); border-radius:4px; padding:2px 8px; }"
        "QPushButton:hover { background:rgba(196,121,90,0.35); color:#F5EDE0; }";

    m_markAllBtn = new QPushButton("Lu", header);
    m_markAllBtn->setStyleSheet(btnStyle);
    connect(m_markAllBtn, &QPushButton::clicked, this, [this]() {
        NotificationHistory::instance().markAllRead();
        populate();
    });

    auto *summaryBtn = new QPushButton("Resume IA", header);
    summaryBtn->setStyleSheet(btnStyle);
    connect(summaryBtn, &QPushButton::clicked, this, [this, summaryBtn]() {
        auto *ai = NotificationAI::globalInstance();
        if (!ai) return;
        summaryBtn->setText("...");
        summaryBtn->setEnabled(false);
        QPointer<NotificationPanel> guard(this);
        QPointer<QPushButton>       btnGuard(summaryBtn);
        ai->generateSummary([guard, btnGuard](const QString &summary) {
            if (!guard) return;
            if (btnGuard) { btnGuard->setText("Resume IA"); btnGuard->setEnabled(true); }
            guard->showSummary(summary);
        });
    });

    auto *closeBtn = new QPushButton("x", header);
    closeBtn->setFixedSize(22, 22);
    closeBtn->setStyleSheet(
        "QPushButton { background:rgba(255,255,255,0.08); color:rgba(245,237,224,0.6);"
        " border:none; border-radius:11px; font-size:11px; font-weight:bold; }"
        "QPushButton:hover { background:#C0392B; color:white; }");
    connect(closeBtn, &QPushButton::clicked, this, &NotificationPanel::close);

    hlay->addWidget(titleLbl);
    hlay->addSpacing(4);
    hlay->addWidget(countLbl);
    hlay->addStretch();
    hlay->addWidget(m_markAllBtn);
    hlay->addWidget(summaryBtn);
    hlay->addWidget(closeBtn);
    lay->addWidget(header);

    auto *sep = new QFrame(this);
    sep->setFrameShape(QFrame::HLine);
    sep->setFixedHeight(1);
    sep->setStyleSheet("background:rgba(196,121,90,0.25);");
    lay->addWidget(sep);

    m_scroll = new QScrollArea(this);
    m_scroll->setWidgetResizable(true);
    m_scroll->setFrameShape(QFrame::NoFrame);
    m_scroll->setFixedHeight(420);
    m_scroll->setStyleSheet(
        "QScrollArea { background:#1A0F07; border:none; }"
        "QScrollBar:vertical { width:4px; background:transparent; }"
        "QScrollBar::handle:vertical { background:#5C3317; border-radius:2px; min-height:20px; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height:0; }");

    m_container = new QWidget();
    m_container->setStyleSheet("background:#1A0F07;");
    m_scroll->setWidget(m_container);
    lay->addWidget(m_scroll);

    populate();
}

void NotificationPanel::populate()
{
    delete m_container->layout();
    auto *vlay = new QVBoxLayout(m_container);
    vlay->setContentsMargins(0, 0, 0, 0);
    vlay->setSpacing(0);

    const auto &items = NotificationHistory::instance().items();
    if (items.isEmpty()) {
        auto *empty = new QLabel("Aucune notification", m_container);
        empty->setAlignment(Qt::AlignCenter);
        empty->setStyleSheet("color:rgba(245,237,224,0.3); font-size:12px; padding:40px;");
        vlay->addWidget(empty);
        vlay->addStretch();
        return;
    }

    static constexpr const char* accentColors[] = {"#378ADD","#EF9F27","#E24B4A","#4CAF50"};
    static constexpr const char* tagLabels[]    = {"Info","Alerte","Urgente","Succes"};
    static constexpr const char* tagBg[]        = {"#1A2E4A","#7A4A1A","#6B2737","#1E3A1E"};
    static constexpr const char* tagFg[]        = {"#85B7EB","#F0C070","#F09595","#A0D090"};

    for (const auto &item : items) {
        const int t = qBound(0, static_cast<int>(item.type), 3);
        const QString timeStr = item.timestamp.date() == QDate::currentDate()
            ? item.timestamp.toString("hh:mm")
            : item.timestamp.toString("dd/MM hh:mm");

        auto *row = new QWidget(m_container);
        row->setObjectName("row");
        row->setStyleSheet(
            QString("QWidget#row { background:%1; %2 border-bottom:1px solid rgba(255,255,255,0.06); }")
            .arg(item.read ? "transparent" : "rgba(255,255,255,0.04)")
            .arg(item.read ? "" : QString("border-left:3px solid %1;").arg(accentColors[t])));

        auto *rlay = new QHBoxLayout(row);
        rlay->setContentsMargins(12, 10, 12, 10);
        rlay->setSpacing(10);

        auto *dot = new QLabel(row);
        dot->setFixedSize(7, 7);
        dot->setStyleSheet(item.read
            ? "background:transparent; border-radius:3px;"
            : QString("background:%1; border-radius:3px;").arg(accentColors[t]));

        auto *textCol = new QVBoxLayout();
        textCol->setSpacing(3);
        textCol->setContentsMargins(0, 0, 0, 0);

        auto *titleLbl2 = new QLabel(item.title, row);
        titleLbl2->setStyleSheet(
            QString("color:%1; font-size:12px; font-weight:bold; background:transparent;")
            .arg(item.read ? "rgba(245,237,224,0.55)" : "#F5EDE0"));
        titleLbl2->setWordWrap(true);

        auto *msgLbl = new QLabel(item.message, row);
        msgLbl->setStyleSheet("color:rgba(245,237,224,0.65); font-size:11px; background:transparent;");
        msgLbl->setWordWrap(true);

        auto *metaRow = new QHBoxLayout();
        metaRow->setSpacing(5);
        metaRow->setContentsMargins(0, 2, 0, 0);

        auto *timeLbl = new QLabel(timeStr, row);
        timeLbl->setStyleSheet("color:rgba(245,237,224,0.3); font-size:10px; background:transparent;");

        auto *tag = new QLabel(tagLabels[t], row);
        tag->setStyleSheet(QString("background:%1; color:%2; font-size:10px;"
                                   " border-radius:3px; padding:1px 6px;")
                           .arg(tagBg[t], tagFg[t]));

        metaRow->addWidget(timeLbl);
        metaRow->addWidget(tag);
        if (item.aiGenerated) {
            auto *aiBadge = new QLabel("IA", row);
            aiBadge->setStyleSheet("background:#3A3000; color:#F5C518; font-size:9px;"
                                   " border-radius:3px; padding:1px 5px;");
            metaRow->addWidget(aiBadge);
        }
        metaRow->addStretch();

        textCol->addWidget(titleLbl2);
        textCol->addWidget(msgLbl);
        textCol->addLayout(metaRow);

        rlay->addWidget(dot, 0, Qt::AlignTop | Qt::AlignHCenter);
        rlay->addLayout(textCol, 1);
        vlay->addWidget(row);
    }
    vlay->addStretch();
}

void NotificationPanel::focusOutEvent(QFocusEvent *event)
{
    QWidget::focusOutEvent(event);
}

void NotificationPanel::showSummary(const QString &text)
{
    auto *existing = m_container->findChild<QWidget*>("summaryBox");
    if (existing) existing->deleteLater();

    // Convertir markdown basique → HTML (regex compilée une seule fois)
    static const QRegularExpression reBold("\\*\\*(.+?)\\*\\*");
    QString html = text.toHtmlEscaped();
    html.replace(reBold, "<b>\\1</b>");
    html.replace('\n', QLatin1String("<br>"));

    auto *box = new QWidget(m_container);
    box->setObjectName("summaryBox");
    box->setStyleSheet("QWidget#summaryBox { background:#2A1505;"
                       " border-left:3px solid #E8A87C; }");

    auto *blay = new QVBoxLayout(box);
    blay->setContentsMargins(12, 8, 12, 10);
    blay->setSpacing(4);

    auto *hdrLbl = new QLabel("✨ Resume IA", box);
    hdrLbl->setStyleSheet("color:#E8A87C; font-size:11px; font-weight:bold; background:transparent;");

    auto *body = new QLabel(box);
    body->setText(html);
    body->setTextFormat(Qt::RichText);
    body->setStyleSheet("color:#F5EDE0; font-size:11px; background:transparent;");
    body->setWordWrap(true);
    body->setTextInteractionFlags(Qt::TextSelectableByMouse);

    blay->addWidget(hdrLbl);
    blay->addWidget(body);

    auto *containerLay = qobject_cast<QVBoxLayout*>(m_container->layout());
    if (containerLay) {
        containerLay->insertWidget(0, box);
        m_scroll->verticalScrollBar()->setValue(0);
    }
}

// ===============================================================
//  NotificationSound
// ===============================================================

void NotificationSound::beep(int frequencyHz, int durationMs, float volume)
{
    QAudioFormat fmt;
    fmt.setSampleRate(44100);
    fmt.setChannelCount(1);
    fmt.setSampleFormat(QAudioFormat::Int16);

    const int samples = fmt.sampleRate() * durationMs / 1000;
    QByteArray pcm(samples * static_cast<int>(sizeof(qint16)), Qt::Uninitialized);
    auto *data = reinterpret_cast<qint16*>(pcm.data());

    for (int i = 0; i < samples; ++i) {
        const float t    = float(i) / fmt.sampleRate();
        const float fade = (i > samples * 0.8f)
                               ? 1.0f - float(i - samples * 0.8f) / (samples * 0.2f)
                               : 1.0f;
        data[i] = static_cast<qint16>(qSin(2.0 * M_PI * frequencyHz * t) * 32767 * volume * fade);
    }

    auto *buf = new QBuffer();
    buf->setData(pcm);
    buf->open(QIODevice::ReadOnly);

    auto *sink = new QAudioSink(fmt);
    QObject::connect(sink, &QAudioSink::stateChanged, sink, [sink, buf](QAudio::State s) {
        if (s == QAudio::IdleState) { sink->deleteLater(); buf->deleteLater(); }
    });
    sink->start(buf);
}

void NotificationSound::playSequence(const QList<QPair<int,int>> &notes)
{
    int delay = 0;
    for (const auto &note : notes) {
        QTimer::singleShot(delay, [note]() { beep(note.first, note.second); });
        delay += note.second + 30;
    }
}

void NotificationSound::play(NotificationWidget::Type type)
{
    switch (type) {
    case NotificationWidget::Success:  playSequence({{880, 80}});                          break;
    case NotificationWidget::Info:     playSequence({{660, 120}});                         break;
    case NotificationWidget::Warning:  playSequence({{550, 130}, {550, 130}});             break;
    case NotificationWidget::Critical: playSequence({{440, 150}, {550, 150}, {660, 200}}); break;
    }
}

// ===============================================================
//  NotificationAI
// ===============================================================

NotificationAI::NotificationAI(const QString &apiKey, const QString &model, QObject *parent)
    : QObject(parent), m_apiKey(apiKey), m_model(model)
    , m_nam(new QNetworkAccessManager(this))
{
    connect(m_nam, &QNetworkAccessManager::finished, this, &NotificationAI::onReply);
}

// Factorisation interne : construit et envoie une requête Groq
QNetworkReply *NotificationAI::postGroq(const QString &system, const QString &user, int maxTokens)
{
    QJsonObject body {
        {"model",      m_model},
        {"max_tokens", maxTokens},
        {"messages", QJsonArray{
            QJsonObject{{"role","system"}, {"content", system}},
            QJsonObject{{"role","user"},   {"content", user}}
        }}
    };
    QNetworkRequest req(QUrl("https://api.groq.com/openai/v1/chat/completions"));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("Authorization", ("Bearer " + m_apiKey).toUtf8());
    return m_nam->post(req, QJsonDocument(body).toJson(QJsonDocument::Compact));
}

void NotificationAI::summarize(const QString &context,
                                std::function<void(const QString&, const QString&,
                                                   NotificationWidget::Type)> onResult)
{
    static const QString sys =
        "Tu es l'assistant IA de CUIREA, une entreprise de maroquinerie en Tunisie. "
        "On te donne un contexte technique brut sur une commande. "
        "Réponds UNIQUEMENT avec un objet JSON valide, sans texte autour, avec exactement ces 3 clés : "
        "{\"title\": \"...\", \"message\": \"...\", \"type\": \"Info|Warning|Critical|Success\"}. "
        "Le message doit être une phrase claire en français, maximum 2 phrases. Pas de markdown.";

    m_pending[postGroq(sys, context, 200)] = {onResult, nullptr};
}

void NotificationAI::generateSummary(std::function<void(const QString &summary)> onResult)
{
    const auto &items = NotificationHistory::instance().items();
    if (items.isEmpty()) { if (onResult) onResult("Aucune notification en cours."); return; }

    static const QStringList typeNames = {"Info", "Alerte", "Urgente", "Succes"};
    QStringList lines;
    int i = 0;
    for (const auto &item : items) {
        if (i++ >= 20) break;
        lines << QString("- [%1] %2 : %3")
                     .arg(typeNames[qBound(0, static_cast<int>(item.type), 3)],
                          item.title, item.message);
    }

    static const QString sys =
        "Tu es l'assistant IA de CUIREA, une entreprise de maroquinerie en Tunisie. "
        "On te donne la liste des notifications récentes du système. "
        "Génère un résumé clair en français en 3 parties :\n"
        "1. Un résumé en 1-2 phrases du nombre et type de notifications.\n"
        "2. L'état général de la production (critique / stable / bon).\n"
        "3. Les 2-3 actions prioritaires à faire maintenant.\n"
        "Sois direct, concis, professionnel. Maximum 6 phrases. Pas de markdown, pas de gras, pas de listes.";

    m_pending[postGroq(sys, "Notifications :\n" + lines.join("\n"), 350)] = {nullptr, onResult};
}

void NotificationAI::onReply(QNetworkReply *reply)
{
    const QByteArray raw  = reply->readAll();
    const auto       err  = reply->error();
    const QString    estr = reply->errorString();
    reply->deleteLater();

    auto it = m_pending.find(reply);
    if (it == m_pending.end()) return;
    auto pending = *it;
    m_pending.erase(it);

    if (err != QNetworkReply::NoError) {
        const QString detail = raw.isEmpty() ? estr : QString::fromUtf8(raw);
        qWarning() << "[NotificationAI]" << detail;
        if (pending.summaryCallback) pending.summaryCallback("Erreur API : " + detail);
        return;
    }

    const QString content = QJsonDocument::fromJson(raw).object()
                                ["choices"].toArray().first().toObject()
                                ["message"].toObject()["content"].toString().trimmed();

    if (pending.summaryCallback) {
        pending.summaryCallback(content.isEmpty() ? "Aucune réponse reçue." : content);
        return;
    }

    QJsonParseError jerr;
    const QJsonObject result = QJsonDocument::fromJson(content.toUtf8(), &jerr).object();
    if (jerr.error != QJsonParseError::NoError || result.isEmpty()) {
        qWarning() << "[NotificationAI] JSON invalide:" << content;
        return;
    }

    const QString typeStr = result["type"].toString();
    NotificationWidget::Type type = NotificationWidget::Info;
    if      (typeStr == "Warning")  type = NotificationWidget::Warning;
    else if (typeStr == "Critical") type = NotificationWidget::Critical;
    else if (typeStr == "Success")  type = NotificationWidget::Success;

    if (pending.callback)
        pending.callback(result["title"].toString(), result["message"].toString(), type);
}

// ===============================================================
//  NotificationServer
// ===============================================================

NotificationServer::NotificationServer(quint16 port, QObject *parent)
    : QObject(parent), m_server(new QTcpServer(this)), m_port(port)
{
    connect(m_server, &QTcpServer::newConnection, this, &NotificationServer::onNewConnection);
}

bool NotificationServer::start()
{
    const bool ok = m_server->listen(QHostAddress::LocalHost, m_port);
    if (ok) qDebug()   << "[NotificationServer] Ecoute sur localhost:" << m_port;
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
    const QJsonObject obj = QJsonDocument::fromJson(data.mid(bodyStart + 4).trimmed()).object();
    if (!obj.isEmpty()) emit webhookReceived(obj);
}

// ===============================================================
//  NotificationWatcher
// ===============================================================

NotificationWatcher::NotificationWatcher(QSqlDatabase db, QObject *parent)
    : QObject(parent), m_db(db), m_timer(new QTimer(this))
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

static bool shouldNotify(QSet<QString> &notified, const QString &key)
{
    return notified.contains(key) ? false : (notified.insert(key), true);
}

static void runCheck(QSqlQuery &q, QSet<QString> &notified, const QString &prefix,
                     std::function<void(QSqlQuery &, const QString &)> notify)
{
    if (!q.exec()) { qWarning() << q.lastError().text(); return; }
    while (q.next()) {
        const QString ref = q.value("reference").toString();
        if (shouldNotify(notified, prefix + ref))
            notify(q, ref);
    }
}

void NotificationWatcher::showWithAI(const QString &context,
                                      const QString &fallbackTitle,
                                      const QString &fallbackMessage,
                                      NotificationWidget::Type type)
{
    if (m_ai) {
        m_ai->summarize(context, [=](const QString &t, const QString &m, NotificationWidget::Type tp) {
            NotificationWidget::show(t, m, tp, {}, {}, {}, {}, 6000, true);
        });
    } else {
        NotificationWidget::show(fallbackTitle, fallbackMessage, type);
    }
}

void NotificationWatcher::checkCommandesEnRetard()
{
    QSqlQuery q(m_db);
    q.prepare("SELECT reference, date_livraison FROM commandes "
              "WHERE priorite = 'Urgente' AND statut NOT IN ('Termine','Annule') "
              "  AND date_livraison < :today");
    q.bindValue(":today", QDate::currentDate().toString("yyyy-MM-dd"));

    runCheck(q, m_notifiedRefs, "retard_", [this](QSqlQuery &row, const QString &ref) {
        const int jours = QDate::fromString(row.value("date_livraison").toString(), "yyyy-MM-dd")
                              .daysTo(QDate::currentDate());
        showWithAI(
            QString("Commande %1 · statut=En_Retard · priorite=Urgente · retard=%2 jours · date_livraison=%3")
                .arg(ref).arg(jours).arg(row.value("date_livraison").toString()),
            "Retard livraison",
            ref + " — " + QString::number(jours) + " jour(s) de retard.",
            NotificationWidget::Critical);
    });
}

void NotificationWatcher::checkCommandesEnAttente()
{
    QSqlQuery q(m_db);
    q.prepare("SELECT reference, date_creation FROM commandes "
              "WHERE statut = 'En Attente' AND date_creation < :limite");
    q.bindValue(":limite", QDate::currentDate().addDays(-7).toString("yyyy-MM-dd"));

    runCheck(q, m_notifiedRefs, "attente_", [this](QSqlQuery &row, const QString &ref) {
        const int jours = QDate::fromString(row.value("date_creation").toString(), "yyyy-MM-dd")
                              .daysTo(QDate::currentDate());
        showWithAI(
            QString("Commande %1 · statut=En_Attente · bloquee_depuis=%2 jours · aucun_employe_assigne")
                .arg(ref).arg(jours),
            "Commande en attente",
            ref + " — bloquée depuis " + QString::number(jours) + " jours.",
            NotificationWidget::Warning);
    });
}

void NotificationWatcher::checkCommandesSuspendues()
{
    QSqlQuery q(m_db);
    q.prepare("SELECT reference FROM commandes "
              "WHERE statut = 'Suspendu' AND date_creation < :limite");
    q.bindValue(":limite", QDate::currentDate().addDays(-3).toString("yyyy-MM-dd"));

    runCheck(q, m_notifiedRefs, "suspendu_", [this](QSqlQuery &, const QString &ref) {
        showWithAI(
            QString("Commande %1 · statut=Suspendu · suspendue_depuis=3+ jours · verification_requise").arg(ref),
            "Commande suspendue",
            ref + " — suspendue depuis 3+ jours, vérification requise.",
            NotificationWidget::Warning);
    });
}

void NotificationWatcher::checkLivraisonsProches()
{
    QSqlQuery q(m_db);
    q.prepare("SELECT reference FROM commandes "
              "WHERE statut NOT IN ('Termine','Annule') AND date_livraison = :demain");
    q.bindValue(":demain", QDate::currentDate().addDays(1).toString("yyyy-MM-dd"));

    runCheck(q, m_notifiedRefs, "livraison24h_", [this](QSqlQuery &, const QString &ref) {
        showWithAI(
            QString("Commande %1 · livraison_dans=24h · verifier_etat_production").arg(ref),
            "Livraison demain",
            ref + " — livraison prévue dans moins de 24 heures.",
            NotificationWidget::Info);
    });
}
