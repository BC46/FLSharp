#include "copy_paste.h"
#include "utils.h"

NAKED void HandleDefaultInputKey_Hook()
{
    #define HANDLE_DEFAULT_INPUT_KEY_OG 0x57CDDA

    __asm {
        mov ecx, esi
        push edi
        call InputBoxWindow::PasteFromClipboard
        mov byte ptr [esp+0x13], 0
        mov eax, HANDLE_DEFAULT_INPUT_KEY_OG
        jmp eax
    }
}

void InputBoxWindow::PasteFromClipboard(KeyMapInfo *kmi)
{
    // Maybe unnecessary?
    // if (this->ime == NULL)
    // {
    //     return;
    // }

    if (kmi->IsCtrlPressed())
    {
        if (toupper(kmi->enteredKey) == L'V')
        {
            if (!OpenClipboard(NULL))
                return;

            HANDLE clipboard = GetClipboardData(CF_UNICODETEXT);

            if (!clipboard)
                return;

            LPCWSTR clipboardStr = static_cast<LPCWSTR>(GlobalLock(clipboard));
            size_t clipboardLen = wcslen(clipboardStr);

            if (!clipboardStr)
                return;

            KeyMapInfo temp;

            for (size_t i = 0; i < clipboardLen && chars.size() < (size_t) maxCharsLength; ++i)
            {
                temp.enteredKey = clipboardStr[i];
                this->AddTypedKey(&temp);
            }

            GlobalUnlock(clipboard);
            CloseClipboard();
        }
        else if (toupper(kmi->enteredKey) == 'C')
        {
            if (!OpenClipboard(NULL))
                return;

            EmptyClipboard();

            size_t inputLength = this->chars.size();
            HGLOBAL clipboardData = GlobalAlloc(GMEM_DDESHARE, sizeof(wchar_t) * (inputLength + 1));
            LPWSTR clipboardStr = static_cast<LPWSTR>(GlobalLock(clipboardData));

            for (size_t i = 0; i < inputLength; ++i)
                clipboardStr[i] = this->chars[i].c;

            clipboardStr[inputLength] = L'\0';

            GlobalUnlock(clipboardData);
            SetClipboardData(CF_UNICODETEXT, clipboardData);
            CloseClipboard();
        }
    }
}

float nine = 9.0f;
float testing = 0.2f;

NAKED void ButtonAnimFix()
{
    __asm {
        // x
        fstp st(0)
        fld dword ptr [edi]
        fmul dword ptr [nine]
        fstp dword ptr [esp+0x8]

        // y
        fld dword ptr [edi+0x4]
        fmul dword ptr [nine]
        fstp dword ptr [esp+0xC]

        // z
        fld dword ptr [edi+0x8]
        fmul dword ptr [nine]
        mov edx, 0x5A08DE
        jmp edx
    }
}

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

// Allows for the Ctrl + V key combination to paste the current clipboard into the input box.
void InitCopyPasteFeature()
{
    #define HANDLE_DEFAULT_INPUT_KEY_ADDR 0x57CE3C

    SetPointer(HANDLE_DEFAULT_INPUT_KEY_ADDR, &HandleDefaultInputKey_Hook);

    int i;
    const DWORD slideAnimationCalls[] = {
        0x56FB13, 0x56FB2A, 0x56FB41, 0x56FB58,             // "FREELANCER SERVERS" menu
        0x56A86F, 0x56A885, 0x56A89B, 0x56A8B1, 0x56A8C7,   // "SELECT A CHARACTER" menu
        0x561A43, 0x561A5A                                  // "CREATE A NEW CHARACTER" menu
    };

    for (i = 0; i < sizeof(slideAnimationCalls) / sizeof(DWORD); ++i)
        Hook(slideAnimationCalls[i], &UITextMsgButton::SlideAnimation_Hook, 6);
}
