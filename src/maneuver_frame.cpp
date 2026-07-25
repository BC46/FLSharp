#include "maneuver_frame.h"
#include "fl_func.h"
#include "utils.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// This function should work for all UI elements in FL.
//FL_FUNC(void HUD_Maneuvers::SetVisibility(bool hidden), 0x5A0C30)
void (HUD_Maneuvers::*SetVisibility_Original)(bool hidden);

FL_FUNC(void Maneuvers::Update(), 0x4D88F0)

Maneuvers* HUD_Maneuvers::GetManeuvers() const
{
    return (Maneuvers*) (((PBYTE) this) - 0x38);
}

// Called when Freelancer wants to show the maneuver frame right after the undocking sequence.
// Here we want to initialize the maneuver frame to fix the flickering.
void HUD_Maneuvers::SetVisibility_Hook(bool hidden)
{
    // Call the original function.
    (this->*SetVisibility_Original)(hidden);

    // Properly initialize the entire maneuver frame.
    GetManeuvers()->Update();
}

// If you launch to space from a base or planet, as soon as the maneuver frame shows up after the undocking sequence,
// the maneuver frame including all of its icons appear to flicker.
// This happens because the maneuver frame is first updated a few frames after it is visible.
// Hence during the first frames it is shown without being initialized, at which point everything looks off.
// The fix is to simply call the maneuver frame's update function right when it is made visible.
// This way the maneuver frame is instantly initialized.
void InitManeuverFrameFix()
{
    #define SHOW_MANEUVER_FRAME_CALL_ADDR (0x4D8592)
    SetVisibility_Original = SetRelPointer(SHOW_MANEUVER_FRAME_CALL_ADDR + 1, &HUD_Maneuvers::SetVisibility_Hook);
}
