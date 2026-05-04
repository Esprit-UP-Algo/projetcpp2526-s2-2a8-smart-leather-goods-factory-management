#ifndef QRFOURNISSEURDIALOG_H
#define QRFOURNISSEURDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>

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
    void onSaveClicked();
    void onPrintClicked();

private:
    void setupUI();
    void generateQrCode();
    QPixmap buildQrPixmap(const QString &data, int size);

    QString m_nomEntreprise;
    QString m_email;
    QString m_telephone;
    QString m_typeProduit;
    QString m_qrData;

    QLabel      *lblQrImage;
    QLabel      *lblInfo;
    QPushButton *btnSave;
    QPushButton *btnPrint;
    QPushButton *btnClose;
};

#endif // QRFOURNISSEURDIALOG_H
