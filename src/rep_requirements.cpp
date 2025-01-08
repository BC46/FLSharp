#include "rep_requirements.h"
#include "utils.h"

void NN_Dealer::PrintFmtStrPurchaseInfo_Hook(UINT idsPurchaseInfo, DealerStack* stack)
{
    // Get the reputation percentage from the stack (custom variable)
    int repPercentage = static_cast<int>(stack->repRequired * 100.0f);

    // Call the original function.
    PrintFmtStrPurchaseInfo ogFunc = GetFuncDef<PrintFmtStrPurchaseInfo>(0x47FD50);
    (this->*ogFunc)(idsPurchaseInfo, repPercentage);
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

    Hook(REP_REQUIREMENTS_NOT_MET_ADDR + 0x9, &NN_Dealer::PrintFmtStrPurchaseInfo_Hook, 5);
    Patch_WORD(REP_REQUIREMENTS_NOT_MET_ADDR, 0x9054); // push esi followed by nop
}
