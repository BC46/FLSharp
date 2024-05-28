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

void AddDcResolutions()
{
    HDC hdc = GetDC(NULL);

    if (!hdc)
        return;

    int horzRes = GetDeviceCaps(hdc, HORZRES);
    int vertRes = GetDeviceCaps(hdc, VERTRES);

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

    // We know resolutions.size() <= 256, so casting it directly to a byte is fine
    BYTE resAmount = resolutions.size();

    Patch((PVOID) 0x4B2521, &resAmount, sizeof(BYTE));
    Patch((PVOID) 0x4B1086, &resAmount, sizeof(BYTE));
    Patch((PVOID) 0x4B1CC1, &resAmount, sizeof(BYTE));
    Patch((PVOID) 0x4B17F0, &resAmount, sizeof(BYTE));
    Patch((PVOID) 0x4B07DA, &resAmount, sizeof(BYTE));
    Patch((PVOID) 0x4ACEF1, &resAmount, sizeof(BYTE));

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
        mov eax, [esp+0x8]
        mov [edi+SELECTED_HEIGHT_OF], eax
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

typedef bool __fastcall SetResolution(PVOID thisptr, PVOID _edx, DWORD width, DWORD unk, DWORD height);

bool __fastcall CallSetResolution(PVOID thisptr, PVOID _edx, DWORD height, DWORD width, DWORD unk)
{
    return ((SetResolution*) 0x4B1C00)(thisptr, _edx, width, unk, height);
}

__declspec(naked) void PostSetRes1()
{
    __asm {
        mov eax, [ebp+ACTIVE_HEIGHT_OF]
        push eax
        call CallSetResolution
        push 0x4AC4B5
        ret
    }
}

__declspec(naked) void PostSetRes2()
{
    __asm {
        mov eax, [edi+SELECTED_HEIGHT_OF]
        push eax
        call CallSetResolution
        push 0x4B1E6A
        ret
    }
}

__declspec(naked) void PostSetRes3()
{
    __asm {
        mov eax, [esi+SELECTED_HEIGHT_OF]
        push eax
        call CallSetResolution
        push 0x4B2599
        ret
    }
}

__declspec(naked) void PostSetRes4()
{
    __asm {
        mov eax, [esi+ACTIVE_HEIGHT_OF]
        push eax
        call CallSetResolution
        push 0x4B2786
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

    // TODO: Test 256+ resolutions
    // Discard lowest resolutions if there's more than 256
    while (resolutions.size() > 256)
    {
        resolutions.erase(it++);
    }

    int resolutionAmount = resolutions.size();

    int additionalSize = NN_PREFERENCES_ALLOC_SIZE
        + resolutionAmount * sizeof(ResolutionInfo) // resolution info
        + resolutionAmount // supported array
        + resolutionAmount * sizeof(int) // indices in menu
        + sizeof(UINT) * 2; // active and selected height

    Patch((PVOID) NN_PREFERENCES_ALLOC_SIZE_PTR, &additionalSize, sizeof(additionalSize));

    // These offsets are always the same so we can just set them once on startup
    DWORD newResStartOffset = NN_PREFERENCES_NEW_DATA;
    DWORD firstBppOffset = NN_PREFERENCES_NEW_DATA + 0x8;

    // 0x8CC
    Patch((PVOID) 0x4B0FEB, &newResStartOffset, sizeof(DWORD));
    Patch((PVOID) 0x4B17FF, &newResStartOffset, sizeof(DWORD));
    Patch((PVOID) 0x4B1C5C, &newResStartOffset, sizeof(DWORD));

    // 0x8D4
    Patch((PVOID) 0x4B24B9, &firstBppOffset, sizeof(DWORD));
    Patch((PVOID) 0x4ACED3, &firstBppOffset, sizeof(DWORD));
    Patch((PVOID) 0x4B076A, &firstBppOffset, sizeof(DWORD));

    SetPointer(INITIALIZE_NN_ELEMENTS_CALL_ADDR, InitializeElements_Hook);

    Hook(0x4A9AAB, (DWORD) CurrentResInfoWrite1, 6);
    Hook(0x4B1046, (DWORD) CurrentResInfoWrite2, 6);
    Hook(0x4B180F, (DWORD) CurrentResInfoWrite3, 6);
    Hook(0x4B1C20, (DWORD) CurrentResInfoWrite4, 6);
    Hook(0x4AC264, (DWORD) CurrentResInfoWrite5, 6);
    Hook(0x4B27A6, (DWORD) CurrentResInfoWrite6, 6);
    Hook(0x4B10C3, (DWORD) CurrentResInfoWrite7, 6);

    Hook(0x4B1F67, (DWORD) CurrentResInfoCheck1, 6, true);
    Hook(0x4B102B, (DWORD) CurrentResInfoCheck2, 5, true);
    Hook(0x4B257A, (DWORD) CurrentResInfoCheck3, 6, true);
    Hook(0x4B1C93, (DWORD) CurrentResInfoCheck4, 8, true);
    Hook(0x4B074E, (DWORD) CurrentResInfoCheck5, 6, true);
    Hook(0x4B0786, (DWORD) CurrentResInfoCheck6, 7, true);

    Hook(0x4AC4B0, (DWORD) PostSetRes1, 5, true);
    Hook(0x4B1E65, (DWORD) PostSetRes2, 5, true);
    Hook(0x4B2594, (DWORD) PostSetRes3, 5, true);
    Hook(0x4B2781, (DWORD) PostSetRes4, 5, true);

    WORD paramBytes = 12;
    Patch((PVOID) 0x4B1D09, &paramBytes, sizeof(WORD));
    Patch((PVOID) 0x4B1D14, &paramBytes, sizeof(WORD));
}
