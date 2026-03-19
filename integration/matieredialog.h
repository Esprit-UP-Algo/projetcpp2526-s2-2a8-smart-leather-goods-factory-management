#ifndef MATIEREDIALOG_H
#define MATIEREDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QDateEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>

class MatiereDialog : public QDialog
{
    Q_OBJECT
public:
    enum DialogMode { AddMode, EditMode, DeleteMode };

    explicit MatiereDialog(QWidget *parent = nullptr, DialogMode mode = AddMode);
    ~MatiereDialog();

    void setMatiereData(const QString &module, const QString &reference, const QString &type,
                        const QString &quantite, const QString &seuil, const QString &dateExp,
                        const QString &photoUrl = "");

    QString getModule()         const;
    QString getReference()      const;
    QString getType()           const;
    QString getQuantite()       const;
    QString getSeuil()          const;
    QString getDateExpiration() const;
    QString getPhotoUrl()       const;

    // Setters individuels pour modifications vocales
    void setModule(const QString &m)    { if (txtModule) txtModule->setText(m); }
    void setReference(const QString &r) { if (txtReference) txtReference->setText(r); }
    void setTypeMatiere(const QString &t) { 
        if (cmbType) {
            int idx = cmbType->findText(t, Qt::MatchContains);
            if (idx >= 0) cmbType->setCurrentIndex(idx);
            else cmbType->setCurrentText(t);
        }
    }
    void setQuantite(const QString &q)  { if (txtQuantite) txtQuantite->setText(q); }
    void setSeuil(const QString &s)     { if (txtSeuil) txtSeuil->setText(s); }

private slots:
    void onSaveClicked();
    void onDeleteConfirmed();
    void onSelectPhoto();

private:
    void setupUI();
    void updatePhotoPreview();

    DialogMode  m_mode;
    QString     m_photoUrl;
    QLabel      *lblTitle, *lblDeleteWarning, *lblPhotoPreview;
    QLineEdit   *txtModule, *txtReference, *txtQuantite, *txtSeuil;
    QComboBox   *cmbType;
    QDateEdit   *dateExpiration;
    QPushButton *btnSave, *btnCancel, *btnDelete, *btnSelectPhoto;
};

#endif // MATIEREDIALOG_H