#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QTimer>
#include "pythonfacerecognition.h"
#include "simplefacecapture.h"

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();
    
    QString getUsername() const;
    QString getPassword() const;
    bool isRememberMe() const;

private slots:
    void onLoginClicked();
    void onFaceLoginClicked();

private:
    void setupUI();
    bool validateCredentials(const QString &username, const QString &password);
    
    QLineEdit *txtUsername;
    QLineEdit *txtPassword;
    QCheckBox *chkRememberMe;
    QPushButton *btnLogin;
    QPushButton *btnFaceLogin;
    QLabel *lblError;
    QLabel *lblTitle;
    QLabel *lblLogo;
    
    QString m_username;
    QString m_password;
};

#endif // LOGINDIALOG_H
