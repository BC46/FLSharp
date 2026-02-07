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

std::map<MouseCursor*, std::shared_ptr<MouseCursor>> groupCursors, tradeRequestCursors;

std::shared_ptr<MouseCursor> CreateCustomCursor(const MouseCursor& originalCursor, DWORD color)
{
    auto result = std::make_shared<MouseCursor>();
    *result = originalCursor;
    result->color = color;

    return result;
}

void (*InitCursors_Original)();

void InitCursors_Hook()
{
    // This function initializes all the standard cursors.
    // After it has finished, we want to create our custom-colored cursors by copying the existing neutral cursors.
    InitCursors_Original();

    MouseCursor* neutralCursor = nullptr;
    MouseCursor* fireNeutralCursor = nullptr;

    LPCSTR normalCursorNames[] = { "friendly", "neutral", "hostile" };
    LPCSTR fireCursorNames[] = { "fire_friendly", "fire_neutral", "fire" };

    std::vector<MouseCursor*> normalCursors, fireCursors;

    // Find the relevant cursors.
    for (UINT i = 0; i < CURSOR_LIST_SIZE; ++i)
    {
        MouseCursor* cursor = CURSOR_LIST[i];

        for (const auto &normalCursorName : normalCursorNames)
        {
            if (strcmp(cursor->nickname, normalCursorName) == 0)
                normalCursors.push_back(cursor);
        }

        for (const auto &fireCursorName : fireCursorNames)
        {
            if (strcmp(cursor->nickname, fireCursorName) == 0)
                fireCursors.push_back(cursor);
        }
    }

    // Get the neutral cursor which we want to copy.
    auto neutralCursorIt = std::find_if(normalCursors.begin(), normalCursors.end(),
        [](const MouseCursor* cursor) { return strcmp(cursor->nickname, "neutral") == 0; });

    if (neutralCursorIt != normalCursors.end())
    {
        auto groupCursor = CreateCustomCursor(**neutralCursorIt, GROUP_MEMBER_COLOR);
        auto tradeRequestCursor = CreateCustomCursor(**neutralCursorIt, TRADE_REQUEST_COLOR);

        for (const auto &normalCursor : normalCursors)
        {
            groupCursors.emplace(normalCursor, groupCursor);
            tradeRequestCursors.emplace(normalCursor, tradeRequestCursor);
        }
    }

    auto fireNeutralCursorIt = std::find_if(fireCursors.begin(), fireCursors.end(),
        [](const MouseCursor* cursor) { return strcmp(cursor->nickname, "fire_neutral") == 0; });

    if (fireNeutralCursorIt != fireCursors.end())
    {
        auto groupCursor = CreateCustomCursor(**fireNeutralCursorIt, GROUP_MEMBER_COLOR);
        auto tradeRequestCursor = CreateCustomCursor(**fireNeutralCursorIt, TRADE_REQUEST_COLOR);

        for (const auto &fireCursor : fireCursors)
        {
            groupCursors.emplace(fireCursor, groupCursor);
            tradeRequestCursors.emplace(fireCursor, tradeRequestCursor);
        }
    }
}

FL_FUNC(void SetCurrentCursor(LPCSTR cursorName, bool unk), 0x41DDE0)
FL_FUNC(IObjRW* FindIObjRW(UINT nickname, DWORD unk), 0x05416C0)

// TODO: target is not
// Check [ebp + 0x928] for bool isTargetting
void FASTCALL SetCurrentCustomAimCursor(const Targetable_Objects& to, const IObjRW *highlightedObj, LPCSTR cursorName, bool unk)
{
    const IObjRW* player = GetPlayerIObjRW();

    // This function updates the CURRENT_CURSOR for targeting (aiming).
    SetCurrentCursor(cursorName, unk);

    if (!player || player->unk_x1C != 1)
        return;

    const IObjRW *target = nullptr;
    // if (to.isAimLocking)
    // {
    //     // I'm not testing the return value because FL doesn't do it either.
    //     player->get_target(target);
    // }

    // if (!target)
    // {
    //     target = highlightedObj;
    // }

    // if (!target || target == player)
    //     return;

    if (highlightedObj != player)
    {
        target = highlightedObj;
    } else if (to.selectedSimple) {
        target = FindIObjRW(to.selectedSimple->nickname, 0);
    }

    // if (to.isAimLocking && to.selectedSimple)
    // {
    //     target = FindIObjRW(to.selectedSimple->nickname, 0);
    // } else if (to.selectedSimple && highlightedObj == player)
    // {
    //     target = FindIObjRW(to.selectedSimple->nickname, 0);
    // } else {
    //     target = highlightedObj;
    // }

    // if (highlightedObj && highlightedObj != player)
    // {
    //     target = highlightedObj;
    // }
    // else
    // {
    //     if (to.selectedSimple)
    //     {
    //         target = FindIObjRW(to.selectedSimple->nickname, 0);
    //     }
    // }

    // if (!to.isAimLocking)
    // {
    //     if (to.selectedSimple)
    //     {
    //         target = FindIObjRW(to.selectedSimple->nickname, 0);
    //     }
    //     else
    //     {
    //         target = highlightedObj;
    //     }
    // }
    // else
    // {
    //     target = highlightedObj;
    // }

    if (!target)
    {
        return;
    }

    std::map<MouseCursor*, std::shared_ptr<MouseCursor>>* customCursorMap = nullptr;
    if (target->is_player())
    {
        if (target->SentTradeRequest())
            customCursorMap = &tradeRequestCursors;
        else if (AreIObjRWsInSameGroup(*target, *player))
            customCursorMap = &groupCursors;
    }

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

    DWORD setCurrentAimCursorCalls[] = { 0x4EC914, 0x4EC953 };
    for (auto aimCursorCall : setCurrentAimCursorCalls)
        Hook(aimCursorCall, SetCurrentAimCursor_Hook, 8);
}
