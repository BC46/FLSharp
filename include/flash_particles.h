#include "Common.h"
#include <vector>

void InitFlashParticlesFix();

struct ParticleParams
{};

struct ParticleInstance
{
    DWORD data;
};

struct GunHandler
{
    DWORD vftable;
    CEGun* gun; // 0x04
    BYTE x08[0x20];
    ParticleInstance* currentFlashParticle; // 0x28
    BYTE x2C[0x18];
    std::vector<ParticleInstance*> flashParticles; // 0x44 (custom data: stores the flash particle for every barrel of the gun)

    void PlayAllFlashParticles(ID_String* idString);
    void __cdecl PlayFlashParticleForBarrel(ID_String* idString, UINT barrelIndex);

    GunHandler* Constructor_Hook(CEGun* gun, PDWORD unk);
    void Destructor_Hook();

private:
    typedef GunHandler* (GunHandler::*Constructor)(CEGun* gun, PDWORD unk);
    typedef void (GunHandler::*Destructor)();
};
