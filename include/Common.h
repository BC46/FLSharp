#pragma once

#include "fl_math.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define Import __declspec(dllimport)

#define ENGINE_TYPE 0x20000

class CEquip{};

class Import CELauncher
{
public:
    UINT GetProjectilesPerFire() const;
};

class Import CEquipManager
{
public:
    CEquip const * FindFirst(UINT type) const;
};

namespace Archetype
{
    struct Ship
    {
    private:
        BYTE data[0x108];
    public:
        Vector angularDrag;
        Vector steeringTorque;
    };
}

class EngineObject
{
public:
    float const get_radius() const;
    Matrix const & get_orientation() const;
};

struct Import CShip : public EngineObject
{
    float get_throttle() const;
    Archetype::Ship const * shiparch() const;
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

struct Import CRemotePhysicsSimulation
{
    bool CheckForSync(Vector const &unk1, Vector const &unk2, Quaternion const &unk3);
};

struct IObjInspectImpl
{
    BYTE data[16];
    CShip* ship;
};

bool Import SinglePlayer();
bool Import IsMPServer();
