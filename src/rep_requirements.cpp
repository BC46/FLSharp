#include "rep_requirements.h"
#include "utils.h"
#include "Freelancer.h"
#include <stdio.h>

#define NAKED __declspec(naked)

#define FMT_VAL_IS_ZERO_CHECK 0x47FE86
#define INSUFFICIENT_REP_IDS_PTR 0x4B9011

UINT insufficientRepIds = 1564;

void NN_Dealer::PrintFmtStrPurchaseInfo_Hook(UINT idsPurchaseInfo, DealerStack* stack)
{
    // Call the original function with the rep percentage.
    PrintFmtStrPurchaseInfo ogFunc = GetFuncDef<PrintFmtStrPurchaseInfo>(0x47FD50);

    *((PBYTE) FMT_VAL_IS_ZERO_CHECK) = 0xEB; // allow the rep percentage to be printed if it's 0
    (this->*ogFunc)(idsPurchaseInfo, GetRepPercentage(stack->repRequired));
    *((PBYTE) FMT_VAL_IS_ZERO_CHECK) = 0x75; // restore the original value to prevent other 0's from being unintentionally printed
}

NAKED void GetShipRepRequirement_Hook()
{
    #define STORE_SHIP_REP_REQUIREMENT_RET_ADDR 0x4B9469

    __asm {
        mov [esi+0xC], eax                                  // overwritten instruction #1
        sub esp, 0x4
        fst dword ptr [esp]                                 // shipLevelRequirement
        push esi                                            // shipListPtr
        mov ecx, ebx                                        // NN_ShipTrader
        call NN_ShipTrader::StoreShipRepRequirement
        mov eax, [esp+0x10]                                 // overwritten instruction #2
        mov ecx, STORE_SHIP_REP_REQUIREMENT_RET_ADDR
        jmp ecx
    }
}

// Calculates the ship index and stores the rep requirement as a percentage in the right location.
void NN_ShipTrader::StoreShipRepRequirement(PBYTE shipListPtr, float repRequirement)
{
    #define SHIP_LIST_PTR_START 0x3FC
    int shipIndex = (shipListPtr - (PBYTE) this - SHIP_LIST_PTR_START) / sizeof(int);

    if (shipIndex >= 0 && shipIndex < SHIP_TRADER_SHIP_AMOUNT)
        this->shipRepPercentages[shipIndex] = GetRepPercentage(repRequirement);
}

LPWSTR NN_ShipTrader::PrintFmtShipRepRequirement()
{
    GetFlString(insufficientRepIds, FL_BUFFER_1, FL_BUFFER_LEN);

    if (selectedShipIndex >= 0 && selectedShipIndex < SHIP_TRADER_SHIP_AMOUNT)
        swprintf(FL_BUFFER_2, FL_BUFFER_1, shipRepPercentages[selectedShipIndex]);
    else
        wcscpy(FL_BUFFER_2, FL_BUFFER_1);

    return FL_BUFFER_2;
}

NAKED void PrintShipRepRequirement_Hook()
{
    #define PRINT_SHIP_REP_REQUIREMENT_RET_ADDR 0x4B9017

    __asm {
        mov ecx, esi                                    // NN_ShipTrader
        call NN_ShipTrader::PrintFmtShipRepRequirement
        push eax                                        // FL_BUFFER_2
        push 0x1D                                       // 0x1D means print from buffer, 0x1E means print from IDS
        mov eax, PRINT_SHIP_REP_REQUIREMENT_RET_ADDR
        jmp eax
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
void InitPrintRepRequirements()
{
    #define REP_REQUIREMENTS_NOT_MET_ADDR 0x480739
    #define NN_SHIPTRADER_OBJ_SIZE_ADDR 0x4B9739
    #define GET_SHIP_REQUIREMENT_ADDR 0x4B9462
    #define PRINT_SHIP_REQUIREMENT_ADDR 0x4B9010

    Hook(REP_REQUIREMENTS_NOT_MET_ADDR + 0x9, &NN_Dealer::PrintFmtStrPurchaseInfo_Hook, 5);
    Patch_WORD(REP_REQUIREMENTS_NOT_MET_ADDR, 0x9054); // push esp followed by nop (replaces param 0 with a stack pointer)

    // Expand the size of the NN_ShipTrader object.
    ReadWriteProtect(NN_SHIPTRADER_OBJ_SIZE_ADDR, sizeof(UINT));
    *(PUINT) NN_SHIPTRADER_OBJ_SIZE_ADDR += sizeof(int[SHIP_TRADER_SHIP_AMOUNT]);

    Hook(GET_SHIP_REQUIREMENT_ADDR, GetShipRepRequirement_Hook, 7, true);

    ReadWriteProtect(FMT_VAL_IS_ZERO_CHECK, sizeof(BYTE));

    ReadWriteProtect(INSUFFICIENT_REP_IDS_PTR, sizeof(UINT));
    insufficientRepIds = *((PUINT) INSUFFICIENT_REP_IDS_PTR);
    Hook(PRINT_SHIP_REQUIREMENT_ADDR, PrintShipRepRequirement_Hook, 7, true);
}
