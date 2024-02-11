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

float CheckRotationDelta(const Quaternion& quat, const Matrix& rot);

void HkMatrixToQuaternion(const Matrix& m, Quaternion &dest);
