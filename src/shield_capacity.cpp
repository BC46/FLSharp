#include "shield_capacity.h"
#include "Common.h"
#include "utils.h"

// Replaces the "ftol" function.
long __fastcall GetShieldCapcity_Hook(const Archetype::ShieldGenerator &shield, float maxCapacity)
{
    // The calculation can be more efficient but to avoid possible differences in rounding, I'm using the same code as FL.
    float shieldCapacity = maxCapacity - (float) ((long) (shield.offlineThreshold * maxCapacity));

    if (shieldCapacity < 0)
        shieldCapacity = 0;

    return (long) shieldCapacity;
}

// In Freelancer, the actual online shield capacity is reduced by the offline_threshold value.
// However, the shield infocards simply show the max_capacity as the shield capacity.
// This code ensures the offline_threshold is taken into account when the value is printed.
void InitShieldCapacityFix()
{
    #define GET_MAX_SHIELD_CAPACITY_ADDR 0x485055
    #define MAX_SHIELD_CAPACITY_FTOL_ADDR 0x48505B

    Patch<WORD>(GET_MAX_SHIELD_CAPACITY_ADDR, 0xB1FF); // fld dword [ecx+0x94] -> push dword [ecx+0x94]
    Hook(MAX_SHIELD_CAPACITY_FTOL_ADDR, GetShieldCapcity_Hook, 5);
}
