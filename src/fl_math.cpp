#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cmath>
#include "fl_math.h"

float copysign(float x, float y)
{
    return (x < 0 && y > 0) || (x > 0 && y < 0) ? -x : x;
}

Quaternion HkMatrixToQuaternion(const Matrix& m)
{
	Quaternion quaternion;
	quaternion.w = sqrtf(max(0, 1 + m.data[0][0] + m.data[1][1] + m.data[2][2])) / 2;
	quaternion.x = sqrtf(max(0, 1 + m.data[0][0] - m.data[1][1] - m.data[2][2])) / 2;
	quaternion.y = sqrtf(max(0, 1 - m.data[0][0] + m.data[1][1] - m.data[2][2])) / 2;
	quaternion.z = sqrtf(max(0, 1 - m.data[0][0] - m.data[1][1] + m.data[2][2])) / 2;
	quaternion.x = copysign(quaternion.x, m.data[2][1] - m.data[1][2]);
	quaternion.y = copysign(quaternion.y, m.data[0][2] - m.data[2][0]);
	quaternion.z = copysign(quaternion.z, m.data[1][0] - m.data[0][1]);
	return quaternion;
}

float QuaternionDotProduct(Quaternion left, Quaternion right)
{
	return left.x * right.x + left.y * right.y + left.z * right.z + left.w * right.w;
}

float QuaternionAngleDifference(Quaternion left, Quaternion right)
{
	float dot = QuaternionDotProduct(left, right);
	return acosf(fabsf(dot)) * 2 * 57.29578f;
}

float CheckRotationDelta(const Quaternion& quat, const Matrix& rot)
{
	Quaternion quat2 = HkMatrixToQuaternion(rot);
	return QuaternionAngleDifference(quat, quat2);
}

Quaternion::Quaternion()
{
    w = 1;
    x = 0;
    y = 0;
    z = 0;
}
