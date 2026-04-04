#ifndef STATSCHARTS_H
#define STATSCHARTS_H

#include <QWidget>
#include <QMap>

class StatsCharts
{
public:
    static QWidget* createStatsWindow(const QMap<QString,int>& stats,
                                      const QMap<QString,int>& monthlyStats);
};

#endif
