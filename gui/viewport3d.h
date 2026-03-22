#pragma once

#include <QWidget>
#include <QPointF>
#include <vector>
#include "core/transformations.h"

class Viewport3D : public QWidget
{
    Q_OBJECT
public:
    explicit Viewport3D(QWidget *parent = nullptr);

    void updateTransform(double roll, double pitch, double yaw,
                         double tx, double ty, double tz);
    void updateWaypoints(const std::vector<tf::Mat4> &waypoints,
                         const tf::Mat4 &T_current, int segIndex);
    void clearInterp();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    QPointF project(double x, double y, double z) const;

    void drawGrid(QPainter &p);
    void drawFrame(QPainter &p, double ox, double oy, double oz,
                   const double R[3][3], double length,
                   int alpha, bool dashed, const QString &labelPrefix);
    void drawArrowLine(QPainter &p, double x0, double y0, double z0,
                       double x1, double y1, double z1,
                       const QColor &color, double lineWidth, bool dashed);
    void drawArrowHead(QPainter &p, const QPointF &tip, const QPointF &from,
                       double size, const QColor &color);
    void drawTrail(QPainter &p, double x, double y, double z);

    // Camera
    double m_azimuth   = -60.0;
    double m_elevation =  25.0;
    double m_zoom      =   1.0;
    QPoint m_lastMousePos;
    bool   m_dragging = false;

    // Current transform
    double m_R[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
    double m_tx = 0, m_ty = 0, m_tz = 0;

    // Interpolation mode
    bool m_interpMode = false;
    std::vector<tf::Mat4> m_waypoints;
    tf::Mat4 m_T_interpCurrent = tf::identity4();
    int m_wpSegIndex = 0;
};
