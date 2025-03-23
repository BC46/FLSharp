#include "copy_paste.h"
#include "utils.h"

#define NAKED __declspec(naked)

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
    if (!OpenClipboard(nullptr))
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

// There exists a WriteTypedKey function which takes the typedKey variable from a KeyMapInfo object and writes it to the input box.
// However, this typedKey variable is the only thing that the function needs from this entire object.
// So we just define a dummy KeyMapInfo object where we fill the character we want to enter in every loop iteration.
void InputBoxWindow::WriteString(LPCWSTR str)
{
    KeyMapInfo temp;

    // Stop when the end of the string has been reached, or if the buffer is full.
    for (size_t i = 0; str[i] != L'\0' && chars.size() < (size_t) maxCharsLength; ++i)
    {
        temp.enteredKey = str[i];
        this->WriteTypedKey(&temp);
    }
}

void InputBoxWindow::CopyToClipboard()
{
    size_t inputLength = this->chars.size();

    // If the chars vector is empty, there isn't anything to copy to the clipboard.
    // If the clipboard won't even open, there's no point in trying either.
    if (inputLength == 0 || !OpenClipboard(nullptr))
        return;

    if (!EmptyClipboard())
        goto _closeClipboard;

    HGLOBAL clipboardData = GlobalAlloc(GMEM_MOVEABLE, sizeof(WCHAR) * (inputLength + 1));

    if (!clipboardData)
        goto _closeClipboard;

    LPWSTR clipboardStr = static_cast<LPWSTR>(GlobalLock(clipboardData));

    if (!clipboardStr)
    {
        GlobalFree(clipboardData);
        goto _closeClipboard;
    }

    // Copy every char from the input box buffer to clipboardStr.
    for (size_t i = 0; i < inputLength; ++i)
        clipboardStr[i] = this->chars[i].c;

    clipboardStr[inputLength] = L'\0'; // Set the null character at the end.

    GlobalUnlock(clipboardData);

    if (!SetClipboardData(CF_UNICODETEXT, clipboardData))
        GlobalFree(clipboardData);

_closeClipboard:
    CloseClipboard();
}

void InputBoxWindow::HandleCopyPaste(KeyMapInfo *kmi)
{
    // I saw this check being made in many key handling function, but for this one I don't think it's necessary.
    // if (this->ime == nullptr)
    //     return;

    if (kmi->IsCtrlPressed())
    {
        // Ctrl + V pressed?
        if (toupper(kmi->enteredKey) == L'V')
        {
            CopyFromClipboard();
        }
        // Ctrl + C pressed?
        else if (toupper(kmi->enteredKey) == L'C')
        {
            CopyToClipboard();
        }
    }
}

// Allows for the Ctrl + C and Ctrl + V key combinations to copy and paste the current clipboard from/to the input box.
void InitCopyPasteFeature()
{
    #define HANDLE_DEFAULT_INPUT_KEY_ADDR 0x57CE3C
    SetPointer(HANDLE_DEFAULT_INPUT_KEY_ADDR, HandleDefaultInputKey_Hook);
}
