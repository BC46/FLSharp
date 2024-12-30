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
    size_t barrelAmount = flashParticles.size();

    for (size_t i = 0; i < barrelAmount; ++i)
    {
        // TODO: cleanup previous instance
        if (this->flashParticles[i])
        {
            this->flashParticles[i] = NULL;
        }

        PlayFlashParticleForBarrel(idString, i);
        this->flashParticles[i] = this->currentFlashParticle;
    }
}

GunHandler* GunHandler::Constructor_Hook(CEGun* gun, PDWORD unk)
{
    #define OG_GUN_HANDLER_CONSTRUCTOR_ADDR 0x52D880

    // Initialize the vector.
    size_t barrelAmount = gun->GetProjectilesPerFire();
    this->flashParticles = std::vector<ParticleInstance*>(barrelAmount, NULL);

    // for (size_t i = 0; i < barrelAmount; ++i)
    //     this->flashParticles.push_back(NULL);

    // Call the original constructor.
    Constructor constructorFunc = GetFuncDef<Constructor>(OG_GUN_HANDLER_CONSTRUCTOR_ADDR);
    return (this->*constructorFunc)(gun, unk);
}

void GunHandler::Destructor_Hook()
{
    #define OG_GUN_HANDLER_DESTRUCTOR_ADDR 0x52CAA0

    size_t barrelAmount = flashParticles.size();

    for (size_t i = 0; i < barrelAmount; ++i)
    {
        if (flashParticles[i])
        {
            // TODO: clean objs
            flashParticles[i] = NULL;
        }
    }

    // Destruct vector.
    this->flashParticles.~vector();

    // Call the original destructor.
    Destructor destructorFunc = GetFuncDef<Destructor>(OG_GUN_HANDLER_DESTRUCTOR_ADDR);
    (this->*destructorFunc)();
}

void InitFlashParticlesFix()
{
    #define GUN_HANDLER_OBJ_SIZE_ADDR 0x5333EB

    // Increase the size of the gun handler obj such that we can add a vector to it.
    ReadWriteProtect(GUN_HANDLER_OBJ_SIZE_ADDR, sizeof(char));
    *((PCHAR) GUN_HANDLER_OBJ_SIZE_ADDR) += (char) 32; // sizeof(GunHandler::flashParticles)

    Hook(0x52D1B4, PlayFlashEffect_Hook, 5, true);
    Hook(0x533408, &GunHandler::Constructor_Hook, 5);
    Hook(0x52D5B0, &GunHandler::Destructor_Hook, 5, true);
}
