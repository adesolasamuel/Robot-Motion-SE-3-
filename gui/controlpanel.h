#pragma once

#include <QWidget>
#include <QSlider>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QTextEdit>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QTimer>
#include <QProgressBar>
#include <map>
#include <vector>

#include "core/transformations.h"

class ControlPanel : public QWidget
{
    Q_OBJECT
public:
    explicit ControlPanel(QWidget *parent = nullptr);

signals:
    void paramsChanged(double roll, double pitch, double yaw,
                       double tx, double ty, double tz);
    void waypointFrame(const std::vector<tf::Mat4> &waypoints,
                       const tf::Mat4 &T_current, int segIndex);

private slots:
    void onSliderChanged();
    void resetAll();
    void toggleAnimation();
    void animateStep();
    void addWaypoint();
    void removeWaypoint();
    void clearWaypoints();
    void playWaypoints();
    void waypointStep();
    void applyMatrix();
    void applyQuaternion();

private:
    QWidget *createSliderRow(const QString &label, const QString &key,
                             double minVal, double maxVal, double step,
                             QWidget *parent);
    void updateDisplays();
    void setSlidersFromPose(double roll, double pitch, double yaw,
                           double tx, double ty, double tz);
    void updatePlayEnabled();
    double getValue(const QString &key) const;

    struct SliderGroup {
        QSlider       *slider;
        QDoubleSpinBox *spinBox;
        double minVal, maxVal, step;
    };

    std::map<QString, SliderGroup> m_sliders;
    QTextEdit   *m_matrixText;
    QLineEdit   *m_quatEdit;
    QPushButton *m_animBtn;
    QTimer      *m_animTimer;
    bool         m_animRunning = false;
    int          m_animStep    = 0;

    // Waypoint-based interpolation
    QListWidget *m_waypointList;
    QPushButton *m_playBtn;
    QDoubleSpinBox *m_durationSpin;
    QProgressBar *m_waypointProgress;
    QTimer      *m_waypointTimer;
    std::vector<tf::Mat4> m_waypoints;
    bool         m_wpRunning   = false;
    int          m_wpTick      = 0;
    int          m_wpSegTicks  = 0;      // ticks per segment
    int          m_wpSegIndex  = 0;      // current segment (pair index)
};
