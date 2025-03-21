#pragma once

#include "Common.h"

struct CETradeLaneEquip
{
    DWORD vftable;
    CSolar* solar;
};

struct TradeLaneEquipObj
{
    DWORD vftable;
    CETradeLaneEquip* tradeLaneEquip;
    BYTE x08[0x28];
    BOOL isDisrupted;

    void SetLightsState_Hook();
};

void InitTradeLaneLightsFix();
