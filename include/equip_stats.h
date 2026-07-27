#include "Common.h"

struct NN_Inventory
{
    void RefreshPlayerInventory_Hook(EquipDescList& list, DWORD unused, bool unk);
    void RefreshDealerInventory_Hook();
};

void InitEquipStatFlickerFix();

void CleanupEquipStatFlickerFix();
