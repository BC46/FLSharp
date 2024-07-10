#include "waypoint.h"
#include "utils.h"

WaypointInfo* WaypointCheck_Hook(UINT index)
{
    WaypointInfo* waypointInfo = ((WaypointCheck*) WAYPOINT_CHECK_ADDR)(index);

    if (!waypointInfo)
        return NULL;

    UINT playerSystem = *(PUINT) PLAYER_SYSTEM_ADDR;

    // Only return the waypoint info if the player is in the same system as the waypoint
    return playerSystem == waypointInfo->system ? waypointInfo : NULL;
}

void InitWaypointFix()
{
    Hook(WAYPOINT_CHECK_CALL_ADDR, WaypointCheck_Hook, 5);
}
