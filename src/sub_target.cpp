#include "sub_target.h"
#include "utils.h"

#define NAKED __declspec(naked)

#define HANDLE_SUBTARGET_HOTKEY_RET_ADDR_PREFIX 0x4E2B00

// Checks if the sub-target button is visible.
// If not, provide a return address which skips the function calls.
DWORD Target::HandleSubtargetHotkey(bool previous)
{
    static const BYTE returnSuffixes[] = { 0x63, 0x86, 0x98, 0xBB };

    FlUiElement* targetButton = previous ? hudTarget.targetPreviousButton : hudTarget.targetNextButton;
    bool visible = targetButton && targetButton->IsVisible();

    return HANDLE_SUBTARGET_HOTKEY_RET_ADDR_PREFIX + returnSuffixes[((int) previous) * 2 + ((int) !visible)];
}

NAKED void HandleSubtargetHotkey_Hook()
{
    __asm {
        lea esi, [edi-0x384]    // overwritten instruction
        push eax                // jump table index (0 for next, 1 for previous)
        mov ecx, esi            // Target*
        call Target::HandleSubtargetHotkey
        jmp eax
    }
}

// JFLP enables the previous and next Sub-Target hotkeys. These work fine when they are activated while the Target View is open.
// However, if the Contact List is open instead, it will display the target and sub-target names in the Contact List.
// This messes up the view because these names should only be displayed in the Target View.
// The issue is fixed by ensuring the previous/next Sub-Target hotkey will only work if its respective button is visible.
void InitSubTargetFix()
{
    #define CONTACT_LIST_HOTKEY_JMP_TABLE_ADDR 0x4E2EEC
    #define SUBTARGET_HOTKEY_COUNT 2

    for (int i = 0; i < SUBTARGET_HOTKEY_COUNT; ++i)
    {
        SetPointer(0x4E2EEC + i * sizeof(DWORD), HandleSubtargetHotkey_Hook);
    }
}
