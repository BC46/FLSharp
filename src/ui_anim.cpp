#include "ui_anim.h"
#include "utils.h"

// In many of the MP-related menus there is an animation for all the buttons where they slide out as you close the menu.
// I noticed that often the button texts slide out about nine times faster than their respective button background.
// Ideally I wanted to make it so that the slide out speeds match, but my attempts proved to be unsuccessful.
// Turns out that the buttons that do have matching slide speeds (e.g. in the main menu), use completely different code to achieve this.
// Now for all animations with the different slide-out speeds I just hide the text when the slide-out animation is active;
// by default this already happens in the slide-in animation.
// Now the animations feel a lot more seamless and smooth.
int UITextMsgButton::UpdatePosition_Hook(BYTE unk1, const Vector* newPosOffset, BYTE unk2)
{
    if (this->textImage)
    {
        // If the textImage is nulled prematurely, then FL will no longer destroy it when it's not needed anymore.
        // Hence it's destroyed here.
        this->textImage->Destroy();
        this->textImage = nullptr;
    }

    this->disableHovering = true;

    return UpdatePosition(unk1, newPosOffset, unk2);
}

void InitSlideUiAnimFix()
{
    const DWORD slideAnimationCalls[] = {
        0x56FB13, 0x56FB2A, 0x56FB41, 0x56FB58,             // "FREELANCER SERVERS" menu
        0x56A86F, 0x56A885, 0x56A89B, 0x56A8B1, 0x56A8C7,   // "SELECT A CHARACTER" menu
        0x561A43, 0x561A5A,                                 // "CREATE A NEW CHARACTER" menu
        0x572F22, 0x572F39                                  // "Account ID" menu
    };

    for (const auto& call : slideAnimationCalls)
        Hook(call, &UITextMsgButton::UpdatePosition_Hook, 6);
}
