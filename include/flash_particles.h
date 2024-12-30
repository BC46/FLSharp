#include "Common.h"
#include <vector>

void InitFlashParticlesFix();

struct GunEffect
{};

struct GunHandler
{
    BYTE x00[0x28];
    GunEffect* gunEffect; // 0x28
    BYTE x2C[0x18];
    std::vector<int> objs; // 0x44

    GunHandler* Constructor_Hook(CEGun* gun, PDWORD unk);
    void Destructor_Hook();

private:
    typedef GunHandler* (GunHandler::*Constructor)(CEGun* gun, PDWORD unk);
    typedef void (GunHandler::*Destructor)();
};
