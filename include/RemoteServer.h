#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define Import __declspec(dllimport)

struct SSPObjUpdateInfo
{
    BYTE data[40];
    float fThrottle;
    BYTE data2[5];
};

class IServerImpl;

typedef void __fastcall SPObjUpdateCall(IServerImpl* server, PVOID edx, SSPObjUpdateInfo &updateInfo, UINT client);

class IServerImpl {
public:
    void SPObjUpdate(IServerImpl* server, PVOID edx, SSPObjUpdateInfo &updateInfo, UINT client)
    {
        SPObjUpdateCall* originalFunction = (SPObjUpdateCall*) *((PDWORD)((char*)vftable + 0xD0));

        (originalFunction)(server, edx, updateInfo, client);
    }

private:
    PVOID vftable;
    BYTE data[8];
};
