#include "articleviewer3d.h"
#include "envloader.h"
#include <algorithm>
#include <QDebug>
#include <QScrollArea>
#include <QFile>

// ═══════════════════════════════════════════════════════════════════════════════
// Render3DWidget — rendu multi-pièces software + mode texture
// ═══════════════════════════════════════════════════════════════════════════════

Render3DWidget::Render3DWidget(QWidget *parent)
    : QWidget(parent), m_baseColor(141,110,99),
      m_rotX(-20), m_rotY(30), m_zoom(1.0f),
      m_scaleX(1), m_scaleY(1), m_scaleZ(1),
      m_textureMode(false)
{
    setMinimumSize(400, 350);
    setMouseTracking(false);
    buildSacAMain();
}

// ── Texture mode ──────────────────────────────────────────────────────────────
void Render3DWidget::setTexture(const QString &imagePath)
{
    if (QFile::exists(imagePath)) {
        m_texture = QPixmap(imagePath);
        if (!m_texture.isNull()) {
            m_textureMode = true;
            m_texturePath = imagePath;
            update();
            return;
        }
    }
    m_textureMode = false;
    update();
}

void Render3DWidget::clearTexture()
{
    m_textureMode = false;
    m_texture = QPixmap();
    m_texturePath.clear();
    update();
}

// -- Couleur / echelle --
void Render3DWidget::setColor(int r, int g, int b)
{
    QColor old = m_baseColor;
    m_baseColor = QColor(r, g, b);
    // Recalculer les couleurs des pieces proportionnellement
    for (int i = 0; i < m_partColors.size(); ++i) {
        QColor &c = m_partColors[i];
        float rr = old.red()   > 0 ? (float)c.red()   / old.red()   : 1.0f;
        float gg = old.green() > 0 ? (float)c.green() / old.green() : 1.0f;
        float bb = old.blue()  > 0 ? (float)c.blue()  / old.blue()  : 1.0f;
        c = QColor(qBound(0,(int)(r*rr),255), qBound(0,(int)(g*gg),255), qBound(0,(int)(b*bb),255));
    }
    update();
}

void Render3DWidget::setScale(float sx, float sy, float sz)
{ m_scaleX=sx; m_scaleY=sy; m_scaleZ=sz; update(); }

// -- setShape : dispatch vers le bon modele composite --
void Render3DWidget::setShape(const QString &type)
{
    m_vertices.clear(); m_faces.clear(); m_partColors.clear();
    m_shapeName = type;
    // Normaliser : enlever accents pour matcher
    QString t = type.trimmed().toLower();
    t.replace(QChar(0xE0),"a").replace(QChar(0xE8),"e").replace(QChar(0xE9),"e")
     .replace(QChar(0xEA),"e").replace(QChar(0xEE),"i").replace(QChar(0xF4),"o")
     .replace(QChar(0xC0),"a").replace(QChar(0xC8),"e").replace(QChar(0xC9),"e")
     .replace(QChar(0xCA),"e").replace(QChar(0xCE),"i").replace(QChar(0xD4),"o");
    // Supprimer tout caractère non-ASCII restant (accents exotiques, encodage Oracle)
    QString clean;
    for (const QChar &c : t) {
        if (c.unicode() < 128) clean += c;
        else clean += ' ';
    }
    t = clean;

    qDebug() << "[3D] setShape type=" << type << " normalized=" << t;

    if      (t.contains("sac") && t.contains("main")) buildSacAMain();
    else if (t.contains("bandoul")) buildSacBandouliere();
    else if (t.contains("tote"))    buildToteBag();
    else if (t.contains("dos"))     buildSacADos();
    else if (t.contains("portefeuille") && t.contains("long")) buildPortefeuilleLong();
    else if (t.contains("portefeuille") && t.contains("compact")) buildPortefeuilleCompact();
    else if (t.contains("porte-cartes") || t.contains("carte")) buildPorteCartes();
    else if (t.contains("ceinture")) buildCeinture();
    else if (t.contains("porte-monnaie") || t.contains("monnaie")) buildPorteMonnaie();
    else if (t.contains("pochette") || t.contains("clutch")) buildPochette();
    else if (t.contains("cle"))     buildPorteCles();
    else {
        qDebug() << "[3D] FALLBACK buildDefault() for type:" << type;
        buildDefault();
    }

    update();
}

// ===========================================================================
// Primitives : chaque appel ajoute des vertices/faces et retourne le colorIdx
// ===========================================================================

int Render3DWidget::addCuboid(float cx, float cy, float cz, float w, float h, float d, const QColor &col)
{
    int ci = m_partColors.size();
    m_partColors.append(col);
    int base = m_vertices.size();
    float hw=w/2, hh=h/2, hd=d/2;
    m_vertices.append({cx-hw, cy-hh, cz-hd});
    m_vertices.append({cx+hw, cy-hh, cz-hd});
    m_vertices.append({cx+hw, cy+hh, cz-hd});
    m_vertices.append({cx-hw, cy+hh, cz-hd});
    m_vertices.append({cx-hw, cy-hh, cz+hd});
    m_vertices.append({cx+hw, cy-hh, cz+hd});
    m_vertices.append({cx+hw, cy+hh, cz+hd});
    m_vertices.append({cx-hw, cy+hh, cz+hd});
    int b = base;
    m_faces.append({{b+0,b+1,b+2,b+3},4,ci});
    m_faces.append({{b+5,b+4,b+7,b+6},4,ci});
    m_faces.append({{b+1,b+5,b+6,b+2},4,ci});
    m_faces.append({{b+4,b+0,b+3,b+7},4,ci});
    m_faces.append({{b+3,b+2,b+6,b+7},4,ci});
    m_faces.append({{b+4,b+5,b+1,b+0},4,ci});
    return ci;
}

