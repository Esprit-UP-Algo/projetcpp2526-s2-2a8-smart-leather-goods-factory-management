#ifndef CLIENTMANAGERDIALOG_H
#define CLIENTMANAGERDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QRadioButton>
#include "client.h"

class ClientManagerDialog : public QDialog
{
    Q_OBJECT

public:
    enum DialogMode {
        AddMode,
        EditMode,
        DeleteMode,
        ExportMode
    };

    explicit ClientManagerDialog(QWidget *parent = nullptr, DialogMode mode = AddMode);
    ~ClientManagerDialog();

    void setClientData(const QString &nom, const QString &prenom, const QString &sexe,
                       const QString &cin, const QString &pays, const QString &ville,
                       const QString &adresse, const QString &email);
    Client getClient() const;

private:
    void setupAddEditUI();
    void setupDeleteUI();
    void setupExportUI();
    void applyStyles();

    DialogMode mode;

    // Add/Edit fields
    QLineEdit *nomEdit;
    QLineEdit *prenomEdit;
    QComboBox *sexeCombo;
    QLineEdit *cinEdit;
    QLineEdit *paysEdit;
    QLineEdit *villeEdit;
    QLineEdit *adresseEdit;
    QLineEdit *emailEdit;
    QDateEdit *dateInscrit;

    // Delete fields
    QString deleteNom;
    QString deletePrenom;
    QString deleteSexe;
    QString deleteCIN;
    QString deletePays;
    QString deleteVille;

    // Export fields
    QComboBox *formatCombo;
    QLineEdit *fileNameEdit;
    QLineEdit *locationEdit;
    QCheckBox *chkNom;
    QCheckBox *chkPrenom;
    QCheckBox *chkSexe;
    QCheckBox *chkCIN;
    QCheckBox *chkPays;
    QCheckBox *chkVille;
    QCheckBox *chkAdresse;
    QCheckBox *chkEmail;
    QRadioButton *radioAll;
    QRadioButton *radioSelected;
    QRadioButton *radioFiltered;
};

#endif // CLIENTMANAGERDIALOG_H
