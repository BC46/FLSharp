#include "mouse.h"
#include "utils.h"
#include "fl_func.h"
#include "Freelancer.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cmath>

#define FASTCALL __fastcall

#define MOUSE_X (*(int*) 0x616840)
#define MOUSE_Y (*(int*) 0x616844)

#define WINDOW_WIDTH (*(int*) 0x679BC8)
#define WINDOW_HEIGHT (*(int*) 0x679BCC)

#define FL_RES_FLAGS (*(PDWORD) 0x679BE5)
#define FULLSCREEN_FLAG (1)

// Set to 1 to record the latest mouse events in memory and write them to FLSharp_mouse_warp.log (in the game's
// working directory) whenever FL's cursor suddenly jumps. This is meant to find rare cursor warps.
// Nothing is written to disk during normal play.
#define MOUSE_WARP_CAPTURE 1

#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <intrin.h>
#pragma intrinsic(_ReturnAddress)

// On Windows 11, SetCursorPos doesn't take effect right away for GetCursorPos, so moving the cursor back after
// DirectInput has warped it to the center of the window while acquiring the mouse sometimes loses the race,
// which makes FL's cursor jump. Therefore DirectInput's SetCursorPos calls are ignored while the mouse is acquired
// in windowed mode (see Acquire_Hook). This works by redirecting the import of SetCursorPos in dinput8.dll.
static bool warpSuppressCursorWarps = false;
static bool warpDinputPosHooked = false;

#define WARP_RECORD_COUNT 512
#define WARP_RECORD_LENGTH 200
#define WARP_JUMP_DISTANCE 250 // In pixels, per frame.
#define WARP_DUMP_INTERVAL_MS 3000
#define WARP_LOG_MAX_BYTES (2 * 1024 * 1024)

static CRITICAL_SECTION warpLock;
static bool warpLockReady = false;
static struct { DWORD tick; char text[WARP_RECORD_LENGTH]; } warpRecords[WARP_RECORD_COUNT];
static unsigned warpRecordNext = 0;
static DWORD warpLastDump = 0;
static bool warpHasLastDump = false;

// The Windows cursor position in client coordinates.
static POINT WarpPointer()
{
    POINT p{ -99999, -99999 };
    if (GetCursorPos(&p))
        ScreenToClient(FL_HWND, &p);
    return p;
}

static void WarpRecord(const char* format, ...)
{
    if (!warpLockReady)
        return;

    char text[WARP_RECORD_LENGTH];
    va_list args;
    va_start(args, format);
    vsnprintf(text, sizeof(text), format, args);
    va_end(args);
    text[sizeof(text) - 1] = '\0';

    EnterCriticalSection(&warpLock);
    unsigned index = warpRecordNext++ % WARP_RECORD_COUNT;
    warpRecords[index].tick = GetTickCount();
    strcpy_s(warpRecords[index].text, text);
    LeaveCriticalSection(&warpLock);
}

static void WarpDump(const char* reason)
{
    if (!warpLockReady)
        return;

    DWORD now = GetTickCount();
    if (warpHasLastDump && now - warpLastDump < WARP_DUMP_INTERVAL_MS)
        return;
    warpHasLastDump = true;
    warpLastDump = now;

    FILE* file = nullptr;
    if (fopen_s(&file, "FLSharp_mouse_warp.log", "a") != 0 || !file)
        return;

    fseek(file, 0, SEEK_END);
    if (ftell(file) > WARP_LOG_MAX_BYTES)
    {
        fclose(file);
        return;
    }

    EnterCriticalSection(&warpLock);
    fprintf(file, "===== %s (tick %lu) =====\n", reason, now);

    unsigned count = warpRecordNext < WARP_RECORD_COUNT ? warpRecordNext : WARP_RECORD_COUNT;
    unsigned first = warpRecordNext - count;
    for (unsigned i = 0; i < count; ++i)
    {
        auto& record = warpRecords[(first + i) % WARP_RECORD_COUNT];
        fprintf(file, "[%10lu] %s\n", record.tick, record.text);
    }
    LeaveCriticalSection(&warpLock);

    fputc('\n', file);
    fclose(file);
}

// Describes which module and offset a return address belongs to, e.g. "dinput8.dll+0x13f45".
static void WarpDescribeCaller(void* address, char* out, size_t size)
{
    HMODULE module = nullptr;
    char path[MAX_PATH] = "?";
    if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR) address, &module))
        GetModuleFileNameA(module, path, MAX_PATH);

    const char* name = strrchr(path, '\\');
    sprintf_s(out, size, "%s+0x%X", name ? name + 1 : path, (unsigned) ((PBYTE) address - (PBYTE) module));
}

