#ifndef STATSRH_H
#define STATSRH_H

#include <QDialog>
#include <QTabWidget>
#include <QLabel>
#include <QMap>

class StatsRHDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StatsRHDialog(QWidget *parent = nullptr);

private:
    void setupUI();
    void loadStatistics();
    void applyStyles();
    void animateKPIs();
    QString adjustColorBrightness(const QString &color, int amount);
    
    // Création des onglets
    QWidget* createDepartementTab();
    QWidget* createPosteTab();
    QWidget* createTableauTab();
    QWidget* createPariteTab();
    
    // Widgets KPI
    QLabel *totalLabel;
    QLabel *departementsLabel;
    QLabel *postesLabel;
    QLabel *hommesLabel;
    QLabel *femmesLabel;
    QLabel *pariteLabel;
    
    // Données
    QMap<QString, int> effectifDepartement;
    QMap<QString, int> effectifPoste;
    QMap<QString, int> effectifSexe;
    int totalEmployes;
    int nombreDepartements;
    int nombrePostes;
    
    QTabWidget *tabWidget;
};

// Widget personnalisé pour dessiner les graphiques
class GraphiqueWidget : public QWidget
{
    Q_OBJECT

public:
    enum TypeGraphique {
        Donut,
        Barres,
        Camembert
    };
    
    explicit GraphiqueWidget(TypeGraphique type, const QMap<QString, int> &data,
                            const QString &titre = "", QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void drawDonut(QPainter &painter);
    void drawBarres(QPainter &painter);
    void drawCamembert(QPainter &painter);
    
    TypeGraphique m_type;
    QMap<QString, int> m_data;
    QString m_titre;
    QList<QColor> m_colors;
};

#endif // STATSRH_H
