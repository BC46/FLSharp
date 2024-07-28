#include "utils.h"

void Patch(DWORD vOffset, LPVOID mem, UINT len)
{
    DWORD _;

    VirtualProtect((PVOID) vOffset, len, PAGE_EXECUTE_READWRITE, &_);
    memcpy((PVOID) vOffset, mem, len);
}

void Nop(DWORD vOffset, UINT len)
{
    DWORD _;

    VirtualProtect((PVOID) vOffset, len, PAGE_EXECUTE_READWRITE, &_);
    memset((PVOID) vOffset, 0x90, len);
}

double getTimeElapsed(const clock_t &lastUpdate)
{
    return (double) (clock() - lastUpdate);
}
