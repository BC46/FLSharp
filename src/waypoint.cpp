#include "waypoint.h"
#include "utils.h"
#include "Freelancer.h"

#define PLAYERSHIP_NAVMAP_OBJ_TYPE 2
#define NAV_MAP_GET_HIGHLIGHTED_OBJ_WAYPOINT_CALL_ADDR 0x493A00
#define NAV_MAP_GET_HIGHLIGHTED_OBJ_BESTPATH_CALL_ADDR 0x493B21
#define SIMPLE_UNVISITED_CHECK_FOR_TARGET_LIST_CALL_ADDR 0x4E4094
#define SIMPLE_VISITED_CHECK_FOR_TARGET_LIST_CALL_ADDR 0x4755B8
#define GET_UNKNOWN_SIMPLE_IDS_FOR_TARGET_LIST_CALL_ADDR 0x4E40AF

// Hook that prevents waypoints from being cleared when the player is in a different system
Waypoint* GetWaypoint_Hook(int index)
{
    Waypoint* waypoint = GetWaypoint(index);

    if (!waypoint)
        return nullptr;

    // Only return the waypoint info if the player is in the same system as the waypoint
    return PLAYER_SYSTEM == waypoint->system ? waypoint : nullptr;
}

// Hook that prevents waypoints from being set at the player ship's location
NavMapObj* NeuroNetNavMap::GetHighlightedObject_Hook(DWORD unk1, DWORD unk2)
{
    NavMapObj* result = GetHighlightedObject(unk1, unk2);

    if (!result)
        return nullptr;

    // Only return the nav map obj if it isn't the player ship
    return result->type == PLAYERSHIP_NAVMAP_OBJ_TYPE ? nullptr : result;
}

// Fixes waypoints being called "Unknown Object" in the target view.
// The two hooks below ensure that waypoints aren't treated as "unvisited".
// Mission Waypoints will be called "Waypoint", too.
bool IsSimpleUnvisited_Hook(const CSimple& simple)
{
    // If the simple is visited, follow the normal routine.
    if (!IsSimpleUnvisited(simple))
        return false;

    // Treat waypoints as "visited".
    return !simple.is_waypoint();
}

bool IsSimpleVisited_Hook(const CSimple& simple)
{
    // If the simple is visited, follow the normal routine.
    if (IsSimpleVisited(simple))
        return true;

    // Treat waypoints as "visited".
    return simple.is_waypoint();
}

// When you open the Current Information window while selecting a player waypoint,
// it always shows "PLAYER WAYPOINT1".
// This hook adds a space and ensures the correct number is printed.
int swprintf_Hook(int waypointIndex)
{
    int waypointNumber = waypointIndex + 1;

    if (Waypoint* waypoint = GetWaypoint(waypointIndex))
    {
        waypointNumber = waypoint->waypointNumber;
    }

    return swprintf_s(FL_BUFFER_1, FL_BUFFER_LEN, L" %d\n", waypointNumber);
}

// TODO: document
UINT GetCShipOrCEqObjName_Hook(const CEqObj &eqObj)
{
    UINT result = GetCShipOrCEqObjName(eqObj);

    if (result == WAYPOINT_IDS && ((CSimple*) &eqObj)->is_waypoint())
    {
        int waypointIndex;
        bool isPlayerWaypoint;

        if (WAYPOINT_WATCHER && WAYPOINT_WATCHER->GetCurrentWaypointInfo(isPlayerWaypoint, waypointIndex))
        {
            if (!isPlayerWaypoint)
                return MISSION_WAYPOINT_IDS;
        }
    }

    return result;
}

// Adds some minor waypoint-related fixes.
void InitWaypointFixes()
{
    Hook(WAYPOINT_CHECK_CALL_ADDR, GetWaypoint_Hook, 5);
    Hook(NAV_MAP_GET_HIGHLIGHTED_OBJ_WAYPOINT_CALL_ADDR, &NeuroNetNavMap::GetHighlightedObject_Hook, 5);
    Hook(NAV_MAP_GET_HIGHLIGHTED_OBJ_BESTPATH_CALL_ADDR, &NeuroNetNavMap::GetHighlightedObject_Hook, 5);
    Hook(SIMPLE_UNVISITED_CHECK_FOR_TARGET_LIST_CALL_ADDR, IsSimpleUnvisited_Hook, 5); // Target selection
    Hook(SIMPLE_VISITED_CHECK_FOR_TARGET_LIST_CALL_ADDR, IsSimpleVisited_Hook, 5); // Current Information window

    // TODO: document
    Patch<WORD>(0x475A6C, 0x74FF);
    Hook(0x475A70, swprintf_Hook, 5);
    Nop(0x475A75, 9);
    GetValue<BYTE>(0x475A8C) -= sizeof(DWORD) * 2;

    Hook(0x475676, GetCShipOrCEqObjName_Hook, 5);
    Hook(0x4E8131, GetCShipOrCEqObjName_Hook, 5);
}
