#pragma once

#include <QWidget>
#include <QString>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

/**
 * @class AIChatWidget
 * @brief Widget de chat IA flottant intégré dans la fenêtre principale.
 *
 * Envoie les messages de l'utilisateur à l'API Groq (modèle llama-3.3-70b-versatile)
 * et affiche les réponses dans une bulle de conversation.
 *
 * Utilisation :
 *  - Appeler setContext() pour fournir un contexte métier à l'IA (ex: onglet actif).
 *  - Appeler toggleChat() pour afficher/masquer le panneau.
 *
 * La clé API est lue depuis la variable d'environnement GROQ_API_KEY via EnvLoader.
 */
class AIChatWidget : public QWidget
{
    Q_OBJECT
public:
    /** @brief Constructeur — initialise le réseau, construit l'UI et masque le panneau. */
    explicit AIChatWidget(QWidget *parent = nullptr);

    /**
     * @brief Définit le contexte métier transmis à l'IA dans le prompt système.
     * @param context Description de l'écran ou de la situation actuelle.
     */
    void setContext(const QString &context);

    /** @brief Affiche ou masque le panneau de chat (bascule). */
    void toggleChat();

private slots:
    /** @brief Lit le texte saisi, l'envoie à l'API et vide le champ. */
    void sendMessage();

    /**
     * @brief Reçoit la réponse HTTP de l'API Groq et affiche le contenu.
     * @param reply Réponse réseau à lire et libérer.
     */
    void onReplyFinished(QNetworkReply *reply);

private:
    /**
     * @brief Ajoute un message dans la zone d'affichage avec mise en forme HTML.
     * @param role "user" ou "assistant" — détermine la couleur et l'alignement.
     * @param text Contenu du message (échappé en HTML).
     */
    void appendMessage(const QString &role, const QString &text);

    /** @brief Construit tous les widgets du panneau (titre, zone chat, champ saisie). */
    void setupUI();

    QString m_context;  ///< Contexte métier injecté dans le prompt système
    bool    m_visible;  ///< État d'affichage du panneau

    // ── Widgets de l'interface ───────────────────────────────────────────────
    QWidget     *m_panel;        ///< Conteneur principal du panneau flottant
    QTextEdit   *m_chatDisplay;  ///< Zone d'affichage de la conversation (lecture seule)
    QLineEdit   *m_inputField;   ///< Champ de saisie du message utilisateur
    QPushButton *m_sendBtn;      ///< Bouton d'envoi
    QPushButton *m_closeBtn;     ///< Bouton de fermeture du panneau
    QLabel      *m_statusLabel;  ///< Indicateur "En train d'écrire..."

    // ── Réseau et historique ─────────────────────────────────────────────────
    QNetworkAccessManager *m_network;  ///< Gestionnaire des requêtes HTTP
    QJsonArray  m_history;             ///< Historique des messages (user + assistant) pour le contexte multi-tour
};
