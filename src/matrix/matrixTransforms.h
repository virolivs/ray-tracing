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

// Returns a rotation matrix around the Y-axis (3D rotation)
Matrix rotationYMatrix(float angle) {
    Matrix m = identityMatrix();

    float c = std::cos(angle);
    float s = std::sin(angle);

    m.data[0][0] = c;
    m.data[0][2] = s;
    m.data[2][0] = -s;
    m.data[2][2] = c;

    return m;
}

// Returns a 2D rotation matrix around the Z-axis (rotation in XY plane)
Matrix rotationZMatrix(float angle) {
    Matrix m = identityMatrix();

    float c = std::cos(angle);
    float s = std::sin(angle);

    m.data[0][0] = c;
    m.data[0][1] = -s;
    m.data[1][0] = s;
    m.data[1][1] = c;

    return m;
}

// Returns a rotation matrix around the X-axis (3D rotation)
Matrix rotationXMatrix(float angle) {
    Matrix m = identityMatrix();

    float c = std::cos(angle);
    float s = std::sin(angle);

    m.data[1][1] = c;
    m.data[1][2] = -s;
    m.data[2][1] = s;
    m.data[2][2] = c;

    return m;
}


// Returns a reflection matrix over the X-axis
Matrix reflectionXMatrix() {
    Matrix m = identityMatrix();
    m.data[1][1] = -1.0f;
    return m;
}

// Returns a reflection matrix over the Y-axis
Matrix reflectionYMatrix() {
    Matrix m = identityMatrix();
    m.data[0][0] = -1.0f;
    return m;
}

// Returns a shear matrix along the X-axis (kx is the shear factor)
Matrix shearXMatrix(float kx) {
    Matrix m = identityMatrix();
    m.data[0][1] = kx;
    return m;
}

// Returns a shear matrix along the Y-axis (ky is the shear factor)
Matrix shearYMatrix(float ky) {
    Matrix m = identityMatrix();
    m.data[1][0] = ky;
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
