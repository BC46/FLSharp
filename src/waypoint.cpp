#include "waypoint.h"

#define WAYPOINT_CHECK_ADDR 0x4C46A0
#define PLAYER_SYSTEM_ADDR 0x673354

typedef PDWORD (WaypointCheck)(UINT index);

PDWORD WaypointCheck_Hook(UINT index)
{
    PDWORD origResult = ((WaypointCheck*) WAYPOINT_CHECK_ADDR)(index);

    if (!origResult)
        return NULL;

    UINT playerSystem = *(PUINT) PLAYER_SYSTEM_ADDR;
    UINT waypointSystem = *(PUINT) ((PBYTE) origResult + 12);

    return playerSystem == waypointSystem ? origResult : NULL;
}