int Render3DWidget::addCylinder(float cx, float cy, float cz, float radius, float height, int seg, const QColor &col)
{
    int ci = m_partColors.size();
    m_partColors.append(col);
    int base = m_vertices.size();
    float hh = height / 2;
    // Top & bottom center
    m_vertices.append({cx, cy + hh, cz}); // base+0 = top center
    m_vertices.append({cx, cy - hh, cz}); // base+1 = bottom center
    // Ring vertices
    for (int i = 0; i < seg; ++i) {
        float a = 2.0f * M_PI * i / seg;
        float x = cx + radius * cosf(a);
        float z = cz + radius * sinf(a);
        m_vertices.append({x, cy + hh, z}); // base+2+i = top ring
        m_vertices.append({x, cy - hh, z}); // base+3+i = bottom ring (base+2+i+1 when stride=2)
    }
    // Side quads + top/bottom triangles
    for (int i = 0; i < seg; ++i) {
        int ni = (i + 1) % seg;
        int t0 = base + 2 + i * 2;      // top ring i
        int b0 = base + 2 + i * 2 + 1;  // bottom ring i
        int t1 = base + 2 + ni * 2;     // top ring next
        int b1 = base + 2 + ni * 2 + 1; // bottom ring next
        m_faces.append({{t0, t1, b1, b0}, 4, ci}); // side
        m_faces.append({{base, t0, t1, 0}, 3, ci}); // top cap
        m_faces.append({{base+1, b1, b0, 0}, 3, ci}); // bottom cap
    }
    return ci;
}

int Render3DWidget::addHalfCylinder(float cx, float cy, float cz, float radius, float length, int seg, const QColor &col)
{
    int ci = m_partColors.size();
    m_partColors.append(col);
    int base = m_vertices.size();
    float hl = length / 2;
    // Generate half-circle arch (from 0 to PI)
    for (int i = 0; i <= seg; ++i) {
        float a = M_PI * i / seg;
        float x = cx + radius * cosf(a);
        float y = cy + radius * sinf(a);
        m_vertices.append({x, y, cz - hl});
        m_vertices.append({x, y, cz + hl});
    }
    for (int i = 0; i < seg; ++i) {
        int i0 = base + i * 2;
        int i1 = base + i * 2 + 1;
        int i2 = base + (i + 1) * 2;
        int i3 = base + (i + 1) * 2 + 1;
        m_faces.append({{i0, i2, i3, i1}, 4, ci});
    }
    return ci;
}

int Render3DWidget::addTorus(float cx, float cy, float cz, float R, float r, int seg1, int seg2, const QColor &col)
{
    int ci = m_partColors.size();
    m_partColors.append(col);
    int base = m_vertices.size();
    for (int i = 0; i < seg1; ++i) {
        float theta = 2*M_PI*i/seg1;
        for (int j = 0; j < seg2; ++j) {
            float phi = 2*M_PI*j/seg2;
            float x = cx + (R + r*cosf(phi))*cosf(theta);
            float y = cy + r*sinf(phi);
            float z = cz + (R + r*cosf(phi))*sinf(theta);
            m_vertices.append({x,y,z});
        }
    }
    for (int i = 0; i < seg1; ++i) {
        int ni = (i+1)%seg1;
        for (int j = 0; j < seg2; ++j) {
            int nj = (j+1)%seg2;
            m_faces.append({{base+i*seg2+j, base+ni*seg2+j, base+ni*seg2+nj, base+i*seg2+nj}, 4, ci});
        }
    }
    return ci;
}

// ===========================================================================
// Modeles composites - chaque produit est assemble a partir de primitives
// ===========================================================================

void Render3DWidget::buildSacAMain()
{
    QColor base = m_baseColor;
    QColor dark = base.darker(140);
    QColor metal(200, 180, 120); // dore

    // Corps principal du sac
    addCuboid(0, 0, 0, 2.0f, 1.4f, 0.8f, base);
    // Rabat superieur
    addCuboid(0, 0.8f, -0.15f, 2.0f, 0.25f, 0.5f, dark);
    // Anse gauche (demi-cylindre)
    addHalfCylinder(-0.5f, 1.2f, 0, 0.5f, 0.12f, 10, dark);
    // Anse droite
    addHalfCylinder(0.5f, 1.2f, 0, 0.5f, 0.12f, 10, dark);
    // Fermoir central (petit cylindre dore)
    addCylinder(0, 0.85f, -0.42f, 0.08f, 0.06f, 8, metal);
    // Pieds du sac (4 petits cylindres)
    addCylinder(-0.7f, -0.75f, -0.25f, 0.06f, 0.06f, 6, metal);
    addCylinder( 0.7f, -0.75f, -0.25f, 0.06f, 0.06f, 6, metal);
    addCylinder(-0.7f, -0.75f,  0.25f, 0.06f, 0.06f, 6, metal);
    addCylinder( 0.7f, -0.75f,  0.25f, 0.06f, 0.06f, 6, metal);
    // Poche avant
    addCuboid(0, -0.15f, -0.42f, 1.2f, 0.7f, 0.04f, dark);
}

