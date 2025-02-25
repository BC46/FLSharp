#include "feature_config.h"
#include "config_reader.h"
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
#include "rep_requirements.h"
#include "cgun_wrapper.h"
#include "temp_fixes.h"

FeatureManager manager;

void Init()
{
    manager.RegisterFeature("better_updates",           InitBetterUpdates,          NULL,                       ApplyOnlyOnClient);
    manager.RegisterFeature("waypoint_fixes",           InitWaypointFixes,          NULL,                       ApplyOnlyOnClient);
    manager.RegisterFeature("projectiles_sound_fix",    InitProjectilesSoundFix,    NULL,                       ApplyOnlyOnClient);
    manager.RegisterFeature("projectiles_server_fix",   InitProjectilesServerFix,   NULL,                       ApplyAlways);
    manager.RegisterFeature("better_resolutions",       InitBetterResolutions,      CleanupBetterResolutions,   ApplyOnlyOnClient);
    manager.RegisterFeature("more_test_sounds",         InitTestSounds,             NULL,                       ApplyOnlyOnClient);
    manager.RegisterFeature("trade_lane_lights_fix",    InitTradeLaneLightsFix,     NULL,                       ApplyOnlyOnClient);
    manager.RegisterFeature("copy_paste_feature",       InitCopyPasteFeature,       NULL,                       ApplyOnlyOnClient);
    manager.RegisterFeature("slide_ui_anim_fix",        InitSlideUiAnimFix,         NULL,                       ApplyOnlyOnClient);
    manager.RegisterFeature("weapon_anim_fix",          InitWeaponAnimFix,          NULL,                       ApplyOnlyOnClient);
    manager.RegisterFeature("flash_particle_fix",       InitFlashParticlesFix,      NULL,                       ApplyOnlyOnClient);
    manager.RegisterFeature("print_rep_requirements",   InitPrintRepRequirements,   NULL,                       ApplyOnlyOnClient);
    manager.RegisterFeature("post_game_deadlock_fix",   InitPostGameDeadlockFix,    NULL,                       ApplyOnlyOnClient);
    manager.RegisterFeature("flight_controls_fix",      InitFlightControlsFix,    NULL,                       ApplyOnlyOnClient);

    ReadConfig("FLSharp.ini", manager);

    manager.InitFeatures();
}

void Cleanup()
{
    manager.CleanupFeatures();
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
