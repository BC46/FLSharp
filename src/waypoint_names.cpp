#include "waypoint_names.h"
#include "Freelancer.h"
#include "utils.h"

#define GET_UNKNOWN_SIMPLE_IDS_FOR_TARGET_LIST_CALL_ADDR 0x4E40AF
#define SIMPLE_UNVISITED_CHECK_FOR_TARGET_LIST_CALL_ADDR 0x4E4094
#define SIMPLE_VISITED_CHECK_FOR_CURRENT_INFO_LIST_CALL_ADDR 0x4755B8

// Fixes waypoints being called "Unknown Object" in the target view and Current Information window.
// The two hooks below ensure that waypoints aren't treated as "unvisited".
bool IsSimpleUnvisited_Hook(const CSimple& simple)
{
    // If the simple is visited, follow the normal routine.
    if (!IsSimpleUnvisited(simple))
        return false;

    // Treat waypoints as "visited".
    return !IsObjectAWaypoint(simple);
}

BYTE GetSimpleVisitedValue_Hook(const CSimple& simple)
{
    BYTE result = GetSimpleVisitedValue(simple);

    // If the simple is unknown and it's a waypoint, set the know visit flag.
    if ((result & KNOW_VISIT_FLAG) == 0 && IsObjectAWaypoint(simple))
        result |= KNOW_VISIT_FLAG;

    return result;
}

// When you open the Current Information window while selecting a player waypoint,
// it always shows "PLAYER WAYPOINT1".
// This hook adds a space and ensures the correct number is printed.
int swprintf_Hook(int waypointIndex)
{
    // FL's original code for getting the waypoint number (incorrect).
    int waypointNumber = waypointIndex + 1;

    // If we can obtain the waypoint, use its waypoint number.
    // If this approach fails, just use the value that FL would originally use.
    if (Waypoint* waypoint = GetWaypoint(waypointIndex))
    {
        waypointNumber = waypoint->waypointNumber;
    }

    // Printf the waypoint number with an added space.
    // This gives "PLAYER WAYPOINT n" instead of "PLAYER WAYPOINTn".
    return swprintf_s(FL_BUFFER_1, FL_BUFFER_LEN, L" %d\n", waypointNumber);
}

// Ensures mission waypoints are called "Mission Waypoint" instead of "Waypoint".
UINT GetCShipOrCEqObjName_Hook(const CEqObj &eqObj)
{
    UINT result = GetCShipOrCEqObjName(eqObj);

    // Check to make sure that we're dealing with a waypoint here.
    if (result == WAYPOINT_IDS && IsObjectAWaypoint(eqObj))
    {
        int waypointIndex;
        bool isPlayerWaypoint;

        // Try to check whether this is a player waypoint.
        if (WAYPOINT_WATCHER && WAYPOINT_WATCHER->GetCurrentWaypointInfo(isPlayerWaypoint, waypointIndex))
        {
            // If it's not a player waypoint, it's a mission waypoint, so return the right IDS.
            if (!isPlayerWaypoint)
                return MISSION_WAYPOINT_IDS;
        }
    }

    return result;
}

MissionObjective* (*GetMissionObjective_Original)(int index);

// When you open the Current Information window while having a random mission waypoint selected, nothing is printed.
// This is because FL assumes a hard-coded mission objective index of 0 which is only correct for story mission waypoints.
// This hook attempts to find the correct index dynamically so that the objective is printed for random mission waypoints.
MissionObjective* GetMissionObjective_Hook(int index)
{
    #define SPACE_OBJECTIVE 0xA

    for (int i = 0; MissionObjective* missionObjective = GetMissionObjective_Original(i); ++i)
    {
        if ((missionObjective->flags & 0xF) == SPACE_OBJECTIVE)
            return missionObjective;
    }

    return nullptr;
}

// Init some waypoint name and infocard fixes.
void InitWaypointNameFixes()
{
    // Fix waypoints being called "Unknown Object" in the target view.
    Hook(SIMPLE_UNVISITED_CHECK_FOR_TARGET_LIST_CALL_ADDR, IsSimpleUnvisited_Hook, 5); // Target selection
    Hook(SIMPLE_VISITED_CHECK_FOR_CURRENT_INFO_LIST_CALL_ADDR, GetSimpleVisitedValue_Hook, 5); // Current Information window

    // Fix the player waypoint being printed incorrectly in the Current Information window.
    #define SWPRINTF_WAYPOINT_PARAMS_ADDR 0x475A6C
    #define WAYPOINT_INFO_PARAMS_CLEANED_STACK_ADDR 0x475A8C
    Patch<WORD>(SWPRINTF_WAYPOINT_PARAMS_ADDR, 0x74FF); // push waypoint number onto stack
    Hook(SWPRINTF_WAYPOINT_PARAMS_ADDR + 4, swprintf_Hook, 5);
    Nop(SWPRINTF_WAYPOINT_PARAMS_ADDR + 4 + 5, 9); // nop out unneeded param pushes
    // Decrease the cleaned stack by 8 bytes because we removed two params from our hook call.
    GetValue<BYTE>(WAYPOINT_INFO_PARAMS_CLEANED_STACK_ADDR) -= sizeof(DWORD) * 2;

    // Ensure player waypoints are called "Waypoint" and mission waypoints "Mission Waypoint".
    #define GET_OBJ_NAME_CURRENT_INFO_CALL_ADDR 0x475676
    #define GET_OBJ_NAME_TARGET_SELECTION_CALL_ADDR 0x4E8131
    Hook(GET_OBJ_NAME_CURRENT_INFO_CALL_ADDR, GetCShipOrCEqObjName_Hook, 5); // Current Information window
    Hook(GET_OBJ_NAME_TARGET_SELECTION_CALL_ADDR, GetCShipOrCEqObjName_Hook, 5); // Target selection

    // Fix nothing being show in the Current Information window for random mission waypoints
    #define GET_MISSION_OBJECTIVE_INFO_WINDOW_CALL_ADDR 0x475A94
    GetMissionObjective_Original = SetRelPointer(GET_MISSION_OBJECTIVE_INFO_WINDOW_CALL_ADDR + 1, GetMissionObjective_Hook);
}
