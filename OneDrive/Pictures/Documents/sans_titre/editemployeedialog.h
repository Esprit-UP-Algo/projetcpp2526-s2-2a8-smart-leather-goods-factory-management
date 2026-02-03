#ifndef EDITEMPLOYEEDIALOG_H
#define EDITEMPLOYEEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QPushButton>
#include <QLabel>

class EditEmployeeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditEmployeeDialog(QWidget *parent = nullptr);
    ~EditEmployeeDialog();

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

#endif // EDITEMPLOYEEDIALOG_H
