#include "weapon_anim.h"
#include "utils.h"

DWORD setModelCallAddr = 0;

// There exist many animations for the weapon models in Freelancer (e.g. barrels rotating or moving back and forth while shooting).
// However, despite all weapon animations already being defined correctly in the ini files, there is a bug in engbase.dll that prevents these animations from playing properly.
// In vanilla FL the internal weapon animation structs have their model set to their actual gun, but this way they don't work.
// The fix is to set the animation structs model to its parent (the ship hull). With this the animations will play correctly.
bool EngAnimation::SetModel_Hook(PDWORD unk, ModelBinary* model)
{
    ModelBinary* currentModel = model;

    // Find the parent (ship hull) of the weapon model.
    while (currentModel)
    {
        // If the parent has been found, ensure this is used as the model for the animation, but keep checking for greater parents.
        if (currentModel->type == MODELTYPE_SHIPHULL)
        {
            model = currentModel;
        }

        currentModel = currentModel->parent;
    }

    // Call original function.
    SetModel setModelFunc = GetFuncDef<SetModel>(setModelCallAddr);
    return (this->*setModelFunc)(unk, model);
}

// This hook allows for e.g. the Cruiser forward gun animation to work without having to modify the model.
// Normally in FL the gun .cmp files have their gun animation included.
// However, the cruiser gun model is part of the Cruiser ship model itself, and thus the animation is also in the ship model.
// In vanilla FL it's not possible to call ship animations when firing a gun.
// This hook allows an animation to be played on the parent of the gun if there is a leading underscore in the animation name (_).
int IAnimation2::Open_Hook(LPCSTR animationScript, int scriptIndex, CAttachedEquip *equip)
{
    // If the animation script has a leading underscore, open the animation on the parent of the equipment.
    if (animationScript && animationScript[0] == '_')
    {
        // Remove the leading underscore.
        ++animationScript;

        if (CObject* parent = equip->parent)
        {
            // Open the animation on the parent.
            return Open(parent->get_archetype()->scriptIndex, parent->engineInstance, animationScript);
        }
    }

    // Open the animation on the attached equipment (normal routine).
    return Open(scriptIndex, equip->GetRootIndex(), animationScript);
}

// Fixes the weapon animations and allows weapons to play ship animations (e.g. wings).
void InitWeaponAnimFix()
{
    #define SET_MODEL_FUNC_FILE_OFFSET_ENGBASE 0xADC0
    #define SET_MODEL_CALL_FILE_OFFSET_ENGBASE 0xB83F
    #define GET_ROOT_INDEX_CALL_ADDR 0x52C8AF
    #define PUSH_ZERO_ADDR 0x52C8BF
    #define ANIM_OPEN_CALL_ADDR 0x52C8C2

    DWORD engbaseHandle = (DWORD) GetModuleHandleA("engbase.dll");

    if (engbaseHandle)
    {
        setModelCallAddr = engbaseHandle + SET_MODEL_FUNC_FILE_OFFSET_ENGBASE;
        Hook(engbaseHandle + SET_MODEL_CALL_FILE_OFFSET_ENGBASE, &EngAnimation::SetModel_Hook, 5);
    }

    // Setup for IAnimation2::Open hook
    BYTE rootIndexPatch[] = { 0x51, 0x55, 0x90, 0x90, 0x90, 0x90 }; // Replace GetRootIndex call with push ecx + push ebx
    Patch(GET_ROOT_INDEX_CALL_ADDR, &rootIndexPatch, sizeof(rootIndexPatch));
    Patch_WORD(PUSH_ZERO_ADDR, 0x9090); // Nop out two zero pushes
    Patch_BYTE(ANIM_OPEN_CALL_ADDR, 0x90); // Nop another instruction

    Hook(ANIM_OPEN_CALL_ADDR + 1, &IAnimation2::Open_Hook, 5);
}
