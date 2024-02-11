#include "update.h"
#include <time.h>

#define Naked __declspec(naked)

const DWORD Get_IOBjRW_ADDR = 0x54BAF0;
const double syncIntervalMs = 1000;
const double rotationCheckIntervalMs = 500;

clock_t timeSinceLastUpdate;

void InitTimeSinceLastUpdate()
{
    timeSinceLastUpdate = clock();
}

// Thanks adoxa
Naked CShip* GetCShip()
{
    __asm {
        call    Get_IOBjRW_ADDR
        test    eax, eax
        jz      noship
        mov     eax, [eax+16]

    noship:
        ret
    }
}

bool hasMaxTimeSinceLastUpdateElapsed()
{
    return (double) (clock() - timeSinceLastUpdate) >= syncIntervalMs;
}

bool isEkFlipped(CEEngine const * engine)
{
    static bool lastEngineState = false;

    bool result = lastEngineState != engine->IsTriggered();
    lastEngineState = engine->IsTriggered();

    return result;
}

bool hasOrientationChanged(CShip* ship)
{
    return false;
}

// Hook for function that determines whether an update should be sent to the server
bool __fastcall CheckForSync_Hook(CRemotePhysicsSimulation* physicsSim, PVOID _edx, Vector const &unk1, Vector const &unk2, Quaternion const &unk3)
{
    // Does the client want to sync?
    if (physicsSim->CheckForSync(unk1, unk2, unk3))
        return true;

    // If the client doesn't want to sync, we do our own checks to see if it should sync regardless

    if (hasMaxTimeSinceLastUpdateElapsed())
        return true;

    CShip* ship = GetCShip();

    if (!ship)
        return false;

    CEEngine const * engine = CEEngine::cast(ship->equipManager.FindFirst(ENGINE_TYPE));

    if (!engine)
        return false;

    return isEkFlipped(engine) || hasOrientationChanged(ship);
}

// Hook for function that sends an update to the server
void __fastcall SPObjUpdate_Hook(IServerImpl* server, PVOID _edx, SSPObjUpdateInfo &updateInfo, UINT client)
{
    CShip* ship = GetCShip();

    // Get throttle from the ship and set it in the update info, provided the ship isn't NULL
    if (ship)
        updateInfo.throttle = ship->get_throttle();

    // Send update to the server
    server->SPObjUpdate(_edx, updateInfo, client);

    // Set time since last update
    timeSinceLastUpdate = clock();
}
