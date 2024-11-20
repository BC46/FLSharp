#include "projectiles.h"
#include "utils.h"

#define NAKED __declspec(naked)

// We hook this function in one instance where it gets called
// because it uses the return value to play the one_shot_sound of the launchers.
// When the function returns 2 (i.e. the launcher has two barrels), the one_shot_sound fails to play.
// In this hook we make sure the return value is 1 at most, which fixes the bug.
// It is not recommended to modify the GetProjectilesPerFire function directly because it gets called in other instances as well.
UINT CELauncher::GetProjectilesPerFire_Hook() const
{
    UINT result = this->GetProjectilesPerFire();

    if (result > 1)
        result = 1;

    return result;
}

typedef UINT (CELauncher::*GetProjectilesPerFireFunc)() const;
GetProjectilesPerFireFunc getProjectilesPerFireFunc = &CELauncher::GetProjectilesPerFire;
GetProjectilesPerFireFunc getProjectilesPerFireHookFunc = &CELauncher::GetProjectilesPerFire_Hook;

void InitProjectilesSoundFix()
{
    SetPointer(PROJECTILES_PER_FIRE_CALL_ADDR, &getProjectilesPerFireHookFunc);
}

DWORD playerLauncherFireRet;

// Hook function that replaces the hard-coded "1" when decrementing ammo with a GetProjectilesPerFire call
NAKED void HandlePlayerLauncherFire_Hook()
{
    __asm {
        push 0x3F800000                     // overwritten instruction
        xchg ecx, edi                       // preserve ecx, while also setting the fired CELauncher as the thisptr
        mov esi, edx                        // preserve edx
        call [getProjectilesPerFireFunc]
        mov ecx, edi                        // restore ecx
        mov edx, esi                        // restore edx
        push eax                            // push projectiles per fire
        jmp [playerLauncherFireRet]
    }
}

// This function may be executed on both the client and server-side
void InitProjectilesServerFix()
{
    DWORD serverHandle = (DWORD) GetModuleHandleA("server.dll");

    // e.g. console.dll enforces the server library to load without causing any issues, so should be fine
    if (!serverHandle)
        serverHandle = (DWORD) LoadLibraryA("server.dll");

    // Add file offset and server handle to set the hook's return address
    playerLauncherFireRet = serverHandle + 0xD91A;

    Hook(serverHandle + 0xD913, &HandlePlayerLauncherFire_Hook, 5, true);
}
