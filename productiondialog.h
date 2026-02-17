#ifndef PRODUCTIONDIALOG_H
#define PRODUCTIONDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QDateEdit>

class ProductionDialog : public QDialog
{
    Q_OBJECT
public:
    enum DialogMode { AddMode, EditMode, DeleteMode };

    explicit ProductionDialog(QWidget *parent = nullptr, DialogMode mode = AddMode);
    ~ProductionDialog();

    void setProductionData(const QString &id, const QString &reference, const QString &produit,
                           const QString &quantite, const QString &statut, const QString &dateDebut,
                           const QString &dateFin, const QString &responsable, const QString &priorite);

    QString getId()          const;
    QString getReference()   const;
    QString getProduit()     const;
    QString getQuantite()    const;
    QString getStatut()      const;
    QString getDateDebut()   const;
    QString getDateFin()     const;
    QString getResponsable() const;
    QString getPriorite()    const;

private slots:
    void onSaveClicked();
    void onDeleteConfirmed();

private:
    void setupUI();

    DialogMode   m_mode;
    QLabel      *lblTitle, *lblDeleteWarning;
    QLineEdit   *txtId, *txtReference, *txtQuantite;
    QComboBox   *cmbProduit, *cmbStatut, *cmbResponsable, *cmbPriorite;
    QDateEdit   *dateDebut, *dateFin;
    QPushButton *btnSave, *btnCancel, *btnDelete;
};

#endif // PRODUCTIONDIALOG_H