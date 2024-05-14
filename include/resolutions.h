#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Freelancer.h"

NN_Preferences* __fastcall InitializeNN_Preferences_Hook(PVOID thisptr, PVOID _edx, DWORD unk1, DWORD unk2);

bool __fastcall InitializeElements_Hook(NN_Preferences* thisptr, PVOID _edx, DWORD unk1, DWORD unk2);
