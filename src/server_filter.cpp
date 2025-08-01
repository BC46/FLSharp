#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "utils.h"

#define NAKED __declspec(naked)
#define DISABLE_SERVER_FILTER_HOVERING_ADDR 0x571592
#define DISABLE_SERVER_FILTER_HOVERING_SKIP_ADDR 0x571600

NAKED void ServerFilterClose_Hook()
{
    __asm {
        mov ecx, [esi+0xC4] // overwritten instruction
        test ecx, ecx
        mov eax, DISABLE_SERVER_FILTER_HOVERING_ADDR + 6
        mov edx, DISABLE_SERVER_FILTER_HOVERING_SKIP_ADDR
        cmove eax, edx
        jmp eax
    }
}

void InitServerFilterCrashFix()
{
    Hook(DISABLE_SERVER_FILTER_HOVERING_ADDR, ServerFilterClose_Hook, 6, true);
}
