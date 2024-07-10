#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <time.h>

void Patch(DWORD vOffset, LPVOID mem, UINT len);

void inline Patch_INT(DWORD vOffset, int value)
{
    Patch(vOffset, &value, sizeof(int));
}

void inline Patch_CHAR(DWORD vOffset, char value)
{
    Patch(vOffset, &value, sizeof(char));
}

void Nop(DWORD vOffset, UINT len);

void Hook(DWORD location, PVOID hookFunc, UINT instrLen, bool jmp = false);

double getTimeElapsed(const clock_t &lastUpdate);

void SetPointer(DWORD location, PVOID p);
