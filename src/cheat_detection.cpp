#include "cheat_detection.h"
#include "logger.h"
#include "fl_func.h"
#include "utils.h"
#include "Common.h"

#define NAKED __declspec(naked)

DWORD getGoodSoldByBaseCallAddr = 0;
DWORD getGoodSoldByBaseRetAddr = 0;
DWORD getNextBaseGoodAddr = 0;

FL_FUNC(const MarketGood* BaseMarket::GetSoldGood(UINT goodId) const, getGoodSoldByBaseCallAddr)
FL_FUNC(void BaseGoodEndIt::GetNextBaseGood(), getNextBaseGoodAddr)

NAKED void GetGoodSoldByBase_Hook()
{
    __asm {
        mov edx, esi
        call [GetGoodSoldByBaseOrPartOfShip]
        jmp [getGoodSoldByBaseRetAddr]
    }
}

const MarketGood* FASTCALL GetGoodSoldByBaseOrPartOfShip(const BaseMarket &baseMarket, const PlayerData &playerData, UINT goodId)
{
    const MarketGood* result = baseMarket.GetSoldGood(goodId);

    if (result)
        return result;

    // If the good is not sold by the base directly, maybe it's part of the purchased ship package.

    // This should only be checked if the player's ship has remained the same while staying on the base.
    bool changedShip = !(playerData.currentShipId && playerData.currentShipId == playerData.shipIdOnLand);
    if (changedShip)
        return nullptr;

    BaseGoodEndIt* goodEndIt = baseMarket.baseGoods->endIt;

    for (BaseGoodEndIt it = *goodEndIt; it.startIt != (BaseGood*) goodEndIt; it.GetNextBaseGood())
    {
        BaseGood* good = it.startIt;

        if (!good->IsShipCandidate())
            continue;

        GoodInfo const *goodInfo = GoodList::find_by_id(good->goodId);

        if (!goodInfo || goodInfo->type != GoodType::Ship)
            continue;

        for (const auto equipDescList : goodInfo->equipDescLists) {
            for (auto it = equipDescList.list.begin(); it != equipDescList.list.end(); ++it) {
                if (it->archId == goodId) {
                    // Return a MarketGood such that the upcoming return value check passes.
                    static const MarketGood validMarketGood = { 0 };
                    return &validMarketGood;
                }
            }
        }
    }

    return nullptr;
}

void InitShipBuyKickFix()
{
    #define GET_GOOD_SOLD_BY_BASE_CALL_OFFSET_SERVER 0x6FEEB

    DWORD serverHandle = (DWORD) GetModuleHandle("server.dll");

    if (!serverHandle)
    {
        Logger::PrintModuleError("InitShipBuyKickFix", "server.dll");
        return;
    }

    getGoodSoldByBaseCallAddr = serverHandle + 0x33000;
    getGoodSoldByBaseRetAddr = serverHandle + 0x6FEF0;
    getNextBaseGoodAddr = serverHandle + 0x35DE0;

    Hook(serverHandle + GET_GOOD_SOLD_BY_BASE_CALL_OFFSET_SERVER, GetGoodSoldByBase_Hook, 5, true);
}
