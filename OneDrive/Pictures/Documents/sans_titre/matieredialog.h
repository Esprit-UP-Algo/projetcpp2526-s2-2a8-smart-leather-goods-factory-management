#ifndef MATIEREDIALOG_H
#define MATIEREDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QDateEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>

class MatiereDialog : public QDialog
{
    Q_OBJECT

public:
    enum DialogMode {
        AddMode,
        EditMode,
        DeleteMode
    };

    explicit MatiereDialog(QWidget *parent = nullptr, DialogMode mode = AddMode);
    ~MatiereDialog();

    void setMatiereData(const QString &module, const QString &reference, const QString &type,
                       const QString &quantite, const QString &seuil, const QString &dateExp);

    QString getModule() const;
    QString getReference() const;
    QString getType() const;
    QString getQuantite() const;
    QString getSeuil() const;
    QString getDateExpiration() const;

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
    QLabel *lblModule;
    QLabel *lblReference;
    QLabel *lblType;
    QLabel *lblQuantite;
    QLabel *lblSeuil;
    QLabel *lblDateExp;
    
    QLineEdit *txtModule;
    QLineEdit *txtReference;
    QComboBox *cmbType;
    QLineEdit *txtQuantite;
    QLineEdit *txtSeuil;
    QDateEdit *dateExpiration;
    
    QPushButton *btnSave;
    QPushButton *btnCancel;
    QPushButton *btnDelete;
    
    QLabel *lblDeleteWarning;
};

#endif // MATIEREDIALOG_H
