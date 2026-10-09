#pragma once
#include <iosfwd>

namespace ppois {
/** Coordinates of a point in three-dimensional space. */
struct Point3 {
    double x = 0, y = 0, z = 0;
};

/** A free vector stored as two endpoints.
 * Arithmetic preserves the left operand's start. Equality compares displacement
 * exactly; ordering compares length. Coordinates must be finite.
 */
class Vector3 {
public:
    Vector3(const Point3& start = {}, const Point3& end = {});
    Vector3(const Vector3&) = default;
    Vector3& operator=(const Vector3&) = default;
    ~Vector3() = default;
    const Point3& start() const;
    const Point3& end() const;
    Point3 displacement() const;
    double length() const;
    /** Throws std::domain_error if either vector has zero length. */
    double cosine(const Vector3& other) const;
    Vector3 operator+(const Vector3& other) const;
    Vector3 operator-(const Vector3& other) const;
    /** Cross product, not scalar product. */
    Vector3 operator*(const Vector3& other) const;
    Vector3 operator*(double scalar) const;
    /** Throws std::domain_error for zero divisor. */
    Vector3 operator/(double scalar) const;
    Vector3& operator+=(const Vector3& other);
    Vector3& operator-=(const Vector3& other);
    Vector3& operator*=(const Vector3& other);
    Vector3& operator*=(double scalar);
    Vector3& operator/=(double scalar);
    bool operator==(const Vector3& other) const;
    bool operator!=(const Vector3& other) const;
    bool operator<(const Vector3& other) const;
    bool operator>(const Vector3& other) const;
    bool operator<=(const Vector3& other) const;
    bool operator>=(const Vector3& other) const;
    /** Format: start.x start.y start.z end.x end.y end.z. */
    friend std::ostream& operator<<(std::ostream& out, const Vector3& vector);
    /** Invalid input sets failbit and leaves the object unchanged. */
    friend std::istream& operator>>(std::istream& in, Vector3& vector);
private:
    Point3 start_, end_;
    Vector3 with_displacement(const Point3& value) const;
};
Vector3 operator*(double scalar, const Vector3& vector);
} // namespace ppois
