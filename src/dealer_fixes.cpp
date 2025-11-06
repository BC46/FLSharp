#include "dealer_fixes.h"
#include "utils.h"
#include "fl_func.h"

FL_FUNC(bool DealerOpenCamera::StartAnimation(LPCSTR name, PVOID unk, NavBar* navBar, DWORD unk2), 0x44BA60)

bool DealerOpenCamera::StartAnimation_Hook(LPCSTR name, PVOID unk, NavBar* navBar, DWORD unk2)
{
    // Return true instead of false if the animation is already in progress. This fixes the bug.
    if (animationInProgress)
        return true;

    return StartAnimation(name, unk, navBar, unk2);
}

void __fastcall SetShipDealerMenuOpened_Hook(PVOID unkUiElement, NavBar& navBar)
{
    navBar.unkUiElement = unkUiElement; // overwritten instruction

    // Don't allow the ship dealer menu to be opened if the room transition hasn't finished yet.
    // Otherwise it'll crash the game.
    bool roomTransitionFinished = (navBar.maneuverFrame->flags & UI_ELEMENT_VISIBLE) == UI_ELEMENT_VISIBLE;
    navBar.shipDealerMenuOpened = roomTransitionFinished;
}

NAKED void GetRoomHotspot_Hook()
{
    __asm {
        mov ebp, eax            // overwritten instruction #1
        test esi, esi
        je null
        mov eax, [esi + 0x1C]   // overwritten instruction #2
        ret
    null:
        xor eax, eax
        ret
    }
}

void (NavBar::*SetHotspot_Original)(PVOID hotspot);

void NavBar::SetHotspot_Hook(PVOID hotspot)
{
    // Yeah, let's not do that.
    if (hotspot)
    {
        (this->*SetHotspot_Original)(hotspot);
    }
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

// There are some rare crashes that can occur when opening the dealer menus.
void InitDealerCrashFix()
{
    #define SET_SHIP_DEALER_MENU_OPENED_ADDR 0x441D28
    #define GET_ROOM_HOTSPOT_ADDR 0x43FFB6
    #define SET_HOTSPOT_CALL_ADDR 0x43E9CA

    // Fixes a crash when clicking on the ship dealer before the room transition has finished.
    Patch<DWORD>(SET_SHIP_DEALER_MENU_OPENED_ADDR, 0xC589DA89); // mov edx, ebx + mov ebp, eax
    Hook(SET_SHIP_DEALER_MENU_OPENED_ADDR + sizeof(DWORD), SetShipDealerMenuOpened_Hook, 5);
    Patch<DWORD>(SET_SHIP_DEALER_MENU_OPENED_ADDR + sizeof(DWORD) + 5, 0x9066E889); // mov eax, ebp + nop

    // Fixes a very rare crash that occurs when randomly clicking on various dealers at a base.
    Hook(GET_ROOM_HOTSPOT_ADDR, GetRoomHotspot_Hook, 5);
    SetHotspot_Original = SetRelPointer(SET_HOTSPOT_CALL_ADDR + 1, &NavBar::SetHotspot_Hook);
}
