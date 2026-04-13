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
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTextEdit>
#include <cmath>

// ── Render3DWidget : rendu multi-pièces avec éclairage ──────────────────────
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
    struct Face { int v[4]; int nv; int colorIdx; }; // colorIdx = index dans m_partColors

    QVector<Point3D> m_vertices;
    QVector<Face> m_faces;
    QVector<QColor> m_partColors; // couleur par pièce
    QColor m_baseColor;
    float m_rotX, m_rotY, m_zoom;
    float m_scaleX, m_scaleY, m_scaleZ;
    QPoint m_lastMouse;
    QString m_shapeName;

    // Primitives de base (retournent l'index de couleur utilisé)
    int addCuboid(float cx, float cy, float cz, float w, float h, float d, const QColor &col);
    int addCylinder(float cx, float cy, float cz, float radius, float height, int seg, const QColor &col);
    int addTorus(float cx, float cy, float cz, float R, float r, int seg1, int seg2, const QColor &col);
    int addHalfCylinder(float cx, float cy, float cz, float radius, float length, int seg, const QColor &col);

    // Modèles composites
    void buildSacAMain();
    void buildSacBandouliere();
    void buildToteBag();
    void buildSacADos();
    void buildPortefeuilleLong();
    void buildPortefeuilleCompact();
    void buildPorteCartes();
    void buildCeinture();
    void buildPorteMonnaie();
    void buildPochette();
    void buildPorteCles();
    void buildDefault();

    QPointF project(Point3D p);
    Point3D rotate(Point3D p);
    float faceDepth(const Face &f);
};

// ── ArticleViewer3D : widget complet avec contrôles + IA Groq ───────────────
class ArticleViewer3D : public QWidget
{
    Q_OBJECT
public:
    explicit ArticleViewer3D(QWidget *parent = nullptr);
    void loadModelForType(const QString &articleType);
    void setColor(int r, int g, int b);
    void setDimensions(float w, float h, float d);
    void setArticleInfo(const QString &nom, const QString &type, const QString &categorie,
                        int r, int g, int b, double larg, double haut, double prof,
                        double prix, const QString &statut);
    void generateAuto();

signals:
    void colorChanged(const QColor &color);
    void dimensionsChanged(float w, float h, float d);

private slots:
    void onGenerateAI3D();
    void onAIReplyFinished(QNetworkReply *reply);

private:
    Render3DWidget *m_render;
    QSlider *m_sliderR, *m_sliderG, *m_sliderB;
    QSlider *m_sliderW, *m_sliderH, *m_sliderD;
    QLabel *m_lblModelName;
    QWidget *m_colorPreview;
    QLabel *m_lblDimensions;

    // IA Groq
    QNetworkAccessManager *m_networkAI;
    QPushButton *m_btnGenerateAI;
    QTextEdit *m_aiDescription;
    QLabel *m_aiStatus;
    QString m_currentType;
    QString m_currentNom;
    QString m_currentCategorie;
    int m_currentR, m_currentG, m_currentB;
    double m_currentLarg, m_currentHaut, m_currentProf;
    double m_currentPrix;
    QString m_currentStatut;

    void setupUI();
    void updateColorPreview();
    void applyAIParameters(const QJsonObject &params);
};

#endif // ARTICLEVIEWER3D_H
