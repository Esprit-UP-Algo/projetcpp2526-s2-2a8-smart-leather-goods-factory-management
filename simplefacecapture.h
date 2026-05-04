#ifndef SIMPLEFACECAPTURE_H
#define SIMPLEFACECAPTURE_H

#include <QWidget>
#include <QCamera>
#include <QVideoWidget>
#include <QImageCapture>
#include <QMediaCaptureSession>
#include <QVBoxLayout>
#include <QLabel>
#include <QImage>

class SimpleFaceCapture : public QWidget
{
    Q_OBJECT

public:
    explicit SimpleFaceCapture(QWidget *parent = nullptr);
    ~SimpleFaceCapture();

    void startCapture();
    void stopCapture();
    
    QImageCapture *m_imageCapture; // Public pour accès externe

signals:
    void frameCaptured(const QImage &image);
    void errorOccurred(const QString &error);

private slots:
    void onImageCaptured(int id, const QImage &preview);
    void onCameraError(QCamera::Error error);

private:
    QCamera *m_camera;
    QMediaCaptureSession *m_captureSession;
    QVideoWidget *m_videoWidget;
    QLabel *m_statusLabel;
};

#endif // SIMPLEFACECAPTURE_H
