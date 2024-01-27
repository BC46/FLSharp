#include "Common.h"
#include "RemoteServer.h"
#include "utils.h"

#include <time.h>

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

clock_t timeSinceLastUpdate;
const double syncIntervalMs = 500;

// Hook for function that determines whether an update should be sent to the server
bool __fastcall CheckForSync_Hook(CRemotePhysicsSimulation* physicsSim, PVOID _edx, Vector const &unk1, Vector const &unk2, Quaternion const &unk3)
{
    // Does the client want to sync?
    if (physicsSim->CheckForSync(unk1, unk2, unk3))
        return true;

    // If the client doesn't want to sync, we do our own checks to see if it should sync regardless

    // Ensure it syncs at least once every 500 ms
    if ((double) (clock() - timeSinceLastUpdate) >= syncIntervalMs)
        return true;

    return false;
}

// Hook for function that sends an update to the server
void __fastcall SPObjUpdate_Hook(IServerImpl* server, PVOID _edx, SSPObjUpdateInfo &updateInfo, UINT client)
{
    CShip* ship = GetCShip();

    // Get throttle from the ship and set it in the update info, provided the ship isn't NULL
    if (ship)
        updateInfo.fThrottle = ship->get_throttle();

    // Send update to the server
    server->SPObjUpdate(_edx, updateInfo, client);

    // Set time since last update
    timeSinceLastUpdate = clock();
}

void Init()
{
    // Hook CheckForSync function call
    Hook(CHECK_FOR_SYNC_CALL_ADDR, (DWORD) CheckForSync_Hook, 5);

    // Hook SPObjUpdate function call
    Hook(OBJ_UPDATE_CALL_ADDR, (DWORD) SPObjUpdate_Hook, 6);

    timeSinceLastUpdate = clock();
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    UNREFERENCED_PARAMETER(hinstDLL);
    UNREFERENCED_PARAMETER(lpReserved);

    if (fdwReason == DLL_PROCESS_ATTACH)
        Init();

    return TRUE;
}
