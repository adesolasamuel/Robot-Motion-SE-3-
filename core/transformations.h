#pragma once

#include <array>
#include <cmath>
#include <tuple>
#include <utility>

namespace tf {

using Mat3 = std::array<std::array<double, 3>, 3>;
using Mat4 = std::array<std::array<double, 4>, 4>;
using Vec3 = std::array<double, 3>;
using Quat = std::array<double, 4>; // [w, x, y, z]

inline constexpr double DEG2RAD = M_PI / 180.0;
inline constexpr double RAD2DEG = 180.0 / M_PI;

Mat3 identity3();
Mat4 identity4();
Mat3 multiply3(const Mat3 &A, const Mat3 &B);

Mat3 rotationX(double angleDeg);
Mat3 rotationY(double angleDeg);
Mat3 rotationZ(double angleDeg);
Mat3 eulerToRotation(double roll, double pitch, double yaw);
std::tuple<double, double, double> rotationToEuler(const Mat3 &R);

Mat3 axisAngleToRotation(const Vec3 &axis, double angleDeg);

Quat rotationToQuaternion(const Mat3 &R);
Mat3 quaternionToRotation(const Quat &q);

Mat4 homogeneousMatrix(const Mat3 &R, const Vec3 &t);
std::pair<Mat3, Vec3> decomposeHomogeneous(const Mat4 &T);

Mat4 interpolateTransforms(const Mat4 &T_start, const Mat4 &T_end, double alpha);

} // namespace tf
