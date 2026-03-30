#include "aichatwidget.h"
#include "envloader.h"
#include <QNetworkRequest>
#include <QUrl>
#include <QScrollBar>

AIChatWidget::AIChatWidget(QWidget *parent)
    : QWidget(parent), m_visible(false)
{
    m_network = new QNetworkAccessManager(this);
    connect(m_network, &QNetworkAccessManager::finished, this, &AIChatWidget::onReplyFinished);

    m_voiceTimer = new QTimer(this);
    m_voiceTimer->setInterval(200); // poll toutes les 200ms
    connect(m_voiceTimer, &QTimer::timeout, this, &AIChatWidget::checkVoiceResult);

    setupUI();
    initSAPI();
    hide();
}

AIChatWidget::~AIChatWidget()
{
    stopSAPI();
}

// ── Construction de l'interface ───────────────────────────────────────────────
void AIChatWidget::setupUI()
{
    // Panneau principal avec fond sombre (thème Catppuccin Mocha)
    m_panel = new QWidget(this);
    m_panel->setFixedSize(380, 520);
    m_panel->setStyleSheet(
        "QWidget { background: #1e1e2e; border-radius: 12px; border: 1px solid #444; }"
    );

    QVBoxLayout *layout = new QVBoxLayout(m_panel);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(8);

    // ── En-tête : titre + bouton fermer ─────────────────────────────────────
    QHBoxLayout *header = new QHBoxLayout();
    QLabel *title = new QLabel("🤖 Assistant IA");
    title->setStyleSheet("color: #cdd6f4; font-weight: bold; font-size: 14px; border: none;");

    m_closeBtn = new QPushButton("✕");
    m_closeBtn->setFixedSize(24, 24);
    m_closeBtn->setStyleSheet(
        "QPushButton { background: #f38ba8; color: white; border-radius: 12px; border: none; font-weight: bold; }"
        "QPushButton:hover { background: #e06c75; }"
    );
    connect(m_closeBtn, &QPushButton::clicked, this, &AIChatWidget::toggleChat);

    header->addWidget(title);
    header->addStretch();
    header->addWidget(m_closeBtn);
    layout->addLayout(header);

    // ── Zone d'affichage de la conversation ─────────────────────────────────
    m_chatDisplay = new QTextEdit();
    m_chatDisplay->setReadOnly(true); // L'utilisateur ne peut pas modifier l'historique
    m_chatDisplay->setStyleSheet(
        "QTextEdit { background: #181825; color: #cdd6f4; border-radius: 8px; "
        "border: 1px solid #313244; font-size: 13px; padding: 6px; }"
    );
    layout->addWidget(m_chatDisplay);

    // ── Label de statut (affiché pendant l'attente de la réponse) ───────────
    m_statusLabel = new QLabel("");
    m_statusLabel->setStyleSheet("color: #a6adc8; font-size: 11px; border: none;");
    layout->addWidget(m_statusLabel);

    // ── Ligne de saisie + bouton envoi ───────────────────────────────────────
    QHBoxLayout *inputRow = new QHBoxLayout();

    m_inputField = new QLineEdit();
    m_inputField->setPlaceholderText("Posez votre question...");
    m_inputField->setStyleSheet(
        "QLineEdit { background: #313244; color: #cdd6f4; border-radius: 8px; "
        "border: 1px solid #45475a; padding: 8px; font-size: 13px; }"
        "QLineEdit:focus { border: 1px solid #89b4fa; }"
    );
    // Envoyer le message en appuyant sur Entrée
    connect(m_inputField, &QLineEdit::returnPressed, this, &AIChatWidget::sendMessage);

    m_sendBtn = new QPushButton("➤");
    m_sendBtn->setFixedSize(38, 38);
    m_sendBtn->setStyleSheet(
        "QPushButton { background: #89b4fa; color: #1e1e2e; border-radius: 8px; "
        "border: none; font-size: 16px; font-weight: bold; }"
        "QPushButton:hover { background: #74c7ec; }"
        "QPushButton:disabled { background: #45475a; }"
    );
    connect(m_sendBtn, &QPushButton::clicked, this, &AIChatWidget::sendMessage);

    // Bouton micro
    m_voiceBtn = new QPushButton("🎤");
    m_voiceBtn->setFixedSize(38, 38);
    m_voiceBtn->setToolTip("Parler à l'assistant");
    m_voiceBtn->setStyleSheet(
        "QPushButton { background: #a6e3a1; color: #1e1e2e; border-radius: 8px; "
        "border: none; font-size: 16px; }"
        "QPushButton:hover { background: #94e2d5; }"
        "QPushButton:checked { background: #f38ba8; }"
    );
    m_voiceBtn->setCheckable(true);
    connect(m_voiceBtn, &QPushButton::clicked, this, &AIChatWidget::toggleVoice);

    inputRow->addWidget(m_inputField);
    inputRow->addWidget(m_voiceBtn);
    inputRow->addWidget(m_sendBtn);
    layout->addLayout(inputRow);

    // Message de bienvenue affiché au démarrage
    appendMessage("assistant", "Bonjour ! Je suis votre assistant IA. Comment puis-je vous aider ?");
}

