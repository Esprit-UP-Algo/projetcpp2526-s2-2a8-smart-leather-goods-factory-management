#ifndef VOICEMATIERES_H
#define VOICEMATIERES_H

#include <QObject>
#include <QWidget>
#include <QString>
#include <QTableWidget>
#include <functional>

// Forward declaration
class MainWindow;

class VoiceMatieres : public QObject
{
    Q_OBJECT

public:
    explicit VoiceMatieres(MainWindow *mainWin, QTableWidget *table);
    ~VoiceMatieres();

    // Ouvrir le dialog de commande vocale
    void openVoiceDialog();

private:
    void processVoiceCommand(const QString &rawCmd);
    QString cellText(QTableWidget *table, int row, int col);

    MainWindow *mainWindow;
    QTableWidget *matiereTable;
};

#endif // VOICEMATIERES_H
