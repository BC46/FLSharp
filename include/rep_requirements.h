#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void InitPrintRepRequirements();

struct DealerStack
{
    BYTE x00[0x24];
    float repRequired;
};

struct NN_Dealer
{
    void PrintFmtStrPurchaseInfo_Hook(UINT idsPurchaseInfo, DealerStack* stack);
    void PrintFmtStrPurchaseInfo(UINT idsPurchaseInfo, int fmtValue);
};

#define SHIP_TRADER_SHIP_AMOUNT 3

struct NN_ShipTrader
{
    BYTE x00[0x3D0];
    int selectedShipIndex; // 0x3D0
    BYTE x3D4[0x74];
    int shipRepPercentages[SHIP_TRADER_SHIP_AMOUNT]; // 0x448

    void StoreShipRepRequirement(PBYTE shipListPtr, float repRequirement);
    LPWSTR NN_ShipTrader::PrintFmtShipRepRequirement();
};

inline int GetRepPercentage(float repValue)
{
    return static_cast<int>(repValue * 100.0f);
}
