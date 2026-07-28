#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Freelancer.h"

struct ObjectDeformable
{
    // If bit 1 is set, that means the ObjectDeformable itself gets freed as well.
    virtual ObjectDeformable* Destroy(DWORD flags = 1);
};

struct NavBar
{
    BYTE x00[0x344];
    bool roomTransitionInProgress; // 0x344
    BYTE x345[0x93];
    FlUiElement* maneuverFrame; // 0x3D8
    BYTE x3DC[0x4];
    ObjectDeformable* dealerCharacter; // 0x3E0
    bool shipDealerMenuOpened; // 0x3E4

    void SetHotspot_Hook(PVOID hotspot);
};

struct DealerOpenCamera
{
    BYTE x00[0x1338];
    bool animationInProgress; // 0x1338

    bool StartAnimation(LPCSTR name, PVOID unk, NavBar* navBar, DWORD unk2);
    bool StartAnimation_Hook(LPCSTR name, PVOID unk, NavBar* navBar, DWORD unk2);
};

void InitDealerOpenFix();

void InitDealerCrashFix();
