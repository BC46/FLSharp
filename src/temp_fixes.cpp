#include "temp_fixes.h"
#include "Freelancer.h"
#include "utils.h"

#define ROTATION_LOCK *((bool*) (0x678E40 + 0x44))
#define AUTO_LEVEL *((bool*) 0x612700)

namespace TempFixes
{
    void (*PostInitDealloc_Original)(PVOID obj);

    // Hook for dealloc function that gets called right after initializing the player's ship (undock or load game in space).
    // This is where we want to make sure rotation lock and auto level are on by default.
    void PostInitDealloc_Hook(PVOID obj)
    {
        // Call original function.
        PostInitDealloc_Original(obj);

        // TODO: Is this volatile?
        // The user could've applied a hex edit to always have rotation lock and auto level off by default.
        // Wouldn't it be be better to set those default values instead of assuming true and then make the in-game behavior manager aware of this?
        ROTATION_LOCK = AUTO_LEVEL = true;
    }
}

void InitFlightControlsFix()
{
    TempFixes::PostInitDealloc_Original = SetRelPointer(POST_INIT_DEALLOC_CALL_ADDR + 1, TempFixes::PostInitDealloc_Hook);
}
