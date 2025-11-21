#pragma once

#include "fl_math.h"
#include "vftable.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <list>
#include "st6.h"

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
    IMPORT LPCSTR get_header_ptr();
    IMPORT bool read_value();
    IMPORT bool is_value(LPCSTR name);
    IMPORT LPCSTR get_value_string(UINT index = 0);
    IMPORT bool get_value_bool(UINT index = 0);
    IMPORT float get_value_float(UINT index = 0);
    IMPORT int get_value_int(UINT index = 0);
    IMPORT LPCSTR get_name_ptr();
    IMPORT LPCSTR get_file_name() const;
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
    FILL_VFTABLE(0);
    FILL_VFTABLE(1);
    FILL_VFTABLE(2);
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

    struct EqObj
    {
        BYTE x00[0x14];
        UINT idsName; // 0x14
        UINT idsInfo; // 0x18

        // st6::list
        IMPORT bool get_undamaged_collision_group_list(std::list<CollisionGroupDesc>& colGroupList) const;
        bool get_undamaged_collision_group_list_Hook(std::list<CollisionGroupDesc>& colGroupList) const;
    };

    struct Ship : public EqObj
    {
        BYTE x1C[0xEC];
        Vector angularDrag; // 0x108
        Vector steeringTorque; // 0x114
    };

    IMPORT Ship* GetShip(UINT shipId);

    struct Solar : public EqObj
    {
    };

    struct ShieldGenerator
    {
        BYTE x00[0x94];
        float maxCapacity; // 0x94
        BYTE x98[0x8];
        float offlineThreshold; // 0xA0
    };
}

class IMPORT EngineObject
{
public:
    float const get_radius() const;
    Matrix const & get_orientation() const;

    long engineInstance; // 0x04
    BYTE x08[0x44];

private:
    FILL_VFTABLE(0);
    FILL_VFTABLE(1);
    FILL_VFTABLE(2);
    FILL_VFTABLE(3);
};

struct IMPORT CObject : public EngineObject
{
    Archetype::Root* get_archetype() const;

    DWORD classType; // 0x4C
};

struct IMPORT CSimple : CObject
{
    BYTE x50[0x60];
    UINT nickname; // xB0
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

struct IMPORT CEqObj : public CSimple
{
private:
    FILL_VFTABLE(4);
    FILL_VFTABLE(5);
    FILL_VFTABLE(6);
    FILL_VFTABLE(7);
    virtual void Vftable_x80();
    virtual void Vftable_x84();

    BYTE xB4[0x30];
public:
    CEquipManager equipManager; // 0xE4
    BYTE x104[0x5C];
    UINT baseId; // 0x160

    virtual UINT get_name() const; // 0x88
    bool is_base() const;
};

struct CShip : public CEqObj
{
    BYTE x164[0x50];
    DWORD groupId; // 0x1B4

    IMPORT float get_throttle() const;
    IMPORT Archetype::Ship const * shiparch() const;
    IMPORT bool is_using_tradelane() const;
    IMPORT UINT get_group_name() const;

    UINT get_group_name_Hook() const;
};

#define CSOLAR_CLASS_TYPE 0x303
struct CSolar : public CEqObj
{
    IMPORT bool is_dynamic() const;
    IMPORT bool is_waypoint() const;
    IMPORT Archetype::Solar const * solararch() const;

    static inline const CSolar* cast(const CObject *obj)
    {
        if ((obj->classType & CSOLAR_CLASS_TYPE) == CSOLAR_CLASS_TYPE)
            return (CSolar*) obj;

        return nullptr;
    }
};

inline bool IsObjectAWaypoint(const CObject& cobject)
{
    const CSolar* solar = CSolar::cast(&cobject);

    if (!solar)
        return false;

    return solar->is_waypoint();
}

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
    bool CheckForSync_Hook(const CShip& ship, Vector const &shipPos, Quaternion const &unk);
};

struct IObjRW // : public IObjInspectImpl
{
    BYTE x04[0xC];
    CObject* cobject; // 0x10

    FILL_VFTABLE(0)
    FILL_VFTABLE(1)
    FILL_VFTABLE(2)
    FILL_VFTABLE(3)
    FILL_VFTABLE(4)
    FILL_VFTABLE(5)
    virtual void Vftable_x60();
    virtual void Vftable_x64();
    virtual int get_attitude_towards(float &attitude, IObjRW const *other) const; // 0x68
    virtual void Vftable_x6C();
    FILL_VFTABLE(7)
    FILL_VFTABLE(8)
    FILL_VFTABLE(9)
    FILL_VFTABLE(A)
    virtual void Vftable_xB0();
    virtual void Vftable_xB4();
    virtual void Vftable_xB8();
    virtual bool is_player() const; // 0xBC
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
{
    UINT ids;
};

IMPORT bool SinglePlayer();
IMPORT bool IsMPServer();

namespace Reputation
{
    namespace Vibe
    {
        IMPORT int GetInfocard(int const& id, unsigned int& idsInfo);
    }
}

struct EquipDesc
{
    DWORD x00;
    UINT archId; // 0x4
};

class EquipDescList
{
public:
    #ifdef USE_ST6
    st6
    #else
    std
    #endif
    ::list<EquipDesc> list; // 0x0
};

enum GoodType : DWORD
{
    Commodity = 0,
    Hull = 2,
    Ship = 3
};

struct GoodInfo
{
    BYTE x00[0x4C];
    GoodType type; // 0x4C
    BYTE x50[0x4];
    UINT shipId; // only if type = Hull, 0x54
    BYTE x58[0x38];
    UINT shipHullId; // only if type = Ship, 0x90
    EquipDescList equipDescLists[3]; // 0x94
};

namespace GoodList
{
    IMPORT GoodInfo const * find_by_id(UINT id);
};


