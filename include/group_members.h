#define WIN32_LEAN_AND_MEAN
#include <windows.h>

enum AttitudeType : int
{
    Hostile = -1,
    Neutral = 0,
    Friendly = 1
};

void InitHostileGroupFormation();

void InitHostileGroupMembersFix();