void Render3DWidget::buildSacBandouliere()
{
    QColor base = m_baseColor;
    QColor dark = base.darker(140);
    QColor metal(200, 180, 120);

    // Corps
    addCuboid(0, 0, 0, 1.6f, 1.1f, 0.5f, base);
    // Rabat
    addCuboid(0, 0.65f, -0.1f, 1.6f, 0.2f, 0.35f, dark);
    // Bandouliere (longue bande diagonale)
    addCuboid(-1.0f, 0.8f, 0, 0.12f, 0.06f, 0.5f, dark);
    addCuboid(-1.3f, 1.2f, 0, 0.7f, 0.06f, 0.12f, dark);
    addCuboid(-1.6f, 1.6f, 0, 0.12f, 0.06f, 0.5f, dark);
    // Fermoir
    addCylinder(0, 0.55f, -0.28f, 0.06f, 0.05f, 8, metal);
    // Poche avant
    addCuboid(0, -0.1f, -0.27f, 0.9f, 0.5f, 0.03f, dark);

}

void Render3DWidget::buildToteBag()
{
    QColor base = m_baseColor;
    QColor dark = base.darker(130);

    // Corps large et ouvert
    addCuboid(0, 0, 0, 2.4f, 1.8f, 0.9f, base);
    // Bords renforces
    addCuboid(0, 0.9f, 0, 2.5f, 0.08f, 0.95f, dark);
    addCuboid(0, -0.9f, 0, 2.5f, 0.08f, 0.95f, dark);
    // Anse gauche
    addHalfCylinder(-0.6f, 1.3f, 0, 0.55f, 0.1f, 10, dark);
    // Anse droite
    addHalfCylinder(0.6f, 1.3f, 0, 0.55f, 0.1f, 10, dark);
    // Poche interieure visible
    addCuboid(0, 0.2f, -0.47f, 1.4f, 0.9f, 0.03f, dark);
}

void Render3DWidget::buildSacADos()
{
    QColor base = m_baseColor;
    QColor dark = base.darker(140);
    QColor metal(200, 180, 120);

    // Corps principal (plus haut que large)
    addCuboid(0, 0, 0, 1.4f, 2.0f, 0.7f, base);
    // Rabat superieur arrondi
    addCuboid(0, 1.1f, -0.1f, 1.4f, 0.2f, 0.5f, dark);
    // Poche avant
    addCuboid(0, -0.2f, -0.38f, 1.0f, 0.9f, 0.12f, dark);
    // Bretelle gauche
    addCuboid(-0.55f, 0.3f, 0.38f, 0.15f, 1.6f, 0.06f, dark);
    // Bretelle droite
    addCuboid(0.55f, 0.3f, 0.38f, 0.15f, 1.6f, 0.06f, dark);
    // Poignee superieure
    addHalfCylinder(0, 1.35f, 0, 0.25f, 0.15f, 8, dark);
    // Fermeture eclair (ligne)
    addCuboid(0, 1.0f, -0.36f, 0.8f, 0.03f, 0.03f, metal);
}

void Render3DWidget::buildPortefeuilleLong()
{
    QColor base = m_baseColor;
    QColor dark = base.darker(130);
    QColor metal(200, 180, 120);

    // Corps plat et long
    addCuboid(0, 0, 0, 1.8f, 0.8f, 0.12f, base);
    // Rabat
    addCuboid(0, 0.45f, 0, 1.8f, 0.1f, 0.12f, dark);
    // Compartiments interieurs (lignes)
    addCuboid(-0.4f, 0, 0.065f, 0.02f, 0.6f, 0.02f, dark);
    addCuboid(0.2f, 0, 0.065f, 0.02f, 0.6f, 0.02f, dark);
    addCuboid(0.6f, 0, 0.065f, 0.02f, 0.6f, 0.02f, dark);
    // Bouton pression
    addCylinder(0, 0.42f, -0.07f, 0.04f, 0.03f, 8, metal);
    // Couture decorative
    addCuboid(0, 0, -0.065f, 1.6f, 0.02f, 0.01f, dark);
}

void Render3DWidget::buildPortefeuilleCompact()
{
    QColor base = m_baseColor;
    QColor dark = base.darker(130);
    QColor metal(200, 180, 120);

    addCuboid(0, 0, 0, 1.1f, 0.8f, 0.12f, base);
    addCuboid(0, 0.45f, 0, 1.1f, 0.08f, 0.12f, dark);
    // Fentes cartes
    addCuboid(-0.25f, 0.05f, 0.065f, 0.02f, 0.5f, 0.02f, dark);
    addCuboid(0.15f, 0.05f, 0.065f, 0.02f, 0.5f, 0.02f, dark);
    addCylinder(0, 0.42f, -0.07f, 0.035f, 0.03f, 8, metal);
}

void Render3DWidget::buildPorteCartes()
{
    QColor base = m_baseColor;
    QColor dark = base.darker(120);

    addCuboid(0, 0, 0, 0.9f, 0.6f, 0.06f, base);
    // Fentes pour cartes (3 niveaux)
    addCuboid(0, 0.15f, -0.035f, 0.75f, 0.12f, 0.01f, dark);
    addCuboid(0, 0.0f,  -0.035f, 0.75f, 0.12f, 0.01f, dark);
    addCuboid(0, -0.15f,-0.035f, 0.75f, 0.12f, 0.01f, dark);
}

