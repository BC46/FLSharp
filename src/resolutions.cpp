#include "resolutions.h"
#include "utils.h"
#include <set>

std::set<ResolutionInfo> resolutions;

bool inline IsResolutionAllowed(const DEVMODE &dm)
{
    return dm.dmPelsWidth >= MIN_RES_WIDTH && dm.dmPelsHeight >= MIN_RES_HEIGHT && (dm.dmBitsPerPel == 16 || dm.dmBitsPerPel == 32);
}

void AddFlResolutions()
{
    static int defaultWidths[]  = { 800, 1024, 1152, 1280, 1600 };
    static int defaultHeights[] = { 600,  768,  864, 960, 1200 };

    for (int i = 0; i < DEFAULT_RES_AMOUNT; ++i)
    {
        resolutions.insert(ResolutionInfo ( defaultWidths[i], defaultHeights[i], 16 ));
        resolutions.insert(ResolutionInfo ( defaultWidths[i], defaultHeights[i], 32 ));
    }
}

void AddWindowRectResolutions()
{
    RECT desktop;

    if (GetWindowRect(GetDesktopWindow(), &desktop))
    {
        resolutions.insert(ResolutionInfo ( desktop.right, desktop.bottom, 16 ));
        resolutions.insert(ResolutionInfo ( desktop.right, desktop.bottom, 32 ));
    }
}

void AddDcResolutions(HDC hdc)
{
    int horzRes = GetDeviceCaps(hdc, HORZRES);
    int vertRes = GetDeviceCaps(hdc, VERTRES);

    resolutions.insert(ResolutionInfo( horzRes, vertRes, 16 ));
    resolutions.insert(ResolutionInfo( horzRes, vertRes, 32 ));
}

void AddDisplaySettingsResolutions(HDC hdc)
{
    int refreshRate = GetDeviceCaps(hdc, VREFRESH);

    DEVMODE dm = { 0 };
    dm.dmSize = sizeof(dm);

    for (int iModeNum = 0; EnumDisplaySettings(NULL, iModeNum, &dm) != 0; ++iModeNum) {
        if (IsResolutionAllowed(dm) && dm.dmDisplayFrequency == refreshRate)
            resolutions.insert(ResolutionInfo( dm.dmPelsWidth, dm.dmPelsHeight, dm.dmBitsPerPel ));
    }
}

NN_Preferences* __fastcall InitializeNN_Preferences_Hook(PVOID thisptr, PVOID _edx, DWORD unk1, DWORD unk2)
{
    resolutions.clear();

    AddFlResolutions();
    AddWindowRectResolutions();

    HDC hdc = GetDC(NULL);

    if (hdc)
    {
        AddDcResolutions(hdc);
        AddDisplaySettingsResolutions(hdc);

        ReleaseDC(NULL, hdc);
    }

    int resolutionAmount = resolutions.size();

    int additionalSize = NN_PREFERENCES_ALLOC_SIZE
        + resolutionAmount * sizeof(ResolutionInfo) // resolution info
        + resolutionAmount // supported array
        + resolutionAmount * sizeof(int); // indices in menu

    Patch((PVOID) NN_PREFERENCES_ALLOC_SIZE_PTR, &additionalSize, sizeof(additionalSize));

    // TODO: patch necessary values like 0x004B249A, 0x004B24B1, 0x004B24B7, 0x004B1002, 0x004B0FE9, 0x004B251F, 0x004B1084, 0x004B17FC

    return ((InitializeNN_Preferences*) INITIALIZE_NN_PREFERENCES_ADDR)(thisptr, _edx, unk1, unk2);
}

bool __fastcall InitializeElements_Hook(NN_Preferences* thisptr, PVOID _edx, DWORD unk1, DWORD unk2)
{
    int i = 0;
    std::set<ResolutionInfo>::iterator it;

    // TODO: Test 256 resolutions
    for (it = resolutions.begin(); it != resolutions.end(); ++it) {
        Patch(((ResolutionInfo*) &thisptr->newData) + i, &(*it), sizeof(ResolutionInfo));

        if (++i == 256)
            break;
    }

    return ((InitializeElements*) INITIALIZE_NN_ELEMENTS_ADDR)(thisptr, _edx, unk1, unk2);
}

void InitCustomResolutions()
{
    DWORD newResStartOffset = NN_PREFERENCES_ALLOC_SIZE;
    DWORD firstBppOffset = NN_PREFERENCES_ALLOC_SIZE + 0x8;

    Patch((PVOID) 0x4B0FEB, &newResStartOffset, sizeof(DWORD));
    Patch((PVOID) 0x4B17FF, &newResStartOffset, sizeof(DWORD));
    //Patch((PVOID) 0x4B17E0, &newResStartOffset, sizeof(DWORD));

    Patch((PVOID) 0x4B24B9, &firstBppOffset, sizeof(DWORD));
    //Patch((PVOID) 0x4B17E0, &firstBppOffset, sizeof(DWORD));
}
