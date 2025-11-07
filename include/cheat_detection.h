#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

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
    BaseGood* test;
    BYTE x00[0xC];
    UINT goodId; // 0x10
    BYTE x14[0xC];
    DWORD goodType; // 0x20
};

struct BaseGoodIt
{
    BaseGood* good;

    void GetNextBaseGood();
};

struct BaseGoodEndIt
{
    BaseGood* startIt;

    void GetNextBaseGood();
};

struct BaseGoodCollection
{
    UINT baseName; // 0x0
    UINT launchpadName; // 0x4
    DWORD unk_x08; // 0x8
    float unk_x0C; // 0xC
    WORD unk_x10; // 0x10
    BaseGoodEndIt* endIt; // 0x14
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

    MarketGood* GetSoldGood(UINT goodId) const;
};

MarketGood* FASTCALL GetGoodSoldByBaseOrPartOfShip(const BaseMarket &baseMarket, const PlayerData &playerData, UINT goodId);

void InitShipBuyKickFix();
