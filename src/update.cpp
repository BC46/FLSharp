#include "Freelancer.h"
#include "update.h"
#include "utils.h"
#include <cmath>

const double minSyncIntervalMs = 75;
const double maxSyncIntervalMs = 2000;
const double rotationCheckIntervalMs = 200;

Quaternion lastOrientation;
float shipTurnThreshold = 30.0f;
clock_t timeSinceLastUpdate;

void SetTimeSinceLastUpdate()
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

bool hasOrientationChanged(CShip* ship, const clock_t &currentTime)
{
    if (!hasTimeElapsed(timeSinceLastUpdate, currentTime, rotationCheckIntervalMs))
        return false;

    float rotationDelta = GetRotationDelta(lastOrientation, ship->get_orientation());
    return rotationDelta > shipTurnThreshold;
}

// Hook for dealloc function that gets called right after initializing the player's ship (undock or load game in space)
// This is where we want to calculate the ship's turn threshold
void PostInitDealloc_Hook(PVOID obj)
{
    // Call original function
    ((Dealloc*) DEALLOC_ADDR)(obj);

    if (SinglePlayer())
        return;

    CShip* ship = GetShip();

    if (!ship)
        return;

    Archetype::Ship const * shipArch = ship->shiparch();

    // TODO: The angular drag is meant to be calculated dynamically using the CShip::get_angular_drag() function
    // However, the angular drag factor is kind of an unused feature in FL and not many mods use it
    float avgDrag = (shipArch->angularDrag.x + shipArch->angularDrag.y) / 2;
    float avgTorque = (shipArch->steeringTorque.x + shipArch->steeringTorque.y) / 2;
    float maxTurnSpeed = (avgTorque / avgDrag) * 57.29578f;

    shipTurnThreshold = min(30.0f, 15 * sqrtf(maxTurnSpeed) / sqrtf(ship->get_radius()));
}

// Hook for function that determines whether an update should be sent to the server
bool __fastcall CheckForSync_Hook(CRemotePhysicsSimulation* physicsSim, PVOID _edx, Vector const &unk1, Vector const &unk2, Quaternion const &unk3)
{
    clock_t currentTime = clock();

    // Prevent the client from sending too many updates in a short amount of time
    // This resolves the jitter issue that occurs playing on a high framerate
    if (!hasTimeElapsed(timeSinceLastUpdate, currentTime, minSyncIntervalMs))
        return false;

    // Does the client want to sync?
    if (physicsSim->CheckForSync(unk1, unk2, unk3))
        return true;

    CShip* ship = GetShip();

    if (!ship)
        return false;

    // If the client doesn't want to sync, we do our own checks below to see if it should sync regardless
    if (hasOrientationChanged(ship, currentTime) || hasTimeElapsed(timeSinceLastUpdate, currentTime, maxSyncIntervalMs))
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

    SetTimeSinceLastUpdate();
}
