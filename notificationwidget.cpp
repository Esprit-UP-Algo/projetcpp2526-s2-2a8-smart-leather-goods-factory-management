#include "notificationwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScreen>
#include <QApplication>
#include <QGraphicsDropShadowEffect>

// Décalage vertical partagé entre toutes les instances (empilage des toasts)
int NotificationWidget::s_stackOffset = 0;

// ─────────────────────────────────────────────────────────────────────────────
NotificationWidget::NotificationWidget(const QString &title,
                                       const QString &message,
                                       Type type,
                                       QWidget *parent)
    : QWidget(parent, Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint)
{
    // WA_TranslucentBackground : nécessaire pour que le border-radius soit visible
    // WA_DeleteOnClose : l'objet se détruit automatiquement à la fermeture
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);
    setFixedWidth(320);

    // ── Couleurs et icône selon le niveau de sévérité ────────────────────────
    QString bg, border, icon;
    switch (type) {
    case Critical:
        bg = "#C0392B"; border = "#E74C3C"; icon = "🔴";
        break;
    case Warning:
        bg = "#E67E22"; border = "#F39C12"; icon = "🟠";
        break;
    default: // Info
        bg = "#2980B9"; border = "#3498DB"; icon = "🔵";
        break;
    }

    // ── Construction du conteneur avec bordure gauche colorée ────────────────
    auto *container = new QWidget(this);
    container->setObjectName("container");
    container->setStyleSheet(QString(
        "#container {"
        "  background: %1;"
        "  border-left: 5px solid %2;"  // Indicateur visuel de sévérité
        "  border-radius: 10px;"
        "  padding: 12px 14px;"
        "}").arg(bg, border));

    // Ombre portée pour donner de la profondeur au toast
    auto *shadow = new QGraphicsDropShadowEffect(container);
    shadow->setBlurRadius(18);
    shadow->setOffset(0, 4);
    shadow->setColor(QColor(0, 0, 0, 100));
    container->setGraphicsEffect(shadow);

    // Layout externe avec marges pour que l'ombre soit visible
    auto *outerLay = new QVBoxLayout(this);
    outerLay->setContentsMargins(8, 8, 8, 8);
    outerLay->addWidget(container);

    auto *lay = new QVBoxLayout(container);
    lay->setSpacing(4);

    // ── Ligne titre : icône + texte ──────────────────────────────────────────
    auto *titleRow = new QHBoxLayout();
    m_icon  = new QLabel(icon, container);
    m_icon->setStyleSheet("font-size:16px;");
    m_title = new QLabel(title, container);
    m_title->setStyleSheet("color:white;font-size:13px;font-weight:bold;");
    m_title->setWordWrap(true);
    titleRow->addWidget(m_icon);
    titleRow->addWidget(m_title, 1);
    lay->addLayout(titleRow);

    // ── Corps du message ─────────────────────────────────────────────────────
    m_message = new QLabel(message, container);
    m_message->setStyleSheet("color:rgba(255,255,255,0.90);font-size:12px;");
    m_message->setWordWrap(true);
    lay->addWidget(m_message);

    adjustSize(); // Adapter la hauteur au contenu

    // ── Positionnement en bas à droite, empilé au-dessus des autres toasts ───
    QRect screen = QApplication::primaryScreen()->availableGeometry();
    int x = screen.right()  - width()  - 20;
    int y = screen.bottom() - height() - 20 - s_stackOffset;
    move(x, y);
    s_stackOffset += height() + 10; // Réserver l'espace pour le prochain toast

    // ── Animation d'apparition (fondu entrant 300 ms) ────────────────────────
    setWindowOpacity(0.0);
    show();
    m_fadeIn = new QPropertyAnimation(this, "opacity", this);
    m_fadeIn->setDuration(300);
    m_fadeIn->setStartValue(0.0);
    m_fadeIn->setEndValue(1.0);
    m_fadeIn->start(QAbstractAnimation::DeleteWhenStopped);

    // ── Fermeture automatique après 5 secondes ───────────────────────────────
    m_autoClose = new QTimer(this);
    m_autoClose->setSingleShot(true);
    connect(m_autoClose, &QTimer::timeout, this, &NotificationWidget::fadeOut);
    m_autoClose->start(5000);
}

// ── Animation de disparition ──────────────────────────────────────────────────
void NotificationWidget::fadeOut()
{
    m_fadeOut = new QPropertyAnimation(this, "opacity", this);
    m_fadeOut->setDuration(400);
    m_fadeOut->setStartValue(1.0);
    m_fadeOut->setEndValue(0.0);

    // À la fin du fondu : libérer l'espace dans la pile et fermer le widget
    connect(m_fadeOut, &QPropertyAnimation::finished, this, [this]() {
        s_stackOffset = qMax(0, s_stackOffset - height() - 10);
        close(); // WA_DeleteOnClose détruira l'objet
    });

    m_fadeOut->start(QAbstractAnimation::DeleteWhenStopped);
}
