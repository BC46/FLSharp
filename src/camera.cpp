#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cmath>

#include "camera.h"
#include "utils.h"
#include "fl_func.h"
#include "Freelancer.h"

#define FASTCALL __fastcall

struct Transform
{
    Matrix rot;
    Vector pos;
};

struct Camera
{
    BYTE x00[0x28];
    Vector pos; // 0x28
    BYTE x34[0x8C];
    PBYTE watchable; // 0xC0
    BYTE xC4[0x88];
    Vector shipOffset; // 0x14C
    BYTE x158[0x98];
    float distFromObj; // 0x1F0
    float x1F4;
    float x1F8;
    float x1FC;

    const IObjRW* GetTarget() const;
};

const IObjRW* Camera::GetTarget() const
{
    if (watchable)
    {
        return (IObjRW*) (watchable - 0x8);
    }

    return nullptr;
}

FL_FUNC(BOOL IsPlayerInCutscene(), 0x41A3E0)

// SetCameraTransform
bool (__fastcall *UpdateCamera_Og)(Camera* camera, DWORD edx, DWORD transform, float deltaTime, bool resetValues);

Vector MulMatAndVec(const Matrix& matrix, const Vector& v)
{
    Vector result;
    result.x = matrix.data[0][0] * v.x + matrix.data[0][1] * v.y + matrix.data[0][2] * v.z;
    result.y = matrix.data[1][0] * v.x + matrix.data[1][1] * v.y + matrix.data[1][2] * v.z;
    result.z = matrix.data[2][0] * v.x + matrix.data[2][1] * v.y + matrix.data[2][2] * v.z;
    return result;
}

Matrix TransposeMatrix(const Matrix& matrix)
{
    Matrix result;

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            result.data[i][j] = matrix.data[j][i];
    return result;
}

Vector offsetCorrection = { 0.0f, 0.0f, 0.0f };
bool cameraSwitched = false;

bool __fastcall UpdateCamera(Camera* camera, DWORD edx, DWORD transform, float deltaTime, bool resetValues)
{
    CShip* ship = GetPlayerShip();
    Vector velocity = ship->get_velocity();
    Matrix orient = ship->get_orientation();

    orient = TransposeMatrix(orient);
    velocity = MulMatAndVec(orient, velocity);
    Vector ogOffsetCorrection = offsetCorrection;

    if (!IsPlayerInCutscene())
    {
        // The provided deltaTime parameter is 0 because this is the "init" function, so we obtain the delta time manually.
        float dt = (float) GetDeltaTime();

        // The velocity fix makes things worse if the frame rate is very low.
        if (dt <= 0.125f)
        {
            offsetCorrection.x += dt * velocity.x;
            offsetCorrection.y += dt * velocity.y;
            offsetCorrection.z += dt * velocity.z;
            cameraSwitched = true;
        }
    }

    bool result = UpdateCamera_Og(camera, edx, transform, deltaTime, resetValues);

    offsetCorrection = ogOffsetCorrection;
    cameraSwitched = false;

    return result;
}

struct VectorStack
{
    Vector result;      // 0x0
    BYTE x0C[0x28];
    const Vector v1;    // 0x34
};

// v2 is the camera offset.
// v1 is the offset based on the target's velocity.
void VectorAdd_Hook(VectorStack& s, const Camera& camera, Vector& v2)
{
    if (cameraSwitched)
    {
        // Correct the velocity values such that the camera does not stutter in the first frame.
        v2.x += offsetCorrection.x;
        v2.y += offsetCorrection.y;
        v2.z += offsetCorrection.z;
    }

    s.result.x = s.v1.x + v2.x;
    s.result.y = s.v1.y + v2.y;
    s.result.z = s.v1.z + v2.z;
}

void InitCameraSwitchFix()
{
    // -z is up on nav map and +z is down on nav map
    // -x is left on nav map and +x is right on nav map
    // +y is up vertically and -y is down vertically
    UpdateCamera_Og = SetRelPointer(0x518DBE + 1, UpdateCamera);

    SetRelPointer(0x00519300 + 1, VectorAdd_Hook);
    Patch<BYTE>(0x5192F7, 0x55);

    // The default z offset is not hardcoded.
    offsetCorrection.x = GetValue<float>(0x5192DA);
	offsetCorrection.y = GetValue<float>(0x5192E2);
}
