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

template <typename Func>
void Hook(DWORD location, Func hookFunc, UINT instrLen, bool jmp = false)
{
    DWORD _;

    // Set the opcode for the call or jmp instruction
    static BYTE callOpcode = 0xE8, jmpOpcode = 0xE9;
    Patch(location, &(jmp ? jmpOpcode : callOpcode), sizeof(BYTE));

    // Set and calculate the relative offset for the hook function
    VirtualProtect((PVOID) location, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &_);
    *(Func*) (location + 1) = hookFunc;
    *(PDWORD) (location + 1) -= location + 5;

    // Nop out excess bytes
    if (instrLen > 5)
        Nop((location + 5), instrLen - 5);
}

double getTimeElapsed(const clock_t &lastUpdate);

void SetPointer(DWORD location, PVOID p);