// ─────────────────────────────────────────────────────────────────────────────
void AIChatWidget::setContext(const QString &context)
{
    // Stocke le contexte qui sera injecté dans le prompt système à chaque requête
    m_context = context;
}

// ─────────────────────────────────────────────────────────────────────────────
void AIChatWidget::toggleChat()
{
    m_visible = !m_visible;
    if (m_visible) {
        // Positionner le panneau en bas à droite de la fenêtre parente
        if (parentWidget()) {
            int x = parentWidget()->width() - 400;
            int y = parentWidget()->height() - 540;
            move(x, y);
            resize(380, 520);
        }
        m_panel->resize(this->size());
        show();
        raise();                   // Passer au premier plan
        m_inputField->setFocus();  // Prêt à saisir immédiatement
    } else {
        hide();
    }
}

// ─────────────────────────────────────────────────────────────────────────────
void AIChatWidget::appendMessage(const QString &role, const QString &text)
{
    // Couleur et alignement différents selon l'émetteur
    QString color = (role == "user") ? "#a6e3a1" : "#89b4fa"; // Vert utilisateur, bleu IA
    QString align = (role == "user") ? "right"   : "left";
    QString name  = (role == "user") ? "Vous"    : "IA";

    m_chatDisplay->append(
        QString("<div style='text-align:%1; margin:4px;'>"
                "<span style='color:%2; font-weight:bold;'>%3:</span> "
                "<span style='color:#cdd6f4;'>%4</span></div>")
        .arg(align, color, name, text.toHtmlEscaped())
    );

    // Faire défiler automatiquement vers le bas pour voir le dernier message
    m_chatDisplay->verticalScrollBar()->setValue(
        m_chatDisplay->verticalScrollBar()->maximum()
    );
}

// ── Envoi d'un message à l'API Groq ──────────────────────────────────────────
void AIChatWidget::sendMessage()
{
    QString userText = m_inputField->text().trimmed();
    if (userText.isEmpty()) return;

    m_inputField->clear();
    m_sendBtn->setEnabled(false);          // Désactiver pendant l'attente
    m_statusLabel->setText("En train d'écrire...");
    appendMessage("user", userText);

    // Ajouter le message utilisateur à l'historique multi-tour
    QJsonObject userMsg;
    userMsg["role"]    = "user";
    userMsg["content"] = userText;
    m_history.append(userMsg);

    // Construire le tableau de messages : prompt système + historique complet
    QJsonArray messages;
    QJsonObject sys;
    sys["role"]    = "system";
    sys["content"] = QString("Tu es un assistant IA intégré dans un logiciel de gestion d'entreprise. "
                             "Contexte actuel : %1. Réponds en français, de façon concise.").arg(m_context);
    messages.append(sys);
    for (const auto &m : m_history)
        messages.append(m);

    // Corps de la requête JSON pour l'API Groq
    QJsonObject body;
    body["model"]       = "llama-3.3-70b-versatile";
    body["messages"]    = messages;
    body["max_tokens"]  = 512;
    body["temperature"] = 0.7; // Créativité modérée

    // Requête HTTP POST avec authentification Bearer
    // Essaie d'abord la variable d'environnement, puis la clé intégrée en fallback
    QString apiKey = EnvLoader::get("GROQ_API_KEY");
    if (apiKey.isEmpty()) {
        m_sendBtn->setEnabled(true);
        m_statusLabel->setText("");
        appendMessage("assistant", "⚠ Clé API non configurée. Vérifiez le fichier .env (GROQ_API_KEY=...)");
        return;
    }
    QNetworkRequest req(QUrl("https://api.groq.com/openai/v1/chat/completions"));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("Authorization", ("Bearer " + apiKey).toUtf8());

    m_network->post(req, QJsonDocument(body).toJson());
}

