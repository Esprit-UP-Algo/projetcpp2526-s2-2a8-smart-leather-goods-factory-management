#ifndef FOURNISSEURDIALOG_H
#define FOURNISSEURDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include "fournisseur.h"

class FournisseurDialog : public QDialog
{
    Q_OBJECT

public:
    enum DialogMode {
        AddMode,
        EditMode,
        DeleteMode
    };

    explicit FournisseurDialog(QWidget *parent = nullptr, DialogMode mode = AddMode);
    ~FournisseurDialog();

    void setFournisseurData(const QString &id, const QString &nomEntreprise, const QString &email,
                           const QString &telephone, const QString &typeProduit,
                           const QString &conditionPaiement, const QString &matriculeFiscal,
                           const QString &statut, const QString &adresse = QString());

    QString getId() const;
    QString getNomEntreprise() const;
    QString getEmail() const;
    QString getTelephone() const;
    QString getTypeProduit() const;
    QString getConditionPaiement() const;
    QString getMatriculeFiscal() const;
    QString getStatut() const;
    QString getAdresse() const;

private slots:
    void onSaveClicked();
    void onCancelClicked();
    void onDeleteConfirmed();

private:
    void setupUI();
    void setupAddMode();
    void setupEditMode();
    void setupDeleteMode();

    DialogMode m_mode;
    
    // UI Elements
    QLabel *lblTitle;
    QLabel *lblId;
    QLabel *lblNomEntreprise;
    QLabel *lblEmail;
    QLabel *lblTelephone;
    QLabel *lblTypeProduit;
    QLabel *lblConditionPaiement;
    QLabel *lblMatriculeFiscal;
    QLabel *lblStatut;
    
    QLineEdit *txtId;
    QLineEdit *txtNomEntreprise;
    QLineEdit *txtEmail;
    QLineEdit *txtTelephone;
    QComboBox *cmbTypeProduit;
    QComboBox *cmbConditionPaiement;
    QLineEdit *txtMatriculeFiscal;
    QComboBox *cmbStatut;
    QLineEdit *txtAdresse;
    
    QPushButton *btnSave;
    QPushButton *btnCancel;
    QPushButton *btnDelete;
    
    QLabel *lblDeleteWarning;
};

#endif // FOURNISSEURDIALOG_H
