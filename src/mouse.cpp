#include "mouse.h"
#include "utils.h"

#define MOUSE_X (*(int*) 0x616840)
#define MOUSE_Y (*(int*) 0x616844)

#define WINDOW_WIDTH (*(int*) 0x679BC8)
#define WINDOW_HEIGHT (*(int*) 0x679BCC)

#define SHOW_MOUSE_CURSOR (*(bool*) 0x6107DC)

// Hook that prevents the mouse cursor from being shown when it is outside the game window.
bool ShowMouseCursor_Hook()
{
    if (MOUSE_X < 0 || MOUSE_Y < 0)
        return false;

    // Freelancer seems to have a pretty hard time figuring out
    // whether the cursor should be shown when it is at the bottom-edge of the window.
    // Whatever, I'll just blame it on DirectInput.
    // It seems it's putting minimal effort into clamping the cursor's y-position near the bottom-edge.
    if (MOUSE_X >= WINDOW_WIDTH || MOUSE_Y >= WINDOW_HEIGHT)
        return false;

    return SHOW_MOUSE_CURSOR;
}

// Fixes the in-game mouse cursor remaining visible in windowed mode
// despite the actual cursor being outside the game window.
void InitMouseFix()
{
    #define SHOW_MOUSE_CURSOR_CHECK_ADDR 0x41F30A

    Hook(SHOW_MOUSE_CURSOR_CHECK_ADDR, ShowMouseCursor_Hook, 5);
}
