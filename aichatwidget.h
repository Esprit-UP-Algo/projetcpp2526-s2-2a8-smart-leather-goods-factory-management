#pragma once

#include <QWidget>
#include <QString>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QProcess>

class AIChatWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AIChatWidget(QWidget *parent = nullptr);
    ~AIChatWidget();

    void setContext(const QString &context);
    void toggleChat();

private slots:
    void sendMessage();
    void onReplyFinished(QNetworkReply *reply);
    void toggleVoice();
    void checkVoiceResult();

private:
    void appendMessage(const QString &role, const QString &text);
    void setupUI();
    void initSAPI();
    void stopSAPI();

    QString m_context;
    bool    m_visible   = false;
    bool    m_listening = false;

    QWidget     *m_panel;
    QTextEdit   *m_chatDisplay;
    QLineEdit   *m_inputField;
    QPushButton *m_sendBtn;
    QPushButton *m_closeBtn;
    QPushButton *m_voiceBtn;
    QLabel      *m_statusLabel;
    QTimer      *m_voiceTimer;
    QProcess    *m_voiceProcess = nullptr;

    QNetworkAccessManager *m_network;
    QJsonArray  m_history;
};
