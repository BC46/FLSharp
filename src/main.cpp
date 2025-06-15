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
#include "infocards.h"

FeatureManager manager;

void Init()
{
    manager.RegisterFeature("better_updates",           InitBetterUpdates,          nullptr,                    ApplyOnlyOnClient);
    manager.RegisterFeature("waypoint_fixes",           InitWaypointFixes,          nullptr,                    ApplyOnlyOnClient);
    manager.RegisterFeature("projectiles_sound_fix",    InitProjectilesSoundFix,    nullptr,                    ApplyOnlyOnClient);
    manager.RegisterFeature("projectiles_server_fix",   InitProjectilesServerFix,   nullptr,                    ApplyAlways);
    manager.RegisterFeature("better_resolutions",       InitBetterResolutions,      CleanupBetterResolutions,   ApplyOnlyOnClient);
    manager.RegisterFeature("more_test_sounds",         InitTestSounds,             nullptr,                    ApplyOnlyOnClient);
    manager.RegisterFeature("trade_lane_lights_fix",    InitTradeLaneLightsFix,     nullptr,                    ApplyOnlyOnClient);
    manager.RegisterFeature("copy_paste_feature",       InitCopyPasteFeature,       nullptr,                    ApplyOnlyOnClient);
    manager.RegisterFeature("slide_ui_anim_fix",        InitSlideUiAnimFix,         nullptr,                    ApplyOnlyOnClient);
    manager.RegisterFeature("weapon_anim_fix",          InitWeaponAnimFix,          nullptr,                    ApplyOnlyOnClient);
    manager.RegisterFeature("flash_particle_fix",       InitFlashParticlesFix,      nullptr,                    ApplyOnlyOnClient);
    manager.RegisterFeature("print_rep_requirements",   InitPrintRepRequirements,   nullptr,                    ApplyOnlyOnClient);
    manager.RegisterFeature("post_game_deadlock_fix",   InitPostGameDeadlockFix,    nullptr,                    ApplyOnlyOnClient);
    manager.RegisterFeature("flight_controls_fix",      InitFlightControlsFix,      nullptr,                    ApplyOnlyOnClient);
    manager.RegisterFeature("dynamic_solar_infocards",  InitDynamicSolarInfocards,  nullptr,                    ApplyOnlyOnClient);

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
    {
        DisableThreadLibraryCalls(hinstDLL);
        Init();
    }
    else if (fdwReason == DLL_PROCESS_DETACH)
    {
        Cleanup();
    }

    return TRUE;
}
