#include "Freelancer.h"
#include "update.h"
#include <time.h>
#include <cmath>

const double syncIntervalMs = 2000;
const double rotationCheckIntervalMs = 200;

Quaternion lastOrientation;
clock_t timeSinceLastUpdate;

void InitTimeSinceLastUpdate()
{
    timeSinceLastUpdate = clock();
}

CShip* GetShip()
{
    IObjRW* iObjRW = ((GetIObjRW*) GET_IOBJRW_ADDR)();
    return !iObjRW ? NULL : iObjRW->ship;
}

bool hasTimeElapsed(const clock_t &lastUpdate, const double &intervalMs)
{
    return (double) (clock() - lastUpdate) >= intervalMs;
}

bool isEkToggled(CEEngine const * engine)
{
    static bool lastEngineState = false;

    bool result = lastEngineState != engine->IsTriggered();
    lastEngineState = engine->IsTriggered();

    return result;
}

bool hasOrientationChanged(CShip* ship)
{
    if (!ship || !hasTimeElapsed(timeSinceLastUpdate, rotationCheckIntervalMs))
        return false;

    Archetype::Ship const * shipArch = ship->shiparch();

    float avgDrag = (shipArch->angularDrag.x + shipArch->angularDrag.y) / 2;
    float avgTorque = (shipArch->steeringTorque.x + shipArch->steeringTorque.y) / 2;
    float maxTurnSpeed = (avgTorque / avgDrag) * 57.29578f;

    float turnThreshold = min(30.0f, 15 * sqrtf(maxTurnSpeed) / sqrtf(ship->get_radius()));
    float rotationDelta = CheckRotationDelta(lastOrientation, ship->get_orientation());

    return rotationDelta > turnThreshold;
}

// Hook for function that determines whether an update should be sent to the server
bool __fastcall CheckForSync_Hook(CRemotePhysicsSimulation* physicsSim, PVOID _edx, Vector const &unk1, Vector const &unk2, Quaternion const &unk3)
{
    CShip* ship = GetShip();
    bool orientationChanged = hasOrientationChanged(ship);

    // Does the client want to sync?
    if (physicsSim->CheckForSync(unk1, unk2, unk3))
        return true;

    // If the client doesn't want to sync, we do our own checks to see if it should sync regardless
    if (orientationChanged || hasTimeElapsed(timeSinceLastUpdate, syncIntervalMs))
        return true;

    if (!ship)
        return false;

    CEEngine const * engine = CEEngine::cast(ship->equipManager.FindFirst(ENGINE_TYPE));

    if (!engine)
        return false;

    return isEkToggled(engine);
}

// Hook for function that sends an update to the server
void __fastcall SPObjUpdate_Hook(IServerImpl* server, PVOID _edx, SSPObjUpdateInfo &updateInfo, UINT client)
{
    CShip* ship = GetShip();

    if (ship)
    {
        // Get throttle from the ship and set it in the update info
        updateInfo.throttle = ship->get_throttle();

        // Set the last orientation
        HkMatrixToQuaternion(ship->get_orientation(), lastOrientation);
    }

    // Send update to the server
    server->SPObjUpdate(_edx, updateInfo, client);

    // Set time since last update
    timeSinceLastUpdate = clock();
}
