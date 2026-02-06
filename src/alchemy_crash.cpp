#include "alchemy_crash.h"
#include "utils.h"
#include "logger.h"

#define FASTCALL __fastcall

// This rewrites the original loop present in alchemy.dll.
// In principle it would have been possible to just patch one asm instruction to fix the bug,
// but rewriting the loop is cooler.
const Alchemy* FASTCALL GetFinishedAle(int maxIndex, const Alchemy* aleArr, float maxProgress)
{
    int i = 0;

    for (; i < maxIndex - 1; ++i) // original loop condition: "i < maxIndex"
    {
        if (maxProgress < aleArr[i + 1].progress)
            break;
    }

    return &aleArr[i];
}

// There is code in alchemy.dll that determines how ALE effects should transition to a different effect.
// Many times per frame it loops over a set of ALEs and finds which element meets the condition.
// However, it assumes that there is at least one element for which this condition holds.
// If not, we get that at the end of the loop, i == maxIndex, causing later code to access an out-of-bounds array element and thus crash (offset 0x701b).
// This occurs under extremely rare circumstances; you can play the game for 1,000 hours straight and not notice anything,
// but one day you start the game and it crashes within 15 minutes. The reason why suddenly no ALE meets this condition is unclear;
// the fact that it's so inconsistent and rare makes it impossible to bisect.
// This hook code rewrites the loop such that it never loops beyond maxIndex - 1.
// If the original problem were to occur, then one or more ALE effects may become invisible, though at least it certainly fixes the crash.
void InitAlchemyCrashFix()
{
    #define GET_FINISHED_ALE_START_FILE_OFFSET_ALCHEMY 0x6FDD
    #define GET_FINISHED_ALE_END_FILE_OFFSET_ALCHEMY 0x6FF7
    DWORD alchemyHandle = (DWORD) GetModuleHandle("alchemy.dll");

    if (alchemyHandle)
    {
        // mov edx, esi followed by push [esp+0x10] (passes the needed parameters to our hook)
        BYTE startPatch[] = { 0x89, 0xF2, 0xFF, 0x74, 0x24, 0x10 };
        Patch(alchemyHandle + GET_FINISHED_ALE_START_FILE_OFFSET_ALCHEMY, startPatch, sizeof(startPatch));
        Hook(alchemyHandle + GET_FINISHED_ALE_START_FILE_OFFSET_ALCHEMY + sizeof(startPatch), GetFinishedAle, 20);

        // mov esi, eax (set the return value so that the rest of the alchemy code can use it)
        Patch<WORD>(alchemyHandle + GET_FINISHED_ALE_END_FILE_OFFSET_ALCHEMY, 0xC689);
    }
    else
    {
        Logger::PrintModuleError("InitAlchemyCrashFix", "alchemy.dll");
    }
}
