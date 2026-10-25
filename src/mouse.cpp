#include "mouse.h"
#include "utils.h"
#include "fl_func.h"
#include "Freelancer.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define FASTCALL __fastcall

#define MOUSE_X (*(int*) 0x616840)
#define MOUSE_Y (*(int*) 0x616844)

#define WINDOW_WIDTH (*(int*) 0x679BC8)
#define WINDOW_HEIGHT (*(int*) 0x679BCC)

#define FL_RES_FLAGS (*(PDWORD) 0x679BE5)
#define FULLSCREEN_FLAG (1)

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

// Where FL last moved the Windows cursor to when handing the mouse back (see SetCursorPos_Hook).
static POINT releasedCursorPos;
static bool hasReleasedCursorPos = false;

// On Windows 11, GetCursorPos does not always see a SetCursorPos call that was made just before it,
// so the position FL just moved the cursor to is used instead when there is one.
static bool GetReleasedCursorPos(POINT& p)
{
    if (!hasReleasedCursorPos)
        return GetCursorPos(&p) != FALSE;

    p = releasedCursorPos;
    hasReleasedCursorPos = false;
    return true;
}

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
    if (GetReleasedCursorPos(p) && ScreenToClient(FL_HWND, &p))
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

#define DISCL_EXCLUSIVE    (0x01)
#define DISCL_NONEXCLUSIVE (0x02)
#define DISCL_FOREGROUND   (0x04)

// Fixes the mouse snapping to the center of the game in (borderless) windowed mode.
// Every frame FL makes sure the mouse is set to exclusive foreground mode (FUN_0041f720) before acquiring it.
// In exclusive mode DirectInput takes over the Windows cursor, and part of that is moving it to the center of the window
// when the mouse is acquired. In windowed mode FL copies the Windows cursor position into its own cursor whenever the
// window receives a mouse move message (FUN_0041e890), so if that message arrives after DirectInput moved the cursor
// (which on Windows 11 can happen), FL's cursor jumps to the center as well.
// Windowed mode uses non-exclusive mode instead, in which DirectInput leaves the Windows cursor alone.
// What exclusive mode did for FL otherwise is done by the hooks below.
// Fullscreen stays exclusive so that the Windows cursor stays locked to the game's screen.
long FASTCALL SetCooperativeLevel_Hook(IDirectInputDevice8& mouseDevice)
{
    DWORD flags = IsGameFullscreen() ? DISCL_EXCLUSIVE | DISCL_FOREGROUND : DISCL_NONEXCLUSIVE | DISCL_FOREGROUND;
    return mouseDevice.SetCooperativeLevel(FL_HWND, flags);
}

#define MOUSE_STATE_FLAGS (*(PBYTE) 0x6167CC)
#define MOUSE_ACTIVE_FLAG (0x01) // Set while FL owns the mouse, cleared when it hands it back to Windows (FUN_004201a0).
#define INPUT_ACTIVE (*(PBYTE) 0x61685C)

// In exclusive mode Windows sends no mouse messages, so FL's cursor was only ever moved by the DirectInput deltas,
// multiplied by the in-game sensitivity. In non-exclusive mode the window gets WM_MOUSEMOVE again, and FL's handler
// for it (FUN_0041e890) copies the Windows cursor position into FL's cursor every time. That throws the sensitivity away,
// and it also means the cursor no longer moves during FL's own update, which is the only place that tells the UI
// the cursor moved - so hovering and dragging lagged behind.
// The handler is still needed to take the mouse back when the cursor re-enters the window, so it is only skipped
// while FL already owns the mouse. This replaces the "mov al, [0x61685C]" at its start; returning 0 makes it return early.
//
// The hidden Windows cursor keeps generating mouse moves while FL owns the mouse, and those that are still queued when FL
// hands the mouse back would otherwise reach the handler right after, which then takes the mouse back at wherever the hidden
// cursor was (or at a position GetCursorPos has not updated yet on Windows 11): the cursor warps as soon as it touches the
// window border. So mouse moves that happened before the hand-back are skipped as well.
static DWORD releaseTime;
static bool hasReleaseTime = false;

bool MouseMove_Hook()
{
    if (!IsGameFullscreen())
    {
        if (MOUSE_STATE_FLAGS & MOUSE_ACTIVE_FLAG)
            return false;

        if (hasReleaseTime && (LONG) ((DWORD) GetMessageTime() - releaseTime) <= 0)
            return false;
    }

    return INPUT_ACTIVE != 0;
}

