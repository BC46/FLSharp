#pragma once

#include <cmath>

class Vector
{
public:
    float x, y, z;

    float Length() const;

    Vector Normalize() const;

    Vector operator+(Vector const& v) const;
    Vector operator-(Vector const& v) const;
    Vector operator*(float c) const;
};

class Quaternion
{
public:
    float w, x, y, z;
};

class Matrix
{
public:
    float data[3][3];

    Matrix Transpose() const;
    Vector operator*(Vector const& v) const;
};

float GetRotationDelta(const Quaternion& quat, const Matrix& rot);

Quaternion MatrixToQuaternion(const Matrix& m);
