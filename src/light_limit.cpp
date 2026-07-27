#include "light_limit.h"
#include "utils.h"
#include "logger.h"
#include "fl_func.h"
#include "Freelancer.h"

#define MAX_ACTIVE_LIGHTS_MINIMUM 4
#define MAX_ACTIVE_LIGHTS_LIMIT 10

FL_FUNC(UINT CreateLights(PVOID* lightBuffer, int bufferLen, PVOID systemLights, float unk1, bool unk2), 0x537F40)
FL_FUNC(void HandleLights(PVOID* lightBuffer, UINT lightAmount, const Camera& camera, PVOID systemLights), 0x4102C0)

int maxActiveLights = MAX_ACTIVE_LIGHTS_MINIMUM;

// Gets called when Freelancer obtains the device caps for the first time.
// This is when we want to get the MaxActiveLights device cap.
long STDCALL IDirect3DDevice8::GetDeviceCaps_Hook(D3DCAPS8* pCaps)
{
    long result = GetDeviceCaps(pCaps);

    if (result == S_OK)
    {
        // It is unclear what happens when the limit is higher than 10.
        // 10 seems to be the maximum that D3D8 returns.
        if (pCaps->MaxActiveLights >= MAX_ACTIVE_LIGHTS_MINIMUM && pCaps->MaxActiveLights <= MAX_ACTIVE_LIGHTS_LIMIT)
            maxActiveLights = (int) pCaps->MaxActiveLights;
    }

    return result;
}

// Create and handle the lights with our larger buffer and maxActiveLights value.
// Inspired by: https://github.com/HaydnTrigg/Liberty/commit/7aaff502fccd2e5b015c21b1d6f8df829cb40473
void CreateAndHandleLights(const Camera& camera, PVOID systemLights, float unk1, bool unk2)
{
    PVOID lightBuffer[MAX_ACTIVE_LIGHTS_LIMIT];

    UINT lightAmount = CreateLights(lightBuffer, maxActiveLights, systemLights, unk1, unk2);
    HandleLights(lightBuffer, lightAmount, camera, systemLights);
}

// Freelancer has an infamous simultaneous active light limit of just four.
// This means that if there are more than four light sources active, then only the first four will work.
// D3D8 has a hard limit of 10 (on most devices), so FL's limit can be safely increased to this amount.
void InitIncreaseLightLimit()
{
    DWORD rp8Handle = (DWORD) GetModuleHandle("rp8.dll");

    if (!rp8Handle)
    {
        Logger::PrintModuleError("InitIncreaseLightLimit", "rp8.dll");
        return;
    }

    #define GET_DEVICE_CAPS_RP8_FILE_OFFSET (0x9112)
    Patch<BYTE>(rp8Handle + GET_DEVICE_CAPS_RP8_FILE_OFFSET, 0x52); // push edx (IDirect3DDevice8*)
    Hook(rp8Handle + GET_DEVICE_CAPS_RP8_FILE_OFFSET + 1, &IDirect3DDevice8::GetDeviceCaps_Hook, 5);

    #define LIGHT_LIMIT_VALUE_ADDR (0x55566F)
    #define GET_LIGHT_BUFFER_ADDR (0x55566A)
    #define CREATE_LIGHTS_CALL_ADDR (0x555671)

    int ogMaxActiveLights = (int) GetValue<char>(LIGHT_LIMIT_VALUE_ADDR);
    if (ogMaxActiveLights >= MAX_ACTIVE_LIGHTS_MINIMUM && ogMaxActiveLights <= MAX_ACTIVE_LIGHTS_LIMIT)
        maxActiveLights = ogMaxActiveLights; // save FL's default max active lights
    Nop(LIGHT_LIMIT_VALUE_ADDR - 1, 2); // wipe out unneeded hardcoded buffer length

    // Push Camera& instead of lightBuffer onto stack.
    Patch<BYTE>(GET_LIGHT_BUFFER_ADDR, 0x8B); // mov eax
    Patch<BYTE>(GET_LIGHT_BUFFER_ADDR + 3, 0x44); // [esp + 0x44]

    SetRelPointer(CREATE_LIGHTS_CALL_ADDR + 1, CreateAndHandleLights);

    // Return directly after the hook.
    PatchBytes(CREATE_LIGHTS_CALL_ADDR + 5, { 0x83, 0xC4, 0x14, 0x5F, 0x5D, 0x5B, 0x83, 0xC4, 0x1C, 0xC3 });

}
