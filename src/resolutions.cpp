#include "resolutions.h"

NN_Preferences* __fastcall InitializeNN_Preferences_Hook(PVOID thisptr, PVOID _edx, DWORD unk1, DWORD unk2)
{
    // TODO: get all resolutions
    // TODO: patch necessary values like 0x004B296A, 0x004B249A, 0x004B24B1, 0x004B24B7, 0x004B1002, 0x004B0FE9, 0x004B251F, 0x004B1084, 0x004B17FC

    // TODO: call original function and return
    return ((InitializeNN_Preferences*) INITIALIZE_NN_PREFERENCES_ADDR)(thisptr, _edx, unk1, unk2);
}

bool __fastcall InitializeElements_Hook(NN_Preferences* thisptr, PVOID _edx, DWORD unk1, DWORD unk2)
{
    // TODO: set the resolutions
    // call original function and return
    return ((InitializeElements*) INITIALIZE_NN_ELEMENTS_ADDR)(thisptr, _edx, unk1, unk2);
}
