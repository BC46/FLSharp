#include "utils.h"

void Patch(DWORD vOffset, const LPVOID mem, UINT len)
{
    ReadWriteProtect(vOffset, len);
    memcpy((PVOID) vOffset, mem, len);
}

void PatchBytes(DWORD vOffset, std::initializer_list<BYTE> bytes)
{
    Patch(vOffset, (LPVOID) bytes.begin(), bytes.size());
}

void Nop(DWORD vOffset, UINT len)
{
    // Recommended Multi-Byte Sequence of NOP Instruction from the x86 instruction set reference.
    // Not sure whether the commented entries are safe to use on older CPUs.
    static const NopStr nopStrTable[] =
    {
      //{ 15, "\x66\x66\x66\x66\x66\x66\x66\x0F\x1F\x84\x00\x00\x00\x00\x00" },
      //{ 14, "\x66\x66\x66\x66\x66\x66\x0F\x1F\x84\x00\x00\x00\x00\x00" },
      //{ 13, "\x66\x66\x66\x66\x66\x0F\x1F\x84\x00\x00\x00\x00\x00" },
      //{ 12, "\x66\x66\x66\x66\x0F\x1F\x84\x00\x00\x00\x00\x00" },
      //{ 11, "\x66\x66\x66\x0F\x1F\x84\x00\x00\x00\x00\x00" },
      //{ 10, "\x66\x66\x0F\x1F\x84\x00\x00\x00\x00\x00" },
        { 9,  "\x66\x0F\x1F\x84\x00\x00\x00\x00\x00" },
        { 8,  "\x0F\x1F\x84\x00\x00\x00\x00\x00" },
        { 7,  "\x0F\x1F\x80\x00\x00\x00\x00" },
        { 6,  "\x66\x0F\x1F\x44\x00\x00" },
        { 5,  "\x0F\x1F\x44\x00\x00" },
        { 4,  "\x0F\x1F\x40\x00" },
        { 3,  "\x0F\x1F\x00" },
        { 2,  "\x66\x90" },
        { 1,  "\x90" },
    };

    for (const auto &nopStr : nopStrTable)
    {
        while (len >= nopStr.len)
        {
            Patch(vOffset, (PBYTE) nopStr.nopSequence, nopStr.len);
            len -= nopStr.len;
            vOffset += nopStr.len;
        }
    }
}

DWORD GetUnloadedModuleHandle(LPCTSTR moduleName)
{
    DWORD handle = (DWORD) GetModuleHandle(moduleName);

    if (!handle)
        handle = (DWORD) LoadLibrary(moduleName);

    return handle;
}
