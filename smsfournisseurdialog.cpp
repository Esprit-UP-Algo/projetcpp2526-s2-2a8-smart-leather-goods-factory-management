#include "smsfournisseurdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QDateTime>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QUrlQuery>
#include <QByteArray>

// ── Configuration Twilio ─────────────────────────────────────
// Remplace ces 3 valeurs par tes infos depuis console.twilio.com
static const QString TWILIO_ACCOUNT_SID = "AC6f730358c45660014261118c80cd611e";
static const QString TWILIO_AUTH_TOKEN  = "b56b3a4da2956177eeb41a9b9f92af37";
static const QString TWILIO_FROM_NUMBER = "+15076981452"; // Ton numéro Twilio
// ─────────────────────────────────────────────────────────────

SmsFournisseurDialog::SmsFournisseurDialog(const QString &nomEntreprise,
                                           const QString &telephone,
                                           QWidget *parent)
    : QDialog(parent), m_nomEntreprise(nomEntreprise), m_telephone(telephone)
{
    setupUI();
    applyStyles();
    setWindowTitle("Envoyer SMS - " + nomEntreprise);
    setMinimumSize(500, 420);
}

void SmsFournisseurDialog::setupUI()
{
    QVBoxLayout *mainLay = new QVBoxLayout(this);
    mainLay->setContentsMargins(28, 24, 28, 24);
    mainLay->setSpacing(16);

    // Titre
    QLabel *title = new QLabel("Envoyer un SMS au fournisseur");
    title->setObjectName("dialogTitle");
    mainLay->addWidget(title);

    // Info fournisseur
    QLabel *infoLabel = new QLabel(
        QString("<b>Fournisseur :</b> %1").arg(m_nomEntreprise));
    infoLabel->setObjectName("infoLabel");
    mainLay->addWidget(infoLabel);

    QFormLayout *form = new QFormLayout();
    form->setLabelAlignment(Qt::AlignRight);
    form->setHorizontalSpacing(14);
    form->setVerticalSpacing(12);

    // Numéro de téléphone (pré-rempli, modifiable)
    txtTelephone = new QLineEdit(m_telephone);
    txtTelephone->setPlaceholderText("Ex: 21612345678");
    form->addRow("Numéro :", txtTelephone);

    // Templates de messages
    cmbTemplate = new QComboBox();
    cmbTemplate->addItem("-- Choisir un modèle --");
    cmbTemplate->addItem("Confirmation de commande");
    cmbTemplate->addItem("Demande de devis");
    cmbTemplate->addItem("Rappel de paiement");
    cmbTemplate->addItem("Message personnalisé");
    form->addRow("Modèle :", cmbTemplate);

    mainLay->addLayout(form);

    // Zone de message
    QLabel *msgLabel = new QLabel("Message :");
    msgLabel->setObjectName("sectionLabel");
    mainLay->addWidget(msgLabel);

    txtMessage = new QTextEdit();
    txtMessage->setPlaceholderText("Rédigez votre message ici...");
    txtMessage->setMaximumHeight(130);
    mainLay->addWidget(txtMessage);

    // Compteur de caractères
    lblCharCount = new QLabel("0 / 160 caractères");
    lblCharCount->setObjectName("noteLabel");
    lblCharCount->setAlignment(Qt::AlignRight);
    mainLay->addWidget(lblCharCount);

    mainLay->addStretch();

    // Boutons
    QHBoxLayout *btnLay = new QHBoxLayout();
    btnLay->addStretch();

    btnCancel = new QPushButton("Annuler");
    btnCancel->setObjectName("cancelButton");
    btnCancel->setMinimumSize(110, 38);

    btnSend = new QPushButton("Envoyer SMS");
    btnSend->setObjectName("sendButton");
    btnSend->setMinimumSize(130, 38);

    btnLay->addWidget(btnCancel);
    btnLay->addWidget(btnSend);
    mainLay->addLayout(btnLay);

    connect(cmbTemplate, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SmsFournisseurDialog::onTemplateChanged);
    connect(txtMessage, &QTextEdit::textChanged, this, &SmsFournisseurDialog::updateCharCount);
    connect(btnSend,   &QPushButton::clicked, this, &SmsFournisseurDialog::onSendClicked);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

void SmsFournisseurDialog::onTemplateChanged(int index)
{
    QString date = QDate::currentDate().toString("dd/MM/yyyy");
    switch (index) {
    case 1:
        txtMessage->setText(
            QString("Bonjour %1,\n"
                    "Nous confirmons la réception de votre commande du %2.\n"
                    "Merci pour votre confiance.\n- CUIREA")
            .arg(m_nomEntreprise, date));
        break;
    case 2:
        txtMessage->setText(
            QString("Bonjour %1,\n"
                    "Nous souhaitons recevoir un devis pour nos besoins actuels.\n"
                    "Merci de nous contacter rapidement.\n- CUIREA")
            .arg(m_nomEntreprise));
        break;
    case 3:
        txtMessage->setText(
            QString("Bonjour %1,\n"
                    "Nous vous rappelons qu'un paiement est en attente.\n"
                    "Merci de régulariser votre situation.\n- CUIREA")
            .arg(m_nomEntreprise));
        break;
    case 4:
        txtMessage->clear();
        txtMessage->setFocus();
        break;
    default:
        break;
    }
}

void SmsFournisseurDialog::updateCharCount()
{
    int count = txtMessage->toPlainText().length();
    lblCharCount->setText(QString("%1 / 160 caractères").arg(count));
    if (count > 160)
        lblCharCount->setStyleSheet("color: red; font-weight: bold;");
    else
        lblCharCount->setStyleSheet("color: #8D6E63;");
}

void SmsFournisseurDialog::onSendClicked()
{
    if (txtTelephone->text().trimmed().isEmpty()) {
        txtTelephone->setStyleSheet("border: 2px solid red;");
        QMessageBox::warning(this, "Validation", "Le numéro de téléphone est obligatoire.");
        return;
    }
    txtTelephone->setStyleSheet("");

    if (txtMessage->toPlainText().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Validation", "Le message ne peut pas être vide.");
        return;
    }

    if (txtMessage->toPlainText().length() > 160) {
        QMessageBox::warning(this, "Validation",
            "Le message dépasse 160 caractères.\nVeuillez le raccourcir.");
        return;
    }

    // Confirmation avant envoi
    QMessageBox confirm(this);
    confirm.setWindowTitle("Confirmer l'envoi");
    confirm.setText(QString("Envoyer ce SMS au %1 ?").arg(txtTelephone->text().trimmed()));
    confirm.setInformativeText(txtMessage->toPlainText());
    confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirm.setDefaultButton(QMessageBox::Yes);
    confirm.setStyleSheet(
        "QMessageBox { background-color: #FAF5F0; }"
        "QMessageBox QLabel { color: #291C0E; font-size: 12px; }"
        "QPushButton { background-color: #8D6E63; color: white; border: none; "
        "border-radius: 6px; padding: 8px 20px; font-size: 11px; font-weight: bold; }"
        "QPushButton:hover { background-color: #A0826D; }"
    );

    if (confirm.exec() != QMessageBox::Yes)
        return;

    // ── Envoi via Twilio API ──────────────────────────────────
    btnSend->setEnabled(false);
    btnSend->setText("Envoi...");

    QString toNumber = txtTelephone->text().trimmed();
    // Ajouter le préfixe +216 si le numéro est local (8 chiffres)
    if (toNumber.length() == 8 && !toNumber.startsWith("+"))
        toNumber = "+216" + toNumber;

    QUrl url(QString("https://api.twilio.com/2010-04-01/Accounts/%1/Messages.json")
             .arg(TWILIO_ACCOUNT_SID));

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    // Authentification Basic (SID:Token en base64)
    QString credentials = TWILIO_ACCOUNT_SID + ":" + TWILIO_AUTH_TOKEN;
    QByteArray base64 = credentials.toUtf8().toBase64();
    request.setRawHeader("Authorization", "Basic " + base64);

    QUrlQuery params;
    params.addQueryItem("To",   toNumber);
    params.addQueryItem("From", TWILIO_FROM_NUMBER);
    params.addQueryItem("Body", txtMessage->toPlainText().trimmed());

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkReply *reply = manager->post(request, params.toString(QUrl::FullyEncoded).toUtf8());

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        btnSend->setEnabled(true);
        btnSend->setText("Envoyer SMS");

        QByteArray response = reply->readAll();
        int httpCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        reply->deleteLater();

        if (httpCode == 201) {
            // Succès
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("SMS Envoyé");
            msgBox.setText("SMS envoyé avec succès !");
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setStyleSheet(
                "QMessageBox { background-color: #FAF5F0; }"
                "QMessageBox QLabel { color: #291C0E; font-size: 12px; }"
                "QPushButton { background-color: #8D6E63; color: white; border: none; "
                "border-radius: 6px; padding: 8px 20px; font-size: 11px; font-weight: bold; }"
                "QPushButton:hover { background-color: #A0826D; }"
            );
            msgBox.exec();
            accept();
        } else {
            // Erreur
            QMessageBox::critical(this, "Erreur d'envoi",
                QString("Échec de l'envoi du SMS.\nCode HTTP: %1\n\n"
                        "Vérifiez vos identifiants Twilio dans smsfournisseurdialog.cpp")
                .arg(httpCode));
        }
    });
}

