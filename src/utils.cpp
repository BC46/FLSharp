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

void Hook(DWORD location, DWORD hookFunc, UINT instrLen)
{
    // Set the opcode for the call instruction
    static BYTE callOpcode = 0xE8;
    Patch((PVOID) location, &callOpcode, sizeof(BYTE));

    // Set and calculate the relative offset for the hook function
    DWORD relOffset = hookFunc - location - 5;
    Patch((PVOID) (location + 1), &relOffset, sizeof(DWORD));

    // Nop out excess bytes
    if (instrLen > 0)
        Nop((PVOID) (location + 5), instrLen - 5);
}

bool hasTimeElapsed(const clock_t &lastUpdate, const double &intervalMs)
{
    return (double) (clock() - lastUpdate) >= intervalMs;
}

