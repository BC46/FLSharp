#include "flash_particles.h"
#include "Common.h"
#include "utils.h"

#define NAKED __declspec(naked)

// This hook gets called when Freelancer wants to play a flash effect animation.
// We intercept this call to play the flash effect for every barrel.
NAKED void PlayFlashEffect_Hook()
{
    #define PLAY_FLASH_EFFECT_RET_ADDR 0x52D271

    __asm {
        mov ecx, ebx // LauncherHandler
        push esi // ID_String
        call LauncherHandler::PlayAllFlashParticles
        mov eax, PLAY_FLASH_EFFECT_RET_ADDR
        jmp eax
    }
}

// This function has some asm setup code which redirects us to FLs original code
// to allow the flash particle to play on a given barrel index.
// This is convenient because this way we are reusing FL's own code.
NAKED void LauncherHandler::PlayFlashParticleForBarrel(ID_String* idString, UINT barrelIndex)
{
    #define GET_BARREL_INFO_FOR_FLASH_PROJ_CALL_ADDR 0x52D1DC

    __asm {
        sub esp, 0x58
        push ebx
        push ebp
        push esi
        push edi
        mov ebx, [esp+0x6C] // LauncherHandler
        mov ecx, [ebx+0x4] // CELauncher
        mov esi, [esp+0x70] // ID_String
        push [esp+0x74] // barrel index
        mov eax, GET_BARREL_INFO_FOR_FLASH_PROJ_CALL_ADDR
        jmp eax
    }
}

// In this function we play the flash particle effect for every barrel, instead of only the first barrel.
// We do this by keeping track of a custom heap-allocated array of size n (n = amount of barrels of the launcher).
void LauncherHandler::PlayAllFlashParticles(ID_String* idString)
{
    UINT barrelAmount = this->launcher->GetProjectilesPerFire();

    // Create the flash particles array if it doesn't exist yet.
    if (!this->flashParticlesArr)
        this->flashParticlesArr = CreateFlashParticlesArray(barrelAmount);

    for (UINT i = 0; i < barrelAmount; ++i)
    {
        // Clean up the previous instance.
        if (this->flashParticlesArr[i])
        {
            this->flashParticlesArr[i]->Dealloc();
            this->flashParticlesArr[i] = NULL;
        }

        // The PlayFlashParticleForBarrel function stores the effect instance in the currentFlashParticle variable (provided creation was successful).
        // However, this offset also stores our custom array.
        // So temporarily keep a copy of the original array pointer, and after calling the function,
        // save the instance in the original array, and then restore the array at the original offset.
        EffectInstance** ogFlashParticlesArr = this->flashParticlesArr;
        PlayFlashParticleForBarrel(idString, i);
        ogFlashParticlesArr[i] = this->currentFlashParticle;
        this->flashParticlesArr = ogFlashParticlesArr;
    }
}

// Creates an array with a size equal to the barrel amount of the launcher.
EffectInstance** CreateFlashParticlesArray(UINT barrelAmount)
{
    EffectInstance** flashParticlesArr = new EffectInstance*[barrelAmount];
    ZeroMemory(flashParticlesArr, barrelAmount * sizeof(EffectInstance*));
    return flashParticlesArr;
}

// This hook gets called when the LauncherHandler's destructor is being executed.
// Here we want to deallocate all flash particles we have stored in the custom array.
void LauncherHandler::Destructor_Hook()
{
    #define OG_LAUNCHER_HANDLER_DESTRUCTOR_ADDR 0x52CAA0

    if (this->flashParticlesArr)
    {
        UINT barrelAmount = this->launcher->GetProjectilesPerFire();

        // Deallocate all active flash particle instances.
        for (UINT i = 0; i < barrelAmount; ++i)
        {
            if (flashParticlesArr[i])
                flashParticlesArr[i]->PostGameDealloc();
        }

        // Destruct the array.
        delete[] this->flashParticlesArr;
        this->flashParticlesArr = NULL; // Prevent the original destructor from deallocating LauncherHandler::currentFlashParticle.
    }

    // Call the original destructor.
    Destructor destructorFunc = GetFuncDef<Destructor>(OG_LAUNCHER_HANDLER_DESTRUCTOR_ADDR);
    (this->*destructorFunc)();
}

// In vanilla Freelancer, if you fire any launcher with a flash particle, the game explicitly plays the particle on barrel index 0 only.
// For most launchers this isn't an issue, but if you have a multi-barrel launcher, the flash effect will only play on the first barrel.
void InitFlashParticlesFix()
{
    #define PLAY_FLASH_EFFECT_ADDR 0x52D1B4
    #define LAUNCHER_HANDLER_DESTRUCTOR_CALL_ADDR 0x52D5B0

    Hook(PLAY_FLASH_EFFECT_ADDR, PlayFlashEffect_Hook, 5, true);
    Hook(LAUNCHER_HANDLER_DESTRUCTOR_CALL_ADDR, &LauncherHandler::Destructor_Hook, 5, true);
}