void Render3DWidget::buildCeinture()
{
    QColor base = m_baseColor;
    QColor dark = base.darker(150);
    QColor metal(210, 190, 130);

    // Boucle (tore aplati)
    addTorus(0, 0, 0, 0.25f, 0.04f, 16, 8, metal);
    // Ardillon (barre centrale de la boucle)
    addCylinder(0, 0, 0, 0.015f, 0.4f, 6, metal);
    // Sangle cuir (longue bande)
    addCuboid(1.2f, 0, 0, 2.0f, 0.25f, 0.04f, base);
    // Passant 1
    addCuboid(0.35f, 0, 0, 0.08f, 0.3f, 0.06f, dark);
    // Passant 2
    addCuboid(0.55f, 0, 0, 0.08f, 0.3f, 0.06f, dark);
    // Trous
    for (int i = 0; i < 5; ++i)
        addCylinder(1.0f + i * 0.2f, 0, 0, 0.02f, 0.05f, 6, dark);
    // Bout arrondi
    addCylinder(2.25f, 0, 0, 0.125f, 0.04f, 10, base);
}

void Render3DWidget::buildPorteMonnaie()
{
    QColor base = m_baseColor;
    QColor dark = base.darker(130);
    QColor metal(200, 180, 120);

    addCuboid(0, 0, 0, 0.9f, 0.65f, 0.2f, base);
    // Fermoir clip
    addCuboid(0, 0.38f, 0, 0.3f, 0.06f, 0.22f, metal);
    // Couture
    addCuboid(0, 0, -0.105f, 0.75f, 0.5f, 0.01f, dark);
    // Soufflet lateral
    addCuboid(-0.46f, 0, 0, 0.02f, 0.5f, 0.18f, dark);
    addCuboid(0.46f, 0, 0, 0.02f, 0.5f, 0.18f, dark);
}

void Render3DWidget::buildPochette()
{
    QColor base = m_baseColor;
    QColor dark = base.darker(130);
    QColor metal(210, 190, 130);

    addCuboid(0, 0, 0, 1.8f, 0.9f, 0.15f, base);
    // Rabat triangulaire
    addCuboid(0, 0.55f, -0.03f, 1.8f, 0.2f, 0.1f, dark);
    // Fermoir elegant
    addCylinder(0, 0.5f, -0.1f, 0.05f, 0.04f, 8, metal);
    // Dragonne
    addCuboid(0.95f, -0.2f, 0, 0.06f, 0.4f, 0.04f, dark);
    addCylinder(0.95f, -0.42f, 0, 0.04f, 0.06f, 6, metal);
}

void Render3DWidget::buildPorteCles()
{
    QColor base = m_baseColor;
    QColor dark = base.darker(140);
    QColor metal(210, 190, 130);

    // Petit etui
    addCuboid(0, 0, 0, 0.5f, 0.7f, 0.15f, base);
    // Bouton pression
    addCylinder(0, 0.4f, -0.08f, 0.04f, 0.03f, 8, metal);
    // Anneau porte-cles
    addTorus(0, 0.6f, 0, 0.15f, 0.02f, 12, 6, metal);
    // Couture
    addCuboid(0, 0, -0.08f, 0.35f, 0.5f, 0.01f, dark);
}

void Render3DWidget::buildDefault()
{
    addCuboid(0, 0, 0, 1.5f, 1.0f, 0.5f, m_baseColor);
}

// ===========================================================================
// Projection, rotation, rendu
// ===========================================================================

Render3DWidget::Point3D Render3DWidget::rotate(Point3D p)
{
    p.x *= m_scaleX; p.y *= m_scaleY; p.z *= m_scaleZ;
    float ry = m_rotY * M_PI / 180.0f;
    float x1 = p.x*cosf(ry) + p.z*sinf(ry);
    float z1 = -p.x*sinf(ry) + p.z*cosf(ry);
    p.x = x1; p.z = z1;
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
    for (int i = 0; i < f.nv; ++i) z += rotate(m_vertices[f.v[i]]).z;
    return z / f.nv;
}

