#pragma once

#include <cassert>
#include <cmath>
#include <ostream>

struct Vector
{
    union
    {
        struct
        {
            double x;
            double y;
            double z;
        };
        double v[3] {};
    };

    Vector() : x { 0.0f }, y { 0.0f }, z { 0.0f } {}
    explicit Vector(double x, double y, double z) : x { x }, y { y }, z { z } {}
    explicit Vector(double s) : x { s }, y { s }, z { s } {}

    Vector(const Vector&) = default;
    ~Vector() = default;
    Vector& operator=(const Vector&) = default;

    double& operator[](const size_t& idx)
    {
        assert(idx <= 2);
        return v[idx];
    }

    double operator[](const size_t& idx) const
    {
        assert(idx <= 2);
        return v[idx];
    }

    Vector operator+(const Vector& u) const
    {
        return Vector { x + u.x, y + u.y, z + u.z };
    }

    Vector operator-(const Vector& u) const
    {
        return Vector { x - u.x, y - u.y, z - u.z };
    }

    Vector operator*(const Vector& u) const
    {
        return Vector { x * u.x, y * u.y, z * u.z };
    }

    Vector operator/(const Vector& u) const
    {
        assert(u.x != 0 && u.y != 0 && u.z != 0);
        return Vector { x / u.x, y / u.y, z / u.z };
    }

    Vector& operator+=(const Vector& u)
    {
        x += u.x;
        y += u.y;
        z += u.z;
        return *this;
    }

    Vector& operator-=(const Vector& u)
    {
        x -= u.x;
        y -= u.y;
        z -= u.z;
        return *this;
    }

    Vector& operator*=(const Vector& u)
    {
        x *= u.x;
        y *= u.y;
        z *= u.z;
        return *this;
    }

    Vector& operator/=(const Vector& u)
    {
        assert(u.x != 0 && u.y != 0 && u.z != 0);
        x /= u.x;
        y /= u.y;
        z /= u.z;
        return *this;
    }

    Vector operator+(const double& s) const
    {
        return Vector { x + s, y + s, z + s };
    }

    Vector operator-(const double& s) const
    {
        return Vector { x - s, y - s, z - s };
    }

    Vector operator*(const double& s) const
    {
        return Vector { x * s, y * s, z * s };
    }

    Vector operator/(const double& s) const
    {
        assert(s != 0);
        return Vector { x / s, y / s, z / s };
    }

    Vector& operator+=(const double& s)
    {
        x += s;
        y += s;
        z += s;
        return *this;
    }

    Vector& operator-=(const double& s)
    {
        x -= s;
        y -= s;
        z -= s;
        return *this;
    }

    Vector& operator*=(const double& s)
    {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    Vector& operator/=(const double& s)
    {
        assert(s != 0);
        x /= s;
        y /= s;
        z /= s;
        return *this;
    }

    bool operator==(const Vector& u) const
    {
        return x == u.x && y == u.y && z == u.z;
    }

    bool operator!=(const Vector& u) const
    {
        return x != u.x || y != u.y || z != u.z;
    }

    Vector abs() const
    {
        return Vector { std::abs(x), std::abs(y), std::abs(z) };
    }

    Vector operator-() const
    {
        return Vector { -x, -y, -z };
    }

    double norm_sqr() const
    {
        return x * x + y * y + z * z;
    }

    double norm() const
    {
        return std::sqrt(norm_sqr());
    }

    Vector normalized() const
    {
        double n = norm();
        assert(n > 0);
        return Vector { x / n, y / n, z / n };
    }

    Vector& normalize()
    {
        double n = norm();
        assert(n > 0);
        x /= n;
        y /= n;
        z /= n;
        return *this;
    }
};

inline Vector operator+(const double& s, const Vector& v)
{
    return Vector { s + v.x, s + v.y, s + v.z };
}

inline Vector operator-(const double& s, const Vector& v)
{
    return Vector { s - v.x, s - v.y, s - v.z };
}

inline Vector operator*(const double& s, const Vector& v)
{
    return Vector { s * v.x, s * v.y, s * v.z };
}

inline Vector operator/(const double& s, const Vector& v)
{
    assert(v.x != 0 && v.y != 0 && v.z != 0);
    return Vector { s / v.x, s / v.y, s / v.z };
}

inline double dot(const Vector& u, const Vector& v)
{
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

inline Vector cross(const Vector& u, const Vector& v)
{
    return Vector { u.y * v.z - u.z * v.y,
                    u.z * v.x - u.x * v.z,
                    u.x * v.y - u.y * v.x };
}

inline std::ostream& operator<<(std::ostream& os, const Vector& v)
{
    os << "Vector(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}
