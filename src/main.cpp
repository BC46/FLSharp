#include "update.h"
#include "waypoint.h"
#include "projectiles.h"
#include "resolutions.h"
#include "test_sounds.h"
#include "trade_lane_lights.h"
#include "copy_paste.h"
#include "ui_anim.h"
#include "weapon_anim.h"
#include "flash_particles.h"

void Init()
{
    if (!IsMPServer())
    {
        InitBetterUpdates();
        InitWaypointFixes();
        InitProjectilesSoundFix();
        InitBetterResolutions();
        InitTestSounds();
        InitTradeLaneLightsFix();
        InitCopyPasteFeature();
        InitSlideUiAnimFix();
        InitWeaponAnimFix();
        InitFlashParticlesFix();
    }

    InitProjectilesServerFix();
}

void Cleanup()
{
    if (!IsMPServer())
    {
        CleanupBetterResolutions();
    }
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    UNREFERENCED_PARAMETER(hinstDLL);
    UNREFERENCED_PARAMETER(lpReserved);

    if (fdwReason == DLL_PROCESS_ATTACH)
        Init();
    else if (fdwReason == DLL_PROCESS_DETACH)
        Cleanup();

    return TRUE;
}