void Render3DWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);

    // Fond clair elegant
    QRadialGradient bg(width()/2, height()/2, qMax(width(),height())*0.7);
    bg.setColorAt(0, QColor(245, 240, 235));
    bg.setColorAt(0.6, QColor(230, 220, 210));
    bg.setColorAt(1, QColor(210, 200, 190));
    p.fillRect(rect(), bg);

    // Grille de sol subtile
    p.setPen(QPen(QColor(0,0,0,20), 1));
    int cy = height() * 0.72;
    for (int i = -10; i <= 10; ++i) {
        int x = width()/2 + i * 40;
        p.drawLine(x, cy - 20, x, cy + 20);
    }
    p.drawLine(0, cy, width(), cy);

    // Mode texture : afficher l'image generee en 3D
    if (m_textureMode && !m_texture.isNull()) {
        paintTextureMode(p);
        return;
    }

    if (m_vertices.isEmpty() || m_faces.isEmpty()) return;

    // Tri par profondeur
    QVector<int> order(m_faces.size());
    for (int i = 0; i < m_faces.size(); ++i) order[i] = i;
    std::sort(order.begin(), order.end(), [this](int a, int b) {
        return faceDepth(m_faces[a]) < faceDepth(m_faces[b]);
    });

    // Ombre au sol
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(0, 0, 0, 25));
    p.drawEllipse(QPointF(width()/2 + 10, height()*0.72), 120 * m_zoom, 20 * m_zoom);

    for (int idx : order) {
        const Face &f = m_faces[idx];
        QPolygonF poly;
        Point3D rotated[4];
        for (int i = 0; i < f.nv; ++i) {
            rotated[i] = rotate(m_vertices[f.v[i]]);
            poly << project(rotated[i]);
        }

        // Normale pour eclairage multi-sources
        float nx=0, ny=0, nz=0;
        if (f.nv >= 3) {
            float ax=rotated[1].x-rotated[0].x, ay=rotated[1].y-rotated[0].y, az=rotated[1].z-rotated[0].z;
            float bx=rotated[2].x-rotated[0].x, by=rotated[2].y-rotated[0].y, bz=rotated[2].z-rotated[0].z;
            nx=ay*bz-az*by; ny=az*bx-ax*bz; nz=ax*by-ay*bx;
            float len=sqrtf(nx*nx+ny*ny+nz*nz);
            if (len>0){nx/=len;ny/=len;nz/=len;}
        }

        // Eclairage : lumiere principale + lumiere d'appoint + ambiance
        float mainLight = qMax(0.0f, nx*0.2f + ny*0.5f + nz*0.8f);
        float fillLight = qMax(0.0f, -nx*0.3f + ny*0.3f + nz*0.1f) * 0.3f;
        float rimLight  = qMax(0.0f, -nz * 0.4f) * 0.15f;
        float light = 0.25f + 0.55f * mainLight + fillLight + rimLight;
        light = qBound(0.18f, light, 1.05f);

        // Couleur de la piece avec eclairage
        QColor partCol = (f.colorIdx >= 0 && f.colorIdx < m_partColors.size())
                         ? m_partColors[f.colorIdx] : m_baseColor;

        int fr = qBound(0, (int)(partCol.red()   * light), 255);
        int fg = qBound(0, (int)(partCol.green() * light), 255);
        int fb = qBound(0, (int)(partCol.blue()  * light), 255);
        QColor faceColor(fr, fg, fb);

        // Contour fin et subtil
        p.setPen(QPen(faceColor.darker(115), 0.5));
        p.setBrush(faceColor);
        p.drawPolygon(poly);

        // Reflet speculaire sur les faces bien orientees (metal/fermoirs)
        if (light > 0.85f && partCol.lightness() > 150) {
            QColor spec(255, 255, 255, (int)((light - 0.85f) * 200));
            p.setBrush(spec);
            p.setPen(Qt::NoPen);
            p.drawPolygon(poly);
        }
    }

    // Label produit
    p.setPen(QColor(120, 100, 80, 180));
    p.setFont(QFont("Segoe UI", 9));
    p.drawText(10, height()-10,
        QString("Glisser pour tourner  |  Molette pour zoomer  |  %1x")
        .arg(QString::number(m_zoom,'f',1)));

    // Nom du modele en haut
    if (!m_shapeName.isEmpty()) {
        p.setPen(QColor(141, 110, 99));
        p.setFont(QFont("Segoe UI", 11, QFont::Bold));
        p.drawText(rect().adjusted(0, 8, 0, 0), Qt::AlignHCenter | Qt::AlignTop, m_shapeName);
    }
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

// ── Rendu mode texture : image 3D avec rotation/zoom ──────────────────────────
void Render3DWidget::paintTextureMode(QPainter &p)
{
    // Calculer la taille de l'image avec zoom
    float baseSize = qMin(width(), height()) * 0.7f * m_zoom;
    float imgW = baseSize * m_scaleX;
    float imgH = baseSize * m_scaleY;
    
    // Appliquer rotation Y (effet de perspective horizontale)
    float rotYRad = m_rotY * M_PI / 180.0f;
    float perspectiveX = cosf(rotYRad);  // Compression horizontale
    float skewFactor = sinf(rotYRad) * 0.3f;  // Effet de cisaillement
    
    // Appliquer rotation X (effet de perspective verticale)
    float rotXRad = m_rotX * M_PI / 180.0f;
    float perspectiveY = cosf(rotXRad);  // Compression verticale
    
    // Dimensions finales avec perspective
    float finalW = imgW * qAbs(perspectiveX);
    float finalH = imgH * qAbs(perspectiveY);
    
    // Limites minimales
    finalW = qMax(50.0f, finalW);
    finalH = qMax(50.0f, finalH);
    
    // Position centrale
    float cx = width() / 2.0f;
    float cy = height() / 2.0f - 20;
    
    // Ombre au sol
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(0, 0, 0, 30));
    p.drawEllipse(QPointF(cx + 10, height() * 0.75), finalW * 0.4, 15 * m_zoom);
    
    // Transformer l'image
    p.save();
    p.translate(cx, cy);
    
    // Appliquer les transformations de perspective
    QTransform transform;
    transform.scale(perspectiveX > 0 ? 1 : -1, perspectiveY > 0 ? 1 : -1);
    transform.shear(skewFactor, rotXRad * 0.1f);
    p.setTransform(transform, true);
    
    // Dessiner l'image avec ombre portee
    QPixmap scaled = m_texture.scaled(finalW, finalH, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    
    // Ombre de l'image
    p.setOpacity(0.15);
    p.drawPixmap(-scaled.width()/2 + 5, -scaled.height()/2 + 5, scaled);
    
    // Image principale
    p.setOpacity(1.0);
    
    // Effet d'eclairage base sur la rotation
    float lightFactor = 0.7f + 0.3f * qMax(0.0f, cosf(rotYRad) * cosf(rotXRad));
    if (lightFactor < 1.0f) {
        // Assombrir legerement si tourne
        QPixmap darkened = scaled;
        QPainter dp(&darkened);
        dp.setCompositionMode(QPainter::CompositionMode_SourceAtop);
        dp.fillRect(darkened.rect(), QColor(0, 0, 0, (int)((1.0f - lightFactor) * 80)));
        dp.end();
        p.drawPixmap(-darkened.width()/2, -darkened.height()/2, darkened);
    } else {
        p.drawPixmap(-scaled.width()/2, -scaled.height()/2, scaled);
    }
    
    // Cadre elegant autour de l'image
    p.setPen(QPen(QColor(141, 110, 99, 100), 2));
    p.setBrush(Qt::NoBrush);
    p.drawRoundedRect(-scaled.width()/2 - 3, -scaled.height()/2 - 3, 
                      scaled.width() + 6, scaled.height() + 6, 8, 8);
    
    p.restore();
    
    // Label
    p.setPen(QColor(120, 100, 80, 180));
    p.setFont(QFont("Segoe UI", 9));
    p.drawText(10, height()-10,
        QString("Glisser pour tourner  |  Molette pour zoomer  |  %1x  |  Image IA")
        .arg(QString::number(m_zoom,'f',1)));
    
    // Nom du modele
    if (!m_shapeName.isEmpty()) {
        p.setPen(QColor(93, 64, 55));
        p.setFont(QFont("Segoe UI", 11, QFont::Bold));
        p.drawText(rect().adjusted(0, 8, 0, 0), Qt::AlignHCenter | Qt::AlignTop, m_shapeName);
    }
}

