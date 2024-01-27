#include "Common.h"
#include "RemoteServer.h"
#include "utils.h"

#define CHECK_FOR_SYNC_CALL_ADDR 0x541602
#define OBJ_UPDATE_CALL_ADDR 0x54167C

#define Naked __declspec(naked)

const DWORD Get_IOBjRW_ADDR = 0x54BAF0;

// Thanks adoxa
Naked CShip* GetCShip()
{
    __asm {
        call    Get_IOBjRW_ADDR
        test    eax, eax
        jz      noship
        mov     eax, [eax+16]

    noship:
        ret
    }
}

// Determines whether an update should be sent to the server
bool __fastcall CheckForSync_Hook(CRemotePhysicsSimulation* physicsSim, PVOID _edx, Vector const &unk1, Vector const &unk2, Quaternion const &unk3)
{
    // Call the original function and sync if it returns true
    if (physicsSim->CheckForSync(unk1, unk2, unk3))
        return true;

    // If the original function doesn't want to sync, we do our own checks to see if it should sync regardless
    // TODO: do checks here

    return false;
}

// Function that sends an update to the server
void __fastcall SPObjUpdate_Hook(IServerImpl* server, PVOID _edx, SSPObjUpdateInfo &updateInfo, UINT client)
{
    CShip* ship = GetCShip();

    // Get throttle from the ship and set it in the update info, provided the ship isn't NULL
    if (ship)
        updateInfo.fThrottle = ship->get_throttle();

    // Call the original function
    server->SPObjUpdate(_edx, updateInfo, client);
}

void Init()
{
    // Hook CheckForSync function call
    Hook(CHECK_FOR_SYNC_CALL_ADDR, (DWORD) CheckForSync_Hook, 5);

    // Hook SPObjUpdate function call
    Hook(OBJ_UPDATE_CALL_ADDR, (DWORD) SPObjUpdate_Hook, 6);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    UNREFERENCED_PARAMETER(hinstDLL);
    UNREFERENCED_PARAMETER(lpReserved);

    if (fdwReason == DLL_PROCESS_ATTACH)
        Init();

    return TRUE;
}
