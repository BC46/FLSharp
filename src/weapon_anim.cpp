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
        // If the parent has been found, ensure this is used as the model for the animation.
        if (currentModel->type == MODELTYPE_SHIPHULL)
        {
            model = currentModel;
            break;
        }

        currentModel = currentModel->parent;
    }

    // Call original function.
    SetModel setModelFunc = GetFuncDef<SetModel>(setModelCallAddr);
    return (this->*setModelFunc)(unk, model);
}

void InitWeaponAnimFix()
{
    #define SET_MODEL_FUNC_FILE_OFFSET_ENGBASE 0xADC0
    #define SET_MODEL_CALL_FILE_OFFSET_ENGBASE 0xB83F

    DWORD engbaseHandle = (DWORD) GetModuleHandleA("engbase.dll");

    if (engbaseHandle)
    {
        setModelCallAddr = engbaseHandle + SET_MODEL_FUNC_FILE_OFFSET_ENGBASE;
        Hook(engbaseHandle + SET_MODEL_CALL_FILE_OFFSET_ENGBASE, &EngAnimation::SetModel_Hook, 5);
    }
}
