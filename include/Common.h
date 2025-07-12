#pragma once

#include "fl_math.h"
#include "vftable.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <list>

#define IMPORT __declspec(dllimport)

#define ENGINE_TYPE 0x20000

IMPORT UINT CreateID(LPCSTR str);

class INI_Reader
{
public:
    IMPORT INI_Reader();
    IMPORT ~INI_Reader();

    IMPORT bool open(LPCSTR path, bool throwExceptionOnFail = false);
    IMPORT bool read_header();
    IMPORT bool is_header(LPCSTR name);
    IMPORT bool read_value();
    IMPORT bool is_value(LPCSTR name);
    IMPORT LPCSTR get_value_string(UINT index = 0);
    IMPORT bool get_value_bool(UINT index = 0);
    IMPORT float get_value_float(UINT index = 0);
    IMPORT int get_value_int(UINT index = 0);
    IMPORT LPCSTR get_name_ptr();
    IMPORT void close();

    // inline UINT get_value_uint(UINT index = 0)
    // {
    //     return static_cast<UINT>(get_value_float(index));
    // }

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

private:
    BYTE x00[0x20];
};

struct CollisionGroupDesc
{};

namespace Archetype
{
    struct Root
    {
        BYTE data[0x44];
        int scriptIndex; // 0x44

        IMPORT int get_script_index() const;
    };

    struct Ship
    {
    private:
        BYTE data[0x108];
    public:
        Vector angularDrag;
        Vector steeringTorque;
    };

    struct EqObj
    {
        IMPORT bool get_undamaged_collision_group_list(std::list<CollisionGroupDesc>& colGroupList) const;
        bool get_undamaged_collision_group_list_Hook(std::list<CollisionGroupDesc>& colGroupList) const;
    };
}

struct IMPORT CObject
{
    Archetype::Root* get_archetype() const;

    DWORD vftable; // 0x00
    long engineInstance; // 0x04
    BYTE x08[0x44];
    DWORD classType; // 0x4C
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
    BYTE x00[0xE4];
public:
    CEquipManager equipManager; // 0xE4
    BYTE x104[0x5C];
    UINT baseId; // 0x160

    bool is_base() const;
};

struct IMPORT CShip : public CEqObj
{
    float get_throttle() const;
    Archetype::Ship const * shiparch() const;
    bool is_using_tradelane() const;
};

#define CSOLAR_CLASS_TYPE 0x303
struct CSolar : public CEqObj
{
    IMPORT bool is_dynamic() const;
    IMPORT bool is_waypoint() const;

    static inline CSolar* cast(CObject *obj)
    {
        if ((obj->classType & CSOLAR_CLASS_TYPE) == CSOLAR_CLASS_TYPE)
            return (CSolar*) obj;

        return nullptr;
    }
};

struct CSimple : CObject
{
    inline bool is_waypoint() const
    {
        if ((classType & CSOLAR_CLASS_TYPE) == CSOLAR_CLASS_TYPE)
            return ((CSolar*) this)->is_waypoint();

        return false;
    }
};

class IMPORT FuseAction
{
public:
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
    CObject* cobject;
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

namespace Reputation
{
    namespace Vibe
    {
        IMPORT int GetInfocard(int const& id, unsigned int& idsInfo);
    }
}
