#pragma once

#include "Common.h"

#define PLAYER_SYSTEM *((PUINT) 0x673354)
#define CHECK_FOR_SYNC_CALL_ADDR 0x541602
#define PUSH_SHIP_POS_SYNC_CHECK_ADDR 0x5415FF
#define POST_INIT_DEALLOC_CALL_ADDR 0x54B8B9
#define OBJ_UPDATE_CALL_ADDR 0x54167C
#define WAYPOINT_CHECK_CALL_ADDR 0x4F4141
#define INIT_NN_ELEMENTS_CALL_ADDR 0x5D4A80
#define TEST_RESOLUTIONS_ADDR 0x4B2440

// The buffer length is denoted in WORDs.
#define FL_BUFFER_1 ((LPWSTR) 0x66DC60)
#define FL_BUFFER_2 ((LPWSTR) 0x66FC60)
#define FL_BUFFER_LEN *((PUINT) 0x6119F8)
#define FL_RESOURCES_HANDLE *((PDWORD) 0x67ECA8)

#define UNKNOWN_OBJECT_IDS 1191
#define WAYPOINT_IDS 1090
#define MISSION_WAYPOINT_IDS 1091

#define KNOW_VISIT_FLAG (1)
#define LAND_VISIT_FLAG (1 << 1)
#define COMMODITY_DEALER_VISIT_FLAG (1 << 2)
#define EQUIPMENT_DEALER_VISIT_FLAG (1 << 3)
#define SHIP_DEALER_VISIT_FLAG (1 << 4)

// Time elapsed since startup in miliseconds
#define FL_TIME_ELAPSED_MS (*(double*) 0x667D38)

// System time in miliseconds
#define TIMING_DELTA_TICK_COUNT (*(PDWORD) 0x667D14)
#define TIME_GET_TIME_VAL (*(PDWORD) 0x667D20)

#define SHOW_MOUSE_CURSOR (*(bool*) 0x6107DC)

#define FL_HWND (*(HWND*) 0x67ECA0)

struct Waypoint
{
    Vector pos;
    UINT system;
    UINT target;
    int waypointNumber;
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

struct AudioOption
{
    UINT idsName;
    UINT idsTooltip;
    UINT defaultVolume;
    DWORD x0C, x10, x14;
};

#define UI_ELEMENT_VISIBLE 0x3

enum TransformType : DWORD
{
    Hide = 1,
    SetModel = 4,
    SetPos = 6,
    SetTextFromBuffer = 0x1D,
    SetTextFromIds = 0x1E,
    TypingEffect = 0x50
};

struct FlUiElement
{
    BYTE x04[0x68];
    BYTE flags; // 0x6C

    FILL_VFTABLE(0)
    FILL_VFTABLE(1)
    FILL_VFTABLE(2)
    virtual void Vftable_x30();
    virtual void Render(); // 0x34
    virtual void Vftable_x38();
    virtual void Vftable_x3C();
    FILL_VFTABLE(4)
    FILL_VFTABLE(5)
    FILL_VFTABLE(6)
    FILL_VFTABLE(7)
    FILL_VFTABLE(8)
    FILL_VFTABLE(9)
    virtual void Vftable_xA0();
    virtual void Vftable_xA4();
    virtual int Transform(TransformType type, DWORD param1, DWORD param2); // 0xA0

    inline bool IsVisible()
    {
        return (flags & UI_ELEMENT_VISIBLE) == UI_ELEMENT_VISIBLE;
    }
};

#define NN_PREFERENCES_NEW_DATA 0x98C

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

    bool SetResolution(UINT width, DWORD unk, UINT height);
};

void StopSound(BYTE soundId);
void StartSound(BYTE soundId);

Waypoint* GetWaypoint(int index);

struct WaypointWatcher
{
    bool GetCurrentWaypointInfo(bool& isPlayerWaypoint, int& waypointIndex);
};

#define WAYPOINT_WATCHER (*((WaypointWatcher**) 0x674BC8))

IObjRW* GetPlayerIObjRW();
CShip* GetPlayerShip();
CShip* GetPlayerShipSafe();

bool AreIObjRWsInSameGroup(const IObjRW& o1, const IObjRW& o2);
bool AreShipsInSameGroup(const CShip* ship1, const CShip* ship2);

bool IsSimpleUnvisited(const CSimple& simple);
BYTE GetSimpleVisitedValue(const CSimple& simple);
UINT GetIdsForUnvisitedSimple(const CSimple& simple);

UINT GetCShipOrCEqObjName(const CEqObj &eqObj);

