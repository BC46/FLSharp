#include "temp_fixes.h"
#include "Freelancer.h"
#include "utils.h"
#include "fl_func.h"

// These mostly keep track of what the current value is (state).
#define ROTATION_LOCK *((bool*) (0x678E40 + 0x44))
#define AUTO_LEVEL *((bool*) 0x612700)

// These represent the actual underlying values of the flight behavior.
#define DEFAULT_ROTATION_LOCK_CMN_OFFSET (0x7249A)
#define DEFAULT_AUTO_LEVEL_CMN_OFFSET (0x86542)

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

        // Set rotation lock and auto level to their default value.
        ROTATION_LOCK = defaultRotationLockValue;
        AUTO_LEVEL = defaultAutoLevelValue;
    }
}

bool GetModuleBool(DWORD moduleHandle, DWORD offset)
{
    DWORD virtualAddr = moduleHandle + offset;
    ReadWriteProtect(virtualAddr, sizeof(bool));
    return *((bool*) virtualAddr);
}

// There is a bug in Freelancer where if you change the rotation lock or auto level from its default option, then load a game,
// the in-game behavior manager gets confused about whether or not these controls are turned on (the state differs from the underlying flight behavior value).
// To fix this, these controls must be set to their default value when the player's ship is initialized.
void InitFlightControlsFix()
{
    DWORD commonHandle = (DWORD) GetModuleHandleA("common.dll");

    if (commonHandle)
    {
        defaultRotationLockValue = GetModuleBool(commonHandle, DEFAULT_ROTATION_LOCK_CMN_OFFSET);
        defaultAutoLevelValue = GetModuleBool(commonHandle, DEFAULT_AUTO_LEVEL_CMN_OFFSET);
    }

    TempFixes::PostInitDealloc_Original = SetRelPointer(POST_INIT_DEALLOC_CALL_ADDR + 1, TempFixes::PostInitDealloc_Hook);
}
