#ifndef METIERDIALOG_H
#define METIERDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QTextEdit>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>

class MetierDialog : public QDialog
{
    Q_OBJECT

public:
    enum DialogMode { AddMode, EditMode };

    explicit MetierDialog(QWidget *parent = nullptr, DialogMode mode = AddMode);
    ~MetierDialog() = default;

    void setMetierData(int id, const QString &code, const QString &libelle,
                       const QString &description, const QString &domaine,
                       const QString &niveau, double salaireBase, bool actif);

    // Getters
    int     getId()                const;
    QString getCode()              const;
    QString getLibelle()           const;
    QString getDescription()       const;
    QString getDomaine()           const;
    QString getNiveauQualification() const;
    double  getSalaireBase()       const;
    bool    isActif()              const;

private slots:
    void onValidate();

private:
    void buildUI(DialogMode mode);
    bool validateInputs();

    int          m_id = 0;
    QLineEdit   *m_code;
    QLineEdit   *m_libelle;
    QTextEdit   *m_description;
    QComboBox   *m_domaine;
    QComboBox   *m_niveau;
    QDoubleSpinBox *m_salaireBase;
    QCheckBox   *m_actif;
    QLabel      *m_errorLabel;
};

#endif // METIERDIALOG_H
