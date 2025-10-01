#pragma once

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
    void PrintFmtStrPurchaseInfo_Hook(UINT idsPurchaseInfo, const DealerStack& stack);
    void PrintFmtStrPurchaseInfo(UINT idsPurchaseInfo, int fmtValue);
};

inline int GetRepPercentage(float repValue)
{
    return static_cast<int>(repValue * 100.0f);
}
