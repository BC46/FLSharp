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

NAKED void PlayFlashEffect_Hook()
{
    __asm {
        mov ecx, dword ptr ds:[ebx+0x4] // overwritten instruction
        push esi
        push ebx
        call CELauncher::PlayFlashEffectForAllBarrels
        mov eax, 0x52D271
        jmp eax
    }
}

NAKED void CELauncher::PlayFlashEffectForBarrel(PVOID launcherInfo, PVOID flashEffect, UINT barrelIndex)
{
    __asm {
        sub esp, 0x58
        push ebx
        push ebp
        push esi
        push edi
        mov ebx, [esp+0x6C]
        mov esi, [esp+0x70]
        push [esp+0x74]
        mov eax, 0x52D1DC
        jmp eax
    }
}

NAKED void CELauncher::PlayFlashEffectForBarrel_Setup(PVOID launcherInfo, PVOID flashEffect, UINT barrelIndex)
{
    __asm {
        push [esp+0xC]
        push [esp+0xC]
        push [esp+0xC]
        call CELauncher::PlayFlashEffectForBarrel
        add esp, 0xC
        ret 0xC
    }
}

void CELauncher::PlayFlashEffectForAllBarrels(LauncherInfo* launcherInfo, PVOID flashEffect)
{
    UINT barrelAmount = GetProjectilesPerFire();

    for (UINT i = 0; i < barrelAmount; ++i)
    {
        launcherInfo->somePtr = NULL;
        PlayFlashEffectForBarrel_Setup(launcherInfo, flashEffect, i);
    }
}

void InitProjectilesSoundFix()
{
    SetPointer(PROJECTILES_PER_FIRE_CALL_ADDR, &getProjectilesPerFireHookFunc);

    Hook(0x52D1D7, PlayFlashEffect_Hook, 5, true);
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

    Hook(serverHandle + 0xD913, HandlePlayerLauncherFire_Hook, 5, true);
}
