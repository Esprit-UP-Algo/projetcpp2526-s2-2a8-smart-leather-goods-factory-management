#ifndef SMSFOURNISSEURDIALOG_H
#define SMSFOURNISSEURDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>

class SmsFournisseurDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SmsFournisseurDialog(const QString &nomEntreprise,
                                  const QString &telephone,
                                  QWidget *parent = nullptr);

    QString getMessage() const;
    QString getTelephone() const;

private slots:
    void onTemplateChanged(int index);
    void onSendClicked();
    void updateCharCount();

private:
    void setupUI();
    void applyStyles();

    QString m_nomEntreprise;
    QString m_telephone;

    QLineEdit  *txtTelephone;
    QTextEdit  *txtMessage;
    QLabel     *lblCharCount;
    QComboBox  *cmbTemplate;
    QPushButton *btnSend;
    QPushButton *btnCancel;
};

#endif // SMSFOURNISSEURDIALOG_H