UINT GetFlStringFromResources(DWORD resourcesHandle, UINT ids, LPWSTR buffer, UINT bufferLen);

inline UINT GetFlString(UINT ids, LPWSTR buffer, UINT bufferLen)
{
    return GetFlStringFromResources(FL_RESOURCES_HANDLE, ids, buffer, bufferLen);
}

class RenderDisplayList
{};

void AppendXmlWsToRdlEx(LPCWSTR ws, UINT wsLen, RenderDisplayList& rdl, DWORD flags);

inline void AppendXmlWsToRdl(LPCWSTR ws, RenderDisplayList& rdl)
{
    AppendXmlWsToRdlEx(ws, wcslen(ws), rdl, 0);
}

#define SHIP_TRADER_SHIP_AMOUNT 3
#define SHIP_TRADER_PLAYER_SHIP_INDEX -2
#define SHIP_TRADER_NONE_SELECTED_INDEX -1

// 0x370 = ShipTrader3DShip*
struct NN_ShipTrader
{
    BYTE x00[0x3CC];
    int shipCount; // 0x3CC
    int selectedShipIndex; // 0x3D0
    BYTE x3D4[0x24];
    float playerReputationWithBaseOwners; // 0x3F8
    int shipStatuses[SHIP_TRADER_SHIP_AMOUNT]; // 0x3FC, basically enums for available, rep too low, or level too low
    BYTE x408[0x40];
    int shipRepPercentages[SHIP_TRADER_SHIP_AMOUNT]; // 0x448

    void StoreShipRepRequirement(int shipIndex, float repRequirement);
    LPWSTR PrintFmtShipRepRequirement();
    PBYTE SwapShipRepPercentages(PBYTE rhsShipStatusAddr);
};

void ExpandNNShipTraderObjMemory();

struct FLCursor
{
    float xPos, yPos, distFromZero;
};

struct ServerFilterDialog
{
    bool OnFrameUpdate_Hook();
};

double GetDeltaTime();
void UpdateDeltaTime();
void UpdateDeltaTimeAndUpTime();

UINT GetNumOfActiveMissionObjectives();

struct Transform
{
    Matrix rot;
    Vector pos; // 0x24
};

struct Camera
{
    BYTE x04[0x24];
    Vector pos; // 0x28
    BYTE x34[0x8C];
    PBYTE watchable; // 0xC0
    BYTE xC4[0x88];
    Vector shipOffset; // 0x14C
    BYTE x158[0x7C];
    float angularAcceleration;      // 0x1D4
    float angularSlerpMultiplier;   // 0x1D8
    float horizontalTurnAngle;      // 0x1DC (per degree)
    float verticalTurnUpAngle;      // 0x1E0
    float verticalTurnDownAngle;    // 0x1E4 (per degree)
    float turnLookAheadSlerpAmount; // 0x1E8 (per degree)
    DWORD x1EC;
    float distFromObj; // 0x1F0
    float x1F4;
    float x1F8;
    float x1FC;

    const IObjRW* GetTarget() const;

    FILL_VFTABLE(0)
    FILL_VFTABLE(1)
    FILL_VFTABLE(2)
    FILL_VFTABLE(3)
    FILL_VFTABLE(4)
    FILL_VFTABLE(5)
    virtual void Vftable_x60();
    virtual void Vftable_x64();
    virtual void Vftable_x68();
    virtual void MainUpdate(float deltaTime); // 0x6C
};

#define TURRET_VIEW_CAMERA ((Camera*) 0x678F60)
#define CINEMATICS_CAMERA ((Camera*) 0x679634)
#define ACTIVE_CAMERA (*(Camera**) 0x6164DC)

BOOL IsPlayerInCutscene();

struct HUD_Target
{
    BYTE x00[0x494];
    FlUiElement* targetBaseBackground; // 0x494
    BYTE x498[0x38];
    FlUiElement* targetScanButton; // 0x4D0
    FlUiElement* targetTractorButton; // 0x4D4
    FlUiElement* targetTradeButton; // 0x4D8
    FlUiElement* targetPreviousButton; // 0x4DC
    FlUiElement* targetNextButton; // 0x4E0
    FlUiElement* targetCommButton; // 0x4E4
    FlUiElement* tradeRequestGroupButton; // 0x4E8
    FlUiElement* targetCloseButton; // 0x4EC
    BYTE x4F0[0x11C];
    FlUiElement* formationList; // 0x60C
    BYTE x610[0x8];
    bool isPlayerInFormation; // 0x618
};
