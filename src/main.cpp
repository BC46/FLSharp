#include "update.h"
#include "waypoint.h"
#include "projectiles.h"
#include "resolutions.h"

void Init()
{
    InitBetterUpdates();
    InitWaypointFix();
    InitProjectilesPerFireFix();
    //InitBetterResolutions();
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    UNREFERENCED_PARAMETER(hinstDLL);
    UNREFERENCED_PARAMETER(lpReserved);

    if (fdwReason == DLL_PROCESS_ATTACH)
        Init();

    return TRUE;
}
