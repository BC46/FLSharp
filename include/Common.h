#pragma once

#include "fl_math.h"
#include "vftable.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define IMPORT __declspec(dllimport)

#define ENGINE_TYPE 0x20000

IMPORT UINT CreateID(LPCSTR str);

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
    bool get_value_bool(UINT index = 0);
    float get_value_float(UINT index = 0);
    LPCSTR get_name_ptr();
    void close();

    inline UINT get_value_uint(UINT index = 0)
    {
        return static_cast<UINT>(get_value_float(index));
    }

    inline UINT get_value_id(UINT index = 0)
    {
        return CreateID(get_value_string(index));
    }

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
    IMPORT UINT GetProjectilesPerFire() const;
    UINT GetProjectilesPerFire_Hook() const;
};

class IMPORT CEquipTraverser
{
public:
    CEquipTraverser(int equipClass);
private:
    BYTE data[0x10];
};

class IMPORT CEquipManager
{
public:
    CEquip const * FindFirst(UINT type) const;
    CEquip * Traverse(CEquipTraverser& equipTraverser);
};

namespace Archetype
{
    struct Root
    {
        BYTE data[0x44];
        int scriptIndex; // 0x44
    };

    struct Ship
    {
    private:
        BYTE data[0x108];
    public:
        Vector angularDrag;
        Vector steeringTorque;
    };
}

struct IMPORT CObject
{
    Archetype::Root* get_archetype() const;

    BYTE x00[0x4];
    long engineInstance;
};

class CAttachedEquip
{
    FILL_VFTABLE(0);
    FILL_VFTABLE(1);
    FILL_VFTABLE(2);
    FILL_VFTABLE(3);
    FILL_VFTABLE(4);
    FILL_VFTABLE(5);
    FILL_VFTABLE(6);
    FILL_VFTABLE(7);
    virtual void Vftable_x80();

public:
    virtual long GetRootIndex() const;

    CObject* parent; // x04
};

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
    bool is_using_tradelane() const;
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

class CRemotePhysicsSimulation
{
public:
    IMPORT bool CheckForSync(Vector const &shipPos, Vector const &shipPos2, Quaternion const &unk);
    bool CheckForSync_Hook(Vector const &shipPos, Vector const &shipPos2, Quaternion const &unk);
};

struct IObjRW
{
    BYTE data[0x10];
    CShip* ship; // actually a CObject* but make it CShip* for convenience
};

struct PhysicsInfo
{
    BYTE x00[0x2F];
    bool autoLevel; // 0x2F
};

class IBehaviorManager
{
public:
    BYTE x00[0x08];
    PhysicsInfo* physicsInfo; // 0x08
    BYTE x0C[0xED];
    bool rotationLock; // 0xF9
};

IMPORT IBehaviorManager* GetBehaviorManager(IObjRW *iObjRw);

struct ID_String
{};

IMPORT bool SinglePlayer();
IMPORT bool IsMPServer();
