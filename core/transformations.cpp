#include "core/transformations.h"
#include <algorithm>

namespace tf {

Mat3 identity3()
{
    return {{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}};
}

Mat4 identity4()
{
    return {{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}};
}

Mat3 multiply3(const Mat3 &A, const Mat3 &B)
{
    Mat3 C{};
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            for (int k = 0; k < 3; ++k)
                C[i][j] += A[i][k] * B[k][j];
    return C;
}

Mat3 rotationX(double angleDeg)
{
    double a = angleDeg * DEG2RAD;
    double c = std::cos(a), s = std::sin(a);
    return {{{1, 0, 0}, {0, c, -s}, {0, s, c}}};
}

Mat3 rotationY(double angleDeg)
{
    double a = angleDeg * DEG2RAD;
    double c = std::cos(a), s = std::sin(a);
    return {{{c, 0, s}, {0, 1, 0}, {-s, 0, c}}};
}

Mat3 rotationZ(double angleDeg)
{
    double a = angleDeg * DEG2RAD;
    double c = std::cos(a), s = std::sin(a);
    return {{{c, -s, 0}, {s, c, 0}, {0, 0, 1}}};
}

Mat3 eulerToRotation(double roll, double pitch, double yaw)
{
    return multiply3(rotationZ(yaw), multiply3(rotationY(pitch), rotationX(roll)));
}

std::tuple<double, double, double> rotationToEuler(const Mat3 &R)
{
    double sy = std::sqrt(R[0][0] * R[0][0] + R[1][0] * R[1][0]);
    bool singular = sy < 1e-6;

    double roll, pitch, yaw;
    if (!singular) {
        roll = std::atan2(R[2][1], R[2][2]) * RAD2DEG;
        pitch = std::atan2(-R[2][0], sy) * RAD2DEG;
        yaw = std::atan2(R[1][0], R[0][0]) * RAD2DEG;
    } else {
        roll = std::atan2(-R[1][2], R[1][1]) * RAD2DEG;
        pitch = std::atan2(-R[2][0], sy) * RAD2DEG;
        yaw = 0.0;
    }
    return {roll, pitch, yaw};
}

Mat3 axisAngleToRotation(const Vec3 &axis, double angleDeg)
{
    double norm = std::sqrt(axis[0] * axis[0] + axis[1] * axis[1] + axis[2] * axis[2]);
    if (norm < 1e-10)
        return identity3();

    double ax = axis[0] / norm, ay = axis[1] / norm, az = axis[2] / norm;
    double a = angleDeg * DEG2RAD;
    double c = std::cos(a), s = std::sin(a);

    Mat3 K = {{{0, -az, ay}, {az, 0, -ax}, {-ay, ax, 0}}};
    Mat3 KK = multiply3(K, K);

    Mat3 R = identity3();
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            R[i][j] += s * K[i][j] + (1 - c) * KK[i][j];
    return R;
}

Quat rotationToQuaternion(const Mat3 &R)
{
    double tr = R[0][0] + R[1][1] + R[2][2];
    double w, x, y, z;

    if (tr > 0) {
        double s = 2.0 * std::sqrt(tr + 1.0);
        w = 0.25 * s;
        x = (R[2][1] - R[1][2]) / s;
        y = (R[0][2] - R[2][0]) / s;
        z = (R[1][0] - R[0][1]) / s;
    } else if (R[0][0] > R[1][1] && R[0][0] > R[2][2]) {
        double s = 2.0 * std::sqrt(1.0 + R[0][0] - R[1][1] - R[2][2]);
        w = (R[2][1] - R[1][2]) / s;
        x = 0.25 * s;
        y = (R[0][1] + R[1][0]) / s;
        z = (R[0][2] + R[2][0]) / s;
    } else if (R[1][1] > R[2][2]) {
        double s = 2.0 * std::sqrt(1.0 + R[1][1] - R[0][0] - R[2][2]);
        w = (R[0][2] - R[2][0]) / s;
        x = (R[0][1] + R[1][0]) / s;
        y = 0.25 * s;
        z = (R[1][2] + R[2][1]) / s;
    } else {
        double s = 2.0 * std::sqrt(1.0 + R[2][2] - R[0][0] - R[1][1]);
        w = (R[1][0] - R[0][1]) / s;
        x = (R[0][2] + R[2][0]) / s;
        y = (R[1][2] + R[2][1]) / s;
        z = 0.25 * s;
    }
    return {w, x, y, z};
}

Mat3 quaternionToRotation(const Quat &q)
{
    double norm = std::sqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
    double w = q[0] / norm, x = q[1] / norm, y = q[2] / norm, z = q[3] / norm;

    return {{
        {1 - 2 * (y * y + z * z), 2 * (x * y - z * w), 2 * (x * z + y * w)},
        {2 * (x * y + z * w), 1 - 2 * (x * x + z * z), 2 * (y * z - x * w)},
        {2 * (x * z - y * w), 2 * (y * z + x * w), 1 - 2 * (x * x + y * y)},
    }};
}

Mat4 homogeneousMatrix(const Mat3 &R, const Vec3 &t)
{
    Mat4 T = identity4();
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j)
            T[i][j] = R[i][j];
        T[i][3] = t[i];
    }
    return T;
}

std::pair<Mat3, Vec3> decomposeHomogeneous(const Mat4 &T)
{
    Mat3 R{};
    Vec3 t{};
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j)
            R[i][j] = T[i][j];
        t[i] = T[i][3];
    }
    return {R, t};
}

Mat4 interpolateTransforms(const Mat4 &T_start, const Mat4 &T_end, double alpha)
{
    auto [R0, t0] = decomposeHomogeneous(T_start);
    auto [R1, t1] = decomposeHomogeneous(T_end);

    Vec3 t{};
    for (int i = 0; i < 3; ++i)
        t[i] = (1 - alpha) * t0[i] + alpha * t1[i];

    Quat q0 = rotationToQuaternion(R0);
    Quat q1 = rotationToQuaternion(R1);

    // Ensure shortest path
    double dot = q0[0] * q1[0] + q0[1] * q1[1] + q0[2] * q1[2] + q0[3] * q1[3];
    if (dot < 0) {
        for (auto &v : q1)
            v = -v;
        dot = -dot;
    }

    dot = std::clamp(dot, -1.0, 1.0);
    double theta = std::acos(dot);

    Quat q{};
    if (theta < 1e-6) {
        for (int i = 0; i < 4; ++i)
            q[i] = (1 - alpha) * q0[i] + alpha * q1[i];
    } else {
        double sinTheta = std::sin(theta);
        for (int i = 0; i < 4; ++i)
            q[i] = (std::sin((1 - alpha) * theta) * q0[i] +
                     std::sin(alpha * theta) * q1[i]) / sinTheta;
    }

    Mat3 R = quaternionToRotation(q);
    return homogeneousMatrix(R, t);
}

} // namespace tf
