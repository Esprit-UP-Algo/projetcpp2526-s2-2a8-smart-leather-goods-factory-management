#ifndef PRODUCTDIALOG_H
#define PRODUCTDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QTextEdit>
#include <QPushButton>
#include <QFrame>
#include <QScrollArea>
#include <QGroupBox>

class ProductDialog : public QDialog
{
    Q_OBJECT

public:
    enum DialogMode {
        AddMode,
        EditMode,
        DeleteMode,
        ViewMode,
        StatisticsMode
    };

    explicit ProductDialog(QWidget *parent = nullptr, DialogMode mode = AddMode);
    ~ProductDialog();

    void setProductData(const QString &id, const QString &ref, const QString &nom,
                       const QString &categorie, const QString &type, const QString &couleur,
                       const QString &dimensions, double prix, double cout, int stock,
                       const QString &statut, const QString &description, const QString &dateCreation);
    
    void setStatisticsData(int total, int disponible, int enProduction, int obsolete, 
                          double valeurStock, const QString &plusDemande);

private:
    DialogMode currentMode;
    
    // Main layout
    QVBoxLayout *mainLayout;
    QFrame *contentFrame;
    QVBoxLayout *contentLayout;
    
    // Title
    QLabel *titleLabel;
    
    // Form widgets
    QScrollArea *scrollArea;
    QWidget *scrollWidget;
    QFormLayout *formLayout;
    
    QLabel *idLabel;
    QLabel *idValue;
    QLineEdit *referenceEdit;
    QLineEdit *nomEdit;
    QComboBox *categorieCombo;
    QLineEdit *typeEdit;
    QLineEdit *couleurEdit;
    QLineEdit *dimensionsEdit;
    QDoubleSpinBox *prixSpin;
    QDoubleSpinBox *coutSpin;
    QSpinBox *stockSpin;
    QComboBox *statutCombo;
    QTextEdit *descriptionEdit;
    
    // Delete mode labels
    QLabel *refValue;
    QLabel *nomValue;
    QLabel *prixValue;
    QLabel *warningLabel;
    QLabel *confirmLabel;
    
    // Statistics labels
    QLabel *statTotalValue = nullptr;
    QLabel *statDisponibleValue = nullptr;
    QLabel *statProductionValue = nullptr;
    QLabel *statObsoleteValue = nullptr;
    QLabel *statValeurValue = nullptr;
    QLabel *statDemandeValue = nullptr;
    
    // Buttons
    QHBoxLayout *buttonLayout;
    QPushButton *confirmButton;
    QPushButton *cancelButton;
    
    void setupAddMode();
    void setupEditMode();
    void setupDeleteMode();
    void setupViewMode();
    void setupStatisticsMode();
    
    void applyStyles();
    void generateReference();

private slots:
    void onConfirmClicked();
    void onCancelClicked();
};

#endif // PRODUCTDIALOG_H
