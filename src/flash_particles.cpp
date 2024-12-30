#include "flash_particles.h"
#include "Common.h"
#include "utils.h"

#define NAKED __declspec(naked)

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
        mov ecx, [esp+0x6C]
        mov ebx, [esp+0x70]
        mov esi, [esp+0x74]
        push [esp+0x78]
        mov eax, 0x52D1DC
        jmp eax
    }
}

void CELauncher::PlayFlashEffectForAllBarrels(LauncherInfo* launcherInfo, PVOID flashEffect)
{
    UINT barrelAmount = GetProjectilesPerFire();

    for (UINT i = 0; i < barrelAmount; ++i)
    {
        launcherInfo->somePtr = NULL;
        PlayFlashEffectForBarrel(launcherInfo, flashEffect, i);
    }
}

GunHandler* GunHandler::Constructor_Hook(CEGun* gun, PDWORD unk)
{
    #define OG_GUN_HANDLER_CONSTRUCTOR_ADDR 0x52D880

    // Initialize the vector.
    // TODO: determine type for vector
    this->objs = std::vector<int>(gun->GetProjectilesPerFire(), NULL);

    // Call the original constructor.
    Constructor constructorFunc = GetFuncDef<Constructor>(OG_GUN_HANDLER_CONSTRUCTOR_ADDR);
    return (this->*constructorFunc)(gun, unk);
}

void GunHandler::Destructor_Hook()
{
    #define OG_GUN_HANDLER_DESTRUCTOR_ADDR 0x52CAA0

    size_t objSize = objs.size();

    for (size_t i = 0; i < objSize; ++i)
    {
        if (!objs[i])
            continue;

        // TODO: clean objs
    }

    this->objs.~vector();

    // Call the original destructor.
    Destructor destructorFunc = GetFuncDef<Destructor>(OG_GUN_HANDLER_DESTRUCTOR_ADDR);
    (this->*destructorFunc)();
}

void InitFlashParticlesFix()
{
    #define GUN_HANDLER_OBJ_SIZE_ADDR 0x5333EB

    // Increase the size of the gun handler obj such that we can add a vector to it.
    ReadWriteProtect(GUN_HANDLER_OBJ_SIZE_ADDR, sizeof(char));
    *((PCHAR) GUN_HANDLER_OBJ_SIZE_ADDR) += (char) sizeof(std::vector<int>);

    Hook(0x52D1D7, PlayFlashEffect_Hook, 5, true);
    Hook(0x533408, &GunHandler::Constructor_Hook, 5);
    Hook(0x52D5B0, &GunHandler::Destructor_Hook, 5, true);
}
