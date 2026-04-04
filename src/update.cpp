#include "Freelancer.h"
#include "update.h"
#include "utils.h"
#include <cmath>

#define M_PIF 3.14159265358979323846f
#define MIN_SYNC_INTERVAL_SEC (40.0f / 1000.0f)
#define MIN_SYNC_INTERVAL_TLR_SEC (750.0f / 1000.0f)
#define MAX_SYNC_INTERVAL_SEC (2000.0f / 1000.0f)
#define ROTATION_CHECK_INTERVAL_SEC (250.0f / 1000.0f)

bool sendUpdateAsap = true;
bool engineKillEnabledLastTime = false;

#define DEFAULT_SHIP_TURN_THRESHOLD 30.0f
float shipTurnThreshold = DEFAULT_SHIP_TURN_THRESHOLD;

Quaternion lastOrientation;
float secElapsedSinceLastUpdate = 0.0;

void ResetTimeSinceLastUpdate()
{
    secElapsedSinceLastUpdate = 0.0;
    sendUpdateAsap = false;
}

void ForceObjUpdate()
{
    secElapsedSinceLastUpdate = MAX_SYNC_INTERVAL_SEC;
    sendUpdateAsap = true;
}

bool IsEkEnabled(const CShip& ship)
{
    // This seems to be a relatively fast operation; Freelancer calls it numerous times per frame.
    CEEngine const * engine = CEEngine::cast(ship.equipManager.FindFirst(ENGINE_TYPE));

    if (!engine)
        return false;

    return !engine->IsTriggered();
}

// Checks if engine kill has been toggled and update the last known value.
bool IsEkToggled(const CShip& ship)
{
    bool engineKillEnabled = IsEkEnabled(ship);

    bool result = engineKillEnabledLastTime != engineKillEnabled;
    engineKillEnabledLastTime = engineKillEnabled;

    return result;
}

bool HasOrientationChanged(const CShip& ship, float secElapsed)
{
    if (secElapsed < ROTATION_CHECK_INTERVAL_SEC)
        return false;

    float rotationDelta = GetRotationDelta(lastOrientation, ship.get_orientation());
    return rotationDelta >= shipTurnThreshold;
}

float GetShipTurnThreshold(const CShip& ship)
{
    Archetype::Ship const * shipArch = ship.shiparch();

    // TODO: The angular drag is meant to be calculated dynamically using the CShip::get_angular_drag() function.
    // However, the angular drag factor is kind of an unused feature in FL and not many mods use it.
    // Though some do for instance to increase the weight of the ship based on the amount of cargo you have.
    // This means the turn speed should be continuously recalculated instead of only once on launch.
    float avgDrag = (shipArch->angularDrag.x + shipArch->angularDrag.y) / 2.0f;
    float avgTorque = (shipArch->steeringTorque.x + shipArch->steeringTorque.y) / 2.0f;
    float maxTurnSpeed = (avgTorque / avgDrag) * (180.0f / M_PIF);

    return min(DEFAULT_SHIP_TURN_THRESHOLD, 15.0f * sqrtf(maxTurnSpeed) / sqrtf(ship.get_radius()));
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
        // TODO: Check if it is really needed to force an update initially.
        // Does FL already correctly position the ship after undocking with a default velocity?
        ForceObjUpdate();

        if (CShip* ship = GetPlayerShip())
            shipTurnThreshold = GetShipTurnThreshold(*ship);
        else
            shipTurnThreshold = DEFAULT_SHIP_TURN_THRESHOLD;
    }
}

bool ShouldSendUpdate(const CShip& ship, float secElapsed)
{
    // Has it been a while since the last update?
    // Has the orientation been changed to some extent?
    return (secElapsed >= MAX_SYNC_INTERVAL_SEC) || HasOrientationChanged(ship, secElapsed);
}

