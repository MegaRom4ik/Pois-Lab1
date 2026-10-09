#include "Vector3.hpp"
#include <algorithm>
#include <cmath>
#include <istream>
#include <limits>
#include <ostream>
#include <stdexcept>

namespace ppois {
namespace {
bool finite(const Point3& point) {
    return std::isfinite(point.x) && std::isfinite(point.y) && std::isfinite(point.z);
}
}
Vector3::Vector3(const Point3& start, const Point3& end) : start_(start), end_(end) {
    if (!finite(start) || !finite(end) || !finite(displacement()))
        throw std::invalid_argument("Coordinates and displacement must be finite");
}
const Point3& Vector3::start() const { return start_; }
const Point3& Vector3::end() const { return end_; }
Point3 Vector3::displacement() const {
    return {end_.x - start_.x, end_.y - start_.y, end_.z - start_.z};
}
double Vector3::length() const {
    const auto value = displacement();
    return std::hypot(value.x, value.y, value.z);
}
Vector3 Vector3::with_displacement(const Point3& value) const {
    return {start_, {start_.x + value.x, start_.y + value.y, start_.z + value.z}};
}
double Vector3::cosine(const Vector3& other) const {
    const double left_length = length(), right_length = other.length();
    if (left_length == 0 || right_length == 0)
        throw std::domain_error("Angle with a zero vector is undefined");
    if (!std::isfinite(left_length) || !std::isfinite(right_length))
        throw std::overflow_error("Vector length exceeds double range");
    const auto left = displacement(), right = other.displacement();
    const double dot = (left.x / left_length) * (right.x / right_length)
        + (left.y / left_length) * (right.y / right_length)
        + (left.z / left_length) * (right.z / right_length);
    return std::clamp(dot, -1.0, 1.0);
}
Vector3 Vector3::operator+(const Vector3& other) const {
    const auto left = displacement(), right = other.displacement();
    return with_displacement({left.x + right.x, left.y + right.y, left.z + right.z});
}
Vector3 Vector3::operator-(const Vector3& other) const {
    const auto left = displacement(), right = other.displacement();
    return with_displacement({left.x - right.x, left.y - right.y, left.z - right.z});
}
Vector3 Vector3::operator*(const Vector3& other) const {
    const auto left = displacement(), right = other.displacement();
    return with_displacement({left.y * right.z - left.z * right.y,
        left.z * right.x - left.x * right.z, left.x * right.y - left.y * right.x});
}
Vector3 Vector3::operator*(double scalar) const {
    if (!std::isfinite(scalar)) throw std::invalid_argument("Scalar must be finite");
    const auto value = displacement();
    return with_displacement({value.x * scalar, value.y * scalar, value.z * scalar});
}
Vector3 Vector3::operator/(double scalar) const {
    if (scalar == 0) throw std::domain_error("Division by zero");
    if (!std::isfinite(scalar)) throw std::invalid_argument("Divisor must be finite");
    const auto value = displacement();
    return with_displacement({value.x / scalar, value.y / scalar, value.z / scalar});
}
Vector3& Vector3::operator+=(const Vector3& other) { return *this = *this + other; }
Vector3& Vector3::operator-=(const Vector3& other) { return *this = *this - other; }
Vector3& Vector3::operator*=(const Vector3& other) { return *this = *this * other; }
Vector3& Vector3::operator*=(double scalar) { return *this = *this * scalar; }
Vector3& Vector3::operator/=(double scalar) { return *this = *this / scalar; }
bool Vector3::operator==(const Vector3& other) const {
    const auto left = displacement(), right = other.displacement();
    return left.x == right.x && left.y == right.y && left.z == right.z;
}
bool Vector3::operator!=(const Vector3& other) const { return !(*this == other); }
bool Vector3::operator<(const Vector3& other) const { return length() < other.length(); }
bool Vector3::operator>(const Vector3& other) const { return other < *this; }
bool Vector3::operator<=(const Vector3& other) const { return !(*this > other); }
bool Vector3::operator>=(const Vector3& other) const { return !(*this < other); }
Vector3 operator*(double scalar, const Vector3& vector) { return vector * scalar; }
std::ostream& operator<<(std::ostream& out, const Vector3& vector) {
    const auto old_precision = out.precision();
    out.precision(std::numeric_limits<double>::max_digits10);
    out << vector.start_.x << ' ' << vector.start_.y << ' ' << vector.start_.z << ' '
        << vector.end_.x << ' ' << vector.end_.y << ' ' << vector.end_.z;
    out.precision(old_precision);
    return out;
}
std::istream& operator>>(std::istream& in, Vector3& vector) {
    Point3 start, end;
    if (!(in >> start.x >> start.y >> start.z >> end.x >> end.y >> end.z)) return in;
    try { vector = Vector3(start, end); }
    catch (const std::invalid_argument&) { in.setstate(std::ios::failbit); }
    return in;
}
} // namespace ppois
