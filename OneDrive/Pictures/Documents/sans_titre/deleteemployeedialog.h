#ifndef DELETEEMPLOYEEDIALOG_H
#define DELETEEMPLOYEEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QPushButton>
#include <QLabel>

class DeleteEmployeeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DeleteEmployeeDialog(QWidget *parent = nullptr);
    ~DeleteEmployeeDialog();

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
    QPushButton *deleteButton;
    QPushButton *cancelButton;
};

#endif
