#include "save_crash.h"
#include "utils.h"
#include "Common.h"

bool Archetype::EqObj::get_undamaged_collision_group_list_Hook(std::list<CollisionGroupDesc>& colGroupList) const
{
    // Oh hell no.
    if (this == nullptr)
        return false;

    return this->get_undamaged_collision_group_list(colGroupList);
}

// Fixes a crash that occurs when Freelancer loads all the save files on startup.
// If one of the save files is malformed/modded, that may cause the Archetype::GetShip function to return a nullptr.
// Freelancer doesn't check the return value, so it potentially calls a class function on a nullptr.
// We implement the nullptr check here.
void InitSaveCrashFix()
{
    #define GET_UNDAMAGED_COL_GROUP_LIST_FILE_OFFSET_SERVER 0x6766E

    // E.g. console.dll enforces the server library to load without causing any issues, so should be fine
    DWORD serverHandle = GetUnloadedModuleHandle("server.dll");

    if (serverHandle)
    {
        Patch<WORD>(serverHandle + GET_UNDAMAGED_COL_GROUP_LIST_FILE_OFFSET_SERVER, 0xBF90);
        SetPointer(serverHandle + GET_UNDAMAGED_COL_GROUP_LIST_FILE_OFFSET_SERVER + 0x2, &Archetype::EqObj::get_undamaged_collision_group_list_Hook);
    }
}