QString SmsFournisseurDialog::getMessage() const
{
    return txtMessage ? txtMessage->toPlainText().trimmed() : QString();
}

QString SmsFournisseurDialog::getTelephone() const
{
    return txtTelephone ? txtTelephone->text().trimmed() : QString();
}

void SmsFournisseurDialog::applyStyles()
{
    setStyleSheet(
        "QDialog { background-color: #FAF5F0; }"
        "QLabel#dialogTitle { color: #291C0E; font-family: 'Times New Roman', serif; "
        "font-size: 18px; font-weight: bold; padding: 6px 0; }"
        "QLabel#infoLabel { color: #291C0E; font-size: 12px; background-color: #FFF8F0; "
        "border: 2px solid #BCAAA4; border-radius: 8px; padding: 10px; }"
        "QLabel#sectionLabel { color: #8D6E63; font-size: 13px; font-weight: bold; }"
        "QLabel#noteLabel { color: #8D6E63; font-size: 10px; font-style: italic; }"
        "QLabel { color: #291C0E; font-family: Arial, sans-serif; font-size: 12px; font-weight: bold; }"
        "QLineEdit, QComboBox { background-color: white; color: #291C0E; border: 2px solid #BCAAA4; "
        "border-radius: 6px; padding: 8px; font-size: 11px; min-height: 25px; }"
        "QLineEdit:focus, QComboBox:focus { border: 2px solid #8D6E63; }"
        "QTextEdit { background-color: white; color: #291C0E; border: 2px solid #BCAAA4; "
        "border-radius: 6px; padding: 8px; font-size: 11px; }"
        "QTextEdit:focus { border: 2px solid #8D6E63; }"
        "QPushButton#sendButton { background-color: #8D6E63; color: white; border: none; "
        "border-radius: 8px; padding: 10px 20px; font-size: 12px; font-weight: bold; }"
        "QPushButton#sendButton:hover { background-color: #A0826D; }"
        "QPushButton#cancelButton { background-color: #E7DDD1; color: #291C0E; "
        "border: 1px solid #BCAAA4; border-radius: 8px; padding: 10px 20px; "
        "font-size: 11px; font-weight: bold; }"
        "QPushButton#cancelButton:hover { background-color: #F0E6DA; }"
        "QComboBox::drop-down { border: none; width: 30px; }"
        "QComboBox::down-arrow { image: none; border-left: 5px solid transparent; "
        "border-right: 5px solid transparent; border-top: 5px solid #8D6E63; margin-right: 10px; }"
    );
}
