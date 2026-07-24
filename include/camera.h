#include "Freelancer.h"

struct ThirdPersonCamera : public Camera
{
    bool Update_Hook(const Transform& transform, float deltaTime, bool resetValues);
};

void InitCameraSwitchFix();
