#include "resolutions.h"
#include "resolutions_asm.h"
#include "utils.h"
#include "fl_func.h"
#include <set>

#define DEFAULT_RES_WIDTH_PTR_1 0x56223F
#define DEFAULT_RES_HEIGHT_PTR_1 (DEFAULT_RES_WIDTH_PTR_1 + 0x7)

#define DEFAULT_RES_WIDTH_PTR_2 0x424E9D
#define DEFAULT_RES_HEIGHT_PTR_2 (DEFAULT_RES_WIDTH_PTR_2 + 0x5)

// sizeof(int) + sizeof(BYTE) = for the indices in menu and supported array entry
#define INDEX_RES_AND_SUP_ARR_ENTRY_SIZE (sizeof(int) + sizeof(BYTE))

std::set<ResolutionInfo> resolutions;
UINT lastSupportedResAmount = 0;
bool lastUnk_x97C = true;
BYTE* lastResSupportedArr = nullptr;

WidthHeight mainMonitorRes;

WidthHeight GetMainMonitorResolution()
{
    WidthHeight result;

    HDC hdc = GetDC(nullptr);

    if (hdc)
    {
        result.width = GetDeviceCaps(hdc, HORZRES);
        result.height = GetDeviceCaps(hdc, VERTRES);
    }
    else
    {
        result.width = 1024;
        result.height = 768;
    }

    ReleaseDC(nullptr, hdc);
    return result;
}

void AddFlResolutions()
{
    const WidthHeight defaultResolutions[] =
        { { 800, 600 }, { 1024, 768 }, { 1152, 864 }, { 1280, 960 }, { 1600, 1200 } };

    for (const auto& defaultRes : defaultResolutions)
    {
        resolutions.emplace(defaultRes.width, defaultRes.height, 16);
        resolutions.emplace(defaultRes.width, defaultRes.height, 32);
    }
}

void AddWindowRectResolutions()
{
    RECT desktop;

    if (GetWindowRect(GetDesktopWindow(), &desktop))
    {
        resolutions.emplace(desktop.right, desktop.bottom, 16);
        resolutions.emplace(desktop.right, desktop.bottom, 32);
    }
}

void AddMainMonitorResolutions()
{
    SetMainResWidth(mainMonitorRes.width);
    SetMainResHeight(mainMonitorRes.height);

    Patch<int>(DEFAULT_RES_WIDTH_PTR_1, mainMonitorRes.width);
    Patch<int>(DEFAULT_RES_WIDTH_PTR_2, mainMonitorRes.width);
    Patch<int>(DEFAULT_RES_HEIGHT_PTR_1, mainMonitorRes.height);
    Patch<int>(DEFAULT_RES_HEIGHT_PTR_2, mainMonitorRes.height);

    resolutions.emplace(mainMonitorRes.width, mainMonitorRes.height, 16);
    resolutions.emplace(mainMonitorRes.width, mainMonitorRes.height, 32);
}

void AddDisplaySettingsResolutions()
{
    bool isMainResNarrow = IsResolutionNarrow(mainMonitorRes.width, mainMonitorRes.height);
    DEVMODE dm = { 0 };
    dm.dmSize = sizeof(dm);

    for (DWORD iModeNum = 0; EnumDisplaySettings(nullptr, iModeNum, &dm) != FALSE; ++iModeNum)
    {
        // Discard resolutions that are not allowed.
        // Moreover, discard resolutions that are too narrow (e.g. 5:4) since FL doesn't run well with those.
        if (!IsResolutionAllowed(dm) || (!isMainResNarrow && IsResolutionNarrow(dm.dmPelsWidth, dm.dmPelsHeight)))
            continue;

        resolutions.emplace(dm.dmPelsWidth, dm.dmPelsHeight, dm.dmBitsPerPel);
    }
}

