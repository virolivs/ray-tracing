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
            float x;
            float y;
            float z;
        };
        float v[3] {};
    };

    Vector() : x { 0.0f }, y { 0.0f }, z { 0.0f } {}
    explicit Vector(float x, float y, float z) : x { x }, y { y }, z { z } {}
    explicit Vector(float s) : x { s }, y { s }, z { s } {}

    Vector(const Vector&) = default;
    ~Vector() = default;
    Vector& operator=(const Vector&) = default;

    float& operator[](const size_t& idx)
    {
        assert(idx <= 2);
        return v[idx];
    }

    float operator[](const size_t& idx) const
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

    Vector operator+(const float& s) const
    {
        return Vector { x + s, y + s, z + s };
    }

    Vector operator-(const float& s) const
    {
        return Vector { x - s, y - s, z - s };
    }

    Vector operator*(const float& s) const
    {
        return Vector { x * s, y * s, z * s };
    }

    Vector operator/(const float& s) const
    {
        assert(s != 0);
        return Vector { x / s, y / s, z / s };
    }

    Vector& operator+=(const float& s)
    {
        x += s;
        y += s;
        z += s;
        return *this;
    }

    Vector& operator-=(const float& s)
    {
        x -= s;
        y -= s;
        z -= s;
        return *this;
    }

    Vector& operator*=(const float& s)
    {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    Vector& operator/=(const float& s)
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

    float norm_sqr() const
    {
        return x * x + y * y + z * z;
    }

    float norm() const
    {
        return std::sqrt(norm_sqr());
    }

    Vector normalized() const
    {
        float n = norm();
        assert(n > 0);
        return Vector { x / n, y / n, z / n };
    }

    Vector& normalize()
    {
        float n = norm();
        assert(n > 0);
        x /= n;
        y /= n;
        z /= n;
        return *this;
    }
};

inline Vector operator+(const float& s, const Vector& v)
{
    return Vector { s + v.x, s + v.y, s + v.z };
}

inline Vector operator-(const float& s, const Vector& v)
{
    return Vector { s - v.x, s - v.y, s - v.z };
}

inline Vector operator*(const float& s, const Vector& v)
{
    return Vector { s * v.x, s * v.y, s * v.z };
}

inline Vector operator/(const float& s, const Vector& v)
{
    assert(v.x != 0 && v.y != 0 && v.z != 0);
    return Vector { s / v.x, s / v.y, s / v.z };
}

inline float dot(const Vector& u, const Vector& v)
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
