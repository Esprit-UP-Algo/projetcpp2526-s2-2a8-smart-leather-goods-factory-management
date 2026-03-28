#include "articleviewer3d.h"
#include <algorithm>

// ═══════════════════════════════════════════════════════════════
// Render3DWidget - Rendu 3D software avec rotation souris + zoom
// ═══════════════════════════════════════════════════════════════

Render3DWidget::Render3DWidget(QWidget *parent)
    : QWidget(parent), m_color(141,110,99), m_rotX(-25), m_rotY(35), m_zoom(1.0f),
      m_scaleX(1), m_scaleY(1), m_scaleZ(1)
{
    setMinimumSize(400, 350);
    setMouseTracking(false);
    buildCuboid(2.0f, 1.5f, 0.8f);
    m_shapeName = "Cuboid";
}

void Render3DWidget::setShape(const QString &type)
{
    m_vertices.clear(); m_faces.clear();
    m_shapeName = type;
    QString t = type.toLower();

    if (t.contains("sac à main") || t.contains("sac a main"))
        buildCuboid(2.0f, 1.5f, 0.8f);
    else if (t.contains("bandoulière") || t.contains("bandouliere"))
        buildCuboid(1.8f, 1.2f, 0.6f);
    else if (t.contains("tote"))
        buildCuboid(2.5f, 2.0f, 1.0f);
    else if (t.contains("dos"))
        buildCuboid(1.5f, 2.2f, 0.8f);
    else if (t.contains("portefeuille long"))
        buildCuboid(1.8f, 0.8f, 0.15f);
    else if (t.contains("portefeuille compact"))
        buildCuboid(1.2f, 0.8f, 0.15f);
    else if (t.contains("porte-cartes") || t.contains("carte"))
        buildCuboid(1.0f, 0.7f, 0.08f);
    else if (t.contains("ceinture"))
        buildTorus(1.2f, 0.06f, 32, 12);
    else if (t.contains("porte-monnaie") || t.contains("monnaie"))
        buildCuboid(1.0f, 0.7f, 0.2f);
    else if (t.contains("pochette") || t.contains("clutch"))
        buildCuboid(1.8f, 1.0f, 0.2f);
    else if (t.contains("clés") || t.contains("cles"))
        buildSphere(0.5f, 12);
    else
        buildCuboid(1.5f, 1.0f, 0.5f);

    update();
}

void Render3DWidget::setColor(int r, int g, int b) { m_color = QColor(r,g,b); update(); }
void Render3DWidget::setScale(float sx, float sy, float sz) { m_scaleX=sx; m_scaleY=sy; m_scaleZ=sz; update(); }

void Render3DWidget::buildCuboid(float w, float h, float d)
{
    float hw=w/2, hh=h/2, hd=d/2;
    m_vertices = {
        {-hw,-hh,-hd},{hw,-hh,-hd},{hw,hh,-hd},{-hw,hh,-hd},
        {-hw,-hh,hd},{hw,-hh,hd},{hw,hh,hd},{-hw,hh,hd}
    };
    m_faces = {
        {{0,1,2,3},4}, {{5,4,7,6},4}, {{1,5,6,2},4},
        {{4,0,3,7},4}, {{3,2,6,7},4}, {{4,5,1,0},4}
    };
}

void Render3DWidget::buildTorus(float R, float r, int seg1, int seg2)
{
    for (int i=0; i<seg1; ++i) {
        float theta = 2*M_PI*i/seg1;
        for (int j=0; j<seg2; ++j) {
            float phi = 2*M_PI*j/seg2;
            float x = (R + r*cosf(phi))*cosf(theta);
            float y = r*sinf(phi);
            float z = (R + r*cosf(phi))*sinf(theta);
            m_vertices.append({x,y,z});
        }
    }
    for (int i=0; i<seg1; ++i) {
        int ni = (i+1)%seg1;
        for (int j=0; j<seg2; ++j) {
            int nj = (j+1)%seg2;
            m_faces.append({{i*seg2+j, ni*seg2+j, ni*seg2+nj, i*seg2+nj}, 4});
        }
    }
}

