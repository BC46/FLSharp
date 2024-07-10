#include "resolutions.h"
#include "utils.h"
#include <set>

std::set<ResolutionInfo> resolutions;
int horzRes, vertRes, tempHeight;

bool inline IsResolutionAllowed(const DEVMODE &dm)
{
    return dm.dmPelsWidth >= MIN_RES_WIDTH && dm.dmPelsHeight >= MIN_RES_HEIGHT && (dm.dmBitsPerPel == 16 || dm.dmBitsPerPel == 32);
}

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

    horzRes = GetDeviceCaps(hdc, HORZRES);
    vertRes = GetDeviceCaps(hdc, VERTRES);

    resolutions.insert(ResolutionInfo( horzRes, vertRes, 16 ));
    resolutions.insert(ResolutionInfo( horzRes, vertRes, 32 ));

    ReleaseDC(NULL, hdc);
}

void AddDisplaySettingsResolutions()
{
    DEVMODE dm = { 0 };
    dm.dmSize = sizeof(dm);

    for (int iModeNum = 0; EnumDisplaySettings(NULL, iModeNum, &dm) != 0; ++iModeNum) {
        if (IsResolutionAllowed(dm))
            resolutions.insert(ResolutionInfo( dm.dmPelsWidth, dm.dmPelsHeight, dm.dmBitsPerPel ));
    }
}

bool __fastcall InitializeElements_Hook(NN_Preferences* thisptr, PVOID _edx, DWORD unk1, DWORD unk2)
{
    ResolutionInfo* nextInfo;
    std::set<ResolutionInfo>::iterator it = resolutions.begin();

    // We are editing heap memory, so it should be editable by default, but make it writeable just to be sure
    DWORD _;
    VirtualProtect(thisptr, *((PUINT) NN_PREFERENCES_ALLOC_SIZE_PTR), PAGE_EXECUTE_READWRITE, &_);

    // Fill Resolution info
    for (int i = 0; it != resolutions.end(); ++it)
    {
        nextInfo = ((ResolutionInfo*) &thisptr->newData) + (i++);
        *nextInfo = *it;
    }

    memset((PBYTE) ++nextInfo, 0x00, resolutions.size());

    PBYTE resIndicesVOffset = (PBYTE) nextInfo + resolutions.size();
    memset(resIndicesVOffset, 0xFF, resolutions.size() * sizeof(int));

    int resSupportedInfoOffset = ((PBYTE) nextInfo) - ((PBYTE) thisptr);
    int resSupportedInfoOffsetNeg = -resSupportedInfoOffset;
    int resIndicesOffset = resIndicesVOffset - ((PBYTE) thisptr);

    // +0x944
    Patch((PVOID) 0x4B1005, &resSupportedInfoOffset, sizeof(int));
    Patch((PVOID) 0x4B24B3, &resSupportedInfoOffset, sizeof(int));
    Patch((PVOID) 0x4B1C73, &resSupportedInfoOffset, sizeof(int));
    Patch((PVOID) 0x4B0773, &resSupportedInfoOffset, sizeof(int));
    Patch((PVOID) 0x4ACEDA, &resSupportedInfoOffset, sizeof(int));

    // weird negated value
    Patch((PVOID) 0x4B24A5, &resSupportedInfoOffsetNeg, sizeof(int));

    // +0x954
    Patch((PVOID) 0x4B249C, &resIndicesOffset, sizeof(int));
    Patch((PVOID) 0x4B17E0, &resIndicesOffset, sizeof(int));
    Patch((PVOID) 0x4B0FFA, &resIndicesOffset, sizeof(int));
    Patch((PVOID) 0x4ACEF9, &resIndicesOffset, sizeof(int));
    Patch((PVOID) 0x4B0764, &resIndicesOffset, sizeof(int));

    return ((InitializeElements*) INITIALIZE_NN_ELEMENTS_ADDR)(thisptr, _edx, unk1, unk2);
}

#define SELECTED_HEIGHT_OF 0x980
#define ACTIVE_HEIGHT_OF 0x984

__declspec(naked) void CurrentResInfoWrite1()
{
    __asm {
        mov [ebp+0x8B8], ebx
        mov [ebp+SELECTED_HEIGHT_OF], ebx
        mov [ebp+ACTIVE_HEIGHT_OF], ebx
        ret
    }
}

__declspec(naked) void CurrentResInfoWrite2()
{
    __asm {
        mov [ebx+0x330], eax
        mov [ebx+SELECTED_HEIGHT_OF], edi
        mov [ebx+ACTIVE_HEIGHT_OF], edi
        ret
    }
}

