#include "Freelancer.h"
#include "fl_func.h"
#include "utils.h"

FL_FUNC(void StopSound(BYTE soundId), 0x5646E0)
FL_FUNC(void StartSound(BYTE soundId), 0x564650)

FL_FUNC(UINT GetFlStringFromResources(DWORD resourcesHandle, UINT ids, LPWSTR buffer, UINT bufferLen), 0x4347E0)

FL_FUNC(NavMapObj* NeuroNetNavMap::GetHighlightedObject(DWORD unk1, DWORD unk2), 0x496D40)

FL_FUNC(Waypoint* GetWaypoint(int index), 0x4C46A0)
FL_FUNC(bool WaypointWatcher::GetCurrentWaypointInfo(bool& isPlayerWaypoint, int& waypointIndex), 0x4F42A0);

FL_FUNC(IObjRW* GetPlayerIObjRW(), 0x54BAF0);

CShip* GetPlayerShip()
{
    IObjRW* playerIObjRW = GetPlayerIObjRW();
    return !playerIObjRW ? nullptr : (CShip*) playerIObjRW->cobject;
}

CShip* GetPlayerShipSafe()
{
    IObjRW* playerIObjRW = GetPlayerIObjRW();

    if (playerIObjRW && playerIObjRW->cobject)
    {
        if ((playerIObjRW->cobject->classType & CSHIP_CLASS_TYPE) == CSHIP_CLASS_TYPE)
            return (CShip*) playerIObjRW->cobject;
    }

    return nullptr;
}

// Assumes both the CObjects of IObjRWs are CShips.
bool AreIObjRWsInSameGroup(const IObjRW& o1, const IObjRW& o2)
{
    auto& ship1 = (const CShip&) *o1.cobject;
    auto& ship2 = (const CShip&) *o2.cobject;

    return ship1.groupId && ship1.groupId == ship2.groupId;
}

FL_FUNC(bool IsSimpleUnvisited(const CSimple& simple), 0x4D4C70);
FL_FUNC(BYTE GetSimpleVisitedValue(const CSimple& simple), 0x4D4D00);
FL_FUNC(UINT GetIdsForUnvisitedSimple(const CSimple& simple), 0x4D4D50);

FL_FUNC(UINT GetCShipOrCEqObjName(const CEqObj &eqObj), 0x5472A0);

FL_FUNC(bool NN_Preferences::SetResolution(UINT width, DWORD unk, UINT height), 0x4B1C00)

void ExpandNNShipTraderObjMemory()
{
    #define NN_SHIPTRADER_OBJ_SIZE_ADDR 0x4B9739
    static bool memoryExpanded = false;

    if (!memoryExpanded)
    {
        // Expand the size of the NN_ShipTrader object if it hasn't been done yet.
        GetValue<UINT>(NN_SHIPTRADER_OBJ_SIZE_ADDR) += sizeof(NN_ShipTrader::shipRepPercentages);
        memoryExpanded = true;
    }
}

FL_FUNC(void UpdateDeltaTime(), 0x42D770)

FL_FUNC(UINT GetNumOfActiveMissionObjectives(), 0x4C4FB0)
