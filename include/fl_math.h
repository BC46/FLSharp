#pragma once

// NOTE: This only works when loaded via Freelancer.exe, not FLServer.exe.
#define FL_180_OVER_PI (*(float*) 0x5D3D38)

class Vector
{
public:
    float x, y, z;
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
};

float GetRotationDelta(const Quaternion& quat, const Matrix& rot);

Quaternion MatrixToQuaternion(const Matrix& m);

#ifdef _MSC_VER
#if _MSC_VER < 1700
inline float copysign(float x, float y)
{
    return (x < 0 && y > 0) || (x > 0 && y < 0) ? -x : x;
}
#endif
#endif
