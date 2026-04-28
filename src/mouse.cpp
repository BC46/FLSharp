#include "mouse.h"
#include "utils.h"
#include "fl_func.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define STDCALL __stdcall

#define MOUSE_X (*(int*) 0x616840)
#define MOUSE_Y (*(int*) 0x616844)

#define WINDOW_WIDTH (*(int*) 0x679BC8)
#define WINDOW_HEIGHT (*(int*) 0x679BCC)

#define SHOW_MOUSE_CURSOR (*(bool*) 0x6107DC)

#define FL_HWND (*(HWND*) 0x67ECA0)

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

bool IsGameFullscreen()
{
    return FL_RES_FLAGS & FULLSCREEN_FLAG;
}

void ForceShowWindowsCursor()
{
    // Same code as FL.
    while (ShowCursor(TRUE) < 1);
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
    if (GetCursorPos(&p) && ScreenToClient(FL_HWND, &p))
    {
        if (p.x < 0 || p.y < 0 || p.x >= WINDOW_WIDTH || p.y > WINDOW_HEIGHT)
        {
            ForceShowWindowsCursor();
        }

        // Prevent the in-game mouse cursor from going outside the screen.
        MOUSE_X = p.x;
        MOUSE_Y = p.y;
    }
}

// Fixes the in-game mouse cursor remaining visible in windowed mode
// despite the actual cursor being outside the game window.
void InitMouseFix()
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
}