bool NN_Preferences::InitElements_Hook(DWORD unk1, DWORD unk2)
{
    ResolutionInfo* nextInfo;
    auto it = resolutions.begin();

    // Fill Resolution info
    for (int i = 0; it != resolutions.end(); ++it)
    {
        nextInfo = ((ResolutionInfo*) &this->newData) + (i++);
        *nextInfo = *it;
    }

    memset((PBYTE) ++nextInfo, 0x00, resolutions.size());

    PBYTE resIndicesVOffset = (PBYTE) nextInfo + resolutions.size();
    memset(resIndicesVOffset, 0xFF, resolutions.size() * sizeof(int));

    this->resSupportedArr = (bool*) nextInfo;
    int resSupportedInfoOffset = ((PBYTE) nextInfo) - ((PBYTE) this);
    int resIndicesOffset = resIndicesVOffset - ((PBYTE) this);

    // +0x944
    const DWORD supportedInfoRefs[] = { 0x4B1005, 0x4B24B3, 0x4B1C73, 0x4B0773, 0x4ACEDA };
    for (const auto& ref : supportedInfoRefs)
        Patch<int>(ref, resSupportedInfoOffset);

    // weird negated value (note the minus sign)
    Patch<int>(0x4B24A5, -resSupportedInfoOffset);

    // +0x954
    const DWORD resIndicesRefs[] = { 0x4B249C, 0x4B17E0, 0x4B0FFA, 0x4ACEF9, 0x4B0764 };
    for (const auto& ref : resIndicesRefs)
        Patch<int>(ref, resIndicesOffset);

    // Call original function
    return InitElements(unk1, unk2);
}

// Dirty hack which adds an additional parameter to the game's internal SetResolution function
// The purpose of putting the new parameter (height) last is so that it doesn't change the offsets of the other two parameters
// There are two variations of this hook, one sets the active height as the height parameter, the other one sets the selected height

bool NN_Preferences::SetResolution_Active_Hook(UINT width, DWORD unk)
{
    return SetResolution(width, unk, this->activeHeight);
}

bool NN_Preferences::SetResolution_Selected_Hook(UINT width, DWORD unk)
{
    return SetResolution(width, unk, this->selectedHeight);
}

void (NN_Preferences::*TestResolutions_Original)(DWORD unk);

// Hook that ensures the resolutions are tested only when necessary (optimization)
void NN_Preferences::TestResolutions_Hook(DWORD unk)
{
    WidthHeight currentMainRes = GetMainMonitorResolution();

    if (lastSupportedResAmount && currentMainRes.Equals(mainMonitorRes))
    {
        // If the monitor settings haven't changed and we know the supported resolution info,
        // set the info without testing the resolutions
        memcpy(this->resSupportedArr, lastResSupportedArr, resolutions.size() * INDEX_RES_AND_SUP_ARR_ENTRY_SIZE);
        this->supportedResAmount = lastSupportedResAmount;
        this->unk_x97C = lastUnk_x97C;
    }
    else
    {
        // If the monitor settings have changed or the resolutions haven't been tested yet,
        // test the resolutions
        (this->*TestResolutions_Original)(unk);

        // Save the supported resolution info for later use
        memcpy(lastResSupportedArr, this->resSupportedArr, resolutions.size() * INDEX_RES_AND_SUP_ARR_ENTRY_SIZE);
        lastSupportedResAmount = this->supportedResAmount;
        lastUnk_x97C = this->unk_x97C;
    }

    mainMonitorRes = currentMainRes;
}

void DiscardLowestResolutions(size_t newSize)
{
    auto it = resolutions.begin();

    while (resolutions.size() > newSize)
    {
        resolutions.erase(it++);
    }
}

FL_FUNC(bool ResolutionInit(DWORD unk1, ResolutionInitInfo* info, DWORD unk2), 0x424DD0)

bool ResolutionInit_Hook(DWORD unk1, ResolutionInitInfo* info, DWORD unk2)
{
    // If a resolution has been set in the ini file which is beyond the display's capabilities, the game may still run with it, but it'll make everything look strange.
    if (info->resolutionInfo.height > mainMonitorRes.height || info->resolutionInfo.width > mainMonitorRes.width)
    {
        // Zero the resolution's width, causing FL to use a default resolution.
        info->resolutionInfo.width = 0;
    }

    return ResolutionInit(unk1, info, unk2);
}

