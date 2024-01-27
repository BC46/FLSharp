#include "Common.h"
#include "RemoteServer.h"
#include "utils.h"

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
