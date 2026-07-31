#pragma once

#include "Freelancer.h"

struct Target
{
    BYTE x00[0x38];
    HUD_Target hudTarget; // 0x38

    DWORD HandleSubtargetHotkey(bool previous);
    DWORD HandleFormationListHotkey(const CShip& playerShip);
};

void InitSubTargetFix();

void InitFormationListFix();
