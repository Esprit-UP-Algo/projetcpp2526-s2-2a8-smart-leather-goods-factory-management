#ifndef EMPLOYEEDIALOG_H
#define EMPLOYEEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QRadioButton>

class EmployeeDialog : public QDialog
{
    Q_OBJECT

public:
    enum DialogMode {
        AddMode,
        EditMode,
        DeleteMode,
        ExportMode
    };

    explicit EmployeeDialog(QWidget *parent = nullptr, DialogMode mode = AddMode);
    ~EmployeeDialog();

    void setEmployeeData(const QString &id, const QString &matricule, const QString &nom,
                        const QString &prenom, const QString &cin, const QString &dateNaissance,
                        const QString &sexe, const QString &adresse, const QString &telephone,
                        const QString &email, const QString &poste, const QString &specialite,
                        const QString &departement, const QString &dateEmbauche, const QString &photoPath = "");

    // Getters pour récupérer les données saisies
    QString getMatricule() const;
    QString getNom() const;
    QString getPrenom() const;
    QString getCin() const;
    QDate getDateNaissance() const;
    QString getSexe() const;
    QString getAdresse() const;
    QString getTelephone() const;
    QString getEmail() const;
    QString getPoste() const;
    QString getSpecialite() const;
    QString getDepartement() const;
    QDate getDateEmbauche() const;
    QString getDeleteId() const { return deleteId; }
    QString getPhotoPath() const;

private slots:
    void onBrowsePhoto();

private:
    void setupAddEditUI();
    void setupDeleteUI();
    void setupExportUI();
    void applyStyles();
    void validateAndAccept(); // Validation des champs

    DialogMode mode;

    // Add/Edit fields
    QLineEdit *matriculeEdit;
    QLineEdit *nomEdit;
    QLineEdit *prenomEdit;
    QLineEdit *cinEdit;
    QDateEdit *dateNaissanceEdit;
    QComboBox *sexeCombo;
    QLineEdit *adresseEdit;
    QLineEdit *telephoneEdit;
    QLineEdit *emailEdit;
    QComboBox *posteCombo;
    QLineEdit *specialiteEdit;
    QComboBox *departementCombo;
    QDateEdit *dateEmbaucheEdit;
    QLabel *photoPreview;
    QPushButton *browsePhotoButton;
    QLineEdit *photoUrlEdit;

    // Delete fields
    QString deleteId;
    QString deleteMatricule;
    QString deleteNom;
    QString deletePrenom;
    QString deleteDepartement;
    QString deletePoste;

    // Export fields
    QComboBox *formatCombo;
    QLineEdit *fileNameEdit;
    QLineEdit *locationEdit;
    QCheckBox *chkID;
    QCheckBox *chkMatricule;
    QCheckBox *chkNom;
    QCheckBox *chkPrenom;
    QCheckBox *chkCIN;
    QCheckBox *chkDateNaissance;
    QCheckBox *chkDepartement;
    QCheckBox *chkPoste;
    QCheckBox *chkTelephone;
    QCheckBox *chkEmail;
    QRadioButton *radioAll;
    QRadioButton *radioSelected;
    QRadioButton *radioFiltered;
};

#endif // EMPLOYEEDIALOG_H