static void* WarpHookImport(HMODULE module, const char* functionName, void* hook)
{
    PBYTE base = (PBYTE) module;
    PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS) (base + ((PIMAGE_DOS_HEADER) base)->e_lfanew);
    IMAGE_DATA_DIRECTORY dir = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    if (!dir.VirtualAddress)
        return nullptr;

    for (PIMAGE_IMPORT_DESCRIPTOR desc = (PIMAGE_IMPORT_DESCRIPTOR) (base + dir.VirtualAddress); desc->Name; ++desc)
    {
        PIMAGE_THUNK_DATA thunk = (PIMAGE_THUNK_DATA) (base + desc->FirstThunk);
        PIMAGE_THUNK_DATA names = (PIMAGE_THUNK_DATA) (base + (desc->OriginalFirstThunk ? desc->OriginalFirstThunk : desc->FirstThunk));

        for (; names->u1.AddressOfData; ++names, ++thunk)
        {
            if (IMAGE_SNAP_BY_ORDINAL(names->u1.Ordinal))
                continue;

            PIMAGE_IMPORT_BY_NAME byName = (PIMAGE_IMPORT_BY_NAME) (base + names->u1.AddressOfData);
            if (strcmp((const char*) byName->Name, functionName) != 0)
                continue;

            // Already hooked, so the current function is our own hook and not the original.
            if (thunk->u1.Function == (DWORD) hook)
                return nullptr;

            DWORD oldProtect;
            VirtualProtect(&thunk->u1.Function, sizeof(void*), PAGE_READWRITE, &oldProtect);
            void* original = (void*) thunk->u1.Function;
            thunk->u1.Function = (DWORD) hook;
            VirtualProtect(&thunk->u1.Function, sizeof(void*), oldProtect, &oldProtect);
            return original;
        }
    }

    return nullptr;
}

static BOOL (WINAPI *WarpSetCursorPosOg)(int, int);
static BOOL (WINAPI *WarpClipCursorOg)(const RECT*);
static void (WINAPI *WarpMouseEventOg)(DWORD, DWORD, DWORD, DWORD, ULONG_PTR);

static BOOL WINAPI WarpSetCursorPos_Hook(int x, int y)
{
#if MOUSE_WARP_CAPTURE
    char caller[MAX_PATH + 32];
    WarpDescribeCaller(_ReturnAddress(), caller, sizeof(caller));

    POINT client{ x, y };
    ScreenToClient(FL_HWND, &client);
    POINT before = WarpPointer();
    WarpRecord("SetCursorPos(client %d,%d) from %s; pointer was (%d,%d); FL cursor=(%d,%d)%s", client.x, client.y, caller, before.x, before.y,
        MOUSE_X, MOUSE_Y, warpSuppressCursorWarps ? " [ignored]" : "");
#endif

    if (warpSuppressCursorWarps)
        return TRUE;

    return WarpSetCursorPosOg(x, y);
}

static BOOL WINAPI WarpClipCursor_Hook(const RECT* rect)
{
    char caller[MAX_PATH + 32];
    WarpDescribeCaller(_ReturnAddress(), caller, sizeof(caller));

    if (rect)
        WarpRecord("ClipCursor(%d,%d,%d,%d) from %s", rect->left, rect->top, rect->right, rect->bottom, caller);
    else
        WarpRecord("ClipCursor(NULL) from %s", caller);

    return WarpClipCursorOg(rect);
}

static void WINAPI WarpMouseEvent_Hook(DWORD flags, DWORD dx, DWORD dy, DWORD data, ULONG_PTR extra)
{
    char caller[MAX_PATH + 32];
    WarpDescribeCaller(_ReturnAddress(), caller, sizeof(caller));
    WarpRecord("mouse_event(flags=%lx, %ld, %ld) from %s", flags, (long) dx, (long) dy, caller);
    WarpMouseEventOg(flags, dx, dy, data, extra);
}

