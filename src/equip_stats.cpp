#include "equip_stats.h"
#include "utils.h"

#define NAKED __declspec(naked)

// int prevUnmountedEquipAmount = -1;

// NAKED void UpdateUnmountedEquipAmount_Hook()
// {
//     #define UPDATE_UNMOUNTED_EQUIP_AMOUNT_RET_ADDR 0x47DF94

//     __asm {
//         mov eax, [edi+0x3C8]
//         mov prevUnmountedEquipAmount, eax       // store unmounted equip amount
//         mov [edi+0x3C8], ebp                    // overwritten instruction
//         mov eax, UPDATE_UNMOUNTED_EQUIP_AMOUNT_RET_ADDR
//         jmp eax
//     }
// }

// #define UPDATE_INFOCARD_ON_MOUNT_ADDR 0x47E059
// #define CLEAR_INFOCARD_ADDR 0x47E068
// #define PRESERVE_INFOCARD_ADDR 0x47E0A9

// NAKED void UpdateInfocardOnMountCheck_Hook()
// {
//     __asm {
//     //     mov eax, [esp+0x1C]     // overwritten instruction #1
//     //     test eax, eax           // overwritten instruction #2
//     //     je clear
//     //     cmp ebp, prevUnmountedEquipAmount
//     //     je preserve
//     //     mov ecx, UPDATE_INFOCARD_ON_MOUNT_ADDR
//     //     jmp ecx
//     // clear:
//     //     mov ecx, CLEAR_INFOCARD_ADDR
//     //     jmp ecx
//     // preserve:
//     //     mov ecx, PRESERVE_INFOCARD_ADDR
//     //     jmp ecx

//         cmp ebp, prevUnmountedEquipAmount
//         jne preserve
//         mov eax, [esp+0x1C]     // overwritten instruction #1
//         test eax, eax           // overwritten instruction #2
//         je clear
//         mov ecx, UPDATE_INFOCARD_ON_MOUNT_ADDR
//         jmp ecx
//     clear:
//         mov ecx, CLEAR_INFOCARD_ADDR
//         jmp ecx
//     preserve:
//         mov ecx, PRESERVE_INFOCARD_ADDR
//         jmp ecx
//     }
// }

// bool checkvalue = false;

// NAKED void UpdateInfocardOnUnmountCheck_Hook()
// {
//     __asm {
//     //     mov eax, [esp+0x50]     // overwritten instruction #1
//     //     cmp eax, ebx            // overwritten instruction #2
//     //     je no_update
//     //     cmp ebp, prevUnmountedEquipAmount
//     //     jne preserve
//     //     mov ecx, UPDATE_INFOCARD_ON_UNMOUNT_ADDR
//     //     jmp ecx
//     // no_update:
//     //     mov ecx, UpdateInfocardOnMountCheck_Hook
//     //     jmp ecx
//     // preserve:
//     //     mov ecx, PRESERVE_INFOCARD_ADDR
//     //     jmp ecx
//         cmp checkvalue, 1
//         je preserve
//         mov eax, [esp+0x50]     // overwritten instruction #1
//         cmp eax, ebx            // overwritten instruction #2
//         je mount_check
//         mov ecx, UPDATE_INFOCARD_ON_UNMOUNT_ADDR
//         jmp ecx
//     mount_check:
//         mov ecx, UPDATE_INFOCARD_ON_MOUNT_CHECK_ADDR
//         jmp ecx
//     preserve:
//         mov checkvalue, 0
//         mov ecx, PRESERVE_INFOCARD_ADDR
//         jmp ecx
//     }
// }

struct NN_Inventory
{
    void RefreshPlayerInventory_Hook(DWORD a, DWORD b, DWORD c);
    void RefreshDealerInventory_Hook();
};

void (NN_Inventory::*RefreshPlayerInventory_Original)(DWORD a, DWORD b, DWORD c);
void (NN_Inventory::*RefreshDealerInventory_Original)();

int count1 = 0;
bool hooked1 = false;

