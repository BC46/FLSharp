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
#define INIT_NN_ELEMENTS_CALL_ADDR 0x5D4A80
#define INIT_NN_ELEMENTS_ADDR 0x4A9790
#define SET_RESOLUTION_ADDR 0x4B1C00
#define NAV_MAP_GET_HIGHLIGHTED_OBJ_ADDR 0x496D40

struct WaypointInfo
{
    Vector pos;
    UINT system;
};

struct NavMapObj
{
    UINT type;
};

struct NeuroNetNavMap
{
    NavMapObj* GetHighlightedObject_Hook(DWORD unk1, DWORD unk2);

private:
    typedef NavMapObj* (NeuroNetNavMap::*GetHighlightedObject)(DWORD unk1, DWORD unk2);
};

// 0x330 = current selected width
// 0x8b8 = current active width (int)
// 0x8cc = start of resolution array (10 * 4 * 3 bytes)
// 0x8d4 = start of resolution array + 0x8 (points to the bpp of the first element)
// 0x944 = array of 10 bytes that contains flags of whether the resolution index is supported (1 = supported, 0 = unsupported)
// 0x94e = unallocated word (2 bytes)
// 0x950 = amount of supported resolutions (integer)
// 0x954 = array of 4 * 10 bytes that contains the indices of the resolutions in the selection menu (-1 is unsupported resolution)
struct NN_Preferences
{
    BYTE x00[0x980];
    UINT selectedHeight;
    UINT activeHeight;
    PBYTE newData;

    bool InitElements_Hook(DWORD unk1, DWORD unk2);
    bool SetResolution_Active_Hook(UINT width, DWORD unk);
    bool SetResolution_Selected_Hook(UINT width, DWORD unk);

private:
    typedef bool (NN_Preferences::*InitElements)(DWORD unk1, DWORD unk2);
    typedef bool (NN_Preferences::*SetResolution)(UINT width, DWORD unk, UINT height);
};
