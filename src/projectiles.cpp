#include "projectiles.h"
#include "utils.h"

// We hook this function in one instance where it gets called
// because it uses the return value to play the one_shot_sound of the launchers.
// When the function returns 2 (i.e. the launcher has two barrels), the one_shot_sound fails to play.
// In this hook we make sure the return value is 1 at most, which fixes the bug.
// It is not recommended to modify the GetProjectilesPerFire function directly because it gets called in other instances as well.
UINT __fastcall GetProjectilesPerFire_Hook(CELauncher *launcher)
{
    UINT result = launcher->GetProjectilesPerFire();

    if (result > 1)
        result = 1;

    return result;
}

void InitProjectilesPerFireFix()
{
    static PVOID projectilesPerFireHookPtr = GetProjectilesPerFire_Hook;
    SetPointer(PROJECTILES_PER_FIRE_CALL_ADDR, &projectilesPerFireHookPtr);
}
