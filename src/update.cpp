#include "Freelancer.h"
#include "update.h"
#include "utils.h"
#include <cmath>

#define M_PI 3.14159265358979323846f

const double minSyncIntervalMs = 40.0;
const double minSyncIntervalTlrMs = 750.0;
const double maxSyncIntervalMs = 2000.0;
const double rotationCheckIntervalMs = 250.0;

bool sendUpdateAsap = true;
bool engineKillEnabledLastTime = false;

#define DEFAULT_SHIP_TURN_THRESHOLD 30.0f
float shipTurnThreshold = DEFAULT_SHIP_TURN_THRESHOLD;

Quaternion lastOrientation;
clock_t timeSinceLastUpdate;

void SetTimeSinceLastUpdate()
{
    timeSinceLastUpdate = clock();
}

CShip* GetPlayerShip()
{
    IObjRW* playerIObjRW = GetPlayerIObjRW();
    return !playerIObjRW ? nullptr : playerIObjRW->ship;
}

// This seems to be a relatively fast operation; Freelancer calls it numerous times per frame.
CEEngine const * GetEngine(CShip* ship)
{
    if (!ship)
        return nullptr;

    return CEEngine::cast(ship->equipManager.FindFirst(ENGINE_TYPE));
}

bool IsEkEnabled(CShip* ship)
{
    CEEngine const * shipEngine = GetEngine(ship);

    if (!shipEngine)
        return false;

    return !shipEngine->IsTriggered();
}

// Checks if engine kill has been toggled and update the last known value.
bool IsEkToggled(CShip* ship)
{
    bool engineKillEnabled = IsEkEnabled(ship);

    bool result = engineKillEnabledLastTime != engineKillEnabled;
    engineKillEnabledLastTime = engineKillEnabled;

    return result;
}

bool HasOrientationChanged(CShip* ship, double timeElapsed)
{
    if (timeElapsed < rotationCheckIntervalMs)
        return false;

    float rotationDelta = GetRotationDelta(lastOrientation, ship->get_orientation());
    return rotationDelta >= shipTurnThreshold;
}

float GetShipTurnThreshold(CShip *ship)
{
    if (!ship)
        return DEFAULT_SHIP_TURN_THRESHOLD;

    Archetype::Ship const * shipArch = ship->shiparch();

    // TODO: The angular drag is meant to be calculated dynamically using the CShip::get_angular_drag() function.
    // However, the angular drag factor is kind of an unused feature in FL and not many mods use it.
    // Though some do for instance to increase the weight of the ship based on the amount of cargo you have.
    // This means the turn speed should be continuously recalculated instead of only once on launch.
    float avgDrag = (shipArch->angularDrag.x + shipArch->angularDrag.y) / 2;
    float avgTorque = (shipArch->steeringTorque.x + shipArch->steeringTorque.y) / 2;
    float maxTurnSpeed = (avgTorque / avgDrag) * (180.0f / M_PI);

    return min(DEFAULT_SHIP_TURN_THRESHOLD, 15.0f * sqrtf(maxTurnSpeed) / sqrtf(ship->get_radius()));
}

namespace Update
{
    void (*PostInitDealloc_Original)(PVOID obj);

    // Hook for dealloc function that gets called right after initializing the player's ship (undock or load game in space)
    // This is where we want to calculate the ship's turn threshold and set some default values
    void PostInitDealloc_Hook(PVOID obj)
    {
        // Call original function
        PostInitDealloc_Original(obj);

        if (SinglePlayer()) // No need to calculate the turn threshold in SP
            return;

        engineKillEnabledLastTime = false;
        sendUpdateAsap = true;

        shipTurnThreshold = GetShipTurnThreshold(GetPlayerShip());
    }
}

bool ShouldSendUpdate(CShip* ship, double timeElapsed)
{
    if (!ship)
        return false;

    // Has it been a while since the last update?
    // Has the orientation been changed to some extent?
    return (timeElapsed >= maxSyncIntervalMs) || HasOrientationChanged(ship, timeElapsed);
}

inline double GetShipMinSyncInterval(CShip* ship)
{
    if (!ship)
        return minSyncIntervalMs;

    // Ensure updates are sent less frequently when the player ship is taking a tradelane to prevent jitter
    return ship->is_using_tradelane() ? minSyncIntervalTlrMs : minSyncIntervalMs;
}

// Hook for function that determines whether an update should be sent to the server
bool CRemotePhysicsSimulation::CheckForSync_Hook(Vector const &shipPos, Vector const &shipPos2, Quaternion const &unk)
{
    double timeElapsed = getTimeElapsed(timeSinceLastUpdate); // Time elapsed since the last update
    CShip* ship = GetPlayerShip();
    bool isEkToggled = IsEkToggled(ship);

    if (timeElapsed < GetShipMinSyncInterval(ship))
    {
        // Prevent the client from sending too many updates in a short amount of time
        // This resolves the jitter issue that occurs when playing on a high framerate

        // TODO: If EK has been toggled twice before the min sync interval has passed, then an asap update should actually not be sent because of this.
        // But then you'd also have to check if the asap update *should* be sent because CheckForSync or ShouldSendUpdate returned true. Eh, this sounds complicated.
        if (!sendUpdateAsap)
            sendUpdateAsap = isEkToggled || CheckForSync(shipPos, shipPos2, unk) || ShouldSendUpdate(ship, timeElapsed);

        return false;
    }
    else if (sendUpdateAsap)
    {
        // If an update has been missed, send an update as soon as this becomes possible, but do it only once
        sendUpdateAsap = false;
        return true;
    }

    // I'll assume that CheckForSync is the more efficient update check.
    return isEkToggled || CheckForSync(shipPos, shipPos2, unk) || ShouldSendUpdate(ship, timeElapsed);
}

// Hook for function that sends an update to the server
void IServerImpl::SPObjUpdate_Hook(SSPObjUpdateInfo &updateInfo, UINT client)
{
    CShip* ship = GetPlayerShip();

    if (ship)
    {
        // Get throttle from the ship and set it in the update info if engine kill is currently disabled.
        // If it's enabled we want to set the throttle value to 0.
        updateInfo.throttle = engineKillEnabledLastTime ? 0.0f : ship->get_throttle();

        // Set the last orientation
        lastOrientation = MatrixToQuaternion(ship->get_orientation());
    }

    // Send update to the server
    SPObjUpdate(updateInfo, client);

    SetTimeSinceLastUpdate();
}

// This allows for extra checks to prevent jitters and allow smoother updates from the client to the server.
// Also fixes a bug where the client always sends the throttle state as 0.
void InitBetterUpdates()
{
    SetTimeSinceLastUpdate();

    Update::PostInitDealloc_Original = SetRelPointer(POST_INIT_DEALLOC_CALL_ADDR + 1, Update::PostInitDealloc_Hook);
    Hook(CHECK_FOR_SYNC_CALL_ADDR, &CRemotePhysicsSimulation::CheckForSync_Hook, 5);
    Hook(OBJ_UPDATE_CALL_ADDR, &IServerImpl::SPObjUpdate_Hook, 6);
}
