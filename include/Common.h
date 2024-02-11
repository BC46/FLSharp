#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define Import __declspec(dllimport)

#define ENGINE_TYPE 0x20000

class CEquip{};

class Import CEquipManager
{
public:
    CEquip const * FindFirst(UINT type) const;
};

struct Import CShip
{
    float get_throttle() const;
private:
    BYTE data[0xE4];
public:
    CEquipManager equipManager;
};

class Import Fuse
{
    // We don't call this function; the only purpose of this is to set the correct offset for IsTriggered() in the vftable
    virtual void Dealloc(bool unk);
};

class Import FuseAction : Fuse
{
public:
    virtual bool IsTriggered() const;
};

class Import CEEngine : public FuseAction
{
public:
    static CEEngine const * cast(CEquip const * equip);
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
