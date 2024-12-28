#include "utils.h"

void Patch(DWORD vOffset, LPVOID mem, UINT len)
{
    ReadWriteProtect(vOffset, len);
    memcpy((PVOID) vOffset, mem, len);
}

void Nop(DWORD vOffset, UINT len)
{
    ReadWriteProtect(vOffset, len);
    memset((PVOID) vOffset, 0x90, len);
}

double getTimeElapsed(const clock_t &lastUpdate)
{
    return (double) (clock() - lastUpdate);
}
