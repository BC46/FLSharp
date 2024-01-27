#include "Common.h"
#include "RemoteServer.h"

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

void Patch(LPVOID vOffset, LPVOID mem, UINT len)
{
    static DWORD _;

    VirtualProtect(vOffset, len, PAGE_EXECUTE_READWRITE, &_);
    memcpy(vOffset, mem, len);
}

void Nop(LPVOID vOffset, UINT len)
{
    if (len == 0)
        return;

    static DWORD _;

    VirtualProtect(vOffset, len, PAGE_EXECUTE_READWRITE, &_);
    memset(vOffset, 0x90, len);
}

void Hook(DWORD location, DWORD hookFunc, UINT instrLen)
{
    // Set the opcode for the call instruction
    static BYTE callOpcode = 0xE8;
    Patch((PVOID) location, &callOpcode, sizeof(BYTE));

    // Set and calculate the relative offset for the hook function
    DWORD relOffset = hookFunc - location - 5;
    Patch((PVOID) (location + 1), &relOffset, sizeof(DWORD));

    // Nop out excess bytes
    Nop((PVOID) (location + 5), instrLen - 5);
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
