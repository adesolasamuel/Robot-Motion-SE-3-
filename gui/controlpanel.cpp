#include "gui/controlpanel.h"
#include "core/transformations.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QFrame>
#include <QListWidget>
#include <QString>
#include <QStringList>
#include <cmath>

ControlPanel::ControlPanel(QWidget *parent)
    : QWidget(parent)
{
    // ---- Global dark stylesheet for the panel ----
    setStyleSheet(R"(
        QWidget { background-color: #333333; color: white; }
        QGroupBox {
            border: 1px solid #555555;
            border-radius: 4px;
            margin-top: 12px;
            padding-top: 16px;
            font: bold 10pt 'Segoe UI';
            color: #00bcd4;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top left;
            padding: 0 6px;
        }
        QSlider::groove:horizontal {
            height: 6px; background: #555555; border-radius: 3px;
        }
        QSlider::handle:horizontal {
            width: 14px; height: 14px; margin: -4px 0;
            background: #00bcd4; border-radius: 7px;
        }
        QDoubleSpinBox {
            background: #555555; color: white; border: none;
            padding: 2px; font: 9pt 'Consolas';
        }
        QTextEdit {
            background: #1e1e1e; color: #4fc3f7; border: none;
            font: 10pt 'Consolas'; padding: 4px;
        }
    )");

    auto *layout = new QVBoxLayout(this);
    layout->setSpacing(4);
    layout->setContentsMargins(8, 8, 8, 8);

    // ---- Title ----
    auto *title = new QLabel(QStringLiteral("Controls"));
    title->setStyleSheet("font: bold 14pt 'Segoe UI'; color: white;");
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    auto *sep = new QFrame();
    sep->setFrameShape(QFrame::HLine);
    sep->setFixedHeight(2);
    sep->setStyleSheet("background: #00bcd4;");
    layout->addWidget(sep);

    // ---- Rotation ----
    auto *rotGroup = new QGroupBox(QStringLiteral("Rotation (Euler ZYX)"));
    auto *rotLayout = new QVBoxLayout(rotGroup);
    rotLayout->setSpacing(2);
    rotLayout->addWidget(createSliderRow("Roll  (X)", "roll",  -180, 180, 1.0, rotGroup));
    rotLayout->addWidget(createSliderRow("Pitch (Y)", "pitch", -180, 180, 1.0, rotGroup));
    rotLayout->addWidget(createSliderRow("Yaw   (Z)", "yaw",   -180, 180, 1.0, rotGroup));
    layout->addWidget(rotGroup);

    // ---- Translation ----
    auto *transGroup = new QGroupBox(QStringLiteral("Translation"));
    auto *transLayout = new QVBoxLayout(transGroup);
    transLayout->setSpacing(2);
    transLayout->addWidget(createSliderRow("Translate X", "tx", -10.0, 10.0, 0.05, transGroup));
    transLayout->addWidget(createSliderRow("Translate Y", "ty", -10.0, 10.0, 0.05, transGroup));
    transLayout->addWidget(createSliderRow("Translate Z", "tz", -10.0, 10.0, 0.05, transGroup));
    layout->addWidget(transGroup);

    // ---- Homogeneous matrix display ----
    auto *matGroup = new QGroupBox(QStringLiteral("Homogeneous Matrix (4\u00d74)"));
    auto *matLayout = new QVBoxLayout(matGroup);
    m_matrixText = new QTextEdit();
    m_matrixText->setReadOnly(false);
    m_matrixText->setFixedHeight(95);
    matLayout->addWidget(m_matrixText);

    auto *applyMatBtn = new QPushButton(QStringLiteral("Apply Matrix"));
    applyMatBtn->setCursor(Qt::PointingHandCursor);
    applyMatBtn->setStyleSheet(
        "QPushButton { background: #1abc9c; color: white; border: none; "
        "border-radius: 4px; padding: 4px 12px; font: bold 9pt 'Segoe UI'; }"
        "QPushButton:hover { background: #16a085; }");
    connect(applyMatBtn, &QPushButton::clicked, this, &ControlPanel::applyMatrix);
    matLayout->addWidget(applyMatBtn);
    layout->addWidget(matGroup);

    // ---- Quaternion display ----
    auto *quatGroup = new QGroupBox(QStringLiteral("Quaternion [w, x, y, z]"));
    auto *quatLayout = new QVBoxLayout(quatGroup);
    m_quatEdit = new QLineEdit(QStringLiteral("[1.0000, 0.0000, 0.0000, 0.0000]"));
    m_quatEdit->setStyleSheet(
        "background: #1e1e1e; color: #81c784; font: 10pt 'Consolas'; padding: 6px; border: none;");
    quatLayout->addWidget(m_quatEdit);

    auto *applyQuatBtn = new QPushButton(QStringLiteral("Apply Quaternion"));
    applyQuatBtn->setCursor(Qt::PointingHandCursor);
    applyQuatBtn->setStyleSheet(
        "QPushButton { background: #1abc9c; color: white; border: none; "
        "border-radius: 4px; padding: 4px 12px; font: bold 9pt 'Segoe UI'; }"
        "QPushButton:hover { background: #16a085; }");
    connect(applyQuatBtn, &QPushButton::clicked, this, &ControlPanel::applyQuaternion);
    quatLayout->addWidget(applyQuatBtn);
    layout->addWidget(quatGroup);

    // ---- Buttons ----
    auto *btnLayout = new QHBoxLayout();

    auto *resetBtn = new QPushButton(QStringLiteral("Reset All"));
    resetBtn->setCursor(Qt::PointingHandCursor);
    resetBtn->setStyleSheet(
        "QPushButton { background: #e74c3c; color: white; border: none; "
        "border-radius: 4px; padding: 6px 16px; font: bold 10pt 'Segoe UI'; }"
        "QPushButton:hover { background: #c0392b; }");
    connect(resetBtn, &QPushButton::clicked, this, &ControlPanel::resetAll);
    btnLayout->addWidget(resetBtn);

    m_animBtn = new QPushButton(QStringLiteral("\u25B6 Animate"));
    m_animBtn->setCursor(Qt::PointingHandCursor);
    m_animBtn->setStyleSheet(
        "QPushButton { background: #27ae60; color: white; border: none; "
        "border-radius: 4px; padding: 6px 16px; font: bold 10pt 'Segoe UI'; }"
        "QPushButton:hover { background: #219a52; }");
    connect(m_animBtn, &QPushButton::clicked, this, &ControlPanel::toggleAnimation);
    btnLayout->addWidget(m_animBtn);

    layout->addLayout(btnLayout);

    // ---- Waypoint Interpolation section ----
    auto *wpGroup = new QGroupBox(QStringLiteral("Waypoint Animation"));
    auto *wpLayout = new QVBoxLayout(wpGroup);
    wpLayout->setSpacing(4);

    // Add / Remove / Clear waypoint buttons
    auto *wpBtnRow = new QHBoxLayout();
    auto *addWpBtn = new QPushButton(QStringLiteral("+ Add Step"));
    addWpBtn->setCursor(Qt::PointingHandCursor);
    addWpBtn->setStyleSheet(
        "QPushButton { background: #2980b9; color: white; border: none; "
        "border-radius: 4px; padding: 5px 10px; font: bold 9pt 'Segoe UI'; }"
        "QPushButton:hover { background: #2471a3; }");
    connect(addWpBtn, &QPushButton::clicked, this, &ControlPanel::addWaypoint);
    wpBtnRow->addWidget(addWpBtn);

    auto *removeWpBtn = new QPushButton(QStringLiteral("- Remove"));
    removeWpBtn->setCursor(Qt::PointingHandCursor);
    removeWpBtn->setStyleSheet(
        "QPushButton { background: #8e44ad; color: white; border: none; "
        "border-radius: 4px; padding: 5px 10px; font: bold 9pt 'Segoe UI'; }"
        "QPushButton:hover { background: #7d3c98; }");
    connect(removeWpBtn, &QPushButton::clicked, this, &ControlPanel::removeWaypoint);
    wpBtnRow->addWidget(removeWpBtn);

    auto *clearWpBtn = new QPushButton(QStringLiteral("Clear All"));
    clearWpBtn->setCursor(Qt::PointingHandCursor);
    clearWpBtn->setStyleSheet(
        "QPushButton { background: #7f8c8d; color: white; border: none; "
        "border-radius: 4px; padding: 5px 10px; font: bold 9pt 'Segoe UI'; }"
        "QPushButton:hover { background: #6c7a7a; }");
    connect(clearWpBtn, &QPushButton::clicked, this, &ControlPanel::clearWaypoints);
    wpBtnRow->addWidget(clearWpBtn);
    wpLayout->addLayout(wpBtnRow);

    // Waypoint list
    m_waypointList = new QListWidget();
    m_waypointList->setFixedHeight(110);
    m_waypointList->setStyleSheet(
        "QListWidget { background: #1e1e1e; color: #e0e0e0; border: none; "
        "font: 8pt 'Consolas'; padding: 2px; }"
        "QListWidget::item:selected { background: #00bcd4; color: black; }");
    wpLayout->addWidget(m_waypointList);

    // Duration per segment
    auto *durRow = new QHBoxLayout();
    auto *durLabel = new QLabel(QStringLiteral("Sec / segment:"));
    durLabel->setStyleSheet("font: 9pt 'Segoe UI'; color: white;");
    durRow->addWidget(durLabel);

    m_durationSpin = new QDoubleSpinBox();
    m_durationSpin->setRange(0.3, 10.0);
    m_durationSpin->setSingleStep(0.5);
    m_durationSpin->setValue(1.5);
    m_durationSpin->setDecimals(1);
    m_durationSpin->setFixedWidth(70);
    durRow->addWidget(m_durationSpin);
    durRow->addStretch();
    wpLayout->addLayout(durRow);

    // Progress bar
    m_waypointProgress = new QProgressBar();
    m_waypointProgress->setRange(0, 100);
    m_waypointProgress->setValue(0);
    m_waypointProgress->setFixedHeight(14);
    m_waypointProgress->setTextVisible(false);
    m_waypointProgress->setStyleSheet(
        "QProgressBar { background: #555555; border: none; border-radius: 7px; }"
        "QProgressBar::chunk { background: #f39c12; border-radius: 7px; }");
    wpLayout->addWidget(m_waypointProgress);

    // Play button
    m_playBtn = new QPushButton(QStringLiteral("\u25B6 Play"));
    m_playBtn->setCursor(Qt::PointingHandCursor);
    m_playBtn->setStyleSheet(
        "QPushButton { background: #f39c12; color: white; border: none; "
        "border-radius: 4px; padding: 6px 16px; font: bold 10pt 'Segoe UI'; }"
        "QPushButton:hover { background: #d68910; }");
    m_playBtn->setEnabled(false);
    connect(m_playBtn, &QPushButton::clicked, this, &ControlPanel::playWaypoints);
    wpLayout->addWidget(m_playBtn);

    layout->addWidget(wpGroup);
    layout->addStretch();

    // ---- Animation timer ----
    m_animTimer = new QTimer(this);
    connect(m_animTimer, &QTimer::timeout, this, &ControlPanel::animateStep);

    // ---- Waypoint interpolation timer ----
    m_waypointTimer = new QTimer(this);
    connect(m_waypointTimer, &QTimer::timeout, this, &ControlPanel::waypointStep);

    updateDisplays();
}

// ---------------------------------------------------------------------------
QWidget *ControlPanel::createSliderRow(const QString &label,
                                       const QString &key,
                                       double minVal, double maxVal,
                                       double step, QWidget *parent)
{
    auto *row    = new QWidget(parent);
    auto *layout = new QHBoxLayout(row);
    layout->setContentsMargins(4, 2, 4, 2);
    layout->setSpacing(6);

    auto *lbl = new QLabel(label);
    lbl->setFixedWidth(90);
    lbl->setStyleSheet("font: 9pt 'Consolas'; color: white;");
    layout->addWidget(lbl);

    const int sliderMin = static_cast<int>(minVal / step);
    const int sliderMax = static_cast<int>(maxVal / step);

    auto *slider = new QSlider(Qt::Horizontal);
    slider->setRange(sliderMin, sliderMax);
    slider->setValue(0);
    layout->addWidget(slider, 1);

    auto *spinBox = new QDoubleSpinBox();
    spinBox->setRange(minVal, maxVal);
    spinBox->setSingleStep(step);
    spinBox->setDecimals(step < 1.0 ? 2 : 0);
    spinBox->setValue(0.0);
    spinBox->setFixedWidth(70);
    layout->addWidget(spinBox);

    m_sliders[key] = {slider, spinBox, minVal, maxVal, step};

    // Slider → SpinBox
    connect(slider, &QSlider::valueChanged, this, [this, key](int val) {
        auto &sg = m_sliders[key];
        double dval = val * sg.step;
        sg.spinBox->blockSignals(true);
        sg.spinBox->setValue(dval);
        sg.spinBox->blockSignals(false);
        onSliderChanged();
    });

    // SpinBox → Slider
    connect(spinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, [this, key](double val) {
        auto &sg = m_sliders[key];
        int ival = static_cast<int>(std::round(val / sg.step));
        sg.slider->blockSignals(true);
        sg.slider->setValue(ival);
        sg.slider->blockSignals(false);
        onSliderChanged();
    });

    return row;
}

double ControlPanel::getValue(const QString &key) const
{
    auto it = m_sliders.find(key);
    if (it != m_sliders.end())
        return it->second.spinBox->value();
    return 0.0;
}

// ---------------------------------------------------------------------------
void ControlPanel::onSliderChanged()
{
    updateDisplays();
    emit paramsChanged(getValue("roll"), getValue("pitch"), getValue("yaw"),
                       getValue("tx"),   getValue("ty"),    getValue("tz"));
}

void ControlPanel::updateDisplays()
{
    const double roll  = getValue("roll");
    const double pitch = getValue("pitch");
    const double yaw   = getValue("yaw");
    const double tx    = getValue("tx");
    const double ty    = getValue("ty");
    const double tz    = getValue("tz");

    auto R = tf::eulerToRotation(roll, pitch, yaw);
    tf::Vec3 t = {tx, ty, tz};
    auto T = tf::homogeneousMatrix(R, t);
    auto q = tf::rotationToQuaternion(R);

    // Matrix text
    QString matStr;
    for (int i = 0; i < 4; ++i) {
        QStringList row;
        for (int j = 0; j < 4; ++j)
            row << QString::number(T[i][j], 'f', 3).rightJustified(8);
        matStr += row.join("  ") + '\n';
    }
    m_matrixText->blockSignals(true);
    m_matrixText->setText(matStr.trimmed());
    m_matrixText->blockSignals(false);

    // Quaternion edit
    m_quatEdit->blockSignals(true);
    m_quatEdit->setText(
        QString("[%1, %2, %3, %4]")
            .arg(q[0], 0, 'f', 4)
            .arg(q[1], 0, 'f', 4)
            .arg(q[2], 0, 'f', 4)
            .arg(q[3], 0, 'f', 4));
    m_quatEdit->blockSignals(false);
}

// ---------------------------------------------------------------------------
void ControlPanel::resetAll()
{
    for (auto &[key, sg] : m_sliders) {
        sg.slider->blockSignals(true);
        sg.spinBox->blockSignals(true);
        sg.slider->setValue(0);
        sg.spinBox->setValue(0.0);
        sg.slider->blockSignals(false);
        sg.spinBox->blockSignals(false);
    }
    onSliderChanged();
}

void ControlPanel::toggleAnimation()
{
    m_animRunning = !m_animRunning;
    if (m_animRunning) {
        m_animBtn->setText(QStringLiteral("\u25A0 Stop"));
        m_animBtn->setStyleSheet(
            "QPushButton { background: #c0392b; color: white; border: none; "
            "border-radius: 4px; padding: 6px 16px; font: bold 10pt 'Segoe UI'; }"
            "QPushButton:hover { background: #a93226; }");
        m_animStep = 0;
        m_animTimer->start(33);  // ~30 FPS
    } else {
        m_animBtn->setText(QStringLiteral("\u25B6 Animate"));
        m_animBtn->setStyleSheet(
            "QPushButton { background: #27ae60; color: white; border: none; "
            "border-radius: 4px; padding: 6px 16px; font: bold 10pt 'Segoe UI'; }"
            "QPushButton:hover { background: #219a52; }");
        m_animTimer->stop();
    }
}

void ControlPanel::animateStep()
{
    if (!m_animRunning) return;

    const int yaw = (m_animStep * 3) % 360 - 180;
    auto &sg = m_sliders[QString("yaw")];

    sg.slider->blockSignals(true);
    sg.spinBox->blockSignals(true);
    sg.slider->setValue(yaw);
    sg.spinBox->setValue(static_cast<double>(yaw));
    sg.slider->blockSignals(false);
    sg.spinBox->blockSignals(false);

    onSliderChanged();
    ++m_animStep;
}

// ---------------------------------------------------------------------------
// Waypoint animation
// ---------------------------------------------------------------------------
static QString poseString(double roll, double pitch, double yaw,
                          double tx, double ty, double tz)
{
    return QString("R(%1, %2, %3)  T(%4, %5, %6)")
        .arg(roll, 0, 'f', 1).arg(pitch, 0, 'f', 1).arg(yaw, 0, 'f', 1)
        .arg(tx, 0, 'f', 2).arg(ty, 0, 'f', 2).arg(tz, 0, 'f', 2);
}

void ControlPanel::addWaypoint()
{
    const double roll  = getValue("roll"),  pitch = getValue("pitch"), yaw = getValue("yaw");
    const double tx    = getValue("tx"),    ty    = getValue("ty"),    tz  = getValue("tz");
    auto R = tf::eulerToRotation(roll, pitch, yaw);
    m_waypoints.push_back(tf::homogeneousMatrix(R, {tx, ty, tz}));

    int idx = static_cast<int>(m_waypoints.size());
    m_waypointList->addItem(
        QString("#%1  %2").arg(idx).arg(poseString(roll, pitch, yaw, tx, ty, tz)));
    updatePlayEnabled();
}

void ControlPanel::removeWaypoint()
{
    int row = m_waypointList->currentRow();
    if (row < 0 || row >= static_cast<int>(m_waypoints.size()))
        return;
    m_waypoints.erase(m_waypoints.begin() + row);
    delete m_waypointList->takeItem(row);
    // Re-number remaining items
    for (int i = 0; i < m_waypointList->count(); ++i) {
        const auto &T = m_waypoints[i];
        auto [R, t] = tf::decomposeHomogeneous(T);
        auto [roll, pitch, yaw] = tf::rotationToEuler(R);
        m_waypointList->item(i)->setText(
            QString("#%1  %2").arg(i + 1).arg(
                poseString(roll, pitch, yaw, t[0], t[1], t[2])));
    }
    updatePlayEnabled();
}

void ControlPanel::clearWaypoints()
{
    m_waypoints.clear();
    m_waypointList->clear();
    updatePlayEnabled();
}

void ControlPanel::updatePlayEnabled()
{
    m_playBtn->setEnabled(m_waypoints.size() >= 2);
}

void ControlPanel::playWaypoints()
{
    if (m_wpRunning) {
        // Stop
        m_wpRunning = false;
        m_waypointTimer->stop();
        m_playBtn->setText(QStringLiteral("\u25B6 Play"));
        m_playBtn->setStyleSheet(
            "QPushButton { background: #f39c12; color: white; border: none; "
            "border-radius: 4px; padding: 6px 16px; font: bold 10pt 'Segoe UI'; }"
            "QPushButton:hover { background: #d68910; }");
        m_waypointProgress->setValue(0);
        return;
    }

    if (m_waypoints.size() < 2)
        return;

    // Stop any running yaw animation
    if (m_animRunning)
        toggleAnimation();

    const int fps = 30;
    m_wpSegTicks = static_cast<int>(m_durationSpin->value() * fps);
    m_wpSegIndex = 0;
    m_wpTick     = 0;
    m_wpRunning  = true;

    m_playBtn->setText(QStringLiteral("\u25A0 Stop"));
    m_playBtn->setStyleSheet(
        "QPushButton { background: #c0392b; color: white; border: none; "
        "border-radius: 4px; padding: 6px 16px; font: bold 10pt 'Segoe UI'; }"
        "QPushButton:hover { background: #a93226; }");

    m_waypointTimer->start(1000 / fps);
}

void ControlPanel::waypointStep()
{
    if (!m_wpRunning) return;

    const int numSegments = static_cast<int>(m_waypoints.size()) - 1;

    // Local alpha within current segment
    const double segAlpha = std::min(
        static_cast<double>(m_wpTick) / m_wpSegTicks, 1.0);

    // Global progress (0..1)
    const double globalAlpha =
        (m_wpSegIndex + segAlpha) / numSegments;

    const auto &T_a = m_waypoints[m_wpSegIndex];
    const auto &T_b = m_waypoints[m_wpSegIndex + 1];
    auto T_cur = tf::interpolateTransforms(T_a, T_b, segAlpha);

    // Update displays
    auto [R, t] = tf::decomposeHomogeneous(T_cur);
    auto q = tf::rotationToQuaternion(R);
    auto T_disp = tf::homogeneousMatrix(R, t);

    QString matStr;
    for (int i = 0; i < 4; ++i) {
        QStringList row;
        for (int j = 0; j < 4; ++j)
            row << QString::number(T_disp[i][j], 'f', 3).rightJustified(8);
        matStr += row.join("  ") + '\n';
    }
    m_matrixText->setText(matStr.trimmed());
    m_quatEdit->setText(
        QString("[%1, %2, %3, %4]")
            .arg(q[0], 0, 'f', 4).arg(q[1], 0, 'f', 4)
            .arg(q[2], 0, 'f', 4).arg(q[3], 0, 'f', 4));

    // Update sliders
    auto [roll, pitch, yaw] = tf::rotationToEuler(R);
    setSlidersFromPose(roll, pitch, yaw, t[0], t[1], t[2]);

    // Progress bar
    m_waypointProgress->setValue(static_cast<int>(globalAlpha * 100));

    // Highlight current segment in list
    m_waypointList->setCurrentRow(m_wpSegIndex);

    // Emit to viewport
    emit waypointFrame(m_waypoints, T_cur, m_wpSegIndex);

    // Advance tick
    ++m_wpTick;
    if (m_wpTick > m_wpSegTicks) {
        // Move to next segment
        m_wpTick = 0;
        ++m_wpSegIndex;
        if (m_wpSegIndex >= numSegments) {
            // Animation finished
            m_wpRunning = false;
            m_waypointTimer->stop();
            m_waypointProgress->setValue(100);
            m_playBtn->setText(QStringLiteral("\u25B6 Play"));
            m_playBtn->setStyleSheet(
                "QPushButton { background: #f39c12; color: white; border: none; "
                "border-radius: 4px; padding: 6px 16px; font: bold 10pt 'Segoe UI'; }"
                "QPushButton:hover { background: #d68910; }");
        }
    }
}

// ---------------------------------------------------------------------------
// Set sliders programmatically without triggering recursive updates
// ---------------------------------------------------------------------------
void ControlPanel::setSlidersFromPose(double roll, double pitch, double yaw,
                                      double tx, double ty, double tz)
{
    const std::pair<QString, double> updates[] = {
        {"roll", roll}, {"pitch", pitch}, {"yaw", yaw},
        {"tx", tx},     {"ty", ty},       {"tz", tz},
    };
    for (auto &[key, val] : updates) {
        auto &sg = m_sliders[key];
        sg.slider->blockSignals(true);
        sg.spinBox->blockSignals(true);
        sg.spinBox->setValue(val);
        sg.slider->setValue(static_cast<int>(std::round(val / sg.step)));
        sg.slider->blockSignals(false);
        sg.spinBox->blockSignals(false);
    }
}

// ---------------------------------------------------------------------------
// Apply manually edited homogeneous matrix
// ---------------------------------------------------------------------------
void ControlPanel::applyMatrix()
{
    const QString text = m_matrixText->toPlainText();
    const QStringList lines = text.split('\n', Qt::SkipEmptyParts);
    if (lines.size() < 4)
        return;

    tf::Mat4 T = tf::identity4();
    for (int i = 0; i < 4; ++i) {
        const QStringList tokens = lines[i].simplified().split(' ', Qt::SkipEmptyParts);
        if (tokens.size() < 4)
            return;
        bool ok = true;
        for (int j = 0; j < 4 && ok; ++j) {
            T[i][j] = tokens[j].toDouble(&ok);
            if (!ok) return;
        }
    }

    auto [R, t] = tf::decomposeHomogeneous(T);
    auto [roll, pitch, yaw] = tf::rotationToEuler(R);

    setSlidersFromPose(roll, pitch, yaw, t[0], t[1], t[2]);
    updateDisplays();
    emit paramsChanged(roll, pitch, yaw, t[0], t[1], t[2]);
}

// ---------------------------------------------------------------------------
// Apply manually edited quaternion
// ---------------------------------------------------------------------------
void ControlPanel::applyQuaternion()
{
    QString text = m_quatEdit->text().trimmed();
    // Strip brackets
    text.remove('[');
    text.remove(']');

    const QStringList tokens = text.split(',', Qt::SkipEmptyParts);
    if (tokens.size() < 4)
        return;

    bool ok = true;
    double w = tokens[0].trimmed().toDouble(&ok); if (!ok) return;
    double x = tokens[1].trimmed().toDouble(&ok); if (!ok) return;
    double y = tokens[2].trimmed().toDouble(&ok); if (!ok) return;
    double z = tokens[3].trimmed().toDouble(&ok); if (!ok) return;

    tf::Quat q = {w, x, y, z};
    tf::Mat3 R = tf::quaternionToRotation(q);
    auto [roll, pitch, yaw] = tf::rotationToEuler(R);

    // Keep current translation
    double tx = getValue("tx"), ty = getValue("ty"), tz = getValue("tz");

    setSlidersFromPose(roll, pitch, yaw, tx, ty, tz);
    updateDisplays();
    emit paramsChanged(roll, pitch, yaw, tx, ty, tz);
}
