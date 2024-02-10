#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define WAYPOINT_CHECK_CALL_ADDR 0x4F4141

PDWORD WaypointCheck_Hook(UINT index);
