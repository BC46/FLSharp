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
    long result = mouseDevice.Acquire();

    if (result == S_OK && !IsGameFullscreen())
    {
        POINT p{ MOUSE_X, MOUSE_Y };

        // TODO: Get the original cursor position by calling GetCursorPos before Acquire instead?
        if (ClientToScreen(FL_HWND, &p))
            SetCursorPos(p.x, p.y);
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
    #define THIS_PTR_ACQUIRE_MOUSE_ADDR 0x41F7D1
    #define ACQUIRE_MOUSE_ADDR 0x41F7D3

    // TODO: "Unacquire" sets cursor pos as well
    Patch<BYTE>(THIS_PTR_ACQUIRE_MOUSE_ADDR, 0x4E); // mov eax, [esi+0x10] -> mov ecx, [esi+0x10]
    Hook(ACQUIRE_MOUSE_ADDR, Acquire_Hook, 6);
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