static void WarpInstallHooks(HMODULE module, bool isDinput)
{
    static HMODULE hooked[8];
    static int hookedCount = 0;

    if (!module)
        return;

    for (int i = 0; i < hookedCount; ++i)
        if (hooked[i] == module)
            return;

    if (hookedCount < 8)
        hooked[hookedCount++] = module;

    void* og;
    if ((og = WarpHookImport(module, "SetCursorPos", WarpSetCursorPos_Hook)) != nullptr)
    {
        WarpSetCursorPosOg = (decltype(WarpSetCursorPosOg)) og;
        if (isDinput)
            warpDinputPosHooked = true;
    }
#if MOUSE_WARP_CAPTURE
    if ((og = WarpHookImport(module, "ClipCursor", WarpClipCursor_Hook)) != nullptr) WarpClipCursorOg = (decltype(WarpClipCursorOg)) og;
    if ((og = WarpHookImport(module, "mouse_event", WarpMouseEvent_Hook)) != nullptr) WarpMouseEventOg = (decltype(WarpMouseEventOg)) og;
#endif
}

// DirectInput is loaded some time after the game starts, so it is hooked when the first Acquire call happens.
static void WarpInstallDinputHooks()
{
    static bool installed = false;
    if (installed)
        return;
    installed = true;

    WarpInstallHooks(GetModuleHandleA("dinput8.dll"), true);

    char path[MAX_PATH];
    GetSystemDirectoryA(path, MAX_PATH);
    strcat_s(path, "\\dinput8.dll");
    WarpInstallHooks(GetModuleHandleA(path), true); // In case dinput8.dll in the game folder is a proxy
}

static void WarpInit()
{
#if MOUSE_WARP_CAPTURE
    InitializeCriticalSection(&warpLock);
    warpLockReady = true;
    WarpInstallHooks(GetModuleHandleA(nullptr), false);
#endif
}

#if MOUSE_WARP_CAPTURE
static int warpPrevX = 0, warpPrevY = 0;
static bool warpPrevValid = false;

static void WarpOnAcquire(long result, POINT before)
{
    POINT after = WarpPointer();
    WarpRecord("Acquire result=%lx pointer before=(%d,%d) after=(%d,%d); FL cursor=(%d,%d)", result, before.x, before.y, after.x, after.y, MOUSE_X, MOUSE_Y);
    warpPrevValid = false; // FL's cursor legitimately changes while the mouse is not acquired
}

static int WarpDistance(int x1, int y1, int x2, int y2)
{
    int dx = x1 - x2, dy = y1 - y2;
    return (int) sqrt((double) dx * dx + (double) dy * dy);
}

// Called with FL's cursor position right before and right after the cursor update of a frame.
static void WarpOnUpdate(int entryX, int entryY)
{
    POINT pointer = WarpPointer();
    WarpRecord("frame: FL cursor (%d,%d) -> (%d,%d); pointer=(%d,%d); flags=%02x", entryX, entryY, MOUSE_X, MOUSE_Y, pointer.x, pointer.y, *(PBYTE) 0x6167CC);

    if (warpPrevValid && WarpDistance(entryX, entryY, warpPrevX, warpPrevY) > WARP_JUMP_DISTANCE)
    {
        WarpRecord("JUMP between frames: FL cursor (%d,%d) -> (%d,%d)", warpPrevX, warpPrevY, entryX, entryY);
        WarpDump("FL cursor changed between frames");
    }
    else if (WarpDistance(entryX, entryY, MOUSE_X, MOUSE_Y) > WARP_JUMP_DISTANCE)
    {
        WarpRecord("JUMP during update: FL cursor (%d,%d) -> (%d,%d)", entryX, entryY, MOUSE_X, MOUSE_Y);
        WarpDump("FL cursor jumped during the update");
    }

    warpPrevX = MOUSE_X;
    warpPrevY = MOUSE_Y;
    warpPrevValid = true;
}
#define WARP_ON_ACQUIRE(result, before) WarpOnAcquire(result, before)
#define WARP_ON_UPDATE(x, y) WarpOnUpdate(x, y)
#else
#define WARP_ON_ACQUIRE(result, before) ((void) 0)
#define WARP_ON_UPDATE(x, y) ((void) 0)
#endif

// Hook that prevents the in-game cursor from being shown when it is outside the game window.
bool ShowMouseCursor_Hook()
{
    if (MOUSE_X < 0 || MOUSE_Y < 0)
        return false;

    if (MOUSE_X >= WINDOW_WIDTH || MOUSE_Y >= WINDOW_HEIGHT)
        return false;

    return SHOW_MOUSE_CURSOR;
}

static bool IsGameFullscreen()
{
    return FL_RES_FLAGS & FULLSCREEN_FLAG;
}

static void ForceShowWindowsCursor()
{
    // Same code as FL.
    while (ShowCursor(TRUE) < 1);
}

