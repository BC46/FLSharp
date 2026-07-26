#include "player_lights.h"
#include "Common.h"
#include "utils.h"

#define FASTCALL __fastcall

#define PLAYER_SIMPLE_ID (*(PUINT) (0x673378))

// Check if the light source should be enabled by using the correct check.
// We only want to enable it if it is mounted on the player's ship.
bool FASTCALL EnableLightSource(const IObjRW* iobjRW)
{
    if (iobjRW)
    {
        UINT simpleId = iobjRW->get_simple_id();
        return simpleId && simpleId == PLAYER_SIMPLE_ID;
    }

    return false;
}

// Freelancer has a check at file offset 131D2B in Freelancer.exe that checks if the light sources should be activated for the player.
// However, the function always returns false because the GetPlayerIObjRW function returns a nullptr at this point.
// Therefore, the light sources are never actually activated.
// Turns out there exist a player ship check that does work correctly in this case.
// The IOBjRW has a simple ID that can be compared to the player's simple ID. That is exactly what this code does.
void InitPlayerLightsFix()
{
    #define ENABLE_LIGHT_SOURCE_CHECK_ADDR (0x531D23)
    PatchBytes(ENABLE_LIGHT_SOURCE_CHECK_ADDR, { 0x8B, 0x4B, 0x08 }); // mov ecx, [ebx+0x8]
    Hook(ENABLE_LIGHT_SOURCE_CHECK_ADDR + 3, EnableLightSource, 8);
}
