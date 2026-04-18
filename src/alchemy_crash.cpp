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
// Edit 18/04/26: It seems that even if the crash is fixed, there are other occurrences where it can happen.
// For instance, 0x778D has a loop which looks like it was directly copy pasted from 0x6FDD.
// Hence, I've looked carefully at the assembly for more similar loops and found two more (but I don't know if they ever get called).
// All instances now have the same fix applied. Hopefully, this fixes all variations of this particular crash.
void InitAlchemyCrashFix()
{
    #define GET_FINISHED_ALE_START_TO_END 0x1A
    DWORD alchemyHandle = (DWORD) GetModuleHandle("alchemy.dll");

    if (!alchemyHandle)
    {
        Logger::PrintModuleError("InitAlchemyCrashFix", "alchemy.dll");
        return;
    }

    static const AleLoop aleLoops[] = {
        { 0x6FDD, 0x10 },
        { 0x778D, 0x10 },
        // { 0x7F4C, 0x3C },
        // { 0x4136D, 0x10 }
        // I noticed these have the exact same kind of loop as the above two.
        // AFAICT however, these are never actually called, unlike the above two which are called every frame.
        // Hence I can't properly test if this hook even works for the latter two instances.
    };

    for (const auto& aleLoop : aleLoops)
    {
        // mov edx, esi followed by push [esp+maxProgressOffset] (passes the needed parameters to our hook)
        PatchBytes(alchemyHandle + aleLoop.startOffset, { 0x89, 0xF2, 0xFF, 0x74, 0x24 });
        Patch<BYTE>(alchemyHandle + aleLoop.startOffset + 5, aleLoop.maxProgressOffset);
        Hook(alchemyHandle + aleLoop.startOffset + 6, GetFinishedAle, 20);

        // mov esi, eax (set the return value so that the rest of the alchemy code can use it)
        Patch<WORD>(alchemyHandle + aleLoop.startOffset + GET_FINISHED_ALE_START_TO_END, 0xC689);
    }
}
