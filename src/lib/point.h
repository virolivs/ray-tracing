#pragma once

#include <cassert>
#include <cmath>
#include <ostream>
#include "vector.h"

struct Point
{
    union
    {
        struct
        {
            float x;
            float y;
            float z;
        };
        float p[3] {};
    };

    Point() : x { 0.0f }, y { 0.0f }, z { 0.0f } {}
    explicit Point(float x, float y, float z) : x { x }, y { y }, z { z } {}
    explicit Point(float s) : x { s }, y { s }, z { s } {}

    Point(const Point&) = default;
    ~Point() = default;
    Point& operator=(const Point&) = default;

    float& operator[](const size_t& idx)
    {
        assert(idx <= 2);
        return p[idx];
    }

    float operator[](const size_t& idx) const
    {
        assert(idx <= 2);
        return p[idx];
    }

    Point operator+(const Vector& v) const
    {
        return Point { x + v.x, y + v.y, z + v.z };
    }

    Point operator-(const Vector& v) const
    {
        return Point { x - v.x, y - v.y, z - v.z };
    }

    Point& operator+=(const Vector& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    Point& operator-=(const Vector& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    Vector operator-(const Point& q) const
    {
        return Vector { x - q.x, y - q.y, z - q.z };
    }

    Point operator+(const float& s) const
    {
        return Point { x + s, y + s, z + s };
    }

    Point operator-(const float& s) const
    {
        return Point { x - s, y - s, z - s };
    }

    Point operator*(const float& s) const
    {
        return Point { x * s, y * s, z * s };
    }

    Point operator/(const float& s) const
    {
        assert(s != 0);
        return Point { x / s, y / s, z / s };
    }

    Point& operator+=(const float& s)
    {
        x += s;
        y += s;
        z += s;
        return *this;
    }

    Point& operator-=(const float& s)
    {
        x -= s;
        y -= s;
        z -= s;
        return *this;
    }

    Point& operator*=(const float& s)
    {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    Point& operator/=(const float& s)
    {
        assert(s != 0);
        x /= s;
        y /= s;
        z /= s;
        return *this;
    }

    bool operator==(const Point& q) const
    {
        return x == q.x && y == q.y && z == q.z;
    }

    bool operator!=(const Point& q) const
    {
        return x != q.x || y != q.y || z != q.z;
    }

    Point abs() const
    {
        return Point { std::abs(x), std::abs(y), std::abs(z) };
    }

    Point operator-() const
    {
        return Point { -x, -y, -z };
    }
};

inline Point operator+(const Vector& v, const Point& p)
{
    return Point { v.x + p.x, v.y + p.y, v.z + p.z };
}

inline Point operator-(const Vector& v, const Point& p)
{
    return Point { v.x - p.x, v.y - p.y, v.z - p.z };
}

inline Point operator+(const float& s, const Point& p)
{
    return Point { s + p.x, s + p.y, s + p.z };
}

inline Point operator-(const float& s, const Point& p)
{
    return Point { s - p.x, s - p.y, s - p.z };
}

inline Point operator*(const float& s, const Point& p)
{
    return Point { s * p.x, s * p.y, s * p.z };
}

inline Point operator/(const float& s, const Point& p)
{
    assert(p.x != 0 && p.y != 0 && p.z != 0);
    return Point { s / p.x, s / p.y, s / p.z };
}

inline std::ostream& operator<<(std::ostream& os, const Point& p)
{
    os << "Point(" << p.x << ", " << p.y << ", " << p.z << ")";
    return os;
}
