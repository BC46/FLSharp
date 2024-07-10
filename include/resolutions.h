#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Freelancer.h"

#define MIN_RES_WIDTH 800
#define MIN_RES_HEIGHT 600

#define NN_PREFERENCES_ALLOC_SIZE_PTR 0x4B296A
#define NN_PREFERENCES_ALLOC_SIZE 0x980
#define NN_PREFERENCES_NEW_DATA 0x988

struct WidthHeight
{
    UINT width, height;
};

struct ResolutionInfo
{
    ResolutionInfo(UINT width, UINT height, UINT bpp)
        : width(width), height(height), bpp(bpp)
    {}

    // bpp = bits per pixel. FL appears to only support 16 and 32
    UINT width, height, bpp;

    // First sort by bpp, then width, then height, all in ascending order
    bool operator < (const ResolutionInfo& other) const
    {
        if (bpp != other.bpp)
            return bpp < other.bpp;
        else if (width != other.width)
            return width < other.width;
        else
            return height < other.height;
    }
};

bool __fastcall InitializeElements_Hook(NN_Preferences* thisptr, PVOID _edx, DWORD unk1, DWORD unk2);

void InitBetterResolutions();
