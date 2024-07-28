#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define Import __declspec(dllimport)

struct SSPObjUpdateInfo
{
    BYTE data[40];
    float throttle;
};

class IServerImpl {
public:
    // Wrapper for the virtual SPObjUpdate function in remoteserver.dll
    // This is the alternative way to adding a bunch of dummy virtual functions to mimic a filled vftable
    inline void SPObjUpdate(SSPObjUpdateInfo &updateInfo, UINT client)
    {
        // Call the original SPObjUpdate function
        (this->*(vftable->SPObjUpdate))(updateInfo, client);
    }

    void SPObjUpdate_Hook(SSPObjUpdateInfo &updateInfo, UINT client);

private:
    typedef void (IServerImpl::*SPObjUpdateFunc)(SSPObjUpdateInfo &updateInfo, UINT client);

    struct IServerImpl_VFTable
    {
        BYTE funcs[0xD0];
        SPObjUpdateFunc SPObjUpdate;
    };

    IServerImpl_VFTable* vftable;
};
