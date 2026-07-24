#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cmath>

#include "camera.h"
#include "utils.h"
#include "fl_func.h"

#define FASTCALL __fastcall

Vector offsetCorrection = { 0.0f, 0.0f, 0.0f };
bool correctOffset = false;

bool (ThirdPersonCamera::*Update_Original)(const Transform& transform, float deltaTime, bool resetValues);

bool ThirdPersonCamera::Update_Hook(const Transform& transform, float deltaTime, bool resetValues)
{
    Vector relVelocity = { 0.0f, 0.0f, 0.0f };

    // Get the relative velocity of the camera's target (should be the player ship).
    if (const IObjRW* targetIObjRW = GetTarget())
    {
        if (const CObject* targetObj = targetIObjRW->cobject)
        {
            Vector velocity = targetObj->get_velocity();
            Matrix orient = targetObj->get_orientation().Transpose();
            relVelocity = orient * velocity;
        }
    }

    // If the player leaves the undocking cutscene, there is no initial stutter, so do not correct the offset in this case.
    if (!IsPlayerInCutscene())
    {
        // The provided deltaTime parameter is 0 because this is the "init" function, so we obtain the delta time manually.
        float dt = (float) GetDeltaTime();

        // The velocity fix makes things worse if the frame rate is very low.
        // In general, the higher the framerate, the better the fix will work, but anything over 20 FPS should suffice.
        // At 10 FPS, there are still very minor stutters with the fix, but at this frame rate it still looks better than without the fix.
        if (dt <= 0.125f)
        {
            offsetCorrection.x = dt * relVelocity.x;
            offsetCorrection.y = dt * relVelocity.y;
            offsetCorrection.z = dt * relVelocity.z;
            correctOffset = true;
        }
    }

    // Call the original function.
    bool result = (this->*Update_Original)(transform, deltaTime, resetValues);

    // Ensures the offset is only corrected as part of the camera "Init" function.
    correctOffset = false;

    return result;
}

// This is a "VectorAdd" function which calculates the relative camera offset.
// Here we want to correct the camera offset when the camera has been switched to third person mode.
void GetRelativeCameraOffset(Vector& result, const Vector& cameraOffset, Vector& velocityOffset)
{
    // This function gets called on every camera update.
    // However, we only want to correct the offset right when the camera switched (first frame).
    if (correctOffset)
    {
        // Correct the velocity offset such that the camera does not stutter in the first frame.
        velocityOffset.x += offsetCorrection.x;
        velocityOffset.y += offsetCorrection.y;
        velocityOffset.z += offsetCorrection.z;
    }

    result.x = cameraOffset.x + velocityOffset.x;
    result.y = cameraOffset.y + velocityOffset.y;
    result.z = cameraOffset.z + velocityOffset.z;
}

// There is a bug where if you switch from third-person mode to turret mode and back to third-person mode,
// the camera stutters a bit. The ship appears to move forward for one frame, and then go back to the correct position.
// The fix is to correct the initial camera position by taking into account the ship's velocity.
// That is what this code aims to do.
// The adjusted offsets are only relevant for the first frame that the third-person mode is active because that's when the initialization happens.
void InitCameraSwitchFix()
{
    #define THIRD_PERSON_CAMERA_INIT_CALL_ADDR (0x518DBE)
    Update_Original = SetRelPointer(THIRD_PERSON_CAMERA_INIT_CALL_ADDR + 1, &ThirdPersonCamera::Update_Hook);

    #define GET_RELATIVE_CAMERA_OFFSET_CALL_ADDR (0x519300)
    SetRelPointer(GET_RELATIVE_CAMERA_OFFSET_CALL_ADDR + 1, GetRelativeCameraOffset);
}
