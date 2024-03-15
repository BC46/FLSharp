#pragma once

#include "Common.h"

#define WAYPOINT_CHECK_ADDR 0x4C46A0
#define PLAYER_SYSTEM_ADDR 0x673354
#define CHECK_FOR_SYNC_CALL_ADDR 0x541602
#define POST_INIT_DEALLOC_CALL_ADDR 0x54B8B9
#define DEALLOC_ADDR 0x5B7E1D
#define OBJ_UPDATE_CALL_ADDR 0x54167C
#define GET_PLAYERIOBJINSPECTIMPL_ADDR 0x54BAF0
#define WAYPOINT_CHECK_CALL_ADDR 0x4F4141

struct WaypointInfo
{
    BYTE data[12];
    UINT system;
};

typedef WaypointInfo* WaypointCheck(UINT index);

typedef IObjInspectImpl* GetPlayerIObjInspectImpl();

typedef void Dealloc(PVOID obj);
