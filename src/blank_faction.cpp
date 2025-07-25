#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "Common.h"
#include "utils.h"

#define FC_UK_GRP_IDS_NAME 197510
#define NONE_IDS 3022

UINT CShip::get_group_name_Hook() const
{
    UINT result = this->get_group_name();

    if (result == FC_UK_GRP_IDS_NAME)
        return NONE_IDS;

    return result;
}

// When you open the Current Information window on a factionless ship (fc_uk_grp),
// one of the lines will say "Faction:".
// This is because the fc_uk_grp faction has no name.
// This code replaces the ids_name of fc_uk_grp only in this particular instance with "None" to make it look nicer.
void InitBlankFactionNameFix()
{
    #define CURRENT_INFO_GET_GROUP_NAME_INFOCARD_CALL_ADDR 0x475950
    Hook(CURRENT_INFO_GET_GROUP_NAME_INFOCARD_CALL_ADDR, &CShip::get_group_name_Hook, 6);
}
