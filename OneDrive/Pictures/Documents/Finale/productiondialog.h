#ifndef PRODUCTIONDIALOG_H
#define PRODUCTIONDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QDateEdit>
#include "production.h"

class ProductionDialog : public QDialog
{
    Q_OBJECT

public:
    enum DialogMode {
        AddMode,
        EditMode,
        DeleteMode
    };

    explicit ProductionDialog(QWidget *parent = nullptr, DialogMode mode = AddMode);
    ~ProductionDialog();

    void setProductionData(const QString &id, const QString &reference, const QString &produit,
                          const QString &quantite, const QString &statut, const QString &dateDebut,
                          const QString &dateFin, const QString &responsable, const QString &priorite);

    QString getId() const;
    QString getReference() const;
    QString getProduit() const;
    QString getQuantite() const;
    QString getStatut() const;
    QString getDateDebut() const;
    QString getDateFin() const;
    QString getResponsable() const;
    QString getPriorite() const;

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
    QLabel *lblReference;
    QLabel *lblProduit;
    QLabel *lblQuantite;
    QLabel *lblStatut;
    QLabel *lblDateDebut;
    QLabel *lblDateFin;
    QLabel *lblResponsable;
    QLabel *lblPriorite;
    
    QLineEdit *txtId;
    QLineEdit *txtReference;
    QComboBox *cmbProduit;
    QLineEdit *txtQuantite;
    QComboBox *cmbStatut;
    QDateEdit *dateDebut;
    QDateEdit *dateFin;
    QComboBox *cmbResponsable;
    QComboBox *cmbPriorite;
    
    QPushButton *btnSave;
    QPushButton *btnCancel;
    QPushButton *btnDelete;
    
    QLabel *lblDeleteWarning;
};

#endif // PRODUCTIONDIALOG_H