// ===========================================================================
// ArticleViewer3D - widget complet avec controles + IA Groq
// ===========================================================================

ArticleViewer3D::ArticleViewer3D(QWidget *parent) : QWidget(parent),
    m_currentR(141), m_currentG(110), m_currentB(99),
    m_currentLarg(1), m_currentHaut(1), m_currentProf(0.5),
    m_currentPrix(0)
{
    m_networkAI = new QNetworkAccessManager(this);
    connect(m_networkAI, &QNetworkAccessManager::finished, this, &ArticleViewer3D::onAIReplyFinished);
    setupUI();
    loadModelForType("Sac a main");
}

void ArticleViewer3D::setupUI()
{
    auto *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0); mainLayout->setSpacing(8);

    m_render = new Render3DWidget(this);
    mainLayout->addWidget(m_render, 3);

    // Panneau controle avec scroll
    auto *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("QScrollArea{border:none;background:#FAF5F0;}");

    auto *ctrl = new QWidget();
    ctrl->setStyleSheet("QWidget{background:#FAF5F0;border-radius:10px;}"
        "QLabel{color:#3E2723;font-size:11px;}"
        "QGroupBox{border:2px solid #8D6E63;border-radius:8px;"
        "margin-top:8px;padding-top:8px;font-weight:bold;color:#5D4037;background:#FFFFFF;}"
        "QSlider::groove:horizontal{background:#D7CCC8;height:6px;border-radius:3px;}"
        "QSlider::handle:horizontal{background:#8D6E63;width:14px;height:14px;"
        "border-radius:7px;margin:-4px 0;}"
        "QSlider::sub-page:horizontal{background:#8D6E63;border-radius:3px;}"
        "QPushButton{background:#8D6E63;color:white;border:none;border-radius:6px;"
        "padding:8px 16px;font-weight:bold;}"
        "QPushButton:hover{background:#A0826D;}");
    auto *ctrlLay = new QVBoxLayout(ctrl);
    ctrlLay->setContentsMargins(10,10,10,10); ctrlLay->setSpacing(8);

    m_lblModelName = new QLabel("3D: Modele");
    m_lblModelName->setStyleSheet("font-size:13px;font-weight:bold;color:#5D4037;padding:5px;");
    m_lblModelName->setAlignment(Qt::AlignCenter);
    ctrlLay->addWidget(m_lblModelName);

    // -- Couleur --
    auto *colorBox = new QGroupBox("  Couleur");
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

    auto *btnPicker = new QPushButton("Selectionner une couleur");
    connect(btnPicker, &QPushButton::clicked, [this]() {
        QColor c = QColorDialog::getColor(QColor(m_sliderR->value(),m_sliderG->value(),m_sliderB->value()), this, "Selectionner une couleur");
        if (c.isValid()) { m_sliderR->setValue(c.red()); m_sliderG->setValue(c.green()); m_sliderB->setValue(c.blue()); }
    });
    colorLay->addWidget(btnPicker);
    ctrlLay->addWidget(colorBox);

    // -- Dimensions --
    auto *dimBox = new QGroupBox("  Dimensions");
    auto *dimLay = new QVBoxLayout(dimBox);
    m_lblDimensions = new QLabel("— × — × — cm");
    m_lblDimensions->setAlignment(Qt::AlignCenter);
    m_lblDimensions->setStyleSheet("font-size:12px;font-weight:bold;color:#5D4037;padding:4px;");
    dimLay->addWidget(m_lblDimensions);

    auto makeDS = [&](const QString &lbl, QSlider *&s, float def) {
        auto *row = new QHBoxLayout();
        auto *l = new QLabel(lbl); l->setFixedWidth(70);
        s = new QSlider(Qt::Horizontal); s->setRange(10,300); s->setValue((int)(def*100));
        auto *v = new QLabel(QString::number(def,'f',1)); v->setFixedWidth(30);
        connect(s, &QSlider::valueChanged, [this,v](int val) {
            v->setText(QString::number(val/100.0,'f',1));
            m_render->setScale(m_sliderW->value()/100.0f, m_sliderH->value()/100.0f, m_sliderD->value()/100.0f);
            // Afficher en cm (×10)
            m_lblDimensions->setText(QString("%1 × %2 × %3 cm")
                .arg(m_sliderW->value()/10.0,0,'f',1)
                .arg(m_sliderH->value()/10.0,0,'f',1)
                .arg(m_sliderD->value()/10.0,0,'f',1));
        });
        row->addWidget(l); row->addWidget(s); row->addWidget(v);
        dimLay->addLayout(row);
    };
    makeDS("Largeur", m_sliderW, 1.0f);
    makeDS("Hauteur", m_sliderH, 1.0f);
    makeDS("Profondeur", m_sliderD, 1.0f);

    auto *btnReset = new QPushButton("Reset");
    connect(btnReset, &QPushButton::clicked, [this]() {
        m_sliderW->setValue(100); m_sliderH->setValue(100); m_sliderD->setValue(100);
        m_sliderR->setValue(141); m_sliderG->setValue(110); m_sliderB->setValue(99);
    });
    dimLay->addWidget(btnReset);
    
    // Bouton pour basculer entre vue 3D geometrique et image IA
    m_btnToggleImage = new QPushButton("Voir Image IA");
    m_btnToggleImage->setStyleSheet(
        "QPushButton{background:#5D4037;color:white;border:none;border-radius:6px;"
        "padding:8px 16px;font-weight:bold;}"
        "QPushButton:hover{background:#8D6E63;}"
        "QPushButton:disabled{background:#BCAAA4;color:#888;}");
    m_btnToggleImage->setEnabled(false);  // Desactive par defaut (pas d'image)
    connect(m_btnToggleImage, &QPushButton::clicked, [this]() {
        if (m_render->property("textureMode").toBool()) {
            // Revenir a la vue 3D geometrique
            m_render->clearTexture();
            m_render->setProperty("textureMode", false);
            m_btnToggleImage->setText("Voir Image IA");
            m_lblModelName->setText("3D: " + m_currentType);
        } else {
            // Afficher l'image IA
            if (!m_imagePath.isEmpty() && QFile::exists(m_imagePath)) {
                m_render->setTexture(m_imagePath);
                m_render->setProperty("textureMode", true);
                m_btnToggleImage->setText("Voir 3D");
                m_lblModelName->setText("Image IA: " + m_currentType);
            }
        }
    });
    dimLay->addWidget(m_btnToggleImage);
    
    ctrlLay->addWidget(dimBox);

    // IA widgets (caches, utilises en interne)
    m_aiDescription = new QTextEdit();
    m_aiDescription->setReadOnly(true);
    m_aiDescription->setVisible(false);
    m_aiStatus = new QLabel("");
    m_aiStatus->setVisible(false);
    m_btnGenerateAI = new QPushButton();
    m_btnGenerateAI->setVisible(false);
    connect(m_btnGenerateAI, &QPushButton::clicked, this, &ArticleViewer3D::onGenerateAI3D);

    ctrlLay->addStretch();

    scrollArea->setWidget(ctrl);
    mainLayout->addWidget(scrollArea, 1);

    setColor(141, 110, 99);
}

