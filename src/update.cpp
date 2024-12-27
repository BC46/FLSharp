#include "Freelancer.h"
#include "update.h"
#include "utils.h"
#include <cmath>

const double minSyncIntervalMs = 35.0;
const double minSyncIntervalTlrMs = 750.0;
const double maxSyncIntervalMs = 1500.0;
const double rotationCheckIntervalMs = 250.0;

bool sendUpdateAsap = false;
bool engineKillDisabledLastTime = false;

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

bool IsEkToggled(CShip* ship)
{
    // TODO: Save the engine somewhere? What happens if you lose it while flying?
    CEEngine const * engine = CEEngine::cast(ship->equipManager.FindFirst(ENGINE_TYPE));

    if (!engine)
        return false;

    bool engineTriggered = engine->IsTriggered();

    bool result = engineKillDisabledLastTime != engineTriggered;
    engineKillDisabledLastTime = engineTriggered;

    return result;
}

bool HasOrientationChanged(CShip* ship, double timeElapsed)
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

    sendUpdateAsap = true;

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

bool ShouldSendUpdate(CShip* ship, double timeElapsed)
{
    if (!ship)
        return false;

    // Has engine kill been toggled?
    // Has it been a while since the last update?
    // Has the orientation been changed to some extent?
    return IsEkToggled(ship) || (timeElapsed >= maxSyncIntervalMs) || HasOrientationChanged(ship, timeElapsed);
}

inline double GetShipMinSyncInterval(CShip* ship)
{
    if (!ship)
        return minSyncIntervalMs;

    // Ensure updates are sent less frequently when the player ship is taking a tradelane to prevent jitter
    return ship->is_using_tradelane() ? minSyncIntervalTlrMs : minSyncIntervalMs;
}

// Hook for function that determines whether an update should be sent to the server
bool CRemotePhysicsSimulation::CheckForSync_Hook(Vector const &unk1, Vector const &unk2, Quaternion const &unk3)
{
    double timeElapsed = getTimeElapsed(timeSinceLastUpdate); // Time elapsed since the last update
    CShip* ship = GetShip();

    if (timeElapsed < GetShipMinSyncInterval(ship))
    {
        // Prevent the client from sending too many updates in a short amount of time
        // This resolves the jitter issue that occurs when playing on a high framerate
        if (!sendUpdateAsap)
            sendUpdateAsap = ShouldSendUpdate(ship, timeElapsed) || CheckForSync(unk1, unk2, unk3);

        return false;
    }
    else if (sendUpdateAsap)
    {
        // Call this function to update the engineKillDisabledLastTime value
        if (ship)
            IsEkToggled(ship);

        // If an update has been missed, send an update as soon as this becomes possible, but do it only once
        sendUpdateAsap = false;
        return true;
    }

    return ShouldSendUpdate(ship, timeElapsed) || CheckForSync(unk1, unk2, unk3);
}

// Hook for function that sends an update to the server
void IServerImpl::SPObjUpdate_Hook(SSPObjUpdateInfo &updateInfo, UINT client)
{
    CShip* ship = GetShip();

    if (ship)
    {
        // Get throttle from the ship and set it in the update info if engine kill is currently disabled.
        // If it's enabled we want to set the throttle value to 0.
        updateInfo.throttle = engineKillDisabledLastTime ? ship->get_throttle() : 0.0f;

        // Set the last orientation
        lastOrientation = MatrixToQuaternion(ship->get_orientation());
    }

    // Send update to the server
    SPObjUpdate(updateInfo, client);

    SetTimeSinceLastUpdate();
}

// This allows for extra checks to prevent jitters and smoother updates from the client to the server.
// Also fixes a bug where the client always sends the throttle state as 0.
void InitBetterUpdates()
{
    SetTimeSinceLastUpdate();

    deallocOriginal = SetRelPointer(POST_INIT_DEALLOC_CALL_ADDR + 1, PostInitDealloc_Hook);
    Hook(CHECK_FOR_SYNC_CALL_ADDR, &CRemotePhysicsSimulation::CheckForSync_Hook, 5);
    Hook(OBJ_UPDATE_CALL_ADDR, &IServerImpl::SPObjUpdate_Hook, 6);
}
