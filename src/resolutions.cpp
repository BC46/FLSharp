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

    // TODO: patch necessary values like 0x004B251F, 0x004B1084, 004B1CBF

    return ((InitializeNN_Preferences*) INITIALIZE_NN_PREFERENCES_ADDR)(thisptr, _edx, unk1, unk2);
}

bool __fastcall InitializeElements_Hook(NN_Preferences* thisptr, PVOID _edx, DWORD unk1, DWORD unk2)
{
    ResolutionInfo* nextInfo;
    std::set<ResolutionInfo>::iterator it = resolutions.begin();

    // TODO: Test 256+ resolutions
    // Discard lowest resolutions if there's more than 256
    while (resolutions.size() > 256)
    {
        resolutions.erase(it++);
    }

    // Fill Resolution info
    for (int i = 0; it != resolutions.end(); ++it)
    {
        nextInfo = ((ResolutionInfo*) &thisptr->newData) + (i++);
        Patch(nextInfo, &(*it), sizeof(ResolutionInfo));
    }

    DWORD _;
    VirtualProtect(thisptr, *((PUINT) NN_PREFERENCES_ALLOC_SIZE_PTR), PAGE_EXECUTE_READWRITE, &_);

    memset((PBYTE) ++nextInfo, 0x00, resolutions.size());

    PBYTE resIndicesVOffset = (PBYTE) nextInfo + resolutions.size();
    memset(resIndicesVOffset, 0xFF, resolutions.size() * sizeof(int));

    int resSupportedInfoOffset = ((PBYTE) nextInfo) - ((PBYTE) thisptr);
    int resSupportedInfoOffsetNeg = -resSupportedInfoOffset;
    int resIndicesOffset = resIndicesVOffset - ((PBYTE) thisptr);

    Patch((PVOID) 0x4B1005, &resSupportedInfoOffset, sizeof(int));
    Patch((PVOID) 0x4B24B3, &resSupportedInfoOffset, sizeof(int));
    Patch((PVOID) 0x4B1005, &resSupportedInfoOffset, sizeof(int));
    Patch((PVOID) 0x4B1C73, &resSupportedInfoOffset, sizeof(int));

    Patch((PVOID) 0x4B24A5, &resSupportedInfoOffsetNeg, sizeof(int));

    Patch((PVOID) 0x4B249C, &resIndicesOffset, sizeof(int));
    Patch((PVOID) 0x4B17E0, &resIndicesOffset, sizeof(int));
    Patch((PVOID) 0x4B0FFA, &resIndicesOffset, sizeof(int));

    return ((InitializeElements*) INITIALIZE_NN_ELEMENTS_ADDR)(thisptr, _edx, unk1, unk2);
}

void InitCustomResolutions()
{
    // These offsets are always the same so we can just set them once on startup
    DWORD newResStartOffset = NN_PREFERENCES_ALLOC_SIZE;
    DWORD firstBppOffset = NN_PREFERENCES_ALLOC_SIZE + 0x8;

    Patch((PVOID) 0x4B0FEB, &newResStartOffset, sizeof(DWORD));
    Patch((PVOID) 0x4B17FF, &newResStartOffset, sizeof(DWORD));
    Patch((PVOID) 0x4B1C5C, &newResStartOffset, sizeof(DWORD));

    Patch((PVOID) 0x4B24B9, &firstBppOffset, sizeof(DWORD));
}