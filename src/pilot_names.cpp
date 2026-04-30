#include "pilot_names.h"
#include "fl_func.h"
#include "Freelancer.h"
#include "utils.h"

FL_FUNC(size_t GetCShipPilotName(const CSimple &simple, StrBuffer &buffer), 0x5472D0)

FL_FUNC(bool GetSimpleName(const CSimple &simple, StrBuffer &buffer, NameType type, bool unk), 0x4E8100)

// GetCShipPilotName calls CShip::get_pilot_name() which returns a truncated string if the length is large.
// GetSimpleName on the other hand returns the full string, so this function is preferred if the entire name is needed.
// Only problem is that GetSimpleName writes stuff to FL_BUFFER_1 and FL_BUFFER_2.
size_t GetCShipPilotName_Hook(const CSimple &simple, StrBuffer &buffer)
{
    // If it's not a ship, just call GetCShipPilotName, otherwise the next code will think it's a pilot.
    // The function doesn't actually get a name and returns 0 in this case,
    // but it does allocate the buffer if it's empty.
    if ((simple.classType & CSHIP_CLASS_TYPE) != CSHIP_CLASS_TYPE)
        return GetCShipPilotName(simple, buffer);

    bool success = GetSimpleName(simple, buffer, NameType::PilotName, true);

    // GetSimpleName modifies FL_BUFFER_1 and FL_BUFFER_2, so null both buffers
    // because the code that comes after this may assume that the buffers are nulled.
    FL_BUFFER_1[0] = FL_BUFFER_2[0] = '\0';

    // Make the return value compatible with that of GetCShipPilotName.
    // Hence return the string length if successful, otherwise call the original function.
    return success && buffer.str ? wcslen(buffer.str) : GetCShipPilotName(simple, buffer);
}

// Many UI elements in FL call GetCShipPilotName which returns a string with a max buffer length of 24.
// Hence if the pilot name is longer than 23 characters, it will be truncated.
// This doesn't matter ofr player names in MP as those are limited to 23 characters.
// However, with some NPCs I did notice the names being truncated, in particular with some Transport and Corsair pilots who often have long names.
// This issue is fixed by hooking all the GetCShipPilotName calls where we instead call GetSimpleName; this function always returns the full string.
void InitPilotNamesFix()
{
        DWORD getPilotNameCallAddrs[] = {
        0x4756F8, // Current Information window
        0x48AB1F, // Hand over your cargo or I'll open fire window
        0x4CB85F, // Comm text
        //0x4E4400  // Sub-target name in target window (not needed as FL already calls GetSimpleName before this but if that fails it falls back to GetCShipPilotName.)
    };

    for (auto getPilotNameCallAddr : getPilotNameCallAddrs)
        Hook(getPilotNameCallAddr, GetCShipPilotName_Hook, 5);
}
