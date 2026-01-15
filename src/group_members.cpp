#include "group_members.h"
#include "utils.h"
#include "Common.h"
#include "Freelancer.h"
#include "logger.h"
#include "fl_func.h"

#define FASTCALL __fastcall
#define NEUTRAL_REP (0.0f)

FL_FUNC(AttitudeType GetAttitudeType(const IObjRW* towards, const IObjRW* from), 0x45A490)

float hostileRepThreshold = -0.6f;

// Assumes both the CObjects of IObjRWs are CShips.
bool AreIObjRWsInSameGroup(const IObjRW& o1, const IObjRW& o2)
{
    auto& ship1 = (const CShip&) *o1.cobject;
    auto& ship2 = (const CShip&) *o2.cobject;

    return ship1.groupId && ship1.groupId == ship2.groupId;
}

int FASTCALL get_attitude_towards_Hook(const IObjRW& target, float& attitude, const IObjRW* player)
{
    int result = target.get_attitude_towards(attitude, player);

    // FL doesn't test the return value so why should I?
    // Check if the reported attitude is hostile and if the target is a player.
    // As a sanity check I'm also checking if the "player" actually is the player.
    if (attitude <= hostileRepThreshold && target.is_player()
        && player && player == GetPlayerIObjRW() && player->cobject)
    {
        if (AreIObjRWsInSameGroup(*player, target))
        {
            // Set the attitude to a value such that FL's return value check thinks the ship is non-hostile.
            attitude = NEUTRAL_REP;
            return S_OK;
        }
    }

    return result;
}

AttitudeType GetAttitudeType_Hook(const IObjRW* towards, const IObjRW* from)
{
    // Call the original function.
    AttitudeType result = GetAttitudeType(towards, from);

    if (result != AttitudeType::Hostile)
        return result;

    // If GetAttitudeType returned Attitude::Hostile, that implies towards and from are both non-zero.
    // Check if the towards object is the player and if "from" is another player.
    if (from->is_player() && towards == GetPlayerIObjRW() && towards->cobject)
    {
        // If they're in the same group, treat them as neutral rather than hostile.
        if (AreIObjRWsInSameGroup(*towards, *from))
        {
            return AttitudeType::Neutral;
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

// Ensures hostile group members are no longer treated as hostile.
// For example if you are near a hostile group member, then you will hear the danger/battle music.
// For such group members there is also an attack marker displayed.
// These things can be quite distracting. The code below ensures they are treated as neutral instead.
void InitHostileGroupMembersFix()
{
    // Doing a trampoline hook was inconvenient here, so just manually hook all the call locations,
    // except for 0x475770 which should be handled by the TODO below.
    const DWORD getAttitudeTypeCalls[] = {
        0x48AEAB, 0x4E4950, 0x4EC10E, 0x4EC71A, 0x4EC891, 0x4F1CFF, 0x4F22E4,
        0x4F2465, 0x53A98C, 0x553290, 0x5532AD, 0x553325, 0x5552A8 };

    for (const auto &call : getAttitudeTypeCalls)
        SetRelPointer(call + 1, GetAttitudeType_Hook);

    // TODO: Check 0x475770 for printing "Group member" into current information window. Both values passed to the func are non-zero.
    // Hook 004757FA, check if the passed address is neutral. Then check if players are group members. If so, print IDS 1551 instead.
    // The problem is that there exists no "Group member" string in the English vanilla FL resource DLLs. There is only an uppercase version.
    // It can be converted to lowercase using towlower but this may not work on localizations that use non-Latin characters.
}
