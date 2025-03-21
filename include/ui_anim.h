#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "vftable.h"
#include "Common.h"

struct BigImage
{
    virtual void Vftable_x00();
    virtual void Vftable_x04();
    virtual DWORD __stdcall Destroy();
};

struct UITextMsgButton
{
public:
    int UpdatePosition_Hook(BYTE unk1, const Vector* newPosOffset, BYTE unk2);

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
    virtual int UpdatePosition(BYTE unk1, const Vector* newPosOffset, BYTE unk2);

    BYTE x04[0x3E8];
    BigImage* textImage; // 0x3EC. textImage = NULL will prevent the text from rendering
    BYTE x3F0[0x81];
    bool disableHovering; // 0x471
};

void InitSlideUiAnimFix();
