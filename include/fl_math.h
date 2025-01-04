#pragma once

class Vector
{
public:
    float x, y, z;
};

class Quaternion
{
public:
    float w, x, y, z;
    Quaternion();
};

class Matrix
{
public:
    float data[3][3];
};

float GetRotationDelta(const Quaternion& quat, const Matrix& rot);

Quaternion MatrixToQuaternion(const Matrix& m);

#ifdef _MSC_VER
#if _MSC_VER < 1700
float inline copysign(float x, float y)
{
    return (x < 0 && y > 0) || (x > 0 && y < 0) ? -x : x;
}
# endif
#endif