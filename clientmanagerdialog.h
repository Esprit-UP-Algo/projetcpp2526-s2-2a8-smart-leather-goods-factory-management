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

    // Remplir les champs depuis un client existant (Edit / Delete)
    void setClientData(const QString &nom, const QString &prenom, const QString &sexe,
                       const QString &cin, const QString &pays, const QString &ville,
                       const QString &adresse, const QString &email);

    // Récupère un Client construit à partir des champs du formulaire
    Client getClient() const;

    // Getters individuels (même pattern que EmployeeDialog)
    QString getNom()             const { return nomEdit     ? nomEdit->text().trimmed()     : ""; }
    QString getPrenom()          const { return prenomEdit  ? prenomEdit->text().trimmed()  : ""; }
    QString getSexe()            const { return sexeCombo   ? sexeCombo->currentText()      : ""; }
    QString getCin()             const { return cinEdit     ? cinEdit->text().trimmed()     : ""; }
    QString getPays()            const { return paysEdit    ? paysEdit->text().trimmed()    : ""; }
    QString getVille()           const { return villeEdit   ? villeEdit->text().trimmed()   : ""; }
    QString getAdresse()         const { return adresseEdit ? adresseEdit->text().trimmed() : ""; }
    QString getEmail()           const { return emailEdit   ? emailEdit->text().trimmed()   : ""; }
    QString getDateInscription() const { return dateInscrit
                                                    ? dateInscrit->date().toString("yyyy-MM-dd") : ""; }

    // Permet de passer l'id lors de l'édition ou suppression
    void setEditingId(int id) { editingId = id; }
    void setDeleteId(int id)  { deleteId  = id; }

private slots:
    void onAccepted();

private:
    void setupAddEditUI();
    void setupDeleteUI();
    void setupExportUI();
    void applyStyles();

    DialogMode mode;

    // ── Champs Add / Edit ──────────────────────────────────────
    QLineEdit *nomEdit;
    QLineEdit *prenomEdit;
    QComboBox *sexeCombo;
    QLineEdit *cinEdit;
    QLineEdit *paysEdit;
    QLineEdit *villeEdit;
    QLineEdit *adresseEdit;
    QLineEdit *emailEdit;
    QDateEdit *dateInscrit;       // mapped → client.date_inscription

    // ── Données conservées pour le mode Delete ─────────────────
    QString deleteNom;
    QString deletePrenom;
    QString deleteSexe;
    QString deleteCIN;
    QString deletePays;
    QString deleteVille;

    // ── Champs Export ──────────────────────────────────────────
    QComboBox  *formatCombo;
    QLineEdit  *fileNameEdit;
    QLineEdit  *locationEdit;
    QCheckBox  *chkNom;
    QCheckBox  *chkPrenom;
    QCheckBox  *chkSexe;
    QCheckBox  *chkCIN;
    QCheckBox  *chkPays;
    QCheckBox  *chkVille;
    QCheckBox  *chkAdresse;
    QCheckBox  *chkEmail;
    QRadioButton *radioAll;
    QRadioButton *radioSelected;
    QRadioButton *radioFiltered;

    // ── IDs ────────────────────────────────────────────────────
    int deleteId;     // id du client à supprimer
    int editingId;    // id du client en cours de modification
};

#endif // CLIENTMANAGERDIALOG_H