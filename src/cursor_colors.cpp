#include "cursor_colors.h"
#include "utils.h"
#include "Freelancer.h"
#include "fl_func.h"

#include <map>
#include <vector>
#include <memory>
#include <algorithm>

#define FASTCALL __fastcall
#define NAKED __declspec(naked)

#define CURSOR_LIST ((MouseCursor**) 0x616744)
#define CURSOR_LIST_SIZE (*(PUINT) 0x616740)
#define CURRENT_CURSOR (*(MouseCursor**) 0x616858)

#define GROUP_MEMBER_COLOR (*(PDWORD) 0x679B88)
#define TRADE_REQUEST_COLOR (*(PDWORD) 0x679B9C)

const IObjRW *lastSelectedObj = nullptr;

FL_FUNC(void Targetable_Objects::UpdateTargeting(), 0x4F2220)

// We want to reset the lastSelectedObj before the targeting is updated
// to ensure lastSelectedObj never points to invalid memory.
void Targetable_Objects::UpdateTargeting_Hook()
{
    lastSelectedObj = nullptr;
    UpdateTargeting();
}

FL_FUNC(const IObjRW* FindIObjRW(UINT nickname, DWORD unk), 0x05416C0)

// Calling FindIObjRW manually every time we want to check the highlighted object is inefficient,
// so we intercept the call that FL make every frame and save the last selected object.
const IObjRW* FindCurrentSelectedIObjRW_Hook(UINT nickname, DWORD unk)
{
    return lastSelectedObj = FindIObjRW(nickname, unk);
}

// Gets called when FL checks the attitude of the targeted (aim locked) object
NAKED void GetAttitudeOfTarget_Hook()
{
    #define GET_ATTITUDE_OF_TARGET_RET_ADDR 0x4F2465

    __asm {
        test eax, eax
        je skip
        mov lastSelectedObj, eax        // save the targeted (aim locked) object
    skip:
        mov edx, [esp+0x30]             // overwritten instructions
        push eax
        push edx
        mov ecx, GET_ATTITUDE_OF_TARGET_RET_ADDR
        jmp ecx
    }
}


std::map<MouseCursor*, std::shared_ptr<MouseCursor>> groupCursors, tradeRequestCursors;

std::shared_ptr<MouseCursor> CreateCustomCursor(const MouseCursor& originalCursor, DWORD color)
{
    auto result = std::make_shared<MouseCursor>();
    *result = originalCursor;
    result->color = color;

    return result;
}

void FillCustomCursorMaps(std::vector<LPCSTR> cursorNames, LPCSTR neutralCursorName)
{
    std::vector<MouseCursor*> cursors;

    // Find the relevant cursors.
    for (UINT i = 0; i < CURSOR_LIST_SIZE; ++i)
    {
        for (const auto normalCursorName : cursorNames)
        {
            if (strcmp(CURSOR_LIST[i]->nickname, normalCursorName) == 0)
                cursors.push_back(CURSOR_LIST[i]);
        }
    }

    // Get the neutral cursor which we want to copy.
    auto neutralCursorIt = std::find_if(cursors.begin(), cursors.end(),
        [neutralCursorName](const MouseCursor* cursor) {
            return strcmp(cursor->nickname, neutralCursorName) == 0;
        }
    );

    // Create new cursors based on the copied neutral cursor
    // and store them by the original friendly, neutral, and hostile version for easy access.
    if (neutralCursorIt != cursors.end())
    {
        auto groupCursor = CreateCustomCursor(**neutralCursorIt, GROUP_MEMBER_COLOR);
        auto tradeRequestCursor = CreateCustomCursor(**neutralCursorIt, TRADE_REQUEST_COLOR);

        for (const auto cursor : cursors)
        {
            groupCursors.emplace(cursor, groupCursor);
            tradeRequestCursors.emplace(cursor, tradeRequestCursor);
        }
    }
}

void (*InitCursors_Original)();