void ArticleViewer3D::loadModelForType(const QString &type)
{
    m_currentType = type;
    m_render->setShape(type);
    m_lblModelName->setText("📦 " + type);
}

void ArticleViewer3D::setArticleInfo(const QString &nom, const QString &type, const QString &categorie,
                                      int r, int g, int b, double larg, double haut, double prof,
                                      double prix, const QString &statut)
{
    m_currentNom = nom;
    m_currentType = type;
    m_currentCategorie = categorie;
    m_currentR = r; m_currentG = g; m_currentB = b;
    m_currentLarg = larg; m_currentHaut = haut; m_currentProf = prof;
    m_currentPrix = prix;
    m_currentStatut = statut;

    loadModelForType(type);
    setColor(r, g, b);
    setDimensions(larg, haut, prof);
}

void ArticleViewer3D::setImagePath(const QString &path)
{
    m_imagePath = path;
    if (!path.isEmpty() && QFile::exists(path)) {
        // Activer le bouton pour permettre de voir l'image
        m_btnToggleImage->setEnabled(true);
        m_btnToggleImage->setText("Voir Image IA");
    } else {
        m_btnToggleImage->setEnabled(false);
        m_render->clearTexture();
    }
}

void ArticleViewer3D::generateAuto()
{
    m_aiStatus->setText("Chargement du modele 3D + IA...");
    m_aiStatus->setStyleSheet("color:#FFCC80;font-size:10px;");
    QTimer::singleShot(300, this, &ArticleViewer3D::onGenerateAI3D);
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
    // Les dimensions viennent en cm de la BD (ex: 30, 25, 10)
    // On normalise pour le rendu 3D (echelle 0.1 a 3.0)
    // Facteur de normalisation : diviser par 10 pour avoir une echelle raisonnable
    float normW = qBound(0.1f, w / 10.0f, 3.0f);
    float normH = qBound(0.1f, h / 10.0f, 3.0f);
    float normD = qBound(0.1f, d / 10.0f, 3.0f);
    
    m_sliderW->setValue((int)(normW * 100)); 
    m_sliderH->setValue((int)(normH * 100)); 
    m_sliderD->setValue((int)(normD * 100));
    m_render->setScale(normW, normH, normD);
    
    // Afficher les dimensions reelles en cm
    m_lblDimensions->setText(QString("%1 x %2 x %3 cm")
        .arg(w, 0, 'f', 1)
        .arg(h, 0, 'f', 1)
        .arg(d, 0, 'f', 1));
    
    emit dimensionsChanged(normW, normH, normD);
}

void ArticleViewer3D::updateColorPreview()
{
    m_colorPreview->setStyleSheet(
        QString("background:rgb(%1,%2,%3);border-radius:6px;")
        .arg(m_sliderR->value()).arg(m_sliderG->value()).arg(m_sliderB->value()));
}

// ===========================================================================
// Generation IA 3D via API Groq
// ===========================================================================

