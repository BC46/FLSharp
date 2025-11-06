#include "dealer_fixes.h"
#include "utils.h"
#include "fl_func.h"

FL_FUNC(bool DealerOpenCamera::StartAnimation(LPCSTR name, PVOID unk, PVOID navBar, DWORD unk2), 0x44BA60)

bool DealerOpenCamera::StartAnimation_Hook(LPCSTR name, PVOID unk, PVOID navBar, DWORD unk2)
{
    // Return true instead of false if the animation is already in progress. This fixes the bug.
    if (animationInProgress)
        return true;

    return StartAnimation(name, unk, navBar, unk2);
}

// In Freelancer there is an infamous bug where if you click the equipment or commodity dealer twice very quickly, the camera goes up but the dealer menu never appears.
// Once the bug has been triggered the dealer menus will continue to not show up until you undock and redock, or reload your save file.
void InitDealerOpenFix()
{
    #define INIT_CAMERA_TRANSITION_EQUIPMENT_DEALER_ADDR 0x4417E7
    #define INIT_CAMERA_TRANSITION_COMMODITY_DEALER_ADDR 0x441862

    DWORD initCameraCalls[] = { INIT_CAMERA_TRANSITION_EQUIPMENT_DEALER_ADDR, INIT_CAMERA_TRANSITION_COMMODITY_DEALER_ADDR };
    for (const auto &call : initCameraCalls)
        Hook(call, &DealerOpenCamera::StartAnimation_Hook, 5);
};
