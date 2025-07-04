#pragma once
#include "matrix.h"
#include <cmath>
#include <memory>

// Returns a 4x4 identity matrix
Matrix identityMatrix() {
    Matrix m = {};
    for (int i = 0; i < 4; ++i)
        m.data[i][i] = 1.0f;
    return m;
}

// Returns a translation matrix for displacement (dx, dy, dz)
Matrix translationMatrix(float dx, float dy, float dz) {
    Matrix m = identityMatrix();
    m.data[0][3] = dx;
    m.data[1][3] = dy;
    m.data[2][3] = dz;
    return m;
}

// Returns a scaling matrix with scale factors (sx, sy, sz)
Matrix scaleMatrix(float sx, float sy, float sz) {
    Matrix m = {};
    m.data[0][0] = sx;
    m.data[1][1] = sy;
    m.data[2][2] = sz;
    m.data[3][3] = 1.0f;
    return m;
}

// Returns a general rotation matrix around axis 'x', 'y' ou 'z'
Matrix rotationMatrix(char axis, float angle) {
    Matrix m = identityMatrix();
    float c = std::cos(angle);
    float s = std::sin(angle);

    switch (axis) {
        case 'x':
        case 'X':
            m.data[1][1] = c;
            m.data[1][2] = -s;
            m.data[2][1] = s;
            m.data[2][2] = c;
            break;
        case 'y':
        case 'Y':
            m.data[0][0] = c;
            m.data[0][2] = s;
            m.data[2][0] = -s;
            m.data[2][2] = c;
            break;
        case 'z':
        case 'Z':
            m.data[0][0] = c;
            m.data[0][1] = -s;
            m.data[1][0] = s;
            m.data[1][1] = c;
            break;
        default:
            // No rotation if invalid axis
            break;
    }

    return m;
}


// Returns a general reflection matrix over a plane defined by booleans:
// reflectX → inverte X (YZ plane), reflectY → inverte Y (XZ plane), reflectZ → inverte Z (XY plane)
Matrix reflectionMatrix(bool reflectX, bool reflectY, bool reflectZ) {
    Matrix m = identityMatrix();
    if (reflectX) m.data[0][0] = -1.0f;
    if (reflectY) m.data[1][1] = -1.0f;
    if (reflectZ) m.data[2][2] = -1.0f;
    return m;
}

// Returns a shear matrix
Matrix shearMatrix(float xy, float xz, float yx, float yz, float zx, float zy) {
    Matrix m = identityMatrix();

    m.data[0][1] = xy; // X em função de Y
    m.data[0][2] = xz; // X em função de Z
    m.data[1][0] = yx; // Y em função de X
    m.data[1][2] = yz; // Y em função de Z
    m.data[2][0] = zx; // Z em função de X
    m.data[2][1] = zy; // Z em função de Y

    return m;
}

// Matrix multiplication operator for 4x4 matrices
Matrix operator*(const Matrix& a, const Matrix& b) {
    Matrix result = {};
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            float sum = 0.0f;
            for (int k = 0; k < 4; ++k)
            {
                sum += a.data[i][k] * b.data[k][j];
            }
            result.data[i][j] = sum;
        }
    }
    return result;
}