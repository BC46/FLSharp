#include "projectiles.h"
#include "utils.h"
#include "logger.h"
#include <algorithm>

#define NAKED __declspec(naked)

// We hook this function in one instance where it gets called
// because it uses the return value to play the one_shot_sound of the launchers.
// When the function returns 2 (i.e. the launcher has two barrels), the one_shot_sound fails to play.
// In this hook we make sure the return value is 1 at most, which fixes the bug.
// It is not recommended to modify the GetProjectilesPerFire function directly because it gets called in other instances as well.
UINT CELauncher::GetProjectilesPerFire_Hook() const
{
    return std::min<UINT>(this->GetProjectilesPerFire(), 1);
}

void InitProjectilesSoundFix()
{
    #define PROJECTILES_PER_FIRE_CALL_ADDR 0x534D0D

    Patch<WORD>(PROJECTILES_PER_FIRE_CALL_ADDR, 0xBB90);
    SetPointer(PROJECTILES_PER_FIRE_CALL_ADDR + 0x2, &CELauncher::GetProjectilesPerFire_Hook);
}

DWORD playerLauncherFireRet;

// Hook function that replaces the hard-coded "1" when decrementing ammo with a GetProjectilesPerFire call.
// This fixes a bug that makes the server decrement the wrong amount of ammo when a player fires a multi-barrel launcher.
// TODO: Can be rewritten to a non-asm hook.
NAKED void HandlePlayerLauncherFire_Hook()
{
    __asm {
        push 0x3F800000                     // overwritten instruction
        xchg ecx, edi                       // preserve ecx, while also setting the fired CELauncher as the thisptr
        mov esi, edx                        // preserve edx
        call dword ptr [CELauncher::GetProjectilesPerFire]
        mov ecx, edi                        // restore ecx
        mov edx, esi                        // restore edx
        push eax                            // push projectiles per fire
        jmp [playerLauncherFireRet]
    }
}

// This function may be executed on both the client and server-side
void InitProjectilesServerFix()
{
    // E.g. console.dll enforces the server library to load without causing any issues, so should be fine
    DWORD serverHandle = GetUnloadedModuleHandle("server.dll");

    if (serverHandle)
    {
        playerLauncherFireRet = serverHandle + 0xD91A;

        Hook(serverHandle + 0xD913, HandlePlayerLauncherFire_Hook, 5, true);
    }
    else
    {
        Logger::PrintModuleError("InitProjectilesServerFix", "server.dll");
    }
}