void Render3DWidget::buildSphere(float radius, int seg)
{
    // Simple UV sphere
    m_vertices.append({0, radius, 0}); // top
    for (int i=1; i<seg; ++i) {
        float phi = M_PI*i/seg;
        for (int j=0; j<seg*2; ++j) {
            float theta = 2*M_PI*j/(seg*2);
            m_vertices.append({radius*sinf(phi)*cosf(theta), radius*cosf(phi), radius*sinf(phi)*sinf(theta)});
        }
    }
    m_vertices.append({0, -radius, 0}); // bottom
    // Top cap
    for (int j=0; j<seg*2; ++j) {
        int nj = (j+1)%(seg*2);
        m_faces.append({{0, 1+j, 1+nj, 0}, 3});
    }
    // Middle
    for (int i=0; i<seg-2; ++i) {
        for (int j=0; j<seg*2; ++j) {
            int nj = (j+1)%(seg*2);
            int c = 1+i*seg*2;
            int nc = 1+(i+1)*seg*2;
            m_faces.append({{c+j, nc+j, nc+nj, c+nj}, 4});
        }
    }
}

Render3DWidget::Point3D Render3DWidget::rotate(Point3D p)
{
    // Apply scale
    p.x *= m_scaleX; p.y *= m_scaleY; p.z *= m_scaleZ;
    // Rotate Y
    float ry = m_rotY * M_PI / 180.0f;
    float x1 = p.x*cosf(ry) + p.z*sinf(ry);
    float z1 = -p.x*sinf(ry) + p.z*cosf(ry);
    p.x = x1; p.z = z1;
    // Rotate X
    float rx = m_rotX * M_PI / 180.0f;
    float y1 = p.y*cosf(rx) - p.z*sinf(rx);
    float z2 = p.y*sinf(rx) + p.z*cosf(rx);
    p.y = y1; p.z = z2;
    return p;
}

QPointF Render3DWidget::project(Point3D p)
{
    float fov = 4.0f * m_zoom;
    float cx = width()/2.0f, cy = height()/2.0f;
    float scale = qMin(width(), height()) / fov;
    return QPointF(cx + p.x * scale, cy - p.y * scale);
}

float Render3DWidget::faceDepth(const Face &f)
{
    float z = 0;
    for (int i=0; i<f.nv; ++i) z += rotate(m_vertices[f.v[i]]).z;
    return z / f.nv;
}

void Render3DWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Background gradient
    QLinearGradient bg(0,0,0,height());
    bg.setColorAt(0, QColor(22,33,62));
    bg.setColorAt(1, QColor(15,52,96));
    p.fillRect(rect(), bg);

    if (m_vertices.isEmpty() || m_faces.isEmpty()) return;

    // Sort faces by depth (painter's algorithm)
    QVector<int> order(m_faces.size());
    for (int i=0; i<m_faces.size(); ++i) order[i]=i;
    std::sort(order.begin(), order.end(), [this](int a, int b) {
        return faceDepth(m_faces[a]) < faceDepth(m_faces[b]);
    });

    // Draw faces
    for (int idx : order) {
        const Face &f = m_faces[idx];
        QPolygonF poly;
        Point3D rotated[4];
        for (int i=0; i<f.nv; ++i) {
            rotated[i] = rotate(m_vertices[f.v[i]]);
            poly << project(rotated[i]);
        }

        // Simple lighting: dot product with light direction
        float nx=0, ny=0, nz=0;
        if (f.nv >= 3) {
            float ax=rotated[1].x-rotated[0].x, ay=rotated[1].y-rotated[0].y, az=rotated[1].z-rotated[0].z;
            float bx=rotated[2].x-rotated[0].x, by=rotated[2].y-rotated[0].y, bz=rotated[2].z-rotated[0].z;
            nx=ay*bz-az*by; ny=az*bx-ax*bz; nz=ax*by-ay*bx;
            float len=sqrtf(nx*nx+ny*ny+nz*nz);
            if (len>0){nx/=len;ny/=len;nz/=len;}
        }
        // Light from top-right-front
        float light = 0.3f + 0.7f * qMax(0.0f, nx*0.3f + ny*0.5f + nz*0.8f);
        light = qBound(0.2f, light, 1.0f);

        QColor faceColor(
            qBound(0, (int)(m_color.red()*light), 255),
            qBound(0, (int)(m_color.green()*light), 255),
            qBound(0, (int)(m_color.blue()*light), 255)
        );

        p.setPen(QPen(faceColor.darker(130), 1));
        p.setBrush(faceColor);
        p.drawPolygon(poly);
    }

    // Info text
    p.setPen(QColor(188,170,164));
    p.setFont(QFont("Segoe UI", 9));
    p.drawText(10, height()-10, QString("🖱 Glisser pour tourner  |  🔄 Molette pour zoomer  |  Zoom: %1x").arg(QString::number(m_zoom,'f',1)));
}

