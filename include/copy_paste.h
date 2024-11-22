#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <vector>
#include "vftable.h"
#include "Common.h"

#define NAKED __declspec(naked)

struct InputChar
{
    wchar_t c;
    DWORD flags; // I don't know whether this field actually represents flags; it's just an educated guess.
    DWORD unk; // Allocated but never assigned.
};

struct KeyMapInfo
{
    BYTE x00[0x8];
    DWORD controlCharacterFlags; // 0x8
    DWORD x0C;
    wchar_t enteredKey; // 0x10

    inline bool IsCtrlPressed()
    {
        return (controlCharacterFlags & 4) == 4;
    }
};

struct InputBoxWindow
{
    BYTE x04[0x498];
    int pos; // 0x49C
    BYTE x4A0[0x24];
    std::vector<InputChar> chars; // 0x4C4
    BYTE x4E4[0x3C];
    int maxCharsLength; // 0x510
    BYTE x514[0x18];
    wchar_t forbiddenChar; // 0x528
    bool noForbiddenChar; // 0x52A
    PDWORD ime; // 0x52C

    FILL_VFTABLE(0)
    FILL_VFTABLE(1)
    FILL_VFTABLE(2)
    FILL_VFTABLE(3)
    FILL_VFTABLE(4)
    FILL_VFTABLE(5)
    FILL_VFTABLE(6)
    FILL_VFTABLE(7)
    FILL_VFTABLE(8)
    FILL_VFTABLE(9)
    FILL_VFTABLE(A)
    FILL_VFTABLE(B)
    virtual void Vftable_xC0();
    virtual void Vftable_xC4();
    virtual bool AddTypedKey(const KeyMapInfo *kmi);

    void HandleCopyPaste(KeyMapInfo *kmi);
};

struct UITextMsgButton
{
public:
    int SlideAnimation_Hook(BYTE unk1, const float* newXPos, BYTE unk2);

private:
    FILL_VFTABLE(0)
    FILL_VFTABLE(1)
    FILL_VFTABLE(2)
    FILL_VFTABLE(3)
    FILL_VFTABLE(4)
    FILL_VFTABLE(5)
    FILL_VFTABLE(6)
    FILL_VFTABLE(7)
    FILL_VFTABLE(8)
    FILL_VFTABLE(9)
    virtual void Vftable_xC0();
    virtual void Vftable_xC4();
    virtual int SlideAnimation(BYTE unk1, const float* newXPos, BYTE unk2);

    BYTE x04[0x3E8];
    PVOID textInfo; // 0x3EC. textInfo = NULL will prevent the text from rendering
    BYTE x3F0[0x81];
    bool disableHovering; // 0x471
};

void HandleDefaultInputKey_Hook();
void InitCopyPasteFeature();
