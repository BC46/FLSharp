#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "vftable.h"

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

void InitSlideUiAnimFix();