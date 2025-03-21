#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Common.h"
#include "vftable.h"

typedef enum ModelType
{
    MODELTYPE_SHIPHULL = 0,
    MODELTYPE_LAUNCHER = 2,
    MODELTYPE_FORCEDWORD = 0xFFFFFFFF
} ModelType;

struct ModelBinary
{
    ModelType type; // 0x00
    BYTE x04[0xC];
    ModelBinary* parent; // 0x10
};

struct EngAnimation
{
    // The first parameter seems to be a pointer to a stack-struct with the first three DWORDS set to 0 and then a ModelBinary*.
    bool SetModel_Hook(PDWORD unk, ModelBinary* model);
    bool SetModel(PDWORD unk, ModelBinary* model);
};

struct IAnimation2
{
    FILL_VFTABLE(0);
    FILL_VFTABLE(1);
    virtual void Vftable_x20();
    virtual int	__stdcall Open(int scriptIndex, long engineInstance, LPCSTR animationScript, int unk1 = 0, int unk2 = 0);

    int Open_Hook(LPCSTR animationScript, int scriptIndex, CAttachedEquip* equip);
};

void InitWeaponAnimFix();
