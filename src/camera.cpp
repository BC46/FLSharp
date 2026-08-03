#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cmath>

#include "camera.h"
#include "utils.h"
#include "fl_func.h"

#define FASTCALL __fastcall

Vector offsetCorrection = { 0.0f, 0.0f, 0.0f };
bool correctOffset = false;

bool (ThirdPersonCamera::*UpdateInit_Original)(const Transform& transform, float deltaTime, bool resetValues);

bool ThirdPersonCamera::UpdateInit_Hook(const Transform& transform, float deltaTime, bool resetValues)
{
    Vector relVelocity = { 0.0f, 0.0f, 0.0f };

    // Get the relative velocity of the camera's target (should be the player ship).
    if (const IObjRW* targetIObjRW = GetTarget())
    {
        if (const CObject* targetObj = targetIObjRW->cobject)
        {
            Vector velocity = targetObj->get_velocity();

            // When taking a trade lane, the actual speed is not respected in the velocity value.
            // Therefore, calculate the actual velocity using the TLR speed.
            // TODO: The correction is still a little bit off.
            if ((targetObj->classType & CSHIP_CLASS_TYPE) == CSHIP_CLASS_TYPE)
            {
                CShip* targetShip = (CShip*) targetObj;
                if (targetShip->is_using_tradelane())
                {
                    float tlrSpeed = targetShip->get_tradelane_speed();
                    velocity = velocity.Normalize();
                    velocity.x *= tlrSpeed;
                    velocity.y *= tlrSpeed;
                    velocity.z *= tlrSpeed;
                }
            }

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
        // In general, the higher the frame rate, the better the fix will work, but anything over 20 FPS should suffice.
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
    bool result = (this->*UpdateInit_Original)(transform, deltaTime, resetValues);

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
    UpdateInit_Original = SetRelPointer(THIRD_PERSON_CAMERA_INIT_CALL_ADDR + 1, &ThirdPersonCamera::UpdateInit_Hook);

    #define GET_RELATIVE_CAMERA_OFFSET_CALL_ADDR (0x519300)
    SetRelPointer(GET_RELATIVE_CAMERA_OFFSET_CALL_ADDR + 1, GetRelativeCameraOffset);
}

bool (ThirdPersonCamera::*Update_Original)(const Transform& transform, float deltaTime, bool resetValues);

// Inspired by the camera acceleration fix from FLUF by Aingar:
// https://codeberg.org/TheStarport/FLUF/src/branch/master/FLUF/Source/Client/Fixes/CameraAcceleration.cpp
bool ThirdPersonCamera::Update_Hook(const Transform& transform, float deltaTime, bool resetValues)
{
    // Save the original camera values.
    float ogAngularAcceleration = angularAcceleration;
    float ogAngularSlerpMultiplier = angularSlerpMultiplier;

    // Correct the angular acceleration and angular slerp multiplier using the 60 FPS deviation ratio.
    const float correctionRatio = 60.0f * deltaTime;
    angularAcceleration *= correctionRatio;
    angularSlerpMultiplier *= correctionRatio;

    // Call the original function.
    bool result = (this->*Update_Original)(transform, deltaTime, resetValues);

    // Restore the original camera values.
    angularAcceleration = ogAngularAcceleration;
    angularSlerpMultiplier = ogAngularSlerpMultiplier;

    return result;
}

// The camera motion/sway in third person being is tied to the frame rate.
// As a result, the camera moves too slow at low frame rates, and too fast at high frame rates.
// This problem is fixed by hooking the third person camera update function and calculating the factor at which
// the frame rate deviates from 60 FPS and using this to correct the camera's angular motion values.
void InitCameraMotionFix()
{
    #define THIRD_PERSON_CAMERA_UPDATE_CALL_ADDR (0x518EBD)
    Update_Original = SetRelPointer(THIRD_PERSON_CAMERA_UPDATE_CALL_ADDR + 1, &ThirdPersonCamera::Update_Hook);
}
