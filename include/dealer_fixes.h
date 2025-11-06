#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define UI_ELEMENT_VISIBLE 0x3

struct ManeuverFrame
{
    BYTE x00[0x6C];
    BYTE flags; // 0x6C
};

struct NavBar
{
    BYTE x00[0x3D8];
    ManeuverFrame* maneuverFrame; // 0x3D8
    BYTE x3DC[0x4];
    PVOID unkUiElement; // 0x3E0
    bool shipDealerMenuOpened; // 0x3E4
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
