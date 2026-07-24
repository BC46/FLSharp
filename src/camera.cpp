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

Camera* previousCamera = nullptr;

FL_FUNC(BOOL IsPlayerInCutscene(), 0x41A3E0)

// SetCameraTransform
bool (__fastcall *DoStuff_Og)(Camera* camera, DWORD edx, DWORD transform, float deltaTime, bool resetValues);

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

float velocityX = 0.0f;
float velocityY = 0.0f;
float velocityZ = 0.0f;

// TODO: rename this
bool __fastcall DoStuff(Camera* camera, DWORD edx, DWORD transform, float deltaTime, bool resetValues)
{
    CShip* ship = GetPlayerShip();
    Vector velocity = ship->get_velocity();
    Matrix orient = ship->get_orientation();

    orient = TransposeMatrix(orient);

    float ogDist = camera->distFromObj;
    float ogDist2 = camera->x1F4;
    Vector ogOffset = camera->shipOffset;

    velocity = MulMatAndVec(orient, velocity);

    // TODO: If delta time is really high (less than 10 fps), skip this
    if (!IsPlayerInCutscene())
    {
        float dt = (float) GetDeltaTime();

        velocityX += dt * velocity.x;
        velocityY += dt * velocity.y;
        velocityZ += dt * velocity.z;
    }

    bool result = DoStuff_Og(camera, edx, transform, deltaTime, resetValues);

    camera->distFromObj = ogDist;

    velocityX = 0.0f;
    velocityY = 0.0f;
    velocityZ = 0.0f; // TODO: get from code?

    return result; // DoStuff_Og(camera, edx, transform, (float) GetDeltaTime(), false);
}

struct VectorStack
{
    Vector result;      // 0x0
    BYTE x0C[0x28];
    const Vector v1;    // 0x34
};

void VectorAdd_Hook(VectorStack& s, const Camera& camera, Vector& v2)
{
    v2.x = velocityX;
    v2.y = velocityY;
    v2.z += velocityZ;

    s.result.x = s.v1.x + v2.x;
    s.result.y = s.v1.y + v2.y;
    s.result.z = s.v1.z + v2.z;
}

void InitCameraSwitchFix()
{
    // -z is up on nav map and +z is down on nav map
    // -x is left on nav map and +x is right on nav map
    // +y is up vertically and -y is down vertically
    DoStuff_Og = SetRelPointer(0x518DBE + 1, DoStuff);

    SetRelPointer(0x00519300 + 1, VectorAdd_Hook);
    Patch<BYTE>(0x5192F7, 0x55);
}
