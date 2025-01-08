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
    void PrintFmtStrPurchaseInfo_Hook(UINT idsPurchaseInfo, DealerStack* Dealerstack);

private:
    typedef void (NN_Dealer::*PrintFmtStrPurchaseInfo)(UINT idsPurchaseInfo, int fmtValue);
};