// Exclusive mode also kept the hidden Windows cursor inside the window. Without it the Windows cursor moves at its own
// speed while FL's cursor moves at the in-game sensitivity, so the two drift apart: the hidden cursor could leave the window
// and click on whatever is behind it while FL's cursor is still inside, and every place where FL picks up the Windows cursor
// position near the border (taking the mouse back, ShowCursor_Hook) made FL's cursor jump to wherever the hidden cursor was.
// So while FL owns the mouse, the hidden cursor is confined to the window and moved onto FL's cursor every frame.
static bool cursorConfined = false;

static void ReleaseCursorConfinement()
{
    if (!cursorConfined)
        return;

    ClipCursor(NULL);
    cursorConfined = false;
}

static bool (FASTCALL *UpdateCursorOg)(void* mouseObj);

bool FASTCALL UpdateCursor_Hook(void* mouseObj)
{
    // Always call the original because it does way more than just updating the cursor position.
    bool result = UpdateCursorOg(mouseObj);

    // FL owns the mouse again, so a position from an earlier hand-back no longer applies.
    hasReleasedCursorPos = false;

    // This is only called while the mouse is acquired, which requires the game window to be in the foreground.
    if (!IsGameFullscreen() && (MOUSE_STATE_FLAGS & MOUSE_ACTIVE_FLAG))
    {
        RECT rect;
        if (GetClientRect(FL_HWND, &rect) && ClientToScreen(FL_HWND, (LPPOINT) &rect.left) && ClientToScreen(FL_HWND, (LPPOINT) &rect.right))
            cursorConfined = ClipCursor(&rect) != FALSE;

        // DirectInput reports the mouse movement itself, not the Windows cursor, so moving the cursor does not feed back into FL's.
        // The confinement keeps it inside the window when FL's cursor has just gone past the border.
        POINT p{ MOUSE_X, MOUSE_Y };
        if (ClientToScreen(FL_HWND, &p))
            SetCursorPos(p.x, p.y);
    }

    return result;
}

// FL calls SetCursorPos only when it hands the mouse back to Windows: it moves the Windows cursor to where its own cursor is.
// The confinement has to be lifted first, otherwise the Windows cursor cannot go there when FL's cursor has left the window.
BOOL STDCALL SetCursorPos_Hook(int x, int y)
{
    ReleaseCursorConfinement();

    BOOL result = SetCursorPos(x, y);

    releaseTime = GetTickCount();
    hasReleaseTime = true;

    // Windows keeps the cursor on a monitor, so that is where it ends up if FL's cursor is beyond the screen edge.
    POINT p{ x, y };
    MONITORINFO info;
    info.cbSize = sizeof(info);
    if (result && GetMonitorInfo(MonitorFromPoint(p, MONITOR_DEFAULTTONEAREST), &info))
    {
        p.x = max(info.rcMonitor.left, min(p.x, info.rcMonitor.right - 1));
        p.y = max(info.rcMonitor.top, min(p.y, info.rcMonitor.bottom - 1));
        releasedCursorPos = p;
        hasReleasedCursorPos = true;
    }

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
    #define THIS_PTR_SET_COOPERATIVE_LEVEL_ADDR 0x41F77E
    #define SET_COOPERATIVE_LEVEL_ADDR 0x41F780

    // mov edx, [eax]; push 5; push ecx; push eax; call [edx+0x34] -> call SetCooperativeLevel_Hook
    Patch<BYTE>(THIS_PTR_SET_COOPERATIVE_LEVEL_ADDR, 0x4E); // mov eax, [esi+0x10] -> mov ecx, [esi+0x10]
    Hook(SET_COOPERATIVE_LEVEL_ADDR, SetCooperativeLevel_Hook, 9);

    // Call to the function that integrates the DirectInput mouse deltas into FL's cursor position.
    #define UPDATE_CURSOR_CALL_ADDR 0x41F267
    UpdateCursorOg = SetRelPointer(UPDATE_CURSOR_CALL_ADDR + 1, UpdateCursor_Hook);

    #define MOUSE_MOVE_HANDLER_ADDR 0x41E890
    Hook(MOUSE_MOVE_HANDLER_ADDR, MouseMove_Hook, 5);

    // The calls to SetCursorPos when focus is lost (FUN_0041ecf0), when the mouse leaves the window (FUN_004201a0),
    // and when the mouse is disabled (FUN_00420350).
    #define SET_CURSOR_POS_FOCUS_LOST_ADDR 0x41EEF8
    #define SET_CURSOR_POS_RELEASE_ADDR 0x42032F
    #define SET_CURSOR_POS_DISABLE_ADDR 0x4204AD
    Hook(SET_CURSOR_POS_FOCUS_LOST_ADDR, SetCursorPos_Hook, 6);
    Hook(SET_CURSOR_POS_RELEASE_ADDR, SetCursorPos_Hook, 6);
    Hook(SET_CURSOR_POS_DISABLE_ADDR, SetCursorPos_Hook, 6);
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
