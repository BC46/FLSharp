#include "Freelancer.h"
#include "update.h"
#include "utils.h"
#include <cmath>

const double minSyncIntervalMs = 125.0;
const double maxSyncIntervalMs = 1500.0;
const double rotationCheckIntervalMs = 250.0;

bool sendUpdateAsap = false;

Quaternion lastOrientation;
float shipTurnThreshold = 30.0f;
clock_t timeSinceLastUpdate;

DWORD deallocOriginal;

void SetTimeSinceLastUpdate()
{
    timeSinceLastUpdate = clock();
}

CShip* GetShip()
{
    typedef IObjInspectImpl* GetPlayerIObjInspectImpl();
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

bool hasOrientationChanged(CShip* ship, double timeElapsed)
{
    if (timeElapsed < rotationCheckIntervalMs)
        return false;

    float rotationDelta = GetRotationDelta(lastOrientation, ship->get_orientation());
    return rotationDelta > shipTurnThreshold;
}

// Hook for dealloc function that gets called right after initializing the player's ship (undock or load game in space)
// This is where we want to calculate the ship's turn threshold
void PostInitDealloc_Hook(PVOID obj)
{
    // Call original function
    typedef void Dealloc(PVOID obj);
    ((Dealloc*) deallocOriginal)(obj);

    if (SinglePlayer()) // No need to calculate the turn threshold in SP
        return;

    sendUpdateAsap = false;

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

bool CRemotePhysicsSimulation::ShouldSendUpdate(Vector const &unk1, Vector const &unk2, Quaternion const &unk3, double timeElapsed)
{
    CShip* ship = GetShip();

    if (!ship)
        return false;

    // If the client doesn't want to sync, we do our own checks below to see if it should sync regardless
    if (hasOrientationChanged(ship, timeElapsed) || (timeElapsed >= maxSyncIntervalMs))
        return true;

    // TODO: Save the engine somewhere? What happens if you lose it while flying?
    CEEngine const * engine = CEEngine::cast(ship->equipManager.FindFirst(ENGINE_TYPE));

    if (!engine)
        return false;

    return isEkToggled(engine);
}

// Hook for function that determines whether an update should be sent to the server
bool CRemotePhysicsSimulation::CheckForSync_Hook(Vector const &unk1, Vector const &unk2, Quaternion const &unk3)
{
    double timeElapsed = getTimeElapsed(timeSinceLastUpdate); // Time elapsed since the last update
    bool sendUpdateNow = ShouldSendUpdate(unk1, unk2, unk3, timeElapsed);

    if (sendUpdateNow)
        return true;

    sendUpdateNow = CheckForSync(unk1, unk2, unk3); // Does the client want to sync?

    if (timeElapsed < minSyncIntervalMs)
    {
        // Prevent the client from sending too many updates in a short amount of time
        // This resolves the jitter issue that occurs when playing on a high framerate
        sendUpdateAsap |= sendUpdateNow;
        return false;
    }
    else if (sendUpdateAsap)
    {
        // If an update has been missed, send an update as soon as this becomes possible, but do it only once
        sendUpdateAsap = false;
        return true;
    }

    return sendUpdateNow;
}

// Hook for function that sends an update to the server
void IServerImpl::SPObjUpdate_Hook(SSPObjUpdateInfo &updateInfo, UINT client)
{
    CShip* ship = GetShip();

    if (ship)
    {
        // Get throttle from the ship and set it in the update info
        updateInfo.throttle = ship->get_throttle();

        // Set the last orientation
        lastOrientation = MatrixToQuaternion(ship->get_orientation());
    }

    // Send update to the server
    SPObjUpdate(updateInfo, client);

    SetTimeSinceLastUpdate();
}

void InitBetterUpdates()
{
    SetTimeSinceLastUpdate();

    deallocOriginal = SetRelPointer(POST_INIT_DEALLOC_CALL_ADDR + 1, PostInitDealloc_Hook);
    Hook(CHECK_FOR_SYNC_CALL_ADDR, CRemotePhysicsSimulation::CheckForSync_Hook, 5);
    Hook(OBJ_UPDATE_CALL_ADDR, IServerImpl::SPObjUpdate_Hook, 6);
}
