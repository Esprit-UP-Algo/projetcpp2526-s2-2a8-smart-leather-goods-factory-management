#ifndef ARTICLEVIEWER3D_H
#define ARTICLEVIEWER3D_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSlider>
#include <QPushButton>
#include <QLabel>
#include <QColorDialog>
#include <QGroupBox>
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QTimer>
#include <cmath>

// Widget de rendu 3D software (pas besoin d'OpenGL)
class Render3DWidget : public QWidget
{
    Q_OBJECT
public:
    explicit Render3DWidget(QWidget *parent = nullptr);

    void setShape(const QString &type);
    void setColor(int r, int g, int b);
    void setScale(float sx, float sy, float sz);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    struct Point3D { float x, y, z; };
    struct Face { int v[4]; int nv; }; // 3 ou 4 vertices

    QVector<Point3D> m_vertices;
    QVector<Face> m_faces;
    QColor m_color;
    float m_rotX, m_rotY;
    float m_zoom;
    float m_scaleX, m_scaleY, m_scaleZ;
    QPoint m_lastMouse;
    QString m_shapeName;

    void buildCuboid(float w, float h, float d);
    void buildTorus(float R, float r, int seg1, int seg2);
    void buildSphere(float radius, int seg);
    QPointF project(Point3D p);
    Point3D rotate(Point3D p);
    float faceDepth(const Face &f);
};

class ArticleViewer3D : public QWidget
{
    Q_OBJECT
public:
    explicit ArticleViewer3D(QWidget *parent = nullptr);
    void loadModelForType(const QString &articleType);
    void setColor(int r, int g, int b);
    void setDimensions(float w, float h, float d);

signals:
    void colorChanged(const QColor &color);
    void dimensionsChanged(float w, float h, float d);

private:
    Render3DWidget *m_render;
    QSlider *m_sliderR, *m_sliderG, *m_sliderB;
    QSlider *m_sliderW, *m_sliderH, *m_sliderD;
    QLabel *m_lblModelName;
    QWidget *m_colorPreview;
    QLabel *m_lblDimensions;

    void setupUI();
    void updateColorPreview();
};

#endif // ARTICLEVIEWER3D_H
