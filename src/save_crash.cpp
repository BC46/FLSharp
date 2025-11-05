#include "save_crash.h"
#include "utils.h"
#include "Common.h"
#include "logger.h"

#define IDS_UNKNOWN 0

bool Archetype::EqObj::get_undamaged_collision_group_list_Hook(std::list<CollisionGroupDesc>& colGroupList) const
{
    // Oh hell no.
    if (this == nullptr)
        return false;

    return this->get_undamaged_collision_group_list(colGroupList);
}

UINT GetShipIdsName_Hook(UINT shipId)
{
    Archetype::Ship* shipArch = Archetype::GetShip(shipId);
    return shipArch ? shipArch->idsName : IDS_UNKNOWN;
}

// Fixes a crash that occurs when Freelancer loads all the save files on startup.
// If one of the save files is malformed/modded, that may cause the Archetype::GetShip function to return a nullptr.
// Freelancer doesn't check the return value, so it potentially calls a class function on a nullptr.
// We implement the nullptr check here.
// Additionally, there is a crash that occurs when selecting a malformed/modded save file in the F1 Load Game Menu,
// (not the one you can access via the the main menu). We fix that here too.
void InitSaveCrashFix()
{
    #define GET_UNDAMAGED_COL_GROUP_LIST_FILE_OFFSET_SERVER 0x6766E
    #define GET_SHIP_IDS_NAME_CALL_ADDR 0x487EBF

    // E.g. console.dll enforces the server library to load without causing any issues, so should be fine
    DWORD serverHandle = GetUnloadedModuleHandle("server.dll");

    if (serverHandle)
    {
        Patch<WORD>(serverHandle + GET_UNDAMAGED_COL_GROUP_LIST_FILE_OFFSET_SERVER, 0xBF90);
        SetPointer(serverHandle + GET_UNDAMAGED_COL_GROUP_LIST_FILE_OFFSET_SERVER + 0x2, &Archetype::EqObj::get_undamaged_collision_group_list_Hook);
    }
    else
    {
        Logger::PrintModuleError("InitSaveCrashFix", "server.dll");
    }

    Hook(GET_SHIP_IDS_NAME_CALL_ADDR, GetShipIdsName_Hook, 9);
}
