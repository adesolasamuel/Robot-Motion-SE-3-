#pragma once

#include <QMainWindow>
#include <vector>
#include "core/transformations.h"

class Viewport3D;
class ControlPanel;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onParamsChanged(double roll, double pitch, double yaw,
                         double tx, double ty, double tz);
    void onWaypointFrame(const std::vector<tf::Mat4> &waypoints,
                         const tf::Mat4 &T_current, int segIndex);

private:
    Viewport3D   *m_viewport;
    ControlPanel *m_controls;
};
