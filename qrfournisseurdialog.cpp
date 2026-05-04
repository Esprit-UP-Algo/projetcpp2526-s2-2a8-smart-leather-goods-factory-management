#include "qrfournisseurdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QPrinter>
#include <QTextDocument>
#include <QPixmap>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QEventLoop>
#include <QDir>
#include <QPageSize>

QrFournisseurDialog::QrFournisseurDialog(const QString &nomEntreprise,
                                         const QString &email,
                                         const QString &telephone,
                                         const QString &typeProduit,
                                         QWidget *parent)
    : QDialog(parent)
    , m_nomEntreprise(nomEntreprise)
    , m_email(email)
    , m_telephone(telephone)
    , m_typeProduit(typeProduit)
{
    m_qrData = QString("BEGIN:VCARD\nVERSION:3.0\nFN:%1\nTEL:%2\nEMAIL:%3\nNOTE:%4\nEND:VCARD")
                   .arg(nomEntreprise, telephone, email, typeProduit);

    setupUI();
    generateQrCode();
    setWindowTitle("QR Code - " + nomEntreprise);
    setMinimumSize(420, 520);
}

void QrFournisseurDialog::setupUI()
{
    setStyleSheet(
        "QDialog { background-color: #FAF5F0; }"
        "QLabel { color: #291C0E; font-family: Arial, sans-serif; font-size: 12px; }"
        "QPushButton { background-color: #8D6E63; color: white; border: none; "
        "border-radius: 8px; padding: 10px 22px; font-size: 12px; font-weight: bold; }"
        "QPushButton:hover { background-color: #A0826D; }"
        "QPushButton#btnClose { background-color: #E7DDD1; color: #291C0E; "
        "border: 1px solid #BCAAA4; }"
        "QPushButton#btnClose:hover { background-color: #F0E6DA; }"
    );

    QVBoxLayout *lay = new QVBoxLayout(this);
    lay->setContentsMargins(28, 24, 28, 24);
    lay->setSpacing(16);

    QLabel *title = new QLabel("QR Code Fournisseur");
    title->setStyleSheet("font-size: 18px; font-weight: bold; color: #291C0E;");
    title->setAlignment(Qt::AlignCenter);
    lay->addWidget(title);

    lblInfo = new QLabel(
        QString("<b>%1</b><br>Tel: %2<br>Email: %3<br>Type: %4")
        .arg(m_nomEntreprise, m_telephone, m_email, m_typeProduit));
    lblInfo->setStyleSheet(
        "background: #FFF8F0; border: 2px solid #BCAAA4; border-radius: 8px; padding: 12px;");
    lblInfo->setAlignment(Qt::AlignCenter);
    lay->addWidget(lblInfo);

    lblQrImage = new QLabel("Generation du QR code...");
    lblQrImage->setAlignment(Qt::AlignCenter);
    lblQrImage->setMinimumSize(250, 250);
    lblQrImage->setStyleSheet(
        "background: white; border: 2px solid #BCAAA4; border-radius: 8px;");
    lay->addWidget(lblQrImage, 1);

    lay->addStretch();

    QHBoxLayout *btnLay = new QHBoxLayout();
    btnLay->setSpacing(10);

    btnSave  = new QPushButton("Enregistrer");
    btnPrint = new QPushButton("Imprimer");
    btnClose = new QPushButton("Fermer");
    btnClose->setObjectName("btnClose");

    btnLay->addWidget(btnSave);
    btnLay->addWidget(btnPrint);
    btnLay->addStretch();
    btnLay->addWidget(btnClose);
    lay->addLayout(btnLay);

    connect(btnSave,  &QPushButton::clicked, this, &QrFournisseurDialog::onSaveClicked);
    connect(btnPrint, &QPushButton::clicked, this, &QrFournisseurDialog::onPrintClicked);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::accept);
}

void QrFournisseurDialog::generateQrCode()
{
    QString encoded = QUrl::toPercentEncoding(m_qrData);
    QString urlStr  = "https://api.qrserver.com/v1/create-qr-code/?size=250x250&data=" + encoded;

    QNetworkAccessManager mgr;
    QNetworkReply *reply = mgr.get(QNetworkRequest(QUrl(urlStr)));

    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() == QNetworkReply::NoError) {
        QPixmap pix;
        pix.loadFromData(reply->readAll());
        lblQrImage->setPixmap(pix.scaled(250, 250, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        lblQrImage->setPixmap(buildQrPixmap(m_nomEntreprise, 250));
    }
    reply->deleteLater();
}

QPixmap QrFournisseurDialog::buildQrPixmap(const QString &data, int size)
{
    QPixmap pix(size, size);
    pix.fill(Qt::white);
    QPainter p(&pix);
    p.setPen(Qt::NoPen);
    int cell = size / 21;
    uint hash = qHash(data);
    for (int row = 0; row < 21; ++row) {
        for (int col = 0; col < 21; ++col) {
            bool dark = (hash >> ((row * 21 + col) % 32)) & 1;
            p.setBrush(dark ? Qt::black : Qt::white);
            p.drawRect(col * cell, row * cell, cell, cell);
        }
    }
    return pix;
}

void QrFournisseurDialog::onSaveClicked()
{
    QString fn = QFileDialog::getSaveFileName(
        this, "Enregistrer QR Code",
        QDir::homePath() + "/QR_" + m_nomEntreprise + ".png",
        "Images PNG (*.png);;Images JPEG (*.jpg)");
    if (fn.isEmpty()) return;

    QPixmap pix = lblQrImage->pixmap();
    if (!pix.isNull()) {
        pix.save(fn);
        QMessageBox::information(this, "Succes", "QR Code enregistre :\n" + fn);
    }
}

void QrFournisseurDialog::onPrintClicked()
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::NativeFormat);
    printer.setPageSize(QPageSize::A4);

    QPixmap pix = lblQrImage->pixmap();
    if (pix.isNull()) return;

    QPainter painter(&printer);
    QRect rect = painter.viewport();
    QSize sz = pix.size();
    sz.scale(rect.size() / 2, Qt::KeepAspectRatio);
    painter.setViewport(rect.x() + (rect.width() - sz.width()) / 2,
                        rect.y() + 100, sz.width(), sz.height());
    painter.setWindow(pix.rect());
    painter.drawPixmap(0, 0, pix);
}
