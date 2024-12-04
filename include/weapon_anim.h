#define WIN32_LEAN_AND_MEAN
#include <windows.h>

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

private:
    typedef bool (EngAnimation::*SetModel)(PDWORD unk, ModelBinary* model);
};

void InitWeaponAnimFix();
