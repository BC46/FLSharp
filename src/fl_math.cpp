#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "fl_math.h"

#define M_PI 3.14159265358979323846f

Quaternion MatrixToQuaternion(const Matrix& m)
{
    Quaternion result;

    result.w = sqrtf(max(0, 1 + m.data[0][0] + m.data[1][1] + m.data[2][2])) / 2;
    result.x = sqrtf(max(0, 1 + m.data[0][0] - m.data[1][1] - m.data[2][2])) / 2;
    result.y = sqrtf(max(0, 1 - m.data[0][0] + m.data[1][1] - m.data[2][2])) / 2;
    result.z = sqrtf(max(0, 1 - m.data[0][0] - m.data[1][1] + m.data[2][2])) / 2;
    result.x = copysign(result.x, m.data[2][1] - m.data[1][2]);
    result.y = copysign(result.y, m.data[0][2] - m.data[2][0]);
    result.z = copysign(result.z, m.data[1][0] - m.data[0][1]);

    return result;
}

float QuaternionDotProduct(const Quaternion &left, const Quaternion &right)
{
    return left.x * right.x + left.y * right.y + left.z * right.z + left.w * right.w;
}

float QuaternionAngleDifference(const Quaternion &left, const Quaternion &right)
{
    float dot = QuaternionDotProduct(left, right);
    return acosf(fabsf(dot)) * 2 * (180.0f / M_PI);
}

float GetRotationDelta(const Quaternion& quat, const Matrix& rot)
{
    return QuaternionAngleDifference(quat, MatrixToQuaternion(rot));
}

Matrix Matrix::Transpose() const
{
    Matrix result;

    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            result.data[i][j] = data[j][i];

    return result;
}

Vector Matrix::operator*(Vector const &v) const
{
    Vector result;
    result.x = data[0][0] * v.x + data[0][1] * v.y + data[0][2] * v.z;
    result.y = data[1][0] * v.x + data[1][1] * v.y + data[1][2] * v.z;
    result.z = data[2][0] * v.x + data[2][1] * v.y + data[2][2] * v.z;
    return result;
}

float Vector::Length() const
{
    return std::sqrtf(x * x + y * y + z * z);
}

Vector Vector::Normalize() const
{
    Vector result;

    float len = Length();
    if (len > 0.0f)
    {
        result.x = x / len;
        result.y = y / len;
        result.z = z / len;
    }
    else
    {
        result.x = result.y = result.z = 0.0f;
    }

    return result;
}

Vector Vector::operator+(Vector const &v) const
{
    Vector result;
    result.x = x + v.x;
    result.y = y + v.y;
    result.z = z + v.z;
    return result;
}

Vector Vector::operator-(Vector const &v) const
{
    Vector result;
    result.x = x - v.x;
    result.y = y - v.y;
    result.z = z - v.z;
    return result;
}

Vector Vector::operator*(float c) const
{
    Vector result;
    result.x = x * c;
    result.y = y * c;
    result.z = z * c;
    return result;
}
