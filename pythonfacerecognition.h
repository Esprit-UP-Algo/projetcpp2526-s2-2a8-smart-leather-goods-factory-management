#ifndef PYTHONFACERECOGNITION_H
#define PYTHONFACERECOGNITION_H

#include <QObject>
#include <QProcess>
#include <QImage>
#include <QString>
#include <QDebug>

class PythonFaceRecognition : public QObject
{
    Q_OBJECT

public:
    explicit PythonFaceRecognition(QObject *parent = nullptr);
    ~PythonFaceRecognition();

    // Vérifier si Python est disponible
    bool isAvailable();

    // Enregistrer un nouveau visage
    bool registerFace(int employeeId, const QImage &faceImage);

    // Reconnaître un visage
    int recognizeFace(const QImage &faceImage);

    // Vérifier si un employé a un visage enregistré
    bool hasFaceRegistered(int employeeId);

signals:
    void recognitionProgress(const QString &message);
    void recognitionCompleted(int employeeId);
    void recognitionFailed(const QString &error);

private:
    QProcess *pythonProcess;
    QString pythonScriptPath;
    QString facesDataPath;

    bool saveFaceImage(int employeeId, const QImage &image);
    QString getPythonCommand();
};

#endif // PYTHONFACERECOGNITION_H
