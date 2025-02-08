#include "rep_requirements.h"
#include "utils.h"

#define NAKED __declspec(naked)

void NN_Dealer::PrintFmtStrPurchaseInfo_Hook(UINT idsPurchaseInfo, DealerStack* stack)
{
    // Call the original function with the rep percentage.
    PrintFmtStrPurchaseInfo ogFunc = GetFuncDef<PrintFmtStrPurchaseInfo>(0x47FD50);
    (this->*ogFunc)(idsPurchaseInfo, GetRepPercentage(stack->repRequired));
}

NAKED void GetShipRepRequirement_Hook()
{
    #define STORE_SHIP_LEVEl_REQUIREMENT_RET_ADDR 0x4B9469

    __asm {
        mov [esi+0xC], eax                                  // overwritten instruction #1
        sub esp, 0x4
        fst dword ptr [esp]                                 // shipLevelRequirement
        push esi                                            // shipListPtr
        mov ecx, ebx                                        // NN_ShipTrader
        call NN_ShipTrader::StoreShipLevelRequirement
        mov eax, [esp+0x10]                                 // overwritten instruction #2
        mov ecx, STORE_SHIP_LEVEl_REQUIREMENT_RET_ADDR
        jmp ecx
    }
}

// Calculates the ship index and stores the rep requirement as a percentage in the right location.
void NN_ShipTrader::StoreShipLevelRequirement(PBYTE shipListPtr, float repRequirement)
{
    #define SHIP_LIST_PTR_START 0x3FC
    int shipIndex = (shipListPtr - (PBYTE) this - SHIP_LIST_PTR_START) / sizeof(int);

    if (shipIndex >= 0 && shipIndex < SHIP_TRADER_SHIP_AMOUNT)
        this->shipRepPercentages[shipIndex] = GetRepPercentage(repRequirement);
}

// In FL there exists the string "You must be on friendlier terms to purchase this."
// which gets printed in the Dealer menu when you do not meet the requirements to purchase
// the selected item. The function that's called to print this supports one additional argument
// that can be used to replace a format specifier in the provided IDS.
// By default the friendlier terms string gets printed with the integer 0 as a dummy argument.
// This code replaces that 0 with the reputation required as a percentage from -100 to 100.
// If the "friendlier terms" IDS is modified to have "%d" included, then that percentage will be printed too.
void InitPrintRepRequirements()
{
    #define REP_REQUIREMENTS_NOT_MET_ADDR 0x480739
    #define NN_SHIPTRADER_OBJ_SIZE_ADDR 0x4B9739
    #define GET_SHIP_REQUIREMENT_ADDR 0x4B9462

    Hook(REP_REQUIREMENTS_NOT_MET_ADDR + 0x9, &NN_Dealer::PrintFmtStrPurchaseInfo_Hook, 5);
    Patch_WORD(REP_REQUIREMENTS_NOT_MET_ADDR, 0x9054); // push esp followed by nop (replaces param 0 with a stack pointer)

    // Expand the size of the NN_ShipTrader object.
    ReadWriteProtect(NN_SHIPTRADER_OBJ_SIZE_ADDR, sizeof(UINT));
    *(PUINT) NN_SHIPTRADER_OBJ_SIZE_ADDR += sizeof(int[SHIP_TRADER_SHIP_AMOUNT]);

    Hook(GET_SHIP_REQUIREMENT_ADDR, GetShipRepRequirement_Hook, 7, true);
}
