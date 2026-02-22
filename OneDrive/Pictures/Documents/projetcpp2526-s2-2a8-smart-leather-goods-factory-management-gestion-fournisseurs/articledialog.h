#ifndef ARTICLEDIALOG_H
#define ARTICLEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLabel>

class ArticleDialog : public QDialog
{
    Q_OBJECT

public:
    enum Mode {
        AddMode,
        EditMode,
        DeleteMode,
        ViewMode
    };

    explicit ArticleDialog(QWidget *parent = nullptr, Mode mode = AddMode);
    ~ArticleDialog();

    void setArticleData(const QString &ref, const QString &nom, const QString &cat,
                       const QString &type, const QString &couleur, const QString &dim,
                       double prix, double cout, const QString &statut);

    QString getReference() const;
    QString getNom() const;
    QString getCategorie() const;
    QString getType() const;
    QString getCouleur() const;
    QString getDimensions() const;
    double getPrixUnitaire() const;
    double getCoutFabrication() const;
    QString getStatut() const;

private slots:
    void onSave();
    void onCancel();

private:
    Mode dialogMode;
    
    QLineEdit *lineEditReference;
    QLineEdit *lineEditNom;
    QComboBox *comboBoxCategorie;
    QLineEdit *lineEditType;
    QLineEdit *lineEditCouleur;
    QLineEdit *lineEditDimensions;
    QDoubleSpinBox *spinBoxPrix;
    QDoubleSpinBox *spinBoxCout;
    QComboBox *comboBoxStatut;
    
    void setupUI();
    void applyStyles();
};

#endif // ARTICLEDIALOG_H
