#include "group_members.h"
#include "utils.h"
#include "Common.h"
#include "Freelancer.h"
#include "logger.h"

#define FASTCALL __fastcall
#define NEUTRAL_REP (0.0f)

float hostileRepThreshold = -0.6f;

bool AreShipsInSameGroup(const CShip& s1, const CShip& s2)
{
    return s1.groupId && s1.groupId == s2.groupId;
}

int FASTCALL get_attitude_towards_Hook(const IObjRW& target, float& attitude, const IObjRW* player)
{
    int result = target.get_attitude_towards(attitude, player);

    // FL doesn't test the return value so why should I?
    // Check if the reported attitude is hostile and if the target is a player.
    if (attitude <= hostileRepThreshold && target.is_player() && player == GetPlayerIObjRW())
    {
        if (AreShipsInSameGroup((CShip&) *player->cobject, (CShip&) *target.cobject))
        {
            // Set the attitude to a value such that FL's return value check thinks the ship is non-hostile.
            attitude = NEUTRAL_REP;
            return S_OK;
        }
    }

    return result;
}

// In Freelancer, it's not possible to enter formation with group members that are hostile to you.
// This code fixes that by checking if the player's selected target is a group member.
void InitHostileGroupFormation()
{
    #define GROUP_FORMATION_REP_CHECK_COMMON_OFFSET 0x6C37C
    #define HOSTILE_REP_THRESHOLD_COMMON_OFFSET 0x13F540

    DWORD commonHandle = (DWORD) GetModuleHandle("common.dll");

    if (commonHandle)
    {
        Hook(commonHandle + GROUP_FORMATION_REP_CHECK_COMMON_OFFSET, get_attitude_towards_Hook, 6);
        hostileRepThreshold = *(float*) (commonHandle + HOSTILE_REP_THRESHOLD_COMMON_OFFSET);
    }
    else
    {
        Logger::PrintModuleError("InitHostileGroupFormation", "common.dll");
    }
}
