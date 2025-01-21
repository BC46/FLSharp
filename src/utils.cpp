#include "utils.h"

void Patch(DWORD vOffset, LPVOID mem, UINT len)
{
    ReadWriteProtect(vOffset, len);
    memcpy((PVOID) vOffset, mem, len);
}

void Nop(DWORD vOffset, UINT len)
{
    while (len > 0)
    {
        LPCSTR nopPatch;
        UINT nopSize = len > 9 ? 9 : len;

        // Recommended Multi-Byte Sequence of NOP Instruction from the x86 instruction set reference.
        if (nopSize == 1)
            nopPatch = "\x90";
        else if (nopSize == 2)
            nopPatch = "\x66\x90";
        else if (nopSize == 3)
            nopPatch = "\x0F\x1F\x00";
        else if (nopSize == 4)
            nopPatch = "\x0F\x1F\x40\x00";
        else if (nopSize == 5)
            nopPatch = "\x0F\x1F\x44\x00\x00";
        else if (nopSize == 6)
            nopPatch = "\x66\x0F\x1F\x44\x00\x00";
        else if (nopSize == 7)
            nopPatch = "\x0F\x1F\x80\x00\x00\x00\x00";
        else if (nopSize == 8)
            nopPatch = "\x0F\x1F\x84\x00\x00\x00\x00\x00";
        else if (nopSize == 9)
            nopPatch = "\x66\x0F\x1F\x84\x00\x00\x00\x00\x00";

        Patch(vOffset, (PBYTE) nopPatch, nopSize);
        len -= nopSize;
        vOffset += nopSize;
    }
}

double getTimeElapsed(const clock_t &lastUpdate)
{
    return (double) (clock() - lastUpdate);
}
