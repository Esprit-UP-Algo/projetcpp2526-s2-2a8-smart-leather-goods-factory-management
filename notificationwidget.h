#ifndef NOTIFICATIONWIDGET_H
#define NOTIFICATIONWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QPropertyAnimation>

/**
 * @class NotificationWidget
 * @brief Notification toast flottante affichée en bas à droite de l'écran.
 *
 * Apparaît avec un fondu entrant (300 ms), reste visible 5 secondes,
 * puis disparaît avec un fondu sortant (400 ms) avant de se détruire.
 *
 * Plusieurs notifications peuvent être empilées verticalement grâce à
 * la variable statique s_stackOffset.
 *
 * Trois niveaux visuels disponibles :
 *  - Info     → fond bleu
 *  - Warning  → fond orange
 *  - Critical → fond rouge
 *
 * Exemple d'utilisation :
 * @code
 *   new NotificationWidget("Titre", "Message détaillé", NotificationWidget::Warning);
 * @endcode
 * L'objet se détruit automatiquement grâce à Qt::WA_DeleteOnClose.
 */
class NotificationWidget : public QWidget
{
    Q_OBJECT
    // Propriété animée pour le fondu (utilisée par QPropertyAnimation)
    Q_PROPERTY(qreal opacity READ windowOpacity WRITE setWindowOpacity)

public:
    /** @brief Niveaux de sévérité de la notification. */
    enum Type { Info, Warning, Critical };

    /**
     * @brief Crée et affiche immédiatement la notification.
     * @param title   Titre affiché en gras.
     * @param message Corps du message.
     * @param type    Niveau de sévérité (détermine la couleur).
     * @param parent  Widget parent (optionnel).
     */
    explicit NotificationWidget(const QString &title,
                                const QString &message,
                                Type type = Warning,
                                QWidget *parent = nullptr);

    /**
     * @brief Décalage vertical cumulé pour empiler les notifications.
     *
     * Incrémenté à la création, décrémenté à la fermeture de chaque notification.
     * Permet d'éviter que plusieurs toasts se superposent.
     */
    static int s_stackOffset;

private:
    /** @brief Lance l'animation de fondu sortant puis ferme le widget. */
    void fadeOut();

    // ── Widgets internes ────────────────────────────────────────────────────
    QLabel *m_icon;     ///< Emoji de sévérité (🔴 / 🟠 / 🔵)
    QLabel *m_title;    ///< Titre de la notification
    QLabel *m_message;  ///< Corps du message

    QTimer             *m_autoClose; ///< Déclenche fadeOut() après 5 secondes
    QPropertyAnimation *m_fadeIn;    ///< Animation d'apparition (opacité 0 → 1)
    QPropertyAnimation *m_fadeOut;   ///< Animation de disparition (opacité 1 → 0)
};

#endif // NOTIFICATIONWIDGET_H
