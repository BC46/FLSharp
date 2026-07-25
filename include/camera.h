#include "Freelancer.h"

struct ThirdPersonCamera : public Camera
{
    // UpdateInit is the same function as Update, but UpdateInit gets called with deltaTime = 0 and resetValues = 1.
    // Update gets called every frame whereas UpdateInit only gets called when the game switches camera.
    bool UpdateInit_Hook(const Transform& transform, float deltaTime, bool resetValues);
    bool Update_Hook(const Transform& transform, float deltaTime, bool resetValues);
};

void InitCameraSwitchFix();

void InitCameraMotionFix();
