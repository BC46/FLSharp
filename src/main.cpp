#include "feature_config.h"
#include "config_reader.h"
#include "version_check.h"
#include "dacom.h"
#include "update.h"
#include "waypoint.h"
#include "waypoint_names.h"
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
#include "save_crash.h"
#include "alchemy_crash.h"
#include "blank_faction.h"
#include "server_filter.h"
#include "dll_crash.h"
#include "logger.h"

FeatureManager manager;

void CheckDllVersions()
{
    std::pair<LPCSTR, UINT32> dlls[] =
    {
        { "common.dll", 1223 },
        { "server.dll", 1223 },
    };

    for (const auto &dll : dlls)
    {
        if (GetDllProductBuildVersion(dll.first) <= dll.second)
        {
            Logger::PrintV10Warning(dll.first);
        }
    }
}

void Init()
{
    // All registered features must be able to work independently of each other.
    // They must not assume a certain load order or that another feature is active/inactive.
    manager.RegisterFeature("better_updates",           InitBetterUpdates,          nullptr,                    ApplyOnlyOnClient);
    manager.RegisterFeature("waypoint_fixes",           InitWaypointFixes,          nullptr,                    ApplyOnlyOnClient);
    manager.RegisterFeature("waypoint_name_fixes",      InitWaypointNameFixes,      nullptr,                    ApplyOnlyOnClient);
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
    manager.RegisterFeature("save_crash_fix",           InitSaveCrashFix,           nullptr,                    ApplyAlways);
    manager.RegisterFeature("alchemy_crash_fix",        InitAlchemyCrashFix,        nullptr,                    ApplyOnlyOnClient);
    manager.RegisterFeature("blank_faction_fix",        InitBlankFactionNameFix,    nullptr,                    ApplyOnlyOnClient);
    manager.RegisterFeature("server_filter_crash_fix",  InitServerFilterCrashFix,   nullptr,                    ApplyOnlyOnClient);
    manager.RegisterFeature("server_filter_speed_fix",  InitServerFilterSpeedFix,   nullptr,                    ApplyOnlyOnClient);
    manager.RegisterFeature("freelancer_dll_crash_fix", InitMissingDllCrashFix,     nullptr,                    ApplyAlways);

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
        CheckDllVersions();
        Init();
    }
    else if (fdwReason == DLL_PROCESS_DETACH)
    {
        Cleanup();
    }

    return TRUE;
}
