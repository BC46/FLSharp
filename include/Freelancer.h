#pragma once

#include "Common.h"

#define PLAYER_SYSTEM_ADDR 0x673354
#define CHECK_FOR_SYNC_CALL_ADDR 0x541602
#define POST_INIT_DEALLOC_CALL_ADDR 0x54B8B9
#define OBJ_UPDATE_CALL_ADDR 0x54167C
#define WAYPOINT_CHECK_CALL_ADDR 0x4F4141
#define INIT_NN_ELEMENTS_CALL_ADDR 0x5D4A80
#define TEST_RESOLUTIONS_ADDR 0x4B2440

#define FL_BUFFER_1 ((LPWSTR) 0x66DC60)
#define FL_BUFFER_2 ((LPWSTR) 0x66FC60)
#define FL_BUFFER_LEN *((PUINT) 0x6119F8)
#define FL_RESOURCES_HANDLE *((PDWORD) 0x67ECA8)

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
    NavMapObj* GetHighlightedObject(DWORD unk1, DWORD unk2);
};

#define NN_PREFERENCES_NEW_DATA 0x98C

struct AudioOption
{
    UINT idsName;
    UINT idsTooltip;
    UINT defaultVolume;
    DWORD x10, x14, x18;
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
    BYTE x00[0x528];
    AudioOption* audioOptions; // pointer to array of audio info from up to 14 UI scroll elements
    BYTE x52C[0x128];
    PVOID scrollElements[14]; // 0x654, array of pointers to 14 volume scroll elements (there's more but we only need up to 14)
    BYTE x68C[0x2C4];
    UINT supportedResAmount;
    BYTE x954[0x28];
    bool unk_x97C;
    BYTE x97D[0x3];
    UINT selectedHeight;
    UINT activeHeight;
    bool* resSupportedArr; // Points to new version of 0x944
    BYTE newData;

    bool InitElements_Hook(DWORD unk1, DWORD unk2);
    bool SetResolution_Active_Hook(UINT width, DWORD unk);
    bool SetResolution_Selected_Hook(UINT width, DWORD unk);
    void TestResolutions_Hook(DWORD unk);
    void VolumeSliderAdjustEnd_Hook(PVOID scrollElement);

    bool InitElements(DWORD unk1, DWORD unk2);
    bool SetResolution(UINT width, DWORD unk, UINT height);
};

void StopSound(BYTE soundId);
void StartSound(BYTE soundId);

UINT GetFlStringFromResources(DWORD resourcesHandle, UINT ids, LPWSTR buffer, UINT bufferLen);

WaypointInfo* WaypointCheck(UINT index);

IObjRW* GetPlayerIObjRW();

inline UINT GetFlString(UINT ids, LPWSTR buffer, UINT bufferLen)
{
    return GetFlStringFromResources(FL_RESOURCES_HANDLE, ids, buffer, bufferLen);
}

#define SHIP_TRADER_SHIP_AMOUNT 3
#define SHIP_TRADER_PLAYER_SHIP_INDEX -2

// 0x370 = ShipTrader3DShip*
// TODO: add generic check to see if index is in valid range between 0 and SHIP_TRADER_SHIP_AMOUNT
struct NN_ShipTrader
{
    BYTE x00[0x3D0];
    int selectedShipIndex; // 0x3D0
    BYTE x3D4[0x74];
    int shipRepPercentages[SHIP_TRADER_SHIP_AMOUNT]; // 0x448

    void StoreShipRepRequirement(PBYTE shipListPtr, float repRequirement);
    LPWSTR NN_ShipTrader::PrintFmtShipRepRequirement();
};

void ExpandNNShipTraderObjMemory();

inline bool IsShipIndexValid(int index)
{
    return index >= 0 && index < SHIP_TRADER_SHIP_AMOUNT;
}
