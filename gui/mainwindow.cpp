#include "gui/mainwindow.h"
#include "gui/viewport3d.h"
#include "gui/controlpanel.h"

#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("Robot Motion Visualizer"));
    setMinimumSize(1100, 650);

    auto *central = new QWidget(this);
    setCentralWidget(central);

    auto *layout = new QHBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // Left: 3‑D viewport
    m_viewport = new Viewport3D(this);
    layout->addWidget(m_viewport, 1);

    // Right: control panel
    m_controls = new ControlPanel(this);
    m_controls->setFixedWidth(380);
    layout->addWidget(m_controls);

    connect(m_controls, &ControlPanel::paramsChanged,
            this, &MainWindow::onParamsChanged);
    connect(m_controls, &ControlPanel::waypointFrame,
            this, &MainWindow::onWaypointFrame);

    showMaximized();
}

void MainWindow::onParamsChanged(double roll, double pitch, double yaw,
                                  double tx, double ty, double tz)
{
    m_viewport->updateTransform(roll, pitch, yaw, tx, ty, tz);
}

void MainWindow::onWaypointFrame(const std::vector<tf::Mat4> &waypoints,
                                  const tf::Mat4 &T_current, int segIndex)
{
    m_viewport->updateWaypoints(waypoints, T_current, segIndex);
}
