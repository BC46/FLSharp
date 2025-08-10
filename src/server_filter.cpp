#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "utils.h"
#include "Freelancer.h"

#define NAKED __declspec(naked)

#define DISABLE_SERVER_FILTER_HOVERING_ADDR 0x571592
#define DISABLE_SERVER_FILTER_HOVERING_SKIP_ADDR 0x571600

#define SERVER_FILTER_ON_FRAME_UPDATE_VFTABLE_ADDR 0x5E20FC

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

// Sometimes when you close the server filter dialog (MP list menu) while interacting with the GUI elements, the game crashes.
// This happens because FL wants to disable the hovering for the GUI elements in the server filter dialog while they no longer exist.
// The problem has been fixed by adding a simple null check.
void InitServerFilterCrashFix()
{
    Hook(DISABLE_SERVER_FILTER_HOVERING_ADDR, ServerFilterClose_Hook, 6, true);
}

bool (ServerFilterDialog::*OnFrameUpdate_Original)(const FLCursor &cursor);

bool ServerFilterDialog::OnFrameUpdate_Hook(const FLCursor &cursor)
{
    UpdateDeltaTime();
    return (this->*OnFrameUpdate_Original)(cursor);
}

// While the server filter window is opened (MP list menu), the delta time value is not updated for some reason.
// If you open the window while the game is stuttering, the delta value remains very high until the window is closed,
// causing the game speed to suddenly become extremely fast.
// This bug is fixed by hooking hte on-frame update function and updating the delta time manually.
void InitServerFilterSpeedFix()
{
    OnFrameUpdate_Original = SetPointer(SERVER_FILTER_ON_FRAME_UPDATE_VFTABLE_ADDR, &ServerFilterDialog::OnFrameUpdate_Hook);
}
