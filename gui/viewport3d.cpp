#include "gui/viewport3d.h"
#include "core/transformations.h"

#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QWheelEvent>
#include <cmath>
#include <algorithm>

Viewport3D::Viewport3D(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(400, 400);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void Viewport3D::updateTransform(double roll, double pitch, double yaw,
                                  double tx, double ty, double tz)
{
    m_interpMode = false;
    auto R = tf::eulerToRotation(roll, pitch, yaw);
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            m_R[i][j] = R[i][j];
    m_tx = tx;
    m_ty = ty;
    m_tz = tz;
    update();
}

void Viewport3D::updateWaypoints(const std::vector<tf::Mat4> &waypoints,
                                  const tf::Mat4 &T_current, int segIndex)
{
    m_interpMode = true;
    m_waypoints = waypoints;
    m_T_interpCurrent = T_current;
    m_wpSegIndex = segIndex;

    auto [R, t] = tf::decomposeHomogeneous(T_current);
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            m_R[i][j] = R[i][j];
    m_tx = t[0]; m_ty = t[1]; m_tz = t[2];
    update();
}

void Viewport3D::clearInterp()
{
    m_interpMode = false;
    update();
}

// 3‑D to 2‑D projection (orthographic projection)

QPointF Viewport3D::project(double x, double y, double z) const
{
    const double az = m_azimuth * M_PI / 180.0;
    const double el = m_elevation * M_PI / 180.0;

    // Eye direction (unit vector from origin toward camera)
    const double ex = std::cos(el) * std::cos(az);
    const double ey = std::cos(el) * std::sin(az);
    const double ez = std::sin(el);

    // Forward = -eye (into the scene)
    const double fx = -ex, fy = -ey, fz = -ez;

    // Right = forward × world‑up (Z)
    double rx = fy * 1.0 - fz * 0.0;  // fy·uz − fz·uy
    double ry = fz * 0.0 - fx * 1.0;  // fz·ux − fx·uz
    double rz = fx * 0.0 - fy * 0.0;  // fx·uy − fy·ux
    const double rlen = std::sqrt(rx * rx + ry * ry + rz * rz);
    if (rlen > 1e-10) { rx /= rlen; ry /= rlen; rz /= rlen; }

    // Camera up = right × forward
    const double cux = ry * fz - rz * fy;
    const double cuy = rz * fx - rx * fz;
    const double cuz = rx * fy - ry * fx;

    // Project onto camera plane
    const double vx = x * rx  + y * ry  + z * rz;
    const double vy = x * cux + y * cuy + z * cuz;

    const double scale = m_zoom * std::min(width(), height()) / 8.0;
    const double cx = width()  / 2.0;
    const double cy = height() / 2.0;

    return {cx + vx * scale, cy - vy * scale};
}


// Viewport Painter

void Viewport3D::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Background
    p.fillRect(rect(), QColor(43, 43, 43));

    // Title
    p.setPen(Qt::white);
    p.setFont(QFont("Segoe UI", 13, QFont::Bold));
    p.drawText(rect().adjusted(0, 12, 0, 0),
               Qt::AlignHCenter | Qt::AlignTop,
               QStringLiteral("Robot Motion Viewer"));

    // Grid on Z = 0
    drawGrid(p);

    // Base coordinate frame display in dashed and semi‑transparent
    const double I[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
    drawFrame(p, 0, 0, 0, I, 1.5, 90, true, QString());

    if (m_interpMode) {
        // Draw ghost frames for each waypoint
        for (size_t w = 0; w < m_waypoints.size(); ++w) {
            auto [Rw, tw] = tf::decomposeHomogeneous(m_waypoints[w]);
            double Rw3[3][3];
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                    Rw3[i][j] = Rw[i][j];

            int alpha = (static_cast<int>(w) == m_wpSegIndex ||
                         static_cast<int>(w) == m_wpSegIndex + 1) ? 110 : 60;
            drawFrame(p, tw[0], tw[1], tw[2], Rw3, 1.2, alpha, true,
                      QString("#%1 ").arg(w + 1));
        }

        // Draw path lines connecting consecutive waypoints
        QPen pathPen(QColor(255, 255, 100, 140), 1.5);
        pathPen.setDashPattern({4, 4});
        p.setPen(pathPen);
        for (size_t w = 0; w + 1 < m_waypoints.size(); ++w) {
            auto [R1, t1] = tf::decomposeHomogeneous(m_waypoints[w]);
            auto [R2, t2] = tf::decomposeHomogeneous(m_waypoints[w + 1]);
            p.drawLine(project(t1[0], t1[1], t1[2]),
                       project(t2[0], t2[1], t2[2]));
        }
    }

    // Trail from origin to translated origin
    const double tn = std::sqrt(m_tx * m_tx + m_ty * m_ty + m_tz * m_tz);
    if (tn > 0.01)
        drawTrail(p, m_tx, m_ty, m_tz);

    // Transformed coordinate frame (solid, fully opaque)
    drawFrame(p, m_tx, m_ty, m_tz, m_R, 1.5, 255, false,
              QStringLiteral("'"));
}

void Viewport3D::drawGrid(QPainter &p)
{
    QPen pen(QColor(68, 68, 68), 0.5);
    p.setPen(pen);

    for (int i = -10; i <= 10; ++i) {
        p.drawLine(project(-10, i, 0), project(10, i, 0));
        p.drawLine(project(i, -10, 0), project(i, 10, 0));
    }
}

void Viewport3D::drawFrame(QPainter &p,
                            double ox, double oy, double oz,
                            const double R[3][3], double length,
                            int alpha, bool dashed,
                            const QString &labelPrefix)
{
    static const QColor baseColors[3] = {
        QColor(231, 76,  60),   // X — red
        QColor(46,  204, 113),  // Y — green
        QColor(52,  152, 219),  // Z — blue
    };
    static const QString labels[3] = {
        QStringLiteral("X"),
        QStringLiteral("Y"),
        QStringLiteral("Z"),
    };

    for (int i = 0; i < 3; ++i) {
        const double ex = ox + R[0][i] * length;
        const double ey = oy + R[1][i] * length;
        const double ez = oz + R[2][i] * length;

        QColor c = baseColors[i];
        c.setAlpha(alpha);

        drawArrowLine(p, ox, oy, oz, ex, ey, ez, c,
                      dashed ? 1.8 : 3.0, dashed);

        // Label at the tip
        const double lx = ox + R[0][i] * length * 1.15;
        const double ly = oy + R[1][i] * length * 1.15;
        const double lz = oz + R[2][i] * length * 1.15;
        const QPointF lp = project(lx, ly, lz);

        p.setPen(c);
        p.setFont(QFont("Segoe UI", 9, QFont::Bold));
        p.drawText(lp + QPointF(4, -4), labelPrefix + labels[i]);
    }
}

void Viewport3D::drawArrowLine(QPainter &p,
                                double x0, double y0, double z0,
                                double x1, double y1, double z1,
                                const QColor &color, double lineWidth,
                                bool dashed)
{
    const QPointF from = project(x0, y0, z0);
    const QPointF to   = project(x1, y1, z1);

    QPen pen(color, lineWidth);
    if (dashed)
        pen.setDashPattern({5, 4});
    pen.setCapStyle(Qt::RoundCap);
    p.setPen(pen);
    p.drawLine(from, to);

    if (!dashed)
        drawArrowHead(p, to, from, 10.0, color);
}

void Viewport3D::drawArrowHead(QPainter &p, const QPointF &tip,
                                const QPointF &from,
                                double size, const QColor &color)
{
    const QPointF d = tip - from;
    const double len = std::sqrt(d.x() * d.x() + d.y() * d.y());
    if (len < 1e-6) return;

    const QPointF dir  = d / len;
    const QPointF perp(-dir.y(), dir.x());

    const QPointF p1 = tip - dir * size + perp * size * 0.35;
    const QPointF p2 = tip - dir * size - perp * size * 0.35;

    QPainterPath path;
    path.moveTo(tip);
    path.lineTo(p1);
    path.lineTo(p2);
    path.closeSubpath();

    p.setPen(Qt::NoPen);
    p.setBrush(color);
    p.drawPath(path);
}

void Viewport3D::drawTrail(QPainter &p, double x, double y, double z)
{
    const QPointF from = project(0, 0, 0);
    const QPointF to   = project(x, y, z);

    QPen pen(QColor(243, 156, 18, 180), 1.2);
    pen.setDashPattern({2, 4});
    p.setPen(pen);
    p.drawLine(from, to);

    // Dot at the transformed origin
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(243, 156, 18, 200));
    p.drawEllipse(to, 4, 4);
}

// Mouse / wheel interaction (camera movement + zooming)

void Viewport3D::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        m_lastMousePos = event->pos();
    }
}

void Viewport3D::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging) {
        const QPoint delta = event->pos() - m_lastMousePos;
        m_azimuth   += delta.x() * 0.5;
        m_elevation  = std::clamp(m_elevation + delta.y() * 0.5, -89.0, 89.0);
        m_lastMousePos = event->pos();
        update();
    }
}

void Viewport3D::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        m_dragging = false;
}

void Viewport3D::wheelEvent(QWheelEvent *event)
{
    const double delta = event->angleDelta().y() / 120.0;
    m_zoom = std::clamp(m_zoom + delta * 0.1, 0.2, 5.0);
    update();
}
