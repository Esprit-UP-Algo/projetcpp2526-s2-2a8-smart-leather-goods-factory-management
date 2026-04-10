#ifndef BILANDIALOG_H
#define BILANDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QTableWidget>
#include <QGroupBox>
#include <QPushButton>
#include <QScrollArea>
#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>

// On stocke les chart views comme QWidget* dans le .h
// pour éviter tout problème de namespace/type incomplet avec QtCharts.
// Le cast vers QChartView se fait uniquement dans le .cpp.

class BilanDialog : public QDialog
{
    Q_OBJECT
public:
    explicit BilanDialog(QWidget *parent = nullptr);

private slots:
    void onPeriodChanged();
    void exportCSV();
    void fetchTauxChange();
    void onTauxReceived(QNetworkReply *reply);

private:
    void setupUI();
    void loadData();

    QLabel *m_lblCA          = nullptr;
    QLabel *m_lblCA_EUR      = nullptr;
    QLabel *m_lblCA_USD      = nullptr;
    QLabel *m_lblBestProduct = nullptr;
    QLabel *m_lblBenefice    = nullptr;
    QLabel *m_lblMarge       = nullptr;
    QLabel *m_lblCommandes   = nullptr;
    QLabel *m_lblDate        = nullptr;

    QNetworkAccessManager *m_networkManager = nullptr;
    double  m_totalCA = 0.0;

    // Stockés comme QWidget* — castés en QChartView* dans le .cpp
    QWidget *m_pieView        = nullptr;
    QWidget *m_barView        = nullptr;
    QWidget *m_splineView     = nullptr;
    QWidget *m_produitView    = nullptr;
    QWidget *m_regionView     = nullptr;

    QComboBox    *m_comboPeriod = nullptr;

    QGroupBox *makeKpiBox(const QString &title, QLabel *&valLabel, bool highlight = false);
    QWidget   *makePieChart();
    QWidget   *makeBarChart();
    QWidget   *makeSplineChart();
    QWidget   *makeHBarChart(const QString &title);
    void       refreshHBar(QWidget *view, const QMap<QString,double> &data);
};

#endif // BILANDIALOG_H
