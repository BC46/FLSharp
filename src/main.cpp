#include "utils.h"
#include "update.h"
#include "waypoint.h"
#include "Freelancer.h"
#include "projectiles.h"

void Init()
{
    SetTimeSinceLastUpdate();

    // Hook various calls
    Hook(POST_INIT_DEALLOC_CALL_ADDR, (DWORD) PostInitDealloc_Hook, 5);
    Hook(CHECK_FOR_SYNC_CALL_ADDR, (DWORD) CheckForSync_Hook, 5);
    Hook(OBJ_UPDATE_CALL_ADDR, (DWORD) SPObjUpdate_Hook, 6);
    Hook(WAYPOINT_CHECK_CALL_ADDR, (DWORD) WaypointCheck_Hook, 5);

    static PVOID projectilesPerFireHookPtr = GetProjectilesPerFire_Hook;
    SetPointer(PROJECTILES_PER_FIRE_CALL_ADDR, &projectilesPerFireHookPtr);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    UNREFERENCED_PARAMETER(hinstDLL);
    UNREFERENCED_PARAMETER(lpReserved);

    if (fdwReason == DLL_PROCESS_ATTACH)
        Init();

    return TRUE;
}
