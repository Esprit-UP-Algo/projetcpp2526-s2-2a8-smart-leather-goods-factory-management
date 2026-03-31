#ifndef MATIEREDETECTION_H
#define MATIEREDETECTION_H

#include <QObject>
#include <QWidget>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QTableWidget>

// Forward declaration
class MainWindow;

class MatiereDetection : public QObject
{
    Q_OBJECT

public:
    explicit MatiereDetection(MainWindow *mainWin, QTableWidget *table, 
                             QNetworkAccessManager *netMgr, const QString &api);
    ~MatiereDetection();

    // Lancer la détection
    void startDetection();

private:
    void showSelectionDialog();
    void startDetectionAnalysis(const QString &imagePath);
    void detectDefectsInImage(const QString &imagePath);
    void showDetectionResults(const QJsonObject &response);
    QString translateDefectClass(const QString &englishName);

    MainWindow *mainWindow;
    QTableWidget *matiereTable;
    QNetworkAccessManager *networkManager;
    QString apiUrl;
};

#endif // MATIEREDETECTION_H
