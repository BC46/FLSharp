#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

struct MissionObjective
{
	BYTE fmtStr[0x16]; // 0x0
	DWORD flags; // 0x18
};

void InitWaypointNameFixes();
