#include "temp_fixes.h"
#include "Freelancer.h"
#include "Common.h"
#include "utils.h"

// These mostly keep track of what the current value is (state).
#define ROTATION_LOCK *((bool*) (0x678E40 + 0x44))
#define AUTO_LEVEL *((bool*) 0x612700)

// These represent the actual default values of the flight behavior.
#define DEFAULT_ROTATION_LOCK_CMN_OFFSET 0x7249A
#define DEFAULT_AUTO_LEVEL_CMN_OFFSET 0x86542

bool defaultRotationLockValue = true;
bool defaultAutoLevelValue = true;

namespace TempFixes
{
    void (*PostInitDealloc_Original)(PVOID obj);

    // Hook for dealloc function that gets called right after initializing the player's ship (undock or load game in space).
    // This is where we want to make sure rotation lock and auto level are set to their default value.
    void PostInitDealloc_Hook(PVOID obj)
    {
        // Call original function.
        PostInitDealloc_Original(obj);

        IBehaviorManager* behaviorManager = GetBehaviorManager(GetPlayerIObjRW());

        if (behaviorManager)
        {
            ROTATION_LOCK = behaviorManager->rotationLock;

            if (behaviorManager->physicsInfo)
                AUTO_LEVEL = behaviorManager->physicsInfo->autoLevel;
            else
                AUTO_LEVEL = defaultAutoLevelValue;
        }
        else
        {
            // If the behavior manager couldn't be retrieved, set rotation lock and auto level to their intended default value.
            ROTATION_LOCK = defaultRotationLockValue;
            AUTO_LEVEL = defaultAutoLevelValue;
        }
    }
}

// There is a bug in Freelancer where if you change the rotation lock or auto level from its default option, then load a game,
// the in-game behavior manager gets confused about whether or not these controls are turned on (the state differs from the underlying flight behavior value).
// To fix this, these controls must be set to their default value when the player's ship is initialized.
void InitFlightControlsFix()
{
    DWORD commonHandle = (DWORD) GetModuleHandle("common.dll");

    // Save the intended default values just in case.
    if (commonHandle)
    {
        defaultRotationLockValue = GetValue<bool>(commonHandle + DEFAULT_ROTATION_LOCK_CMN_OFFSET);
        defaultAutoLevelValue = GetValue<bool>(commonHandle + DEFAULT_AUTO_LEVEL_CMN_OFFSET);
    }

    TempFixes::PostInitDealloc_Original = SetRelPointer(POST_INIT_DEALLOC_CALL_ADDR + 1, TempFixes::PostInitDealloc_Hook);
}
