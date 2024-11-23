#include "copy_paste.h"
#include "utils.h"

NAKED void HandleDefaultInputKey_Hook()
{
    #define HANDLE_DEFAULT_INPUT_KEY_OG 0x57CDDA

    __asm {
        mov ecx, esi
        push edi
        call InputBoxWindow::HandleCopyPaste
        mov byte ptr [esp+0x13], 0
        mov eax, HANDLE_DEFAULT_INPUT_KEY_OG
        jmp eax
    }
}

void InputBoxWindow::CopyFromClipboard()
{
    if (!OpenClipboard(NULL))
        return;

    HANDLE clipboard = GetClipboardData(CF_UNICODETEXT);

    if (!clipboard)
        goto _closeClipboard;
    
    LPCWSTR clipboardStr = static_cast<LPCWSTR>(GlobalLock(clipboard));

    if (!clipboardStr)
        goto _closeClipboard;

    WriteString(clipboardStr);
    GlobalUnlock(clipboard);
    
_closeClipboard:
    CloseClipboard();
}

// There exists a AddTypedKey function which takes the typedKey variable from a KeyMapInfo object and writes it to the input box.
// However, this typedKey variable is the only thing that the function needs from this entire object.
// So we just define a dummy KeyMapInfo object where we fill the character we want to enter in every loop iteration.
void InputBoxWindow::WriteString(LPCWSTR str)
{
    KeyMapInfo temp;
    size_t strLen = wcslen(str);

    // Stop when the end of the string has been reached, or if the buffer is full.
    for (size_t i = 0; i < strLen && chars.size() < (size_t) maxCharsLength; ++i)
    {
        temp.enteredKey = str[i];
        this->AddTypedKey(&temp);
    }
}

void InputBoxWindow::CopyToClipboard()
{
    if (!OpenClipboard(NULL))
        return;

    EmptyClipboard();

    size_t inputLength = this->chars.size();
    HGLOBAL clipboardData = GlobalAlloc(GMEM_DDESHARE, sizeof(WCHAR) * (inputLength + 1));

    if (!clipboardData)
        goto _closeClipboard;

    LPWSTR clipboardStr = static_cast<LPWSTR>(GlobalLock(clipboardData));

    if (!clipboardStr)
        goto _closeClipboard;

    // Copy every char from the buffer to clipboardStr.
    for (size_t i = 0; i < inputLength; ++i)
        clipboardStr[i] = this->chars[i].c;

    clipboardStr[inputLength] = L'\0';

    if (GlobalUnlock(clipboardData) == TRUE)
        SetClipboardData(CF_UNICODETEXT, clipboardData);
    else
        GlobalFree(clipboardData);

_closeClipboard:
    CloseClipboard();
}

void InputBoxWindow::HandleCopyPaste(KeyMapInfo *kmi)
{
    if (kmi->IsCtrlPressed())
    {
        // Ctrl + V pressed?
        if (toupper(kmi->enteredKey) == L'V')
        {
            CopyFromClipboard();
        }
        // Ctrl + C pressed?
        else if (toupper(kmi->enteredKey) == 'C')
        {
            CopyToClipboard();
        }
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
