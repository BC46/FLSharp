#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "vftable.h"
#include "Freelancer.h"

struct BigImage
{
    virtual void Vftable_x00();
    virtual void Vftable_x04();
    virtual DWORD __stdcall Destroy();
};

struct UITextMsgButton : public FlUiElement
{
public:
    int UpdatePosition_Hook(TransformType type, const Vector* newPosOffset, DWORD unk);

    BYTE x70[0x37C];
    BigImage* textImage; // 0x3EC. textImage = nullptr will prevent the text from rendering
    BYTE x3F0[0x81];
    bool disableHovering; // 0x471
};

void InitSlideUiAnimFix();
