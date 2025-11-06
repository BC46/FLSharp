#define WIN32_LEAN_AND_MEAN
#include <windows.h>

struct DealerOpenCamera
{
    BYTE x00[0x1338];
    bool animationInProgress;

    bool StartAnimation(LPCSTR name, PVOID unk, PVOID navBar, DWORD unk2);
    bool StartAnimation_Hook(LPCSTR name, PVOID unk, PVOID navBar, DWORD unk2);
};

void InitDealerOpenFix();
