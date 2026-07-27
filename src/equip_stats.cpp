#include "equip_stats.h"
#include "utils.h"

#define NAKED __declspec(naked)

#define UPDATE_PLAYER_INV_INFOCARD_CHECK_ADDR ((WORD*) 0x47E00D)
#define UPDATE_DEALER_INV_INFOCARD_CHECK_ADDR ((BYTE*) 0x482C43)

void (NN_Inventory::*RefreshPlayerInventory_Original)(EquipDescList& list, DWORD unused, bool unk);
void (NN_Inventory::*RefreshDealerInventory_Original)();

int playerInvCount = 0;
bool playerInvPatched = false;

// Checks if RefreshInventory was recursively called.
// If so, don't allow the player infocard to be updated.
void NN_Inventory::RefreshPlayerInventory_Hook(EquipDescList& list, DWORD unused, bool unk)
{
    ++playerInvCount;

    (this->*RefreshPlayerInventory_Original)(list, unused, unk);

    if (playerInvCount >= 2)
    {
        // If the RefreshPlayerInventory function is called recursively, disable the infocard printing.
        *UPDATE_PLAYER_INV_INFOCARD_CHECK_ADDR = 0x10EB;
        playerInvPatched = true;
    }
    else if (playerInvPatched)
    {
        // Turn the infocard printing back on after the recursive call's caller is finished.
        *UPDATE_PLAYER_INV_INFOCARD_CHECK_ADDR = 0x1574;
        playerInvPatched = false;
    }

    --playerInvCount;
}

bool quantity0 = false;
int dealerInvCount = 0;
int iterations = 3;

// The flickering happens in the dealer menu when an item is purchased which is now no longer available.
// Usually because it has a limited quantity while the player purchases all copies
void NN_Inventory::RefreshDealerInventory_Hook()
{
    // This one is a bit more tricky than the player inventory as the dealer inventory prints
    // the "wrong" infocard for two iterations after the quantity 0 equipment has been purchased.
    // So we keep track of the number of iterations when patching and restoring.
    if (dealerInvCount == 0 && quantity0 && iterations >= 3)
    {
        *UPDATE_DEALER_INV_INFOCARD_CHECK_ADDR = 0xEB;
        iterations = 2;
    }

    ++dealerInvCount;
    (this->*RefreshDealerInventory_Original)();
    --dealerInvCount;

    if (dealerInvCount == 0 && quantity0)
    {
        if (--iterations <= 0)
        {
            *UPDATE_DEALER_INV_INFOCARD_CHECK_ADDR = 0x74;
            quantity0 = false;
            iterations = 3;
        }
    }
}

// Check if an item was bought which is now no longer available (quantity 0).
NAKED void SubtractQuantityOfBoughtItem_Hook()
{
    #define SUB_QUANTITY_OF_BOUGHT_ITEM_RET_ADDR 0x47EB3A

    __asm {
        sub eax, edi        // overwritten instruction #1
        mov [ebx+0x1C], eax // overwritten instruction #2
        test eax, eax
        sete quantity0      // test if the quantity is now 0
        mov eax, SUB_QUANTITY_OF_BOUGHT_ITEM_RET_ADDR
        jmp eax
    }
}

// If you mount or unmount equipment in the inventory or dealer menu, the stat infocard will sometimes flicker.
// This happens because for one or two frames when the equipment is already moved, your "cursor" is still in the same place as before,
// and thus it either selects an empty hardpoint, or a another type of equipment. This is then reflected in the stat infocard.
// The same problem happens when you sell an equipment you have and rebuy it. This is because this item has a limited quantity
// and goes away when you buy the last copies.
// We fix both problems by ensuring the infocard is not updated when the flickering happens.
// TODO: I think these solutions area really ugly, but I couldn't find a better way. It would be nice if a better approach could be found.
void InitEquipStatFlickerFix()
{
    ReadWriteProtect((DWORD) UPDATE_PLAYER_INV_INFOCARD_CHECK_ADDR, sizeof(WORD));
    ReadWriteProtect((DWORD) UPDATE_DEALER_INV_INFOCARD_CHECK_ADDR, sizeof(BYTE));

    #define REFERSH_PLAYER_INV_ADDR 0x47D2F0
    RefreshPlayerInventory_Original = Trampoline(REFERSH_PLAYER_INV_ADDR, &NN_Inventory::RefreshPlayerInventory_Hook, 6);

    #define REFERSH_DEALER_INV_ADDR 0x482A90
    RefreshDealerInventory_Original = Trampoline(REFERSH_DEALER_INV_ADDR, &NN_Inventory::RefreshDealerInventory_Hook, 6);

    #define SUB_QUANTITY_OF_BOUGHT_ITEM_ADDR 0x47EB35
    Hook(SUB_QUANTITY_OF_BOUGHT_ITEM_ADDR, SubtractQuantityOfBoughtItem_Hook, 5, true);
}

void CleanupEquipStatFlickerFix()
{
    CleanupTrampoline(RefreshPlayerInventory_Original);
    CleanupTrampoline(RefreshDealerInventory_Original);
}
