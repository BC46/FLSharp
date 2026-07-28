#include "sub_target.h"
#include "utils.h"

#define NAKED __declspec(naked)

#define HANDLE_SUBTARGET_HOTKEY_RET_ADDR_PREFIX 0x4E2B00

DWORD Target::HandleSubtargetHotkey(bool previous)
{
    static const BYTE returnSuffixes[] = { 0x65, 0x86, 0x9A, 0xBB };

    FlUiElement* targetButton = previous ? hudTarget.targetPreviousButton : hudTarget.targetNextButton;
    bool visible = targetButton && targetButton->IsVisible();

    return HANDLE_SUBTARGET_HOTKEY_RET_ADDR_PREFIX + returnSuffixes[((int) previous) * 2 + ((int) !visible)];
}

NAKED void HandleSubtargetHotkey_Hook()
{
    __asm {
        lea esi, [edi-0x384]    // overwritten instruction #1
        mov ecx, esi            // Target*
        call Target::HandleSubtargetHotkey
        mov ecx, esi            // overwritten instruction #2
        jmp eax
    }
}

void InitSubTargetFix()
{
    const DWORD subtargetHotkeyHandlers[] = { 0x4E2B5D, 0x4E2B92 };
    for (WORD i = 0; i < _countof(subtargetHotkeyHandlers); ++i)
    {
        Patch<WORD>(subtargetHotkeyHandlers[i], 0x006A + (i << 8));
        Hook(subtargetHotkeyHandlers[i] + 2, HandleSubtargetHotkey_Hook, 6, true);
    }
}
