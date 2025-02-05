#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Freelancer.h"

#define MIN_RES_WIDTH 800
#define MIN_RES_HEIGHT 600

#define NN_PREFERENCES_ALLOC_SIZE_PTR 0x4B296A
#define NN_PREFERENCES_ALLOC_SIZE 0x980

struct WidthHeight
{
    UINT width, height;

    bool Equals(const WidthHeight &other)
    {
        return memcmp(this, &other, sizeof(WidthHeight)) == 0;
    }
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

struct ResolutionInitInfo
{
    BYTE x00[0x8];
    ResolutionInfo resolutionInfo;
};

bool inline IsResolutionAllowed(const DEVMODE &dm)
{
    return dm.dmPelsWidth >= MIN_RES_WIDTH && dm.dmPelsHeight >= MIN_RES_HEIGHT && (dm.dmBitsPerPel == 16 || dm.dmBitsPerPel == 32);
}

// Returns true if the given resolution is narrower than 4:3.
bool inline IsResolutionNarrow(UINT width, UINT height)
{
    #define MIN_4_BY_3_FACTOR (4.0f / 3.0f) - 0.02f

    if (height == 0)
        return true;

    return ((float) width / (float) height) < MIN_4_BY_3_FACTOR;
}

void InitBetterResolutions();
void CleanupBetterResolutions();
