#include "utils.h"

void Patch(DWORD vOffset, LPVOID mem, UINT len)
{
    static DWORD _;

    VirtualProtect((PVOID) vOffset, len, PAGE_EXECUTE_READWRITE, &_);
    memcpy((PVOID) vOffset, mem, len);
}

void Nop(DWORD vOffset, UINT len)
{
    static DWORD _;

    VirtualProtect((PVOID) vOffset, len, PAGE_EXECUTE_READWRITE, &_);
    memset((PVOID) vOffset, 0x90, len);
}

void Hook(DWORD location, PVOID hookFunc, UINT instrLen, bool jmp)
{
    // Set the opcode for the call or jmp instruction
    static BYTE callOpcode = 0xE8, jmpOpcode = 0xE9;
    Patch(location, &(jmp ? jmpOpcode : callOpcode), sizeof(BYTE));

    // Set and calculate the relative offset for the hook function
    DWORD relOffset = (DWORD) hookFunc - location - 5;
    Patch((location + 1), &relOffset, sizeof(DWORD));

    // Nop out excess bytes
    if (instrLen > 5)
        Nop((location + 5), instrLen - 5);
}

void SetPointer(DWORD location, PVOID p)
{
    DWORD pRef = (DWORD) &p;
    Patch(location, (PVOID) pRef, sizeof(DWORD));
}

double getTimeElapsed(const clock_t &lastUpdate)
{
    return (double) (clock() - lastUpdate);
}
