#include "cheat_detection.h"
#include "logger.h"
#include "fl_func.h"
#include "utils.h"
#include "Common.h"
#include <algorithm>

#define NAKED __declspec(naked)

DWORD getGoodSoldByBaseCallAddr = 0;
DWORD baseGoodItAdvanceAddr = 0;

FL_FUNC(const MarketGood* BaseMarket::GetSoldGood(UINT goodId) const, getGoodSoldByBaseCallAddr)
FL_FUNC(void BaseGoodIt::Advance(), baseGoodItAdvanceAddr)

NAKED void GetGoodSoldByBase_Hook()
{
    __asm {
        mov edx, esi                        // PlayerData&
        jmp GetGoodSoldByBaseOrPartOfShip
    }
}

bool ShipPackageContainsGood(GoodInfo const &shipPackage, UINT goodId)
{
    for (const auto& equipDescList : shipPackage.equipDescLists) {
        bool containsGoodId = std::any_of(equipDescList.list.begin(), equipDescList.list.end(),
            [goodId](const EquipDesc &equipDesc) { return equipDesc.archId == goodId; });

        if (containsGoodId)
            return true;
    }

    return false;
}

bool BaseGoodCollection::HasShipPackageWithGood(UINT goodId)
{
    // Iterate over all the base's sold goods and try to find the ship packages.
    for (auto goodIt = goods.begin(); goodIt != goods.end(); ((BaseGoodIt*) &goodIt)->Advance())
    {
        if (!goodIt->IsShipCandidate())
            continue;

        GoodInfo const *goodInfo = GoodList::find_by_id(goodIt->goodId);

        // Is it a ship package?
        if (goodInfo && goodInfo->type == GoodType::Ship)
        {
            if (ShipPackageContainsGood(*goodInfo, goodId))
                return true;
        }
    }

    return false;
}

const MarketGood* FASTCALL GetGoodSoldByBaseOrPartOfShip(const BaseMarket &baseMarket, const PlayerData &playerData, UINT goodId)
{
    const MarketGood* result = baseMarket.GetSoldGood(goodId);

    if (result)
        return result;

    // If the good is not sold by the base directly, maybe it's part of the purchased ship package.
    // This should only be checked if the player's ship has remained the same while staying on the base.
    if (playerData.currentShipId
        && playerData.currentShipId == playerData.shipIdOnLand
        && baseMarket.baseGoods->HasShipPackageWithGood(goodId))
    {
        // Return a MarketGood such that FL's return value check passes.
        static const MarketGood validMarketGood = { 0 };
        return &validMarketGood;
    }

    return nullptr;
}

// In Freelancer there is a bug where if you have a server with players on it
// and a player purchases a ship which they already have and then undock, they get kicked from the server.
// This is because on undock, FL's anticheat does a check to see if you obtained any equipment which is not sold by the base.
// This check only proceeds if your ship hasn't changed since you landed on the base.
// If you buy a ship, you usually get some additional equipment as part of the package (e.g. shield).
// However, after re-buying the same ship and undocking, you still have the same ship as far as the game is concerned,
// and you have a shield which is not sold by the base, and thus you get kicked.
// This code fixes it by checking if the "cheated" equipment is part of any of the base's offered ship packages.
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
    baseGoodItAdvanceAddr = serverHandle + 0x35DE0;

    Hook(serverHandle + GET_GOOD_SOLD_BY_BASE_CALL_OFFSET_SERVER, GetGoodSoldByBase_Hook, 5);
}
