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

// SPObjUpdate function definition
typedef void (__fastcall SPObjUpdateCall)(IServerImpl* server, PVOID _edx, SSPObjUpdateInfo &updateInfo, UINT client);

class IServerImpl {
public:
    // Wrapper for the virtual SPObjUpdate function in remoteserver.dll
    void SPObjUpdate(PVOID _edx, SSPObjUpdateInfo &updateInfo, UINT client)
    {
        // Get function pointer from the vftable and the determined offset
        SPObjUpdateCall* originalFunction = (SPObjUpdateCall*) *((PDWORD)((char*)vftable + 0xD0));

        // Call the original SPObjUpdate function
        (originalFunction)(this, _edx, updateInfo, client);
    }

private:
    PVOID vftable;
    BYTE data[8];
};
