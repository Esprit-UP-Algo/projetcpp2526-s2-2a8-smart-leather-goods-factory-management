#include "pythonfacerecognition.h"
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>

PythonFaceRecognition::PythonFaceRecognition(QObject *parent)
    : QObject(parent)
    , pythonProcess(nullptr)
{
    // Chemin vers le script Python (dans le même dossier que l'exécutable)
    pythonScriptPath = QCoreApplication::applicationDirPath() + "/face_recognizer.py";
    
    // Dossier pour stocker les images de visages
    facesDataPath = QCoreApplication::applicationDirPath() + "/faces_data";
    
    // Créer le dossier s'il n'existe pas
    QDir dir;
    if (!dir.exists(facesDataPath)) {
        dir.mkpath(facesDataPath);
        qDebug() << "📁 Dossier créé:" << facesDataPath;
    }
}

PythonFaceRecognition::~PythonFaceRecognition()
{
    if (pythonProcess) {
        pythonProcess->kill();
        pythonProcess->waitForFinished();
        delete pythonProcess;
    }
}

QString PythonFaceRecognition::getPythonCommand()
{
    // Essayer différentes commandes Python
    QStringList pythonCommands = {"python", "python3", "py"};
    
    for (const QString &cmd : pythonCommands) {
        QProcess testProcess;
        testProcess.start(cmd, QStringList() << "--version");
        testProcess.waitForFinished(3000);
        
        if (testProcess.exitCode() == 0) {
            qDebug() << "✅ Python trouvé:" << cmd;
            return cmd;
        }
    }
    
    return "python"; // Par défaut
}

bool PythonFaceRecognition::isAvailable()
{
    QProcess process;
    QString pythonCmd = getPythonCommand();
    
    // Vérifier si Python est installé
    process.start(pythonCmd, QStringList() << "--version");
    process.waitForFinished(3000);
    
    if (process.exitCode() != 0) {
        qDebug() << "❌ Python non trouvé";
        return false;
    }
    
    // Vérifier si le script existe
    if (!QFile::exists(pythonScriptPath)) {
        qDebug() << "❌ Script Python non trouvé:" << pythonScriptPath;
        return false;
    }
    
    // Vérifier si face_recognition est installé
    process.start(pythonCmd, QStringList() << "-c" << "import face_recognition");
    process.waitForFinished(3000);
    
    if (process.exitCode() != 0) {
        qDebug() << "❌ Module face_recognition non installé";
        return false;
    }
    
    qDebug() << "✅ Reconnaissance faciale disponible";
    return true;
}

bool PythonFaceRecognition::saveFaceImage(int employeeId, const QImage &image)
{
    QString imagePath = QString("%1/employee_%2.jpg").arg(facesDataPath).arg(employeeId);
    
    bool saved = image.save(imagePath, "JPEG", 95);
    
    if (saved) {
        qDebug() << "✅ Image sauvegardée:" << imagePath;
    } else {
        qDebug() << "❌ Erreur sauvegarde image:" << imagePath;
    }
    
    return saved;
}

bool PythonFaceRecognition::registerFace(int employeeId, const QImage &faceImage)
{
    if (faceImage.isNull()) {
        emit recognitionFailed("Image invalide");
        return false;
    }
    
    emit recognitionProgress("💾 Sauvegarde de l'image...");
    
    // Sauvegarder l'image
    if (!saveFaceImage(employeeId, faceImage)) {
        emit recognitionFailed("Erreur lors de la sauvegarde de l'image");
        return false;
    }
    
    emit recognitionProgress("🔍 Détection du visage...");
    
    // Appeler le script Python pour encoder le visage
    QString pythonCmd = getPythonCommand();
    QString imagePath = QString("%1/employee_%2.jpg").arg(facesDataPath).arg(employeeId);
    
    QProcess process;
    process.start(pythonCmd, QStringList() 
                  << pythonScriptPath 
                  << "register" 
                  << QString::number(employeeId) 
                  << imagePath);
    
    process.waitForFinished(10000); // 10 secondes max
    
    QString output = process.readAllStandardOutput();
    QString error = process.readAllStandardError();
    
    qDebug() << "Python output:" << output;
    qDebug() << "Python error:" << error;
    
    if (process.exitCode() == 0 && output.contains("SUCCESS")) {
        emit recognitionProgress("✅ Visage enregistré avec succès!");
        emit recognitionCompleted(employeeId);
        return true;
    } else {
        QString errorMsg = "Erreur lors de l'enregistrement";
        if (output.contains("NO_FACE")) {
            errorMsg = "Aucun visage détecté dans l'image";
        } else if (output.contains("MULTIPLE_FACES")) {
            errorMsg = "Plusieurs visages détectés. Un seul visage requis.";
        }
        
        emit recognitionFailed(errorMsg);
        return false;
    }
}

int PythonFaceRecognition::recognizeFace(const QImage &faceImage)
{
    if (faceImage.isNull()) {
        emit recognitionFailed("Image invalide");
        return -1;
    }
    
    emit recognitionProgress("🔍 Analyse du visage...");
    
    // Sauvegarder l'image temporaire
    QString tempPath = QString("%1/temp_recognition.jpg").arg(facesDataPath);
    if (!faceImage.save(tempPath, "JPEG", 95)) {
        emit recognitionFailed("Erreur lors de la sauvegarde temporaire");
        return -1;
    }
    
    // Appeler le script Python pour reconnaître le visage
    QString pythonCmd = getPythonCommand();
    
    QProcess process;
    process.start(pythonCmd, QStringList() 
                  << pythonScriptPath 
                  << "recognize" 
                  << tempPath);
    
    process.waitForFinished(10000); // 10 secondes max
    
    QString output = process.readAllStandardOutput();
    QString error = process.readAllStandardError();
    
    qDebug() << "Python output:" << output;
    qDebug() << "Python error:" << error;
    
    // Nettoyer le fichier temporaire
    QFile::remove(tempPath);
    
    if (process.exitCode() == 0 && output.contains("RECOGNIZED:")) {
        // Extraire l'ID de l'employé
        QStringList parts = output.split(":");
        if (parts.size() >= 2) {
            int employeeId = parts[1].trimmed().toInt();
            emit recognitionProgress(QString("✅ Employé reconnu: ID %1").arg(employeeId));
            emit recognitionCompleted(employeeId);
            return employeeId;
        }
    } else if (output.contains("NO_FACE")) {
        emit recognitionFailed("Aucun visage détecté");
    } else if (output.contains("UNKNOWN")) {
        emit recognitionFailed("Visage non reconnu");
    } else {
        emit recognitionFailed("Erreur lors de la reconnaissance");
    }
    
    return -1;
}

bool PythonFaceRecognition::hasFaceRegistered(int employeeId)
{
    QString imagePath = QString("%1/employee_%2.jpg").arg(facesDataPath).arg(employeeId);
    return QFile::exists(imagePath);
}