// ── Traitement de la réponse de l'API ────────────────────────────────────────
void AIChatWidget::onReplyFinished(QNetworkReply *reply)
{
    m_sendBtn->setEnabled(true);  // Réactiver le bouton d'envoi
    m_statusLabel->setText("");

    // Vérifier les erreurs réseau
    if (reply->error() != QNetworkReply::NoError) {
        // Lire le corps de la réponse pour obtenir le message d'erreur détaillé
        QByteArray errBody = reply->readAll();
        QJsonDocument errDoc = QJsonDocument::fromJson(errBody);
        QString errMsg = reply->errorString();
        if (!errDoc.isNull() && errDoc["error"]["message"].isString())
            errMsg = errDoc["error"]["message"].toString();
        appendMessage("assistant", "❌ Erreur API : " + errMsg);
        reply->deleteLater();
        return;
    }

    // Parser la réponse JSON et extraire le contenu du message
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    QString content = doc["choices"][0]["message"]["content"].toString();

    if (content.isEmpty()) {
        appendMessage("assistant", "❌ Réponse vide de l'API.");
    } else {
        appendMessage("assistant", content);

        // Ajouter la réponse à l'historique pour maintenir le contexte multi-tour
        QJsonObject assistantMsg;
        assistantMsg["role"]    = "assistant";
        assistantMsg["content"] = content;
        m_history.append(assistantMsg);
    }

    reply->deleteLater(); // Libérer la mémoire de la réponse
}

// ── Initialisation SAPI ───────────────────────────────────────────────────────
void AIChatWidget::initSAPI()
{
    // Vérifier que PowerShell est disponible (requis pour la reconnaissance vocale)
    QProcess test;
    test.start("powershell", {"-Command", "echo ok"});
    test.waitForFinished(2000);
    if (test.exitCode() != 0) {
        m_voiceBtn->setEnabled(false);
        m_voiceBtn->setToolTip("PowerShell non disponible");
    }
}

void AIChatWidget::stopSAPI()
{
    m_voiceTimer->stop();
    if (m_voiceProcess) {
        m_voiceProcess->kill();
        m_voiceProcess->waitForFinished(1000);
        delete m_voiceProcess;
        m_voiceProcess = nullptr;
    }
}

// ── Bascule écoute vocale ─────────────────────────────────────────────────────
void AIChatWidget::toggleVoice()
{
    if (!m_listening) {
        // Démarrer l'écoute via PowerShell + System.Speech
        m_listening = true;
        m_voiceBtn->setChecked(true);
        m_statusLabel->setText("🎤 Écoute en cours...");

        // Script PowerShell inline : écoute 6 secondes et retourne le texte reconnu
        QString script =
            "Add-Type -AssemblyName System.Speech;"
            "$r = New-Object System.Speech.Recognition.SpeechRecognitionEngine;"
            "$r.SetInputToDefaultAudioDevice();"
            "$g = New-Object System.Speech.Recognition.DictationGrammar;"
            "$r.LoadGrammar($g);"
            "$res = $r.Recognize([TimeSpan]::FromSeconds(6));"
            "if ($res) { Write-Output $res.Text } else { Write-Output '' }";

        m_voiceProcess = new QProcess(this);
        connect(m_voiceProcess, QOverload<int,QProcess::ExitStatus>::of(&QProcess::finished),
                this, &AIChatWidget::checkVoiceResult);
        m_voiceProcess->start("powershell", {"-NoProfile", "-Command", script});

    } else {
        // Annuler l'écoute
        stopSAPI();
        m_listening = false;
        m_voiceBtn->setChecked(false);
        m_statusLabel->setText("");
    }
}

// ── Récupération du résultat vocal ───────────────────────────────────────────
void AIChatWidget::checkVoiceResult()
{
    m_listening = false;
    m_voiceBtn->setChecked(false);
    m_statusLabel->setText("");

    if (!m_voiceProcess) return;

    QString recognized = QString::fromUtf8(m_voiceProcess->readAllStandardOutput()).trimmed();
    m_voiceProcess->deleteLater();
    m_voiceProcess = nullptr;

    if (!recognized.isEmpty()) {
        m_inputField->setText(recognized);
        sendMessage();
    } else {
        m_statusLabel->setText("⚠ Rien compris, réessayez.");
        QTimer::singleShot(2000, this, [this]{ m_statusLabel->setText(""); });
    }
}
