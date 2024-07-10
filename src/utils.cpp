#include "utils.h"

void Patch(LPVOID vOffset, LPVOID mem, UINT len)
{
    static DWORD _;

    VirtualProtect(vOffset, len, PAGE_EXECUTE_READWRITE, &_);
    memcpy(vOffset, mem, len);
}

void Nop(LPVOID vOffset, UINT len)
{
    static DWORD _;

    VirtualProtect(vOffset, len, PAGE_EXECUTE_READWRITE, &_);
    memset(vOffset, 0x90, len);
}

void Hook(DWORD location, DWORD hookFunc, UINT instrLen, bool jmp)
{
    // Set the opcode for the call or jmp instruction
    static BYTE callOpcode = 0xE8, jmpOpcode = 0xE9;
    Patch((PVOID) location, &(jmp ? jmpOpcode : callOpcode), sizeof(BYTE));

    // Set and calculate the relative offset for the hook function
    DWORD relOffset = hookFunc - location - 5;
    Patch((PVOID) (location + 1), &relOffset, sizeof(DWORD));

    // Nop out excess bytes
    if (instrLen > 5)
        Nop((PVOID) (location + 5), instrLen - 5);
}

void SetPointer(DWORD location, PVOID p)
{
    DWORD pRef = (DWORD) &p;
    Patch((PVOID) location, (PVOID) pRef, sizeof(DWORD));
}

double getTimeElapsed(const clock_t &lastUpdate)
{
    return (double) (clock() - lastUpdate);
}
