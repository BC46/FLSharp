#pragma once

#include "vftable.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define STDCALL __stdcall

#define DPNCANCEL_ALL_OPERATIONS    0x00008000
#define DPNCLOSE_IMMEDIATE          0x00000001

// Redefining this struct because I don't want the project to depend on the DirectX SDK...
struct IDirectPlay8Client
{
    FILL_VFTABLE(0)
    virtual void Vftable_x10();
    virtual void Vftable_x14();
    virtual long STDCALL CancelAsyncOperation(const DWORD hAsyncHandle, const DWORD dwFlags); // 0x18
    virtual void Vftable_x1C();
    FILL_VFTABLE(2)
    virtual void Vftable_x30();
    virtual void Vftable_x34();
    virtual long STDCALL Close(const DWORD dwFlags); // 0x38

    long STDCALL Close_Hook(const DWORD dwFlags);
};

void InitPostGameDeadlockFix();

void InitQuitMessageFix();
void CleanupQuitMessageFix();
