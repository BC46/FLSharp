#include "waypoint.h"
#include "utils.h"
#include "Freelancer.h"

#define PLAYERSHIP_NAVMAP_OBJ_TYPE 2
#define NAV_MAP_GET_HIGHLIGHTED_OBJ_WAYPOINT_CALL_ADDR 0x493A00
#define NAV_MAP_GET_HIGHLIGHTED_OBJ_BESTPATH_CALL_ADDR 0x493B21

// Hook that prevents waypoints from being cleared when the player is in a different system
WaypointInfo* WaypointCheck_Hook(UINT index)
{
    WaypointInfo* waypointInfo = WaypointCheck(index);

    if (!waypointInfo)
        return nullptr;

    // Only return the waypoint info if the player is in the same system as the waypoint
    return PLAYER_SYSTEM == waypointInfo->system ? waypointInfo : nullptr;
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

// Adds some minor waypoint-related fixes.
void InitWaypointFixes()
{
    Hook(WAYPOINT_CHECK_CALL_ADDR, WaypointCheck_Hook, 5);
    Hook(NAV_MAP_GET_HIGHLIGHTED_OBJ_WAYPOINT_CALL_ADDR, &NeuroNetNavMap::GetHighlightedObject_Hook, 5);
    Hook(NAV_MAP_GET_HIGHLIGHTED_OBJ_BESTPATH_CALL_ADDR, &NeuroNetNavMap::GetHighlightedObject_Hook, 5);
}
