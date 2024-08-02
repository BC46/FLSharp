#include "resolutions.h"
#include "resolutions_asm.h"
#include "utils.h"
#include <set>

#define DEFAULT_RES_WIDTH_PTR_1 0x56223C
#define DEFAULT_RES_HEIGHT_PTR_1 (DEFAULT_RES_WIDTH_PTR_1 + 0x7)

#define DEFAULT_RES_WIDTH_PTR_2 0x424E9D
#define DEFAULT_RES_HEIGHT_PTR_2 (DEFAULT_RES_WIDTH_PTR_2 + 0x5)

std::set<ResolutionInfo> resolutions;

void AddFlResolutions()
{
    const WidthHeight defaultRes[] =
        { { 800, 600 }, { 1024, 768 }, { 1152, 864 }, { 1280, 960 }, { 1600, 1200 } };

    for (int i = 0; i < sizeof(defaultRes) / sizeof(WidthHeight); ++i)
    {
        resolutions.insert(ResolutionInfo ( defaultRes[i].width, defaultRes[i].height, 16 ));
        resolutions.insert(ResolutionInfo ( defaultRes[i].width, defaultRes[i].height, 32 ));
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

void AddDcResolutions()
{
    HDC hdc = GetDC(NULL);

    if (!hdc)
        return;

    int horzRes = GetDeviceCaps(hdc, HORZRES);
    int vertRes = GetDeviceCaps(hdc, VERTRES);
    ReleaseDC(NULL, hdc);

    SetHorzRes(horzRes);
    SetVertRes(vertRes);

    Patch_INT(DEFAULT_RES_WIDTH_PTR_1, horzRes);
    Patch_INT(DEFAULT_RES_WIDTH_PTR_2, horzRes);
    Patch_INT(DEFAULT_RES_HEIGHT_PTR_1, vertRes);
    Patch_INT(DEFAULT_RES_HEIGHT_PTR_2, vertRes);

    resolutions.insert(ResolutionInfo( horzRes, vertRes, 16 ));
    resolutions.insert(ResolutionInfo( horzRes, vertRes, 32 ));
}

void AddDisplaySettingsResolutions()
{
    DEVMODE dm = { 0 };
    dm.dmSize = sizeof(dm);

    for (int iModeNum = 0; EnumDisplaySettings(NULL, iModeNum, &dm) != FALSE; ++iModeNum)
    {
        if (IsResolutionAllowed(dm))
            resolutions.insert(ResolutionInfo( dm.dmPelsWidth, dm.dmPelsHeight, dm.dmBitsPerPel ));
    }
}

bool NN_Preferences::InitElements_Hook(DWORD unk1, DWORD unk2)
{
    int i;
    ResolutionInfo* nextInfo;
    std::set<ResolutionInfo>::iterator it = resolutions.begin();

    // We are editing heap memory, so it should be editable by default, but make it writeable just to be sure
    DWORD _;
    VirtualProtect(this, *((PUINT) NN_PREFERENCES_ALLOC_SIZE_PTR), PAGE_EXECUTE_READWRITE, &_);

    // Fill Resolution info
    for (i = 0; it != resolutions.end(); ++it)
    {
        nextInfo = ((ResolutionInfo*) &newData) + (i++);
        *nextInfo = *it;
    }

    memset((PBYTE) ++nextInfo, 0x00, resolutions.size());

    PBYTE resIndicesVOffset = (PBYTE) nextInfo + resolutions.size();
    memset(resIndicesVOffset, 0xFF, resolutions.size() * sizeof(int));

    int resSupportedInfoOffset = ((PBYTE) nextInfo) - ((PBYTE) this);
    int resIndicesOffset = resIndicesVOffset - ((PBYTE) this);

    // +0x944
    const DWORD supportedInfoRefs[] = { 0x4B1005, 0x4B24B3, 0x4B1C73, 0x4B0773, 0x4ACEDA };
    for (i = 0; i < sizeof(supportedInfoRefs) / sizeof(DWORD); ++i)
        Patch_INT(supportedInfoRefs[i], resSupportedInfoOffset);

    // weird negated value (note the minus sign)
    Patch_INT(0x4B24A5, -resSupportedInfoOffset);

    // +0x954
    const DWORD resIndicesRefs[] = { 0x4B249C, 0x4B17E0, 0x4B0FFA, 0x4ACEF9, 0x4B0764 };
    for (i = 0; i < sizeof(resIndicesRefs) / sizeof(DWORD); ++i)
        Patch_INT(resIndicesRefs[i], resIndicesOffset);

    // Call original function
    InitElements initElementsFunc = GetFuncDef<InitElements>(INIT_NN_ELEMENTS_ADDR);
    return (this->*initElementsFunc)(unk1, unk2);
}

// Dirty hack which adds an additional parameter to the game's internal SetResolution function
// The purpose of putting the new parameter (height) last is so that it doesn't change the offsets of the other two parameters
// There are two variations of this hook, one sets the active height as the height parameter, the other one sets the selected height

bool NN_Preferences::SetResolution_Active_Hook(UINT width, DWORD unk)
{
    SetResolution setResFunc = GetFuncDef<SetResolution>(SET_RESOLUTION_ADDR);
    return (this->*setResFunc)(width, unk, activeHeight);
}

bool NN_Preferences::SetResolution_Selected_Hook(UINT width, DWORD unk)
{
    SetResolution setResFunc = GetFuncDef<SetResolution>(SET_RESOLUTION_ADDR);
    return (this->*setResFunc)(width, unk, selectedHeight);
}

void DiscardLowestResolutions(size_t newSize)
{
    std::set<ResolutionInfo>::iterator it = resolutions.begin();

    while (resolutions.size() > newSize)
    {
        resolutions.erase(it++);
    }
}

void InitBetterResolutions()
{
    AddDisplaySettingsResolutions();

    // Make sure there can only be 127 resolutions at most after the resolutions below have been added too
    DiscardLowestResolutions(127 - 14);

    AddFlResolutions();
    AddWindowRectResolutions();
    AddDcResolutions();

    int i;
    int resolutionAmount = resolutions.size();

    int additionalSize = NN_PREFERENCES_ALLOC_SIZE
        + resolutionAmount * sizeof(ResolutionInfo) // resolution info
        + resolutionAmount // supported array
        + resolutionAmount * sizeof(int) // indices in menu
        + sizeof(UINT) * 2; // active and selected height

    // Expand the allocated heap memory of the NN_Preferences object so that we can store more resolutions
    Patch(NN_PREFERENCES_ALLOC_SIZE_PTR, &additionalSize, sizeof(additionalSize));

    // These offsets below are always the same so we can just set them once on startup

    // Patch resolution amount (byte, 0xA)
    const DWORD resAmountRefs[] = { 0x4B2521, 0x4B1086, 0x4B1CC1, 0x4B17F0, 0x4B07DA, 0x4ACEF1 };
    // We know resolutions.size() <= 127, so casting it directly to a byte is fine
    for (i = 0; i < sizeof(resAmountRefs) / sizeof(DWORD); ++i)
        Patch_CHAR(resAmountRefs[i], (BYTE) resolutions.size());

    // Patch references to the start of the resolution array such that it points to the new one (0x8CC)
    const DWORD resStartRefs[] = { 0x4B0FEB, 0x4B17FF, 0x4B1C5C };
    for (i = 0; i < sizeof(resStartRefs) / sizeof(DWORD); ++i)
        Patch_INT(resStartRefs[i], NN_PREFERENCES_NEW_DATA);

    // Patch references to the first bpp in the resolution array (0x8D4)
    const DWORD firstBppRefs[] = { 0x4B24B9, 0x4ACED3, 0x4B076A };
    for (i = 0; i < sizeof(firstBppRefs) / sizeof(DWORD); ++i)
        Patch_INT(firstBppRefs[i], NN_PREFERENCES_NEW_DATA + 0x8);

    // Set hook that copies the resolutions into the right location when called
    SetPointer(INIT_NN_ELEMENTS_CALL_ADDR, NN_Preferences::InitElements_Hook);

    // Places where the current resolution info is written to (selected and/or active width)
    Hook(0x4A9AAB, CurrentResInfoWrite1, 6);
    Hook(0x4B1046, CurrentResInfoWrite2, 6);
    Hook(0x4B180F, CurrentResInfoWrite3, 6);
    Hook(0x4B1C20, CurrentResInfoWrite4, 6);
    Hook(0x4AC264, CurrentResInfoWrite5, 6);
    Hook(0x4B27A6, CurrentResInfoWrite6, 6);
    Hook(0x4B10C3, CurrentResInfoWrite7, 6);

    // Places where the current resolution info is checked or compared (selected and/or active width)
    Hook(0x4B1F67, CurrentResInfoCheck1, 6, true);
    Hook(0x4B102B, CurrentResInfoCheck2, 5, true);
    Hook(0x4B257A, CurrentResInfoCheck3, 6, true);
    Hook(0x4B1C93, CurrentResInfoCheck4, 8, true);
    Hook(0x4B074E, CurrentResInfoCheck5, 6, true);
    Hook(0x4B0786, CurrentResInfoCheck6, 7, true);
    Hook(0x4ACEE2, CurrentResInfoCheck7, 5, true);

    // Places a hook where a function is called which sets the new resolution
    // This is hooked because we need this function to take an additional parameter (the height)
    Hook(0x4AC4B0, NN_Preferences::SetResolution_Active_Hook, 5);
    Hook(0x4B1E65, NN_Preferences::SetResolution_Selected_Hook, 5);
    Hook(0x4B2594, NN_Preferences::SetResolution_Selected_Hook, 5);
    Hook(0x4B2781, NN_Preferences::SetResolution_Active_Hook, 5);

    // Places that determine the width of the "default" resolution
    Hook(0x4ACEAB, DefaultResSet1, 5, true);
    Hook(0x4ACEBB, DefaultResSet2, 7, true);

    // Change the amount of bytes that are cleaned from the stack when the "set resolution function" returns because an additional parameter has been added
    WORD paramBytes = 12; // 0xC
    Patch(0x4B1D09, &paramBytes, sizeof(WORD));
    Patch(0x4B1D14, &paramBytes, sizeof(WORD));
}
