#include "flash_particles.h"
#include "Common.h"
#include "utils.h"

#define NAKED __declspec(naked)

NAKED void PlayFlashEffect_Hook()
{
    __asm {
        mov ecx, ebx
        push esi // ID_String
        call GunHandler::PlayAllFlashParticles
        mov eax, 0x52D271
        jmp eax
    }
}

NAKED void GunHandler::PlayFlashParticleForBarrel(ID_String* idString, UINT barrelIndex)
{
    __asm {
        sub esp, 0x58
        push ebx
        push ebp
        push esi
        push edi
        mov ebx, [esp+0x6C] // GunHandler
        mov ecx, [ebx+0x4] // CEGun
        mov esi, [esp+0x70] // ID_String
        push [esp+0x74] // barrel index
        mov eax, 0x52D1DC
        jmp eax
    }
}

void GunHandler::PlayAllFlashParticles(ID_String* idString)
{
    if (!this->flashParticles)
        CreateFlashParticlesArray();

    size_t barrelAmount = this->gun->GetProjectilesPerFire();

    for (size_t i = 0; i < barrelAmount; ++i)
    {
        // Clean up previous instance.
        if (this->flashParticles[i])
        {
            this->flashParticles[i]->Dealloc();
            this->flashParticles[i] = NULL;
        }

        EffectInstance** ogFlashParticles = this->flashParticles;

        // Play a new flash particle and save its instance.
        PlayFlashParticleForBarrel(idString, i);
        ogFlashParticles[i] = this->currentFlashParticle;
        this->flashParticles = ogFlashParticles;
    }
}

void GunHandler::CreateFlashParticlesArray()
{
    // Initialize the array.
    UINT barrelAmount = this->gun->GetProjectilesPerFire();
    this->flashParticles = new EffectInstance*[barrelAmount];
    ZeroMemory(this->flashParticles, barrelAmount * sizeof(EffectInstance*));
}

void GunHandler::Destructor_Hook()
{
    #define OG_GUN_HANDLER_DESTRUCTOR_ADDR 0x52CAA0

    if (this->flashParticles)
    {
        size_t barrelAmount = this->gun->GetProjectilesPerFire();

        // Deallocate all flash particles.
        for (size_t i = 0; i < barrelAmount; ++i)
        {
            if (flashParticles[i])
                flashParticles[i]->PostGameDealloc();
        }

        // Destruct the array.
        delete[] this->flashParticles;
    }

    // Call the original destructor.
    Destructor destructorFunc = GetFuncDef<Destructor>(OG_GUN_HANDLER_DESTRUCTOR_ADDR);
    (this->*destructorFunc)();
}

void InitFlashParticlesFix()
{
    #define GUN_HANDLER_OBJ_SIZE_ADDR 0x5333EB
    #define FLASH_PARTICLES_DEALLOC_CHECK 0x52CAED

    // Increase the size of the gun handler obj such that we can add an array to it.
    ReadWriteProtect(GUN_HANDLER_OBJ_SIZE_ADDR, sizeof(char));
    *((PCHAR) GUN_HANDLER_OBJ_SIZE_ADDR) += sizeof(EffectInstance**);

    Hook(0x52D1B4, PlayFlashEffect_Hook, 5, true);
    Hook(0x52D5B0, &GunHandler::Destructor_Hook, 5, true);

    BYTE skipDeallocPatch[3] = { 0xE9, 0x81, 0x00 };
    Patch(FLASH_PARTICLES_DEALLOC_CHECK, skipDeallocPatch, sizeof(skipDeallocPatch));
    Patch_BYTE(FLASH_PARTICLES_DEALLOC_CHECK + 0x5, 0x90);
}