void InitCursors_Hook()
{
    // This function initializes all the standard cursors.
    // After it has finished, we want to create our custom-colored cursors by copying the existing neutral cursors.
    InitCursors_Original();

    std::vector<LPCSTR> normalCursorNames = { "friendly", "neutral", "hostile" };
    std::vector<LPCSTR> fireCursorNames = { "fire_friendly", "fire_neutral", "fire" };

    FillCustomCursorMaps(normalCursorNames, "neutral");
    FillCustomCursorMaps(fireCursorNames, "fire_neutral");
}

FL_FUNC(void SetCurrentCursor(LPCSTR cursorName, bool unk), 0x41DDE0)

void FASTCALL SetCurrentCustomAimCursor(const Targetable_Objects& to, const IObjRW *highlightedObj, LPCSTR cursorName, bool unk)
{
    // This function updates the CURRENT_CURSOR for targeting (aiming).
    SetCurrentCursor(cursorName, unk);

    // Check if the player can be obtained.
    const IObjRW* player = GetPlayerIObjRW();
    if (!player || player->unk_x1C != 1)
        return;

    // Try to get the target.
    const IObjRW *target = nullptr;
    if (highlightedObj != player && !to.isAimLocking)
    {
        target = highlightedObj;
    }
    else if (lastSelectedObj)
    {
        target = lastSelectedObj;
    }

    if (!target)
        return;

    // If the target has been found, check if it is a player who sent a trade request or is a group member.
    std::map<MouseCursor*, std::shared_ptr<MouseCursor>>* customCursorMap = nullptr;
    if (target->is_player())
    {
        if (target->SentTradeRequest())
            customCursorMap = &tradeRequestCursors;
        else if (AreIObjRWsInSameGroup(*target, *player))
            customCursorMap = &groupCursors;
    }

    // If we found a better suitable custom cursor, set it as the current cursor.
    if (customCursorMap)
    {
        auto it = customCursorMap->find(CURRENT_CURSOR);

        if (it != customCursorMap->end())
        {
            it->second->animState = CURRENT_CURSOR->animState;
            CURRENT_CURSOR = it->second.get();
        }
    }
}

// Gets called when FL changes the current aim cursor.
NAKED void SetCurrentAimCursor_Hook()
{
    __asm {
        mov ecx, ebp                    // Targetable_Objects&
        mov edx, esi                    // IObjRW *highlightedObj
        jmp SetCurrentCustomAimCursor
    }
}

// In Multiplayer, if you hover over a group member with the mouse, the cursor does not honor the pink group color.
// Similarly, if you hover over someone who sent you a trade request, cursor is not dark purple, either.
// This code fixes this by creating custom cursors based on the existing neutral cursors
// and showing them if it has been detected that the target is a group member or someone who sent a trade request.
void InitMoreCursorColors()
{
    #define INIT_CURSORS_CALL_ADDR 0x59D60B
    InitCursors_Original = SetRelPointer(INIT_CURSORS_CALL_ADDR + 1, InitCursors_Hook);

    #define UPDATE_TARGETING_CALL_ADDR 0x4EC5EE
    Hook(UPDATE_TARGETING_CALL_ADDR, &Targetable_Objects::UpdateTargeting_Hook, 5);

    #define FIND_SELECTED_IOBJRW_CALL_ADDR 0x4F22D6
    Hook(FIND_SELECTED_IOBJRW_CALL_ADDR, FindCurrentSelectedIObjRW_Hook, 5);

    #define GET_ATTITUDE_OF_TARGET_ADDR 0x4F245F
    Hook(GET_ATTITUDE_OF_TARGET_ADDR, GetAttitudeOfTarget_Hook, 6, true);

    DWORD setCurrentAimCursorCalls[] = { 0x4EC914, 0x4EC953 };
    for (auto aimCursorCall : setCurrentAimCursorCalls)
        Hook(aimCursorCall, SetCurrentAimCursor_Hook, 8);
}
