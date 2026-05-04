#include "simplefacecapture.h"
#include <QDebug>
#include <QMediaDevices>

SimpleFaceCapture::SimpleFaceCapture(QWidget *parent)
    : QWidget(parent)
    , m_camera(nullptr)
    , m_captureSession(nullptr)
    , m_videoWidget(nullptr)
    , m_imageCapture(nullptr)
    , m_statusLabel(nullptr)
{
    // Layout principal
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    // Widget vidéo pour afficher la caméra
    m_videoWidget = new QVideoWidget(this);
    m_videoWidget->setMinimumSize(640, 480);
    layout->addWidget(m_videoWidget);
    
    // Label de statut
    m_statusLabel = new QLabel("Caméra non démarrée", this);
    m_statusLabel->setStyleSheet("padding: 5px; background-color: #FFF3CD; border-radius: 4px;");
    m_statusLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_statusLabel);
    
    setLayout(layout);
}

SimpleFaceCapture::~SimpleFaceCapture()
{
    stopCapture();
}

void SimpleFaceCapture::startCapture()
{
    qDebug() << "🎥 Démarrage de la capture vidéo...";
    
    // Obtenir la caméra par défaut
    const QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
    
    if (cameras.isEmpty()) {
        m_statusLabel->setText("❌ Aucune caméra détectée");
        emit errorOccurred("Aucune caméra disponible");
        qDebug() << "❌ Aucune caméra trouvée";
        return;
    }
    
    qDebug() << "📷 Caméras disponibles:" << cameras.size();
    for (const QCameraDevice &cameraDevice : cameras) {
        qDebug() << "  -" << cameraDevice.description();
    }
    
    // Créer la caméra
    m_camera = new QCamera(cameras.first(), this);
    
    // Créer la session de capture
    m_captureSession = new QMediaCaptureSession(this);
    m_captureSession->setCamera(m_camera);
    m_captureSession->setVideoOutput(m_videoWidget);
    
    // Créer l'objet de capture d'image
    m_imageCapture = new QImageCapture(this);
    m_captureSession->setImageCapture(m_imageCapture);
    
    // Connecter les signaux
    connect(m_camera, &QCamera::errorOccurred, this, &SimpleFaceCapture::onCameraError);
    connect(m_imageCapture, &QImageCapture::imageCaptured, this, &SimpleFaceCapture::onImageCaptured);
    
    // Démarrer la caméra
    m_camera->start();
    
    m_statusLabel->setText("✅ Caméra active - Prêt à capturer");
    qDebug() << "✅ Caméra démarrée";
}

void SimpleFaceCapture::stopCapture()
{
    if (m_camera) {
        m_camera->stop();
        qDebug() << "🛑 Caméra arrêtée";
    }
    
    // Nettoyer les objets
    if (m_imageCapture) {
        delete m_imageCapture;
        m_imageCapture = nullptr;
    }
    
    if (m_captureSession) {
        delete m_captureSession;
        m_captureSession = nullptr;
    }
    
    if (m_camera) {
        delete m_camera;
        m_camera = nullptr;
    }
    
    m_statusLabel->setText("Caméra arrêtée");
}

void SimpleFaceCapture::onImageCaptured(int id, const QImage &preview)
{
    Q_UNUSED(id);
    
    qDebug() << "📸 Image capturée:" << preview.size();
    m_statusLabel->setText("📸 Image capturée!");
    
    emit frameCaptured(preview);
}

void SimpleFaceCapture::onCameraError(QCamera::Error error)
{
    QString errorMsg;
    
    switch (error) {
        case QCamera::NoError:
            return;
        case QCamera::CameraError:
            errorMsg = "Erreur caméra générique";
            break;
        default:
            errorMsg = "Erreur caméra inconnue";
            break;
    }
    
    qDebug() << "❌ Erreur caméra:" << errorMsg;
    m_statusLabel->setText("❌ " + errorMsg);
    emit errorOccurred(errorMsg);
}