void Render3DWidget::mousePressEvent(QMouseEvent *e) { m_lastMouse = e->pos(); }

void Render3DWidget::mouseMoveEvent(QMouseEvent *e)
{
    QPoint delta = e->pos() - m_lastMouse;
    m_rotY += delta.x() * 0.5f;
    m_rotX += delta.y() * 0.5f;
    m_rotX = qBound(-89.0f, m_rotX, 89.0f);
    m_lastMouse = e->pos();
    update();
}

void Render3DWidget::wheelEvent(QWheelEvent *e)
{
    m_zoom += e->angleDelta().y() > 0 ? 0.1f : -0.1f;
    m_zoom = qBound(0.3f, m_zoom, 5.0f);
    update();
}

// ═══════════════════════════════════════════════════════════════
// ArticleViewer3D - Widget complet avec contrôles
// ═══════════════════════════════════════════════════════════════

ArticleViewer3D::ArticleViewer3D(QWidget *parent) : QWidget(parent)
{
    setupUI();
    loadModelForType("Sac à main");
}

void ArticleViewer3D::setupUI()
{
    auto *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0); mainLayout->setSpacing(8);

    // Rendu 3D
    m_render = new Render3DWidget(this);
    mainLayout->addWidget(m_render, 3);

    // Panneau contrôle
    auto *ctrl = new QWidget();
    ctrl->setStyleSheet("QWidget{background:#16213E;border-radius:10px;}"
                        "QLabel{color:#E0E0E0;font-size:11px;}"
                        "QGroupBox{border:2px solid #8D6E63;border-radius:8px;"
                        "margin-top:8px;padding-top:8px;font-weight:bold;color:#FFCC80;}"
                        "QSlider::groove:horizontal{background:#0F3460;height:6px;border-radius:3px;}"
                        "QSlider::handle:horizontal{background:#FFCC80;width:14px;height:14px;"
                        "border-radius:7px;margin:-4px 0;}"
                        "QSlider::sub-page:horizontal{background:#8D6E63;border-radius:3px;}"
                        "QPushButton{background:#8D6E63;color:white;border:none;border-radius:6px;"
                        "padding:8px 16px;font-weight:bold;}"
                        "QPushButton:hover{background:#A0826D;}");
    auto *ctrlLay = new QVBoxLayout(ctrl);
    ctrlLay->setContentsMargins(10,10,10,10); ctrlLay->setSpacing(8);

    m_lblModelName = new QLabel("📦 Modèle : —");
    m_lblModelName->setStyleSheet("font-size:13px;font-weight:bold;color:#FFCC80;padding:5px;");
    m_lblModelName->setAlignment(Qt::AlignCenter);
    ctrlLay->addWidget(m_lblModelName);

    // Couleur
    auto *colorBox = new QGroupBox("  🎨  Couleur");
    auto *colorLay = new QVBoxLayout(colorBox);
    m_colorPreview = new QWidget(); m_colorPreview->setFixedHeight(25);
    m_colorPreview->setStyleSheet("background:rgb(141,110,99);border-radius:6px;");
    colorLay->addWidget(m_colorPreview);

    auto makeCS = [&](const QString &lbl, QSlider *&s, const QString &col, int def) {
        auto *row = new QHBoxLayout();
        auto *l = new QLabel(lbl); l->setFixedWidth(15);
        l->setStyleSheet(QString("color:%1;font-weight:bold;").arg(col));
        s = new QSlider(Qt::Horizontal); s->setRange(0,255); s->setValue(def);
        auto *v = new QLabel(QString::number(def)); v->setFixedWidth(30);
        connect(s, &QSlider::valueChanged, [this,v](int val) {
            v->setText(QString::number(val));
            m_render->setColor(m_sliderR->value(), m_sliderG->value(), m_sliderB->value());
            updateColorPreview();
        });
        row->addWidget(l); row->addWidget(s); row->addWidget(v);
        colorLay->addLayout(row);
    };
    makeCS("R", m_sliderR, "#EF5350", 141);
    makeCS("G", m_sliderG, "#66BB6A", 110);
    makeCS("B", m_sliderB, "#42A5F5", 99);

    auto *btnPicker = new QPushButton("🎨 Choisir");
    connect(btnPicker, &QPushButton::clicked, [this]() {
        QColor c = QColorDialog::getColor(QColor(m_sliderR->value(),m_sliderG->value(),m_sliderB->value()), this);
        if (c.isValid()) { m_sliderR->setValue(c.red()); m_sliderG->setValue(c.green()); m_sliderB->setValue(c.blue()); }
    });
    colorLay->addWidget(btnPicker);
    ctrlLay->addWidget(colorBox);

    // Dimensions
    auto *dimBox = new QGroupBox("  📐  Dimensions");
    auto *dimLay = new QVBoxLayout(dimBox);
    m_lblDimensions = new QLabel("1.0 × 1.0 × 1.0");
    m_lblDimensions->setAlignment(Qt::AlignCenter);
    m_lblDimensions->setStyleSheet("font-size:12px;font-weight:bold;color:#FFCC80;padding:4px;");
    dimLay->addWidget(m_lblDimensions);

    auto makeDS = [&](const QString &lbl, QSlider *&s, float def) {
        auto *row = new QHBoxLayout();
        auto *l = new QLabel(lbl); l->setFixedWidth(70);
        s = new QSlider(Qt::Horizontal); s->setRange(10,300); s->setValue((int)(def*100));
        auto *v = new QLabel(QString::number(def,'f',1)); v->setFixedWidth(30);
        connect(s, &QSlider::valueChanged, [this,v](int val) {
            v->setText(QString::number(val/100.0,'f',1));
            m_render->setScale(m_sliderW->value()/100.0f, m_sliderH->value()/100.0f, m_sliderD->value()/100.0f);
            m_lblDimensions->setText(QString("%1 × %2 × %3")
                .arg(m_sliderW->value()/100.0,0,'f',1)
                .arg(m_sliderH->value()/100.0,0,'f',1)
                .arg(m_sliderD->value()/100.0,0,'f',1));
        });
        row->addWidget(l); row->addWidget(s); row->addWidget(v);
        dimLay->addLayout(row);
    };
    makeDS("Largeur", m_sliderW, 1.0f);
    makeDS("Hauteur", m_sliderH, 1.0f);
    makeDS("Profondeur", m_sliderD, 1.0f);

    auto *btnReset = new QPushButton("🔄 Reset");
    connect(btnReset, &QPushButton::clicked, [this]() {
        m_sliderW->setValue(100); m_sliderH->setValue(100); m_sliderD->setValue(100);
        m_sliderR->setValue(141); m_sliderG->setValue(110); m_sliderB->setValue(99);
    });
    dimLay->addWidget(btnReset);
    ctrlLay->addWidget(dimBox);
    ctrlLay->addStretch();
    mainLayout->addWidget(ctrl, 1);

    setColor(141, 110, 99);
}

void ArticleViewer3D::loadModelForType(const QString &type)
{
    m_render->setShape(type);
    m_lblModelName->setText("📦 " + type);
}

void ArticleViewer3D::setColor(int r, int g, int b)
{
    m_sliderR->setValue(r); m_sliderG->setValue(g); m_sliderB->setValue(b);
    m_render->setColor(r, g, b);
    updateColorPreview();
    emit colorChanged(QColor(r,g,b));
}

void ArticleViewer3D::setDimensions(float w, float h, float d)
{
    m_sliderW->setValue((int)(w*100)); m_sliderH->setValue((int)(h*100)); m_sliderD->setValue((int)(d*100));
    m_render->setScale(w, h, d);
    emit dimensionsChanged(w, h, d);
}

void ArticleViewer3D::updateColorPreview()
{
    m_colorPreview->setStyleSheet(
        QString("background:rgb(%1,%2,%3);border-radius:6px;")
        .arg(m_sliderR->value()).arg(m_sliderG->value()).arg(m_sliderB->value()));
}