// Expands the hard-coded resolutions array of size 10 used in the options menu to allow for up to 127 resolutions instead.
// The new resolutions are determined dynamically based on the current user's main monitor resolution.
// Also adds an optimization to make the game only verify the resolutions when necessary.
// Moreover, FL can now distinguish resolutions that have the same width but a different height.
void InitBetterResolutions()
{
    mainMonitorRes = GetMainMonitorResolution();
    AddDisplaySettingsResolutions();

    // Make sure there can only be 127 resolutions at most after the resolutions below have been added too
    DiscardLowestResolutions(127 - 14);

    AddFlResolutions();
    AddWindowRectResolutions();
    AddMainMonitorResolutions();
    // Hook the resolution call address to allow for an additional resolution check.
    Hook(0x5B17AE, ResolutionInit_Hook, 5);

    int resolutionAmount = resolutions.size();

    lastResSupportedArr = new BYTE[resolutionAmount * INDEX_RES_AND_SUP_ARR_ENTRY_SIZE];

    int additionalSize = NN_PREFERENCES_ALLOC_SIZE
        + resolutionAmount * sizeof(ResolutionInfo) // resolution info
        + resolutionAmount // supported array
        + resolutionAmount * sizeof(int) // indices in menu
        + sizeof(UINT) * 3; // active and selected height + pointer to supported array

    // Expand the allocated heap memory of the NN_Preferences object so that we can store more resolutions
    Patch(NN_PREFERENCES_ALLOC_SIZE_PTR, &additionalSize, sizeof(additionalSize));

    // These offsets below are always the same so we can just set them once on startup

    // Patch resolution amount (byte, 0xA)
    const DWORD resAmountRefs[] = { 0x4B2521, 0x4B1086, 0x4B1CC1, 0x4B17F0, 0x4B07DA, 0x4ACEF1 };
    // We know resolutions.size() <= 127, so casting it directly to a byte is fine
    for (const auto& ref : resAmountRefs)
        Patch<char>(ref, (BYTE) resolutions.size());

    // Patch references to the start of the resolution array such that it points to the new one (0x8CC)
    const DWORD resStartRefs[] = { 0x4B0FEB, 0x4B17FF, 0x4B1C5C };
    for (const auto& ref : resStartRefs)
        Patch<int>(ref, NN_PREFERENCES_NEW_DATA);

    // Patch references to the first bpp in the resolution array (0x8D4)
    const DWORD firstBppRefs[] = { 0x4B24B9, 0x4ACED3, 0x4B076A };
    for (const auto& ref : firstBppRefs)
        Patch<int>(ref, NN_PREFERENCES_NEW_DATA + 0x8);

    // Set hook that copies the resolutions into the right location when called
    SetPointer(INIT_NN_ELEMENTS_CALL_ADDR, &NN_Preferences::InitElements_Hook);

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
    Hook(0x4AC4B0, &NN_Preferences::SetResolution_Active_Hook, 5);
    Hook(0x4B1E65, &NN_Preferences::SetResolution_Selected_Hook, 5);
    Hook(0x4B2594, &NN_Preferences::SetResolution_Selected_Hook, 5);
    Hook(0x4B2781, &NN_Preferences::SetResolution_Active_Hook, 5);

    // Hook test resolutions functions so that we only test the resolutions when it's actually necessary (optimization)
    TestResolutions_Original = Trampoline(TEST_RESOLUTIONS_ADDR, &NN_Preferences::TestResolutions_Hook, 8);

    // Places that determine the width of the "default" resolution
    Hook(0x4ACEAB, DefaultResSet1, 5, true);
    Hook(0x4ACEBB, DefaultResSet2, 7, true);

    // Increase the amount of bytes that are cleaned from the stack when the "set resolution function" returns because an additional parameter has been added
    GetValue<WORD>(0x4B1D09) += sizeof(DWORD);
    GetValue<WORD>(0x4B1D14) += sizeof(DWORD);
}

void CleanupBetterResolutions()
{
    delete[] lastResSupportedArr;
    CleanupTrampoline(TestResolutions_Original);
}