#define MOUSE_DEVICE (*(IDirectInputDevice8**) 0x6167C8)

// Code that FL calls to force show the Windows cursor when it is on the edge of the game window.
// FL doesn't properly track mouse when you move the cursor outside the screen.
// TODO: Currently in windowed mode, if your mouse is on the bottom edge of the window,
// both the in-game and Windows cursor become invisible.
// I couldn't get around it because DirectInput threw a wrench.
// Every time I wanted to show the Windows cursor right on the bottom edge, it would either start flickering or warp to the center.
// I spent way too much time on trying to get it to work so I gave up and just let it be invisible for now.
void STDCALL ShowCursor_Hook()
{
    // if (IsGameFullscreen())
    // {
    //     // Normal routine, although that'll probably never happen because this code is never called in fullscreen mode AFAIK...
    //     ForceShowWindowsCursor();
    //     return;
    // }

    // Get the actual cursor position.
    POINT p;
    if (GetCursorPos(&p) && ScreenToClient(FL_HWND, &p))
    {
        // TODO: Well, here's an annoying edge case that I can't fix.
        // If you have the game running in borderless windowed mode on a large primary monitor,
        // and you move your mouse in the top-left corner towards a smaller secondary monitor on the left,
        // the mouse cursor is supposed to "teleport" to the top right-corner of the second monitor without issues.
        // This seems to happen fine if you try it for the first time. However, if you give focus to another window on the second monitor,
        // and then let the FL window regain focus, the mouse cursor will now get stuck if you retry the above steps.
        // If you place the ForceShowWindowsCursor call at the very start of the function without the if-statements, this issue does not occur.
        // However, this results in other things being broken, for instance the flickering of the cursor when the mouse is near the edges.
        // The exact purpose of this hook is to fix this flickering bug in particular.
        if (p.x < 0 || p.y < 0 || p.x >= WINDOW_WIDTH || p.y > WINDOW_HEIGHT)
        {
            ForceShowWindowsCursor();
        }

        // Prevent the in-game mouse cursor from going outside the screen.
        MOUSE_X = p.x;
        MOUSE_Y = p.y;
    }
}

// Fixes the mouse snapping to the center of the game in (borderless) windowed mode.
long FASTCALL Acquire_Hook(IDirectInputDevice8& mouseDevice)
{
    // Acquire can sometimes warp the mouse to the center of the game window in (borderless) windowed mode.
    // To fix that, we set the cursor position to the original value after the Acquire call.
    // The original value is the actual Windows cursor position right before Acquire,
    // not FL's own cursor position, because the two can differ (see UpdateCursor_Hook).
    POINT original;
    bool hasOriginal = GetCursorPos(&original);

    WarpInstallDinputHooks();

#if MOUSE_WARP_CAPTURE
    POINT originalClient = WarpPointer();
#endif

    // Prevent DirectInput from warping the cursor in the first place, if that is possible.
    bool suppressWarps = warpDinputPosHooked && !IsGameFullscreen();
    warpSuppressCursorWarps = suppressWarps;
    long result = mouseDevice.Acquire();
    warpSuppressCursorWarps = false;

#if MOUSE_WARP_CAPTURE
    WARP_ON_ACQUIRE(result, originalClient);
#endif

    // Otherwise move the cursor back afterwards, which usually works.
    if (result == S_OK && hasOriginal && !suppressWarps && !IsGameFullscreen())
        SetCursorPos(original.x, original.y);

    return result;
}

// FL moves its own cursor by integrating the DirectInput mouse deltas multiplied by the in-game sensitivity,
// while the Windows cursor moves with the Windows pointer speed. In windowed mode the two cursors therefore
// drift apart. When FL's cursor reaches the window border, FL hands control back to Windows and our ShowCursor_Hook
// copies the Windows cursor position into FL's cursor, which is perceived as the cursor suddenly snapping.
// To prevent this drift, FL's cursor follows the Windows cursor every frame while the mouse is acquired.
#define MOUSE_STATE_FLAGS (*(PBYTE) 0x6167CC)
#define CLAMP_CURSOR_FLAG (0x10) // When set, FL clamps its cursor to the screen instead of leaving the window (e.g. in flight).

static bool (FASTCALL *UpdateCursorOg)(void* mouseObj);

