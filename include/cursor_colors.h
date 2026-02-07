#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "Common.h"

// Read from the [Cursor] values in DATA\mouse.ini.
struct MouseCursor
{
    UINT32 nicknameLen; // 0x0
    char nickname[32]; // 0x4
    PDWORD unk_0x24; // 0x24
    UINT32 animNameLen; // 0x28
    char animName[24]; // 0x2C
    float hotspotX; // 0x44
    float hotspotY; // 0x48
    DWORD color; // rgba, 0x4C
    BYTE x50[0x10];
    int animValue1; // 0x60
    int animState; // should be preserved when copying, 0x64
    BYTE x68[0x28];
};

struct Targetable_Objects
{
    BYTE x00[0x3F0];
    const CSimple* selectedSimple; // 0x3F0
    BYTE x3F4[0x534];
    bool isAimLocking; // 0x928

    void UpdateTargeting_Hook();
    void UpdateTargeting();
};

void InitMoreCursorColors();
