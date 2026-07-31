#include "target_trade.h"
#include "utils.h"

void HUD_TargetH::RenderTargetBaseBackground_Hook()
{
    // Overwritten instruction.
    targetBaseBackground->Render();

    // The original code also tests the second bit of the 0x6C flags prior to the call,
    // but the Render function already takes care of that.
    if (targetTradeButton)
        targetTradeButton->Render();
}

// When you have a player selected such that the "Request to Trade" button is visible,
// and then open the Target Status window, the RTT button blinks briefly.
// Note, I'm not referring to the RTT button flickering which happens when opening the
// Target Status window while *no& player is selected; this can fixed with the following patch: Freelancer.exe, 0E19C6, 6A 00 -> EB 09.
// The blinking happens because during the opening animation, the main frame is made hidden.
// At this point, its children, including the RTT button, will not render.
// The game fixes this by rendering the RTT button manually during the animation.
// However, the condition they used to check whether RTT button should be rendered is not entirely correct.
// As a result, for a short period, the RTT button is rendered twice; once by the main render loop,
// and another time by a manual Render call. Hence the "blinking".
// Essentially, the original code looks like this:
//
// if ((condition1 || !condition2) && targetTradeButton && targetTradeButton->IsVisible())
//      targetTradeButton->Render();
// if (condition2)
//      hudTarget->Render();
// else
//      targetBaseBackground->Render();
//
// How about putting the targetTradeButton->Render() call inside the same condition body as targetBaseBackground->Render()?
// Have you thought of that, DA?
void InitTargetTradeBlinkFix()
{
    // Prevent FL from manually rendering the Target Trade button. We will render it in our code instead.
    #define TARGET_TRADE_BUTTON_VISIBLE_CHECK_ADDR 0x4E1BF1
    Patch<BYTE>(0x4E1BF1, 0xEB); // jmp

    #define RENDER_TARGET_BASE_BACKGROUND_ADDR 0x4E1C07
    PatchBytes(RENDER_TARGET_BASE_BACKGROUND_ADDR, { 0x89, 0xF1, 0x5E }); // mov ecx, esi and pop esi
    Hook(RENDER_TARGET_BASE_BACKGROUND_ADDR + 3, &HUD_TargetH::RenderTargetBaseBackground_Hook, 9, true);
}
