#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifdef USE_ST6
#include "st6.h"
#else
#include <list>
namespace st6 = std;
#endif

#define FASTCALL __fastcall

struct PlayerData
{
    BYTE x00[0x264];
    UINT currentShipId; // 0x264
    BYTE x268[0xBC];
    UINT shipIdOnLand; // 0x324
};

struct BaseGood
{
    BYTE x00[0x8];
    UINT goodId; // 0x8
    float price; // 0xC
    int minQuantity; // 0x10
    int maxQuantity; // 0x14
    DWORD unk_x18; // 0x18

    inline bool IsShipCandidate() const
    {
        return unk_x18 == 0 || unk_x18 == 2;
    }
};

struct BaseGoodIt
{
    BaseGood* good; // 0x0

    void Advance();
};

struct BaseGoodCollection
{
    UINT baseName; // 0x0
    UINT launchpadName; // 0x4
    DWORD unk_x08; // 0x8
    float unk_x0C; // 0xC
    st6::list<BaseGood> goods; // 0x10

    bool HasShipPackageWithGood(UINT shipId, UINT goodId);
};

struct MarketGood
{
    BYTE x00[0x10];
    DWORD type; // 0x10
};

struct BaseMarket
{
    UINT baseName; // 0x0
    BaseGoodCollection* baseGoods; // 0x4

    const MarketGood* GetSoldGood(UINT goodId) const;
};

const MarketGood* FASTCALL GetGoodSoldByBaseOrPartOfShip(const BaseMarket &baseMarket, const PlayerData &playerData, UINT goodId);

void InitShipBuyKickFix();