// Checks if RefreshInventory was recursively called.
// If so, do not allow the
void NN_Inventory::RefreshPlayerInventory_Hook(DWORD a, DWORD b, DWORD c)
{
    ++count1;

    (this->*RefreshPlayerInventory_Original)(a, b, c);

    if (count1 >= 2)
    {
        PatchBytes(0x47E00D, { 0xE9, 0x97, 0x00, 0x00, 0x00, 0x90, 0x90, 0x90 });
        hooked1 = true;
    }
    else if (hooked1)
    {
        PatchBytes(0x47E00D, { 0x74, 0x15, 0x8B, 0x97, 0x48, 0x03, 0x00, 0x00 });
        hooked1 = false;
    }

    --count1;
}

int count2 = 0;
bool hooked2 = false;
bool quantity0 = false;
int rounds = 3;

#define UPDATE_INFOCARD_DEALER_ITEM_CHECK 0x482C3D

void NN_Inventory::RefreshDealerInventory_Hook()
{
    // lastSelectedItem = 0;
    if (count2 == 0 && quantity0 && rounds == 3)
    {
        Patch<BYTE>(0x482C43, 0xEB);
        rounds = 2;
    }

    ++count2;

    (this->*RefreshDealerInventory_Original)();

    --count2;

    if (count2 == 0 && quantity0)
    {
        if (--rounds == 0)
        {
            Patch<BYTE>(0x482C43, 0x74);
            quantity0 = false;
            rounds = 3;
        }
    }

    // if (count2 == 0)
    // {
    //     Hook(UPDATE_INFOCARD_DEALER_ITEM_CHECK, UpdateInfocardDealerItemCheck_Hook, 6, true);
    // }
}

#define UPDATE_INFOCARD_DEALER_ITEM 0x482C45
#define PRESERVE_INFOCARD_DEALER_ITEM 0x482C5B

// PDWORD lastSelectedItem = 0;

// NAKED void UpdateInfocardDealerItemCheck_Hook()
// {
//     __asm {
//         mov eax, [esp+0x1C]         // overwritten instruction #1
//         test eax, eax               // overwritten instruction #2
//         je preserve
//         mov lastSelectedItem, eax
//         mov ecx, UPDATE_INFOCARD_DEALER_ITEM
//         jmp ecx
//     preserve:
//         mov ecx, PRESERVE_INFOCARD_DEALER_ITEM
//         jmp ecx
//     }
// }

NAKED void SubtractQuantityOfBoughtItem_Hook()
{
    __asm {
        sub eax, edi
        mov [ebx+0x1C], eax
        test eax, eax
        sete quantity0
        mov eax, 0x47EB3A
        jmp eax
    }
}

void InitEquipStatFlickerFix()
{
    #define UPDATE_UNMOUNTED_EQUIP_AMOUNT_ADDR 0x47DF8E
    //Hook(UPDATE_UNMOUNTED_EQUIP_AMOUNT_ADDR, UpdateUnmountedEquipAmount_Hook, 6, true);

    #define UPDATE_INFOCARD_ON_UNMOUNT_CHECK_ADDR 0x47E036
    //Hook(UPDATE_INFOCARD_ON_UNMOUNT_CHECK_ADDR, UpdateInfocardOnUnmountCheck_Hook, 6, true);

    RefreshPlayerInventory_Original = Trampoline(0x47D2F0, &NN_Inventory::RefreshPlayerInventory_Hook, 6);
    RefreshDealerInventory_Original = Trampoline(0x482A90, &NN_Inventory::RefreshDealerInventory_Hook, 6);

    #define UPDATE_INFOCARD_DEALER_ITEM_CHECK 0x482C3D
    //Hook(UPDATE_INFOCARD_DEALER_ITEM_CHECK, UpdateInfocardDealerItemCheck_Hook, 6, true);

    //Hook(UPDATE_INFOCARD_ON_MOUNT_CHECK_ADDR, UpdateInfocardOnMountCheck_Hook, 6, true);
    Hook(0x0047EB35, SubtractQuantityOfBoughtItem_Hook, 5, true);
}
