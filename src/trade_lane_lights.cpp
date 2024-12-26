#include "trade_lane_lights.h"
#include "utils.h"

// Hook that gets called each time a tradelane has been disrupted or restored.
// When it is disrupted, we turn off the lights, and when it's restored, we turn on the lights.
void TradeLaneEquipObj::SetLightsState_Hook()
{
    #define CELIGHTEQUIP_CLASS_TYPE 1

    bool activateLights = this->isDisrupted == FALSE;
    CSolar* tradeLaneSolar = this->tradeLaneEquip->solar;

    CEquipTraverser tr = CEquipTraverser(CELIGHTEQUIP_CLASS_TYPE);

    // Loop over all the TLR's light equip objects and turn them on/off.
    while (CEquip* equip = tradeLaneSolar->equipManager.Traverse(tr))
    {
        CELightEquip* lightEquip = CELightEquip::cast(equip);

        if (lightEquip)
        {
            lightEquip->Activate(activateLights);
        }
    }
}

void InitTradeLaneLightsFix()
{
    #define IS_TLR_DISRUPTED_CHECK_ADDR 0x516965
    #define ENABLE_TLR_LIGHTS_CALL_ADDR 0x516978

    Patch_BYTE(IS_TLR_DISRUPTED_CHECK_ADDR, 0xEB); // Redirect trade lane disrupt calls to the hook below as well.

    // FL has legacy code that deallocates all light objects when a tradelane is disrupted and when it is restored it attempts to re-create all the light objects.
    // However, the latter doesn't work in the retail version of FL, resulting in the trade lanes remaining off permanently after one disruption.
    // The legacy code looked overly complicated for such a simple task and there seemed to be no easy way to just "fix" it.
    // Hence new code has been written that simply activates/deactivates the lights based on whether or not the tradelane is disrupted.
    Hook(ENABLE_TLR_LIGHTS_CALL_ADDR, &TradeLaneEquipObj::SetLightsState_Hook, 5);
}
