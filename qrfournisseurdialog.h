#ifndef QRFOURNISSEURDIALOG_H
#define QRFOURNISSEURDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class QrFournisseurDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QrFournisseurDialog(const QString &nomEntreprise,
                                 const QString &email,
                                 const QString &telephone,
                                 const QString &typeProduit,
                                 QWidget *parent = nullptr);

private slots:
    void onQrDownloaded(QNetworkReply *reply);

private:
    void setupUI();
    void loadQrCode();
    void applyStyles();

    QString m_nomEntreprise;
    QString m_email;
    QString m_telephone;
    QString m_typeProduit;
    QString m_formUrl;

    QLabel *lblQrImage;
    QLabel *lblUrl;
    QLabel *lblStatus;
    QNetworkAccessManager *m_manager;
};

#endif // QRFOURNISSEURDIALOG_H