static int FollowPointerAxis(int flCoord, int pointerCoord, int size)
{
    // The Windows cursor is outside of the window, which is the case when the mouse is not confined to it.
    if (pointerCoord < 0 || pointerCoord >= size)
        return pointerCoord;

    // The Windows cursor is stopped at the border, but FL's cursor is trying to move beyond it.
    // Keep FL's coordinate so that FL still notices that the border has been reached.
    if (flCoord < 0 && pointerCoord <= 0)
        return flCoord;

    if (flCoord >= size && pointerCoord >= size - 1)
        return flCoord;

    return pointerCoord;
}

bool FASTCALL UpdateCursor_Hook(void* mouseObj)
{
    int entryX = MOUSE_X;
    int entryY = MOUSE_Y;

    // Always call the original because it does way more than just updating the cursor position.
    bool result = UpdateCursorOg(mouseObj);

    if (!IsGameFullscreen() && !(MOUSE_STATE_FLAGS & CLAMP_CURSOR_FLAG))
    {
        POINT p;
        if (GetCursorPos(&p) && ScreenToClient(FL_HWND, &p))
        {
            MOUSE_X = FollowPointerAxis(MOUSE_X, p.x, WINDOW_WIDTH);
            MOUSE_Y = FollowPointerAxis(MOUSE_Y, p.y, WINDOW_HEIGHT);
        }
    }

    WARP_ON_UPDATE(entryX, entryY);

    return result;
}

// Fixes the in-game mouse cursor remaining visible in windowed mode
// despite the actual cursor being outside the game window.
void InitCursorFix()
{
    #define SHOW_MOUSE_CURSOR_CHECK_ADDR 0x41F30A
    Hook(SHOW_MOUSE_CURSOR_CHECK_ADDR, ShowMouseCursor_Hook, 5);

    #define SHOW_WIN_CURSOR_ADDR 0x420335
    Hook(SHOW_WIN_CURSOR_ADDR, ShowCursor_Hook, 19);

    // Freelancer appears to have a pretty hard time figuring out
    // whether the cursor should be shown when it is at the bottom-edge of the window.
    // It seems it's putting minimal effort into clamping the cursor's y-position near the bottom-edge.
    // This patch prevents that from happening.
    #define CURSOR_BOTTOM_BORDER_CHECK_ADDR 0x41EA9B
    Patch<BYTE>(CURSOR_BOTTOM_BORDER_CHECK_ADDR, 0xEB);
    // TODO: Do the same for 0x41ECBF? Doesn't look necessary.
}

void InitMouseWarpFix()
{
    #define THIS_PTR_ACQUIRE_MOUSE_ADDR 0x41F7D1
    #define ACQUIRE_MOUSE_ADDR 0x41F7D3

    // TODO: "Unacquire" sets cursor pos as well
    Patch<BYTE>(THIS_PTR_ACQUIRE_MOUSE_ADDR, 0x4E); // mov eax, [esi+0x10] -> mov ecx, [esi+0x10]
    Hook(ACQUIRE_MOUSE_ADDR, Acquire_Hook, 6);

    // Call to the function that integrates the DirectInput mouse deltas into FL's cursor position.
    #define UPDATE_CURSOR_CALL_ADDR 0x41F267
    UpdateCursorOg = SetRelPointer(UPDATE_CURSOR_CALL_ADDR + 1, UpdateCursor_Hook);

#if MOUSE_WARP_CAPTURE
    WarpInit();
#endif
}

void (*DestroyStartupScreenOg)();

#define STARTUP_SCREEN_OBJ (*(PVOID*) 0x67C3BC)

// Fixes the mouse cursor being initially positioned in the top-left corner in fullscreen mode.
// With this fix, the initial position of the mouse cursor will be in the center of the screen, like most fullscreen games.
void DestroyStartupScreen_Hook()
{
    // This function gets called every time before the main menu is opened.
    // We want our code to only be called before the very first menu visit
    // When the game starts, it creates a startup screen object and destroys it after it is shown.
    // Therefore, this object will no longer exist in subsequent menu visits.
    // We can check if it is the first menu visit by testing if the startup screen object exists.
    if (STARTUP_SCREEN_OBJ && IsGameFullscreen())
    {
        POINT p;
        if (GetCursorPos(&p) && ScreenToClient(FL_HWND, &p))
        {
            MOUSE_X = p.x;
            MOUSE_Y = p.y;
        }
    }

    DestroyStartupScreenOg();
}

void InitMenuMousePosFix()
{
    #define DESTROY_STARTUP_SCREEN_CALL_ADDR (0x57432A)
    DestroyStartupScreenOg = SetRelPointer(DESTROY_STARTUP_SCREEN_CALL_ADDR + 1, DestroyStartupScreen_Hook);
}
