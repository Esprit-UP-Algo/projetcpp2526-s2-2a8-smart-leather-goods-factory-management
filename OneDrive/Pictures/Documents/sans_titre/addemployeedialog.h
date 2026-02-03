#ifndef ADDEMPLOYEEDIALOG_H
#define ADDEMPLOYEEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QPushButton>
#include <QLabel>

class AddEmployeeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddEmployeeDialog(QWidget *parent = nullptr);
    ~AddEmployeeDialog();

private:
    void setupUI();
    void applyStyles();

    QLineEdit *matriculeEdit;
    QLineEdit *nomEdit;
    QLineEdit *prenomEdit;
    QLineEdit *cinEdit;
    QDateEdit *dateNaissanceEdit;
    QComboBox *sexeCombo;
    QLineEdit *adresseEdit;
    QLineEdit *telephoneEdit;
    QLineEdit *emailEdit;
    QLineEdit *posteEdit;
    QLineEdit *specialiteEdit;
    QComboBox *departementCombo;
    QDateEdit *dateEmbaucheEdit;
    QLabel *photoPreview;
    QLineEdit *photoUrlEdit;
    QPushButton *browsePhotoButton;
    QPushButton *saveButton;
    QPushButton *cancelButton;
};

#endif // ADDEMPLOYEEDIALOG_H
