#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define Import __declspec(dllimport)

struct SSPObjUpdateInfo
{
    BYTE data[40];
    float throttle;
};

class IServerImpl;

// SPObjUpdate function definition
typedef void (__fastcall SPObjUpdateCall)(IServerImpl* server, PVOID _edx, SSPObjUpdateInfo &updateInfo, UINT client);

class IServerImpl {
public:
    // Wrapper for the virtual SPObjUpdate function in remoteserver.dll
    // Very ugly I know, but I couldn't find an easier way to go about it...
    inline void SPObjUpdate(PVOID _edx, SSPObjUpdateInfo &updateInfo, UINT client)
    {
        // Get function pointer from the vftable
        SPObjUpdateCall* originalFunction = (SPObjUpdateCall*) vftable->SPObjUpdate;

        // Call the original SPObjUpdate function
        (originalFunction)(this, _edx, updateInfo, client);
    }

private:
    struct IServerImpl_VFTable
    {
        BYTE funcs[0xD0];
        PVOID SPObjUpdate;
    };

    IServerImpl_VFTable* vftable;
};