void ArticleViewer3D::onGenerateAI3D()
{
    QString apiKey = EnvLoader::get("GROQ_API_KEY");
    qDebug() << "[IA 3D] Cle API:" << (apiKey.isEmpty() ? "VIDE" : "OK len=" + QString::number(apiKey.length()));

    if (apiKey.isEmpty()) {
        m_aiStatus->setText("GROQ_API_KEY manquante dans .env");
        m_aiStatus->setStyleSheet("color:#EF5350;font-size:10px;");
        return;
    }

    m_btnGenerateAI->setEnabled(false);
    m_aiStatus->setText("L'IA analyse le produit...");
    m_aiStatus->setStyleSheet("color:#FFCC80;font-size:10px;");
    m_aiDescription->clear();

    QString prompt = QString(
        "Tu es un expert en maroquinerie de luxe. "
        "Voici un article en cuir de notre catalogue :\n"
        "- Nom : %1\n- Type : %2\n- Categorie : %3\n"
        "- Couleur : RGB(%4,%5,%6)\n- Dimensions : %7x%8x%9 cm\n"
        "- Prix : %10 DT\n- Statut : %11\n\n"
        "Decris ce produit en detail (matiere cuir, finitions, coutures, texture, style) "
        "et propose une couleur cuir realiste optimale.\n\n"
        "Reponds UNIQUEMENT en JSON :\n"
        "{\"description\":\"...\",\"couleur_r\":<0-255>,\"couleur_g\":<0-255>,"
        "\"couleur_b\":<0-255>,\"couleur_nom\":\"...\","
        "\"largeur\":<0.5-3.0>,\"hauteur\":<0.5-3.0>,\"profondeur\":<0.1-2.0>,"
        "\"matiere\":\"...\",\"style\":\"...\"}"
    ).arg(m_currentNom, m_currentType, m_currentCategorie)
     .arg(m_currentR).arg(m_currentG).arg(m_currentB)
     .arg(m_currentLarg,0,'f',1).arg(m_currentHaut,0,'f',1).arg(m_currentProf,0,'f',1)
     .arg(m_currentPrix,0,'f',2).arg(m_currentStatut);

    QJsonArray messages;
    QJsonObject sys; sys["role"]="system"; sys["content"]="Expert maroquinerie. JSON uniquement.";
    QJsonObject usr; usr["role"]="user"; usr["content"]=prompt;
    messages.append(sys); messages.append(usr);

    QJsonObject body;
    body["model"] = "llama-3.3-70b-versatile";
    body["messages"] = messages;
    body["temperature"] = 0.7;
    body["max_tokens"] = 500;

    QNetworkRequest req(QUrl("https://api.groq.com/openai/v1/chat/completions"));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("Authorization", ("Bearer " + apiKey).toUtf8());

    m_networkAI->post(req, QJsonDocument(body).toJson());
    qDebug() << "[IA 3D] Requete envoyee pour:" << m_currentNom << m_currentType;
}

void ArticleViewer3D::onAIReplyFinished(QNetworkReply *reply)
{
    m_btnGenerateAI->setEnabled(true);
    reply->deleteLater();

    int httpCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug() << "[IA 3D] HTTP:" << httpCode << "Erreur:" << reply->error();

    if (reply->error() != QNetworkReply::NoError) {
        m_aiStatus->setText(QString("Erreur HTTP %1 : %2").arg(httpCode).arg(reply->errorString()));
        m_aiStatus->setStyleSheet("color:#EF5350;font-size:10px;");
        return;
    }

    QByteArray data = reply->readAll();
    qDebug() << "[IA 3D] Reponse:" << data.left(300);

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) { m_aiStatus->setText("Reponse invalide"); return; }

    QJsonArray choices = doc.object()["choices"].toArray();
    if (choices.isEmpty()) { m_aiStatus->setText("Pas de reponse"); return; }

    QString content = choices[0].toObject()["message"].toObject()["content"].toString();
    int js = content.indexOf('{'), je = content.lastIndexOf('}');
    if (js < 0 || je < 0) {
        m_aiStatus->setText("JSON non trouve");
        m_aiDescription->setText(content);
        return;
    }

    QJsonDocument pd = QJsonDocument::fromJson(content.mid(js, je-js+1).toUtf8());
    if (!pd.isObject()) {
        m_aiStatus->setText("JSON invalide");
        m_aiDescription->setText(content);
        return;
    }

    applyAIParameters(pd.object());
}

void ArticleViewer3D::applyAIParameters(const QJsonObject &p)
{
    int r = qBound(0, p["couleur_r"].toInt(141), 255);
    int g = qBound(0, p["couleur_g"].toInt(110), 255);
    int b = qBound(0, p["couleur_b"].toInt(99), 255);
    setColor(r, g, b);

    float larg = qBound(0.1f, (float)p["largeur"].toDouble(1.0), 3.0f);
    float haut = qBound(0.1f, (float)p["hauteur"].toDouble(1.0), 3.0f);
    float prof = qBound(0.1f, (float)p["profondeur"].toDouble(0.5), 3.0f);
    setDimensions(larg, haut, prof);

    QString txt = QString(
        "Couleur: %1\n"
        "Dimensions: %2 x %3 x %4\n"
        "Matiere: %5\n"
        "Style: %6\n"
        "Description: %7"
    ).arg(p["couleur_nom"].toString())
     .arg(larg,0,'f',1).arg(haut,0,'f',1).arg(prof,0,'f',1)
     .arg(p["matiere"].toString(), p["style"].toString(), p["description"].toString());

    m_aiDescription->setText(txt);
    m_aiStatus->setText("Modele 3D genere par IA !");
    m_aiStatus->setStyleSheet("color:#66BB6A;font-size:10px;font-weight:bold;");
}
