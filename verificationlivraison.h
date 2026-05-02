#ifndef VERIFICATIONLIVRAISON_H
#define VERIFICATIONLIVRAISON_H

#include <QDialog>
#include <QLabel>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QCheckBox>

class Arduino;

class VerificationLivraison : public QDialog
{
    Q_OBJECT

public:
    explicit VerificationLivraison(const QString &fournisseurNom, Arduino *arduino, QWidget *parent = nullptr);
    ~VerificationLivraison();

    double getQuantiteCommandee() const;
    bool isArduinoConnected() const;
    double getMeasuredWeight() const { return m_measuredWeight; }
    bool isDeliveryValid() const { return m_isValid; }

private slots:
    void onDemarrerMesure();
    void onAnnuler();
    void onWeightReceived(double kg);

private:
    void setupUI();

    QString m_fournisseurNom;
    Arduino *m_arduino;
    double m_measuredWeight = 0.0;
    bool m_isValid = false;
    
    QLabel *lblTitle;
    QLabel *lblMatiere;
    QLabel *lblQuantite;
    QDoubleSpinBox *spinQuantite;
    QCheckBox *chkArduino;
    QPushButton *btnDemarrer;
    QPushButton *btnAnnuler;
    QLabel *lblStatus;
};

#endif // VERIFICATIONLIVRAISON_H
