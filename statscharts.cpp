#include "statscharts.h"
#include <QtCharts>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QPieSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QBarCategoryAxis>


QWidget* StatsCharts::createStatsWindow(const QMap<QString,int>& stats,
                                        const QMap<QString,int>& monthlyStats)
{
    QWidget *window = new QWidget();
    window->setWindowTitle("Statistiques des clients");
    window->resize(1200, 800);

    QVBoxLayout *layout = new QVBoxLayout(window);

    // =========================
    // ===== PREP PIE DATA =====
    // =========================
    QList<QPair<QString,int>> list;
    int total = 0;

    for(auto it = stats.begin(); it != stats.end(); ++it)
    {
        list.append(qMakePair(it.key(), it.value()));
        total += it.value();
    }

    std::sort(list.begin(), list.end(), [](auto a, auto b){
        return a.second > b.second;
    });

    // ===== PIE CHART =====
    QPieSeries *pieSeries = new QPieSeries();

    int limit = 5;
    int others = 0;

    for(int i = 0; i < list.size(); i++)
    {
        if(i < limit)
            pieSeries->append(list[i].first, list[i].second);
        else
            others += list[i].second;
    }

    if(others > 0)
        pieSeries->append("Others", others);

    QList<QColor> colors = {
        QColor("#4CAF50"), QColor("#2196F3"), QColor("#FF9800"),
        QColor("#E91E63"), QColor("#9C27B0"), QColor("#009688")
    };

    QChart *pieChart = new QChart();
    pieChart->setTitle(QString("Clients par ville (Total: %1)").arg(total));
    pieChart->legend()->setAlignment(Qt::AlignRight);
    pieChart->setAnimationOptions(QChart::SeriesAnimations);

    // =========================
    // ===== BAR CHART =====
    // =========================
    QBarSeries *barSeries = new QBarSeries();
    QBarSet *set = new QBarSet("Clients");

    *set << 0;
    barSeries->append(set);

    QChart *barChart = new QChart();
    barChart->addSeries(barSeries);
    barChart->setTitle("D├®tails de la r├®gion");

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(QStringList() << "Hover");

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, 1);

    barChart->addAxis(axisX, Qt::AlignBottom);
    barSeries->attachAxis(axisX);

    barChart->addAxis(axisY, Qt::AlignLeft);
    barSeries->attachAxis(axisY);

    QChartView *barView = new QChartView(barChart);
    barView->setRenderHint(QPainter::Antialiasing);

    // =========================
    // ===== PIE CONFIG =====
    // =========================
    int i = 0;
    for(QPieSlice *slice : pieSeries->slices())
    {
        slice->setBrush(colors[i % colors.size()]);
        i++;

        slice->setLabel(QString("%1 (%2%)")
                            .arg(slice->label())
                            .arg(slice->percentage() * 100, 0, 'f', 1));

        slice->setLabelVisible(true);

        QObject::connect(slice, &QPieSlice::hovered, [=](bool state){
            slice->setExploded(state);

            if(state)
            {
                int value = slice->value();

                // Ô£à Update BAR visually
                set->remove(0, set->count());
                *set << value;

                axisY->setRange(0, value > 0 ? value : 1);

                axisX->clear();
                axisX->append(QStringList() << slice->label());


            }
            else
            {

            }
        });
    }

    pieSeries->setLabelsVisible(true);
    pieSeries->setLabelsPosition(QPieSlice::LabelOutside);

    pieChart->addSeries(pieSeries);

    QChartView *pieView = new QChartView(pieChart);
    pieView->setRenderHint(QPainter::Antialiasing);

    // =========================
    // ===== RIGHT PANEL =====
    // =========================
    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->addWidget(barView);

    QWidget *rightWidget = new QWidget();
    rightWidget->setLayout(rightLayout);

    // =========================
    // ===== LINE CHART =====
    // =========================
    QLineSeries *lineSeries = new QLineSeries();
    lineSeries->setPointsVisible(true);
    lineSeries->setMarkerSize(8);
    lineSeries->setPointLabelsVisible(true);
    lineSeries->setPointLabelsFormat("@yPoint");

    QStringList months;
    int maxVal = 0;
    int index = 0;

    for(auto it = monthlyStats.begin(); it != monthlyStats.end(); ++it)
    {
        months << it.key();

        int value = it.value();
        lineSeries->append(index, value);

        if(value > maxVal)
            maxVal = value;

        index++;
    }

    QChart *lineChart = new QChart();
    lineChart->addSeries(lineSeries);
    lineChart->setTitle("Croissance mensuelle des clients");
    lineChart->setAnimationOptions(QChart::AllAnimations);

    QBarCategoryAxis *axisX_line = new QBarCategoryAxis();
    axisX_line->append(months);

    QValueAxis *axisY_line = new QValueAxis();
    axisY_line->setLabelFormat("%d");
    axisY_line->setMinorTickCount(0);
    axisY_line->setRange(0, maxVal + 2);

    lineChart->addAxis(axisX_line, Qt::AlignBottom);
    lineChart->addAxis(axisY_line, Qt::AlignLeft);

    lineSeries->attachAxis(axisX_line);
    lineSeries->attachAxis(axisY_line);

    QVector<QPointF> points = lineSeries->pointsVector();
    lineSeries->clear();

    QTimer *timer = new QTimer(window);
    int *i_anim = new int(0);

    QObject::connect(timer, &QTimer::timeout, [=]() mutable {
        if(*i_anim < points.size())
        {
            lineSeries->append(points[*i_anim]);
            (*i_anim)++;
        }
        else
        {
            timer->stop();
        }
    });

    timer->start(100); // smooth animation

    QChartView *lineView = new QChartView(lineChart);
    lineView->setRenderHint(QPainter::Antialiasing);
    lineView->setMinimumHeight(300);

    // =========================
    // ===== LAYOUT =====
    // =========================
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addWidget(pieView);
    topLayout->addWidget(rightWidget);

    layout->addLayout(topLayout);
    layout->addWidget(lineView);

    window->setLayout(layout);

    return window;
}
