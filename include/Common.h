#pragma once

#include "fl_math.h"
#include "vftable.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define IMPORT __declspec(dllimport)

#define ENGINE_TYPE 0x20000

class IMPORT INI_Reader
{
public:
    INI_Reader();
    ~INI_Reader();

    bool open(LPCSTR path, bool throwExceptionOnFail = false);
    bool read_header();
    bool is_header(LPCSTR name);
    bool read_value();
    bool is_value(LPCSTR name);
    LPCSTR get_value_string(UINT index = 0);
    void close();

private:
    BYTE data[0x1565];
};

class IMPORT CEquip
{
public:
    FILL_VFTABLE(1);
    FILL_VFTABLE(2);
    FILL_VFTABLE(3);
    virtual bool Activate(bool value);
};

class IMPORT CELightEquip : public CEquip
{
public:
    static CELightEquip * cast(CEquip * equip);
};

class CELauncher
{
public:
    UINT IMPORT GetProjectilesPerFire() const;
    UINT GetProjectilesPerFire_Hook() const;
};

class IMPORT CEquipTraverser
{
public:
    CEquipTraverser(int equipClass);
private:
    BYTE data[0x100]; // Check if this works
};

class IMPORT CEquipManager
{
public:
    CEquip const * FindFirst(UINT type) const;
    CEquip * Traverse(CEquipTraverser& equipTraverser);
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

struct IMPORT CEqObj : public EngineObject
{
private:
    BYTE data[0xE4];
public:
    CEquipManager equipManager;
};

struct IMPORT CShip : public CEqObj
{
    float get_throttle() const;
    Archetype::Ship const * shiparch() const;
};

struct CSolar : public CEqObj
{};

class IMPORT FuseAction
{
public:
    // We never call Dealloc; the only purpose of defining it is to set the correct offset for IsTriggered() in the vftable
    virtual void Dealloc(bool unk);
    virtual bool IsTriggered() const;
};

class IMPORT CEEngine : public FuseAction
{
public:
    static CEEngine const * cast(CEquip const * equip);
};

struct CRemotePhysicsSimulation
{
    bool IMPORT CheckForSync(Vector const &unk1, Vector const &unk2, Quaternion const &unk3);
    bool CheckForSync_Hook(Vector const &unk1, Vector const &unk2, Quaternion const &unk3);
    bool ShouldSendUpdate(Vector const &unk1, Vector const &unk2, Quaternion const &unk3, double timeElapsed);
};

struct IObjInspectImpl
{
    BYTE data[0x10];
    CShip* ship;
};

bool IMPORT SinglePlayer();
bool IMPORT IsMPServer();