__declspec(naked) void CurrentResInfoWrite3()
{
    __asm {
        mov [edi+0x330], eax
        mov [edi+SELECTED_HEIGHT_OF], ecx
        ret
    }
}

__declspec(naked) void CurrentResInfoWrite4()
{
    __asm {
        mov [ebp+0x8B8], eax
        mov eax, [esp+0x20]
        mov [ebp+ACTIVE_HEIGHT_OF], eax
        ret
    }
}

// Selected to active
__declspec(naked) void CurrentResInfoWrite5()
{
    __asm {
        mov [ebp+0x8B8], eax
        mov eax, [ebp+SELECTED_HEIGHT_OF]
        mov [ebp+ACTIVE_HEIGHT_OF], eax
        ret
    }
}

// Active to selected
__declspec(naked) void CurrentResInfoWrite6()
{
    __asm {
        mov [esi+0x330], eax
        mov eax, [esi+ACTIVE_HEIGHT_OF]
        mov [esi+ACTIVE_HEIGHT_OF], eax
        ret
    }
}

__declspec(naked) void CurrentResInfoWrite7()
{
    __asm {
        mov [ebx+0x330], eax
        mov [ebx+SELECTED_HEIGHT_OF], edx
        mov [ebx+ACTIVE_HEIGHT_OF], edx
        ret
    }
}

__declspec(naked) void CurrentResInfoCheck1()
{
    __asm {
        mov eax, [ebp+ACTIVE_HEIGHT_OF]
        cmp eax, [ebp+SELECTED_HEIGHT_OF]
        jne notequal
        mov cl, byte ptr ss:[ebp+0x8BC]
        push 0x04B1F6D
        ret
    notequal:
        push 0x04B1F75
        ret
    }
}

__declspec(naked) void CurrentResInfoCheck2()
{
    __asm {
        cmp [esp+0x88], edi
        jne notequal
        push edi
        push eax
        cmp ecx, 0x20
        lea edx, [esp+0xAC]
        sete cl
        push 0x4B103A
        ret
    notequal:
        push 0x4B1075
        ret
    }
}

__declspec(naked) void CurrentResInfoCheck3()
{
    __asm {
        mov ecx, [esi+ACTIVE_HEIGHT_OF]
        cmp ecx, [esi+SELECTED_HEIGHT_OF]
        jne notequal
        mov cl, byte ptr ss:[esi+0x8BC]
        push 0x4B2580
        ret
    notequal:
        push 0x4B2588
        ret
    }
}

__declspec(naked) void CurrentResInfoCheck4()
{
    __asm {
        cmp edi, [esp+0x48]
        jne notequal
        push 0xFFFFFFFF
        push edi
        push esi
        lea ecx, [esp+0x20]
        push 0x4B1C9B
        ret
    notequal:
        push 0x4B1CB7
        ret
    }
}

__declspec(naked) void CurrentResInfoCheck5()
{
    __asm {
        mov edi, [ebp+ACTIVE_HEIGHT_OF]
        cmp edi, [ebp+SELECTED_HEIGHT_OF]
        jne notequal
        mov cl, byte ptr ss:[ebp+0x8BC]
        push 0x4B0754
        ret
    notequal:
        push 0x4B0760
        ret
    }
}

__declspec(naked) void CurrentResInfoCheck6()
{
    __asm {
        mov eax, [ebp+ACTIVE_HEIGHT_OF]
        cmp eax, [ebx-0x4]
        jne notequal
        mov ecx, [ebx]
        xor eax, eax
        cmp ecx, 0x20
        movzx ecx, byte ptr ss:[ebp+0x8BC]
        sete al
        push 0x4B0797
        ret
    notequal:
        push 0x4B07D1
        ret
    }
}

__declspec(naked) void CurrentResInfoCheck7()
{
    __asm {
        cmp [ecx-8], edx
        jne notequal
        mov ebx, [ecx-4]
        cmp ebx, [tempHeight]
        jne notequal
        push 0x4ACEE7
        ret
    notequal:
        push 0x4ACEEB
        ret
    }
}

// Dirty hack which adds an additional parameter to the game's internal CallSetResolution function
// The purpose of putting the new parameter (height) last is so that it doesn't change the offsets of the other two parameters
// There are two variations of this hook, one sets the active height as the height parameter, the other one sets the selected height
typedef bool __fastcall SetResolution(NN_Preferences* preferences, PVOID _edx, UINT width, DWORD unk, UINT height);

