#include "Freelancer.h"
#include "update.h"
#include "utils.h"
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
    IObjInspectImpl* playerIObjInspect = ((GetPlayerIObjInspectImpl*) GET_PLAYERIOBJINSPECTIMPL_ADDR)();
    return !playerIObjInspect ? NULL : playerIObjInspect->ship;
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
    if (!hasTimeElapsed(timeSinceLastUpdate, rotationCheckIntervalMs))
        return false;

    Archetype::Ship const * shipArch = ship->shiparch();

    // TODO: Hook 0x0054B850 in Freelancer.exe (hook for right after the ship gets set)
    // This way we don't have to re-calculate the ship info each time the orientation has changed
    // But what happens if these values change dynamically? (e.g. your ship becomes heavier as you tractor in more cargo.)
    float avgDrag = (shipArch->angularDrag.x + shipArch->angularDrag.y) / 2;
    float avgTorque = (shipArch->steeringTorque.x + shipArch->steeringTorque.y) / 2;
    float maxTurnSpeed = (avgTorque / avgDrag) * 57.29578f;

    float turnThreshold = min(30.0f, 15 * sqrtf(maxTurnSpeed) / sqrtf(ship->get_radius()));
    float rotationDelta = GetRotationDelta(lastOrientation, ship->get_orientation());

    return rotationDelta > turnThreshold;
}

// Hook for function that determines whether an update should be sent to the server
bool __fastcall CheckForSync_Hook(CRemotePhysicsSimulation* physicsSim, PVOID _edx, Vector const &unk1, Vector const &unk2, Quaternion const &unk3)
{
    // Does the client want to sync?
    if (physicsSim->CheckForSync(unk1, unk2, unk3))
        return true;

    CShip* ship = GetShip();

    if (!ship)
        return false;

    // If the client doesn't want to sync, we do our own checks below to see if it should sync regardless
    if (hasOrientationChanged(ship) || hasTimeElapsed(timeSinceLastUpdate, syncIntervalMs))
        return true;

    // TODO: Save the engine somewhere? What happens if you lose it while flying?
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