inline float GetShipMinSyncInterval(const CShip& ship)
{
    // Ensure updates are sent less frequently when the player ship is taking a tradelane to prevent jitter
    return ship.is_using_tradelane() ? MIN_SYNC_INTERVAL_TLR_SEC : MIN_SYNC_INTERVAL_SEC;
}

void (*SendUpdatesToServer_Original)(float deltaTime);

// Hook that keeps track of the elapsed time.
void SendUpdatesToServer_Hook(float deltaTime)
{
    if (!SinglePlayer())
    {
        secElapsedSinceLastUpdate += deltaTime;
        SendUpdatesToServer_Original(deltaTime);
    }
}

// Hook for function that determines whether an update should be sent to the server
bool CRemotePhysicsSimulation::CheckForSync_Hook(const CShip& ship, Vector const &shipPos, Quaternion const &unk)
{
    bool isEkToggled = IsEkToggled(ship);
    bool syncResult = CheckForSync(shipPos, shipPos, unk);

    if (secElapsedSinceLastUpdate < GetShipMinSyncInterval(ship))
    {
        // Prevent the client from sending too many updates in a short amount of time
        // This resolves the jitter issue that occurs when playing on a high framerate

        // TODO: If EK has been toggled twice before the min sync interval has passed, then an asap update should actually not be sent because of this.
        // But then you'd also have to check if the asap update *should* be sent because CheckForSync or ShouldSendUpdate returned true. Eh, this sounds complicated.
        if (!sendUpdateAsap)
            sendUpdateAsap = syncResult || isEkToggled || ShouldSendUpdate(ship, secElapsedSinceLastUpdate);

        return false;
    }
    else if (sendUpdateAsap)
    {
        // If an update has been missed, send an update as soon as this becomes possible, but do it only once
        return true;
    }

    return syncResult || isEkToggled || ShouldSendUpdate(ship, secElapsedSinceLastUpdate);
}

// Hook for function that sends an update to the server
void IServerImpl::SPObjUpdate_Hook(const CShip& ship, SSPObjUpdateInfo &updateInfo, UINT client)
{
    // Get throttle from the ship and set it in the update info if engine kill is currently disabled.
    // If it's enabled we want to set the throttle value to 0.
    updateInfo.throttle = engineKillEnabledLastTime ? 0.0f : ship.get_throttle();

    // Send update to the server
    SPObjUpdate(updateInfo, client);

    ResetTimeSinceLastUpdate();
    lastOrientation = MatrixToQuaternion(ship.get_orientation());
}

// This allows for extra checks to prevent jitters and allow smoother updates from the client to the server.
// Also fixes a bug where the client always sends the throttle state as 0.
void InitBetterUpdates()
{
    #define SEND_UPDATES_TO_SERVER_CALL_ADDR (0x54B16D)
    #define SERVER_UPDATE_SP_CHECK_ADDR (0x54158C)

    SendUpdatesToServer_Original = SetRelPointer(SEND_UPDATES_TO_SERVER_CALL_ADDR + 1, SendUpdatesToServer_Hook);
    // Wipe out the original single player check because it is already checked for in the hook.
    Nop(SERVER_UPDATE_SP_CHECK_ADDR, 14);

    Update::PostInitDealloc_Original = SetRelPointer(POST_INIT_DEALLOC_CALL_ADDR + 1, Update::PostInitDealloc_Hook);

    Hook(CHECK_FOR_SYNC_CALL_ADDR, &CRemotePhysicsSimulation::CheckForSync_Hook, 5);
    Patch<BYTE>(PUSH_SHIP_POS_SYNC_CHECK_ADDR, 0x57); // push eax -> push edi (provide the CShip& to the CheckForSync hook)

    Patch<BYTE>(OBJ_UPDATE_CALL_ADDR, 0x57); // push edi (provide the CShip& to the SPObjUpdate hook)
    Hook(OBJ_UPDATE_CALL_ADDR + 1, &IServerImpl::SPObjUpdate_Hook, 5);
}
