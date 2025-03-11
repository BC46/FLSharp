#include "Freelancer.h"
#include "fl_func.h"
#include "utils.h"

FL_FUNC(void StopSound(BYTE soundId), 0x5646E0)
FL_FUNC(void StartSound(BYTE soundId), 0x564650)

FL_FUNC(UINT GetFlStringFromResources(DWORD resourcesHandle, UINT ids, LPWSTR buffer, UINT bufferLen), 0x4347E0)

FL_FUNC(NavMapObj* NeuroNetNavMap::GetHighlightedObject(DWORD unk1, DWORD unk2), 0x496D40)

FL_FUNC(WaypointInfo* WaypointCheck(UINT index), 0x4C46A0)

FL_FUNC(IObjRW* GetPlayerIObjRW(), 0x54BAF0);

FL_FUNC(bool NN_Preferences::InitElements(DWORD unk1, DWORD unk2), 0x4A9790)
FL_FUNC(bool NN_Preferences::SetResolution(UINT width, DWORD unk, UINT height), 0x4B1C00)

void ExpandNNShipTraderObjMemory()
{
    #define NN_SHIPTRADER_OBJ_SIZE_ADDR 0x4B9739
    static bool memoryExpanded = false;

    if (!memoryExpanded)
    {
        // Expand the size of the NN_ShipTrader object if it hasn't been done yet.
        GetValue<UINT>(NN_SHIPTRADER_OBJ_SIZE_ADDR) += sizeof(int[SHIP_TRADER_SHIP_AMOUNT]);
        memoryExpanded = true;
    }
}
