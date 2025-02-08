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

private:
    typedef void (NN_Dealer::*PrintFmtStrPurchaseInfo)(UINT idsPurchaseInfo, int fmtValue);
};

#define SHIP_TRADER_SHIP_AMOUNT 3

struct NN_ShipTrader
{
    BYTE x00[0x448];
    int shipRepPercentages[SHIP_TRADER_SHIP_AMOUNT];

    void StoreShipLevelRequirement(PBYTE shipListPtr, float repRequirement);
};

inline int GetRepPercentage(float repValue)
{
    return static_cast<int>(repValue * 100.0f);
}
