#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "vftable.h"

struct SSPObjUpdateInfo
{
    BYTE x00[40];
    float throttle; // 0x28
};

class IServerImpl {
public:
    FILL_VFTABLE(0)
    FILL_VFTABLE(1)
    FILL_VFTABLE(2)
    FILL_VFTABLE(3)
    FILL_VFTABLE(4)
    FILL_VFTABLE(5)
    FILL_VFTABLE(6)
    FILL_VFTABLE(7)
    FILL_VFTABLE(8)
    FILL_VFTABLE(9)
    FILL_VFTABLE(A)
    FILL_VFTABLE(B)
    FILL_VFTABLE(C)
    virtual void SPObjUpdate(SSPObjUpdateInfo &updateInfo, UINT client);

    void SPObjUpdate_Hook(SSPObjUpdateInfo &updateInfo, UINT client);
};
