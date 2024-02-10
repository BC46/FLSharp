#include "utils.h"
#include "update.h"

void Init()
{
    // Hook various calls
    Hook(CHECK_FOR_SYNC_CALL_ADDR, (DWORD) CheckForSync_Hook, 5);
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
