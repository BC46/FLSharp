#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define Import __declspec(dllimport)

struct Import CShip
{
    float get_throttle() const;
};

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

struct Import CRemotePhysicsSimulation
{
    bool CheckForSync(Vector const &unk1, Vector const &unk2, Quaternion const &unk3);
};
