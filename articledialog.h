#ifndef ARTICLEDIALOG_H
#define ARTICLEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QLabel>
#include <QSlider>
#include <QPushButton>
#include <QProgressBar>

class Tripo3DGenerator;

class ArticleDialog : public QDialog
{
    Q_OBJECT

public:
    enum Mode { AddMode, EditMode, DeleteMode, ViewMode };

    explicit ArticleDialog(QWidget *parent = nullptr, Mode mode = AddMode);
    ~ArticleDialog();

    void setArticleData(const QString &ref, const QString &nom, const QString &cat,
                        const QString &type, const QString &modele3d,
                        int cr, int cg, int cb,
                        double largeur, double hauteur, double profondeur,
                        double prix, double cout, const QString &statut);

    QString getReference() const;
    QString getNom() const;
    QString getCategorie() const;
    QString getType() const;
    QString getModele3D() const;
    int getCouleurR() const;
    int getCouleurG() const;
    int getCouleurB() const;
    double getLargeur() const;
    double getHauteur() const;
    double getProfondeur() const;
    double getPrixUnitaire() const;
    double getCoutFabrication() const;
    QString getStatut() const;
    QString getPhotoPath() const;

private slots:
    void onSave();
    void onCancel();
    void onTypeChanged(const QString &type);
    void updateColorPreview();
    void onGenerate3D();

private:
    Mode dialogMode;

    QLineEdit *lineEditReference;
    QLineEdit *lineEditNom;
    QComboBox *comboBoxCategorie;
    QComboBox *comboBoxType;
    QLineEdit *lineEditModele3D;
    QSlider *sliderR, *sliderG, *sliderB;
    QLabel *lblColorPreview;
    QLabel *lblRVal, *lblGVal, *lblBVal;
    QDoubleSpinBox *spinLargeur, *spinHauteur, *spinProfondeur;
    QDoubleSpinBox *spinBoxPrix;
    QDoubleSpinBox *spinBoxCout;
    QComboBox *comboBoxStatut;

    // Tripo3D
    QPushButton *btnGenerate3D;
    QProgressBar *progress3D;
    QLabel *lblPreview3D;
    QLabel *lblStatus3D;
    Tripo3DGenerator *m_tripoGen;
    QString m_photoPath;

    void setupUI();
    void applyStyles();
};

#endif // ARTICLEDIALOG_H
