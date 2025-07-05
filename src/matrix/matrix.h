#pragma once
#include "../src/lib/point.h"
#include "../src/lib/vector.h"
#include <memory>

struct Matrix {
    double data[4][4];

    // Multiplies the matrix by a point (homogeneous coordinates, w = 1)
    Point applyToPoint(const Point& p) const {
        double x = data[0][0] * p.x + data[0][1] * p.y + data[0][2] * p.z + data[0][3];
        double y = data[1][0] * p.x + data[1][1] * p.y + data[1][2] * p.z + data[1][3];
        double z = data[2][0] * p.x + data[2][1] * p.y + data[2][2] * p.z + data[2][3];
        double w = data[3][0] * p.x + data[3][1] * p.y + data[3][2] * p.z + data[3][3];

        // If w is not 0 or 1, normalize the result
        if (w != 0.0f and w!= 1.0f) {
            x /= w;
            y /= w;
            z /= w;
        }
        return Point(x, y, z);
    }

    // Multiplies the matrix by a vector (homogeneous coordinates, w = 0)
    // This ignores translation, as direction vectors are not affected by position shifts.
    Vector applyToVector(const Vector& v) const {
        double x = data[0][0] * v.x + data[0][1] * v.y + data[0][2] * v.z;
        double y = data[1][0] * v.x + data[1][1] * v.y + data[1][2] * v.z;
        double z = data[2][0] * v.x + data[2][1] * v.y + data[2][2] * v.z;
        return Vector(x, y, z);
    }
};