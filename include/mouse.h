#pragma once

#include "vftable.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define STDCALL __stdcall

void InitCursorFix();

void InitMouseWarpFix();

// Redefining this because I don't want the project to depend on the DirectX SDK...
struct IDirectInputDevice8
{
    FILL_VFTABLE(0)
    virtual void Vftable_x10();
    virtual void Vftable_x14();
    virtual void Vftable_x18();
    virtual long STDCALL Acquire(); // 0x1C
    virtual long STDCALL Unacquire(); // 0x20
    virtual void Vftable_x24();
    virtual void Vftable_x28();
    virtual void Vftable_x2C();
    virtual void Vftable_x30();
    virtual long STDCALL SetCooperativeLevel(HWND hwnd, DWORD flags); // 0x34
};
