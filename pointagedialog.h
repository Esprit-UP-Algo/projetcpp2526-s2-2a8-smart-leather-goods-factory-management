#ifndef POINTAGEDIALOG_H
#define POINTAGEDIALOG_H

#include <QDialog>
#include <QTableView>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include "pointage.h"

class PointageDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PointageDialog(int idEmploye,
                            const QString &nomEmploye,
                            QWidget *parent = nullptr);
    ~PointageDialog();

private slots:
    void rafraichirCalendrier();
    void afficherSalaire();

private:
    void setupUI();
    void applyStyles();
    
    int     m_idEmploye;
    QString m_nomEmploye;
    Pointage m_pointage;
    
    QTableView  *m_tableView;
    QLabel      *m_lblTitre;
    QLabel      *m_lblSalaire;
    QComboBox   *m_comboMois;
    QSpinBox    *m_spinAnnee;
    QPushButton *m_btnRefresh;
    QPushButton *m_btnSalaire;
    QPushButton *m_btnFermer;
};

#endif // POINTAGEDIALOG_H
