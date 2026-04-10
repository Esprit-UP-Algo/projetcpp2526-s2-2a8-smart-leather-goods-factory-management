#include "qrfournisseurdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QUrl>
#include <QUrlQuery>
#include <QDesktopServices>
#include <QPixmap>
#include <QClipboard>
#include <QApplication>

#include "qrfournisseurdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QUrl>
#include <QUrlQuery>
#include <QDesktopServices>
#include <QPixmap>
#include <QClipboard>
#include <QApplication>
#include <QFile>
#include <QDir>
#include <QStandardPaths>

QrFournisseurDialog::QrFournisseurDialog(const QString &nomEntreprise,
                                         const QString &email,
                                         const QString &telephone,
                                         const QString &typeProduit,
                                         QWidget *parent)
    : QDialog(parent),
      m_nomEntreprise(nomEntreprise),
      m_email(email),
      m_telephone(telephone),
      m_typeProduit(typeProduit)
{
    // Extraire le HTML depuis les ressources Qt vers un fichier temporaire
    QString tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    QString htmlPath = tempDir + "/commande_fournisseur.html";

    QFile src(":/commande_fournisseur.html");
    if (src.open(QIODevice::ReadOnly)) {
        QFile dst(htmlPath);
        if (dst.open(QIODevice::WriteOnly)) {
            dst.write(src.readAll());
            dst.close();
        }
        src.close();
    }

    // Construire l'URL avec paramètres pré-remplis
    QUrlQuery params;
    params.addQueryItem("fournisseur", nomEntreprise);
    params.addQueryItem("email",       email);
    params.addQueryItem("telephone",   telephone);
    params.addQueryItem("type_produit",typeProduit);

    QUrl fileUrl = QUrl::fromLocalFile(htmlPath);
    fileUrl.setQuery(params);
    m_formUrl = fileUrl.toString();

    setupUI();
    applyStyles();
    loadQrCode();

    setWindowTitle("QR Code - " + nomEntreprise);
    setMinimumSize(420, 520);
}

void QrFournisseurDialog::setupUI()
{
    QVBoxLayout *mainLay = new QVBoxLayout(this);
    mainLay->setContentsMargins(28, 24, 28, 24);
    mainLay->setSpacing(14);

    QLabel *title = new QLabel("QR Code Commande Rapide");
    title->setObjectName("dialogTitle");
    title->setAlignment(Qt::AlignCenter);
    mainLay->addWidget(title);

    QLabel *infoLabel = new QLabel(
        QString("<b>Fournisseur :</b> %1<br>"
                "<b>Type produit :</b> %2")
        .arg(m_nomEntreprise, m_typeProduit));
    infoLabel->setObjectName("infoLabel");
    infoLabel->setAlignment(Qt::AlignCenter);
    mainLay->addWidget(infoLabel);

    // Zone QR code
    lblQrImage = new QLabel();
    lblQrImage->setObjectName("qrLabel");
    lblQrImage->setFixedSize(250, 250);
    lblQrImage->setAlignment(Qt::AlignCenter);
    lblQrImage->setText("Chargement...");
    mainLay->addWidget(lblQrImage, 0, Qt::AlignCenter);

    lblStatus = new QLabel("Scannez ce QR code pour accéder au formulaire de commande pré-rempli");
    lblStatus->setObjectName("noteLabel");
    lblStatus->setAlignment(Qt::AlignCenter);
    lblStatus->setWordWrap(true);
    mainLay->addWidget(lblStatus);

    // Lien cliquable
    lblUrl = new QLabel(QString("<a href='%1'>Ouvrir le lien</a>").arg(m_formUrl));
    lblUrl->setObjectName("linkLabel");
    lblUrl->setAlignment(Qt::AlignCenter);
    lblUrl->setOpenExternalLinks(true);
    lblUrl->setWordWrap(true);
    mainLay->addWidget(lblUrl);

    mainLay->addStretch();

    // Boutons
    QHBoxLayout *btnLay = new QHBoxLayout();
    btnLay->addStretch();

    QPushButton *btnCopy = new QPushButton("Copier le lien");
    btnCopy->setObjectName("copyButton");
    btnCopy->setMinimumSize(120, 38);

    QPushButton *btnOpen = new QPushButton("Ouvrir formulaire");
    btnOpen->setObjectName("openButton");
    btnOpen->setMinimumSize(130, 38);

    QPushButton *btnClose = new QPushButton("Fermer");
    btnClose->setObjectName("cancelButton");
    btnClose->setMinimumSize(100, 38);

    btnLay->addWidget(btnCopy);
    btnLay->addWidget(btnOpen);
    btnLay->addWidget(btnClose);
    mainLay->addLayout(btnLay);

    connect(btnCopy, &QPushButton::clicked, this, [this]() {
        QApplication::clipboard()->setText(m_formUrl);
        lblStatus->setText("Lien copié dans le presse-papiers !");
    });
    connect(btnOpen, &QPushButton::clicked, this, [this]() {
        QDesktopServices::openUrl(QUrl(m_formUrl));
    });
    connect(btnClose, &QPushButton::clicked, this, &QDialog::accept);

    m_manager = new QNetworkAccessManager(this);
    connect(m_manager, &QNetworkAccessManager::finished,
            this, &QrFournisseurDialog::onQrDownloaded);
}

void QrFournisseurDialog::loadQrCode()
{
    // API QR Server — gratuite, pas de clé requise
    QString qrApiUrl = QString("https://api.qrserver.com/v1/create-qr-code/?size=250x250&data=%1")
                       .arg(QString::fromUtf8(QUrl::toPercentEncoding(m_formUrl)));

    m_manager->get(QNetworkRequest(QUrl(qrApiUrl)));
}

void QrFournisseurDialog::onQrDownloaded(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QPixmap pixmap;
        pixmap.loadFromData(reply->readAll());
        lblQrImage->setPixmap(pixmap);
        lblStatus->setText("Scannez ce QR code pour accéder au formulaire de commande pré-rempli");
    } else {
        lblQrImage->setText("Erreur de chargement\nVérifiez votre connexion");
        lblStatus->setText("Impossible de générer le QR code.");
    }
    reply->deleteLater();
}

void QrFournisseurDialog::applyStyles()
{
    setStyleSheet(
        "QDialog { background-color: #FAF5F0; }"
        "QLabel#dialogTitle { color: #291C0E; font-family: 'Times New Roman', serif; "
        "font-size: 18px; font-weight: bold; padding: 6px 0; }"
        "QLabel#infoLabel { color: #291C0E; font-size: 12px; background-color: #FFF8F0; "
        "border: 2px solid #BCAAA4; border-radius: 8px; padding: 10px; }"
        "QLabel#qrLabel { background-color: white; border: 3px solid #BCAAA4; "
        "border-radius: 8px; padding: 5px; }"
        "QLabel#noteLabel { color: #8D6E63; font-size: 11px; font-style: italic; }"
        "QLabel#linkLabel { color: #5D4037; font-size: 11px; }"
        "QLabel { color: #291C0E; font-family: Arial, sans-serif; font-size: 12px; }"
        "QPushButton#copyButton { background-color: #8D6E63; color: white; border: none; "
        "border-radius: 8px; padding: 10px 20px; font-size: 11px; font-weight: bold; }"
        "QPushButton#copyButton:hover { background-color: #A0826D; }"
        "QPushButton#openButton { background-color: #6D4C41; color: white; border: none; "
        "border-radius: 8px; padding: 10px 20px; font-size: 11px; font-weight: bold; }"
        "QPushButton#openButton:hover { background-color: #8D6E63; }"
        "QPushButton#cancelButton { background-color: #E7DDD1; color: #291C0E; "
        "border: 1px solid #BCAAA4; border-radius: 8px; padding: 10px 20px; "
        "font-size: 11px; font-weight: bold; }"
        "QPushButton#cancelButton:hover { background-color: #F0E6DA; }"
    );
}
