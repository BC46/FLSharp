#include "ui_anim.h"
#include "utils.h"

// In many of the MP-related menus there is an animation for all the buttons where they slide out as you close the menu.
// I noticed that often the button texts slide out about nine times faster than their respective button background.
// Ideally I wanted to make it so that the slide out speeds match, but my attempts proved to be unsuccesful.
// Turns out that the buttons that do have matching slide speeds (e.g. in the main menu), use completely different code to achieve this.
// Now for all animations with the different slide-out speeds I just hide the text when the slide-out animation is active;
// by default this already happens in the slide-in animation.
// Now the animations feel a lot more seamless and smooth.
int UITextMsgButton::SlideAnimation_Hook(BYTE unk1, const float* newXPos, BYTE unk2)
{
    this->textInfo = NULL;
    this->disableHovering = true;

    return SlideAnimation(unk1, newXPos, unk2);
}

void InitSlideUiAnimFix()
{
    const DWORD slideAnimationCalls[] = {
        0x56FB13, 0x56FB2A, 0x56FB41, 0x56FB58,             // "FREELANCER SERVERS" menu
        0x56A86F, 0x56A885, 0x56A89B, 0x56A8B1, 0x56A8C7,   // "SELECT A CHARACTER" menu
        0x561A43, 0x561A5A                                  // "CREATE A NEW CHARACTER" menu
    };

    for (int i = 0; i < sizeof(slideAnimationCalls) / sizeof(DWORD); ++i)
        Hook(slideAnimationCalls[i], &UITextMsgButton::SlideAnimation_Hook, 6);
}