bool __fastcall CallSetResolution_Active(NN_Preferences* preferences, PVOID _edx, UINT width, DWORD unk)
{
    return ((SetResolution*) 0x4B1C00)(preferences, _edx, width, unk, preferences->activeHeight);
}

bool __fastcall CallSetResolution_Selected(NN_Preferences* preferences, PVOID _edx, UINT width, DWORD unk)
{
    return ((SetResolution*) 0x4B1C00)(preferences, _edx, width, unk, preferences->selectedHeight);
}

__declspec(naked) void DefaultResSet1()
{
    __asm {
        mov edx, [horzRes]
        mov ebx, [vertRes]
        mov [tempHeight], ebx
        push 0x4ACEB0
        ret
    }
}

__declspec(naked) void DefaultResSet2()
{
    __asm {
        mov edx, 0x320
        mov [tempHeight], 0x258
        cmp eax, esi
        jbe conditionmet
        push 0x4ACEC4
        ret
    conditionmet:
        push 0x4ACEC9
        ret
    }
}

void InitBetterResolutions()
{
    AddFlResolutions();
    AddWindowRectResolutions();
    AddDcResolutions();
    AddDisplaySettingsResolutions();

    std::set<ResolutionInfo>::iterator it = resolutions.begin();

    // Discard lowest resolutions if there's more than 127 (this is a hard limit)
    while (resolutions.size() > 127)
    {
        resolutions.erase(it++);
    }

    int resolutionAmount = resolutions.size();

    int additionalSize = NN_PREFERENCES_ALLOC_SIZE
        + resolutionAmount * sizeof(ResolutionInfo) // resolution info
        + resolutionAmount // supported array
        + resolutionAmount * sizeof(int) // indices in menu
        + sizeof(UINT) * 2; // active and selected height

    // Expand the allocated heap memory of the NN_Preferences object so that we can store more resolutions
    Patch((PVOID) NN_PREFERENCES_ALLOC_SIZE_PTR, &additionalSize, sizeof(additionalSize));

    // These offsets are always the same so we can just set them once on startup
    DWORD newResStartOffset = NN_PREFERENCES_NEW_DATA;
    DWORD firstBppOffset = NN_PREFERENCES_NEW_DATA + 0x8;

    // We know resolutions.size() <= 127, so casting it directly to a byte is fine
    BYTE resAmountByte = resolutions.size();

    // Patch resolution amount (byte)
    Patch((PVOID) 0x4B2521, &resAmountByte, sizeof(BYTE));
    Patch((PVOID) 0x4B1086, &resAmountByte, sizeof(BYTE));
    Patch((PVOID) 0x4B1CC1, &resAmountByte, sizeof(BYTE));
    Patch((PVOID) 0x4B17F0, &resAmountByte, sizeof(BYTE));
    Patch((PVOID) 0x4B07DA, &resAmountByte, sizeof(BYTE));
    Patch((PVOID) 0x4ACEF1, &resAmountByte, sizeof(BYTE));

    // Patch references to the start of the resolution array such that it points to the new one (0x8CC)
    Patch((PVOID) 0x4B0FEB, &newResStartOffset, sizeof(DWORD));
    Patch((PVOID) 0x4B17FF, &newResStartOffset, sizeof(DWORD));
    Patch((PVOID) 0x4B1C5C, &newResStartOffset, sizeof(DWORD));

    // Patch references to the first bpp in the resolution array (0x8D4)
    Patch((PVOID) 0x4B24B9, &firstBppOffset, sizeof(DWORD));
    Patch((PVOID) 0x4ACED3, &firstBppOffset, sizeof(DWORD));
    Patch((PVOID) 0x4B076A, &firstBppOffset, sizeof(DWORD));

    // Set hook that copies the resolutions into the right location when called
    SetPointer(INITIALIZE_NN_ELEMENTS_CALL_ADDR, InitializeElements_Hook);

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
    Hook(0x4AC4B0, CallSetResolution_Active, 5);
    Hook(0x4B1E65, CallSetResolution_Selected, 5);
    Hook(0x4B2594, CallSetResolution_Selected, 5);
    Hook(0x4B2781, CallSetResolution_Active, 5);

    // Places that determine the width of the "default" resolution
    Hook(0x4ACEAB, DefaultResSet1, 5, true);
    Hook(0x4ACEBB, DefaultResSet2, 7, true);

    // Change the amount of bytes that are cleaned from the stack when the function returns
    WORD paramBytes = 12; // 0xC
    Patch((PVOID) 0x4B1D09, &paramBytes, sizeof(WORD));
    Patch((PVOID) 0x4B1D14, &paramBytes, sizeof(WORD));
}
