#include "rep_requirements.h"
#include "utils.h"
#include "Freelancer.h"
#include "fl_func.h"
#include <stdio.h>

#define NAKED __declspec(naked)

UINT insufficientRepIds = 1564;

FL_FUNC(void NN_Dealer::PrintFmtStrPurchaseInfo(UINT idsPurchaseInfo, int fmtValue), 0x47FD50)

// Converts the reputation value to a percentage.
int GetRepPercentage(float repValue)
{
    return static_cast<int>(repValue * 100.0f);
}

void NN_Dealer::PrintFmtStrPurchaseInfo_Hook(UINT idsPurchaseInfo, const DealerStack& stack)
{
    static BYTE& fmtValIsZeroCheck = GetValue<BYTE>(0x47FE86);
    BYTE originalCheckValue = fmtValIsZeroCheck;
    fmtValIsZeroCheck = 0xEB; // allow the rep percentage to be printed if it's 0
    PrintFmtStrPurchaseInfo(idsPurchaseInfo, GetRepPercentage(stack.repRequired));
    fmtValIsZeroCheck = originalCheckValue; // restore the original value to prevent other 0's from being unintentionally printed
}

NAKED void GetShipRepRequirement_Hook()
{
    #define STORE_SHIP_REP_REQUIREMENT_RET_ADDR 0x4B9469

    __asm {
        mov [esi+0xC], eax                                  // overwritten instruction #1
        push ecx
        fst dword ptr [esp]                                 // shipLevelRequirement
        push ebp                                            // shipIndex
        mov ecx, ebx                                        // NN_ShipTrader
        call NN_ShipTrader::StoreShipRepRequirement
        mov eax, [esp+0x10]                                 // overwritten instruction #2
        mov ecx, STORE_SHIP_REP_REQUIREMENT_RET_ADDR
        jmp ecx
    }
}

// Calculates the ship index and stores the rep requirement as a percentage in the right location.
void NN_ShipTrader::StoreShipRepRequirement(int shipIndex, float repRequirement)
{
    // This code is run in a loop from 0 to shipCount - 1, so the shipIndex should always be valid.
    this->shipRepPercentages[shipIndex] = GetRepPercentage(repRequirement);
}

LPWSTR NN_ShipTrader::PrintFmtShipRepRequirement()
{
    GetFlString(insufficientRepIds, FL_BUFFER_1, FL_BUFFER_LEN);

    // The selectedShipIndex is always correctly calculated before this code is called.
    swprintf_s(FL_BUFFER_2, FL_BUFFER_LEN, FL_BUFFER_1, shipRepPercentages[selectedShipIndex]);
    return FL_BUFFER_2;
}

NAKED void PrintShipRepRequirement_Hook()
{
    #define PRINT_SHIP_REP_REQUIREMENT_RET_ADDR 0x4B9017

    __asm {
        mov ecx, esi                                    // NN_ShipTrader*
        call NN_ShipTrader::PrintFmtShipRepRequirement
        push eax                                        // buffer
        push 0x1D                                       // 0x1D means print from buffer, 0x1E means print from IDS
        mov eax, PRINT_SHIP_REP_REQUIREMENT_RET_ADDR
        jmp eax
    }
}

PBYTE NN_ShipTrader::SwapShipRepPercentages(PBYTE rhsShipStatusAddr)
{
    #define SHIP_STATUS_PTR_START (offsetof(NN_ShipTrader, shipStatuses))
    int rhsShipIndex = (rhsShipStatusAddr - (PBYTE) this - SHIP_STATUS_PTR_START) / sizeof(int);

    // Swap the left-hand side and the right-hand side.
    std::swap(shipRepPercentages[rhsShipIndex - 1], shipRepPercentages[rhsShipIndex]);

    return rhsShipStatusAddr; // restore eax
}

// Fixes the ship rep percentages being wrong when FL reorders the ships.
// FL does a stable sort on the ships based on their availability.
// This hook is called every time FL swaps two ships as part of the sorting algorithm.
// We swap the ship rep percentages to keep them in sync with FL's ship ordering.
NAKED void SwapShips_Hook()
{
    // One could do "push edi" to send the lhsShipIndex directly to the function,
    // but the rhsShipIndex can be calculated from rhsShipStatusAddr.
    __asm {
        mov ecx, ebx                                    // NN_ShipTrader*
        push eax                                        // rhsShipStatusAddr
        call NN_ShipTrader::SwapShipRepPercentages
        mov ecx, [eax+0x14]                             // overwritten instruction #1
        xor dl, dl                                      // overwritten instruction #2
        ret
    }
}

// In FL there exists the string "You must be on friendlier terms to purchase this."
// which gets printed in the Dealer menu when you do not meet the requirements to purchase
// the selected item. The function that's called to print this supports one additional argument
// that can be used to replace a format specifier in the provided IDS.
// By default the friendlier terms string gets printed with the integer 0 as a dummy argument.
// This code replaces that 0 with the reputation required as a percentage from -100 to 100.
// If the "friendlier terms" IDS is modified to have "%d" included, then that percentage will be printed too.
// Printing this value for the ships is more involving as it requires the value to be format-printed manually.
// Moreover, the required ship reputation values have to be saved somewhere as Freelancer's original code doesn't do this.
void InitPrintRepRequirements()
{
    #define REP_REQUIREMENTS_NOT_MET_ADDR 0x480739
    #define GET_SHIP_REQUIREMENT_ADDR 0x4B9462
    #define PRINT_SHIP_REQUIREMENT_ADDR 0x4B9010
    #define SWAP_SHIPS_ADDR 0x4B9545

    insufficientRepIds = GetValue<UINT>(0x4B9011); // 1564 by default

    Hook(REP_REQUIREMENTS_NOT_MET_ADDR + 0x9, &NN_Dealer::PrintFmtStrPurchaseInfo_Hook, 5);
    Patch<WORD>(REP_REQUIREMENTS_NOT_MET_ADDR, 0x9054); // push esp followed by nop (replaces param 0 with a stack pointer)

    ExpandNNShipTraderObjMemory();

    Hook(GET_SHIP_REQUIREMENT_ADDR, GetShipRepRequirement_Hook, 7, true);
    Hook(PRINT_SHIP_REQUIREMENT_ADDR, PrintShipRepRequirement_Hook, 7, true);

    Hook(SWAP_SHIPS_ADDR, SwapShips_Hook, 5);
}
