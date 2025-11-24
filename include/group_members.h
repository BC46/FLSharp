#define WIN32_LEAN_AND_MEAN
#include <windows.h>

enum AttitudeType : int
{
    Hostile = -1,
    Neutral = 0,
    Friendly = 1
};

// Read from the [Cursor] values in DATA\mouse.ini.
struct MouseCursor
{
	size_t nicknameLen; // 0x0
	BYTE nickname[32]; // 0x4
	PDWORD unk_0x24; // 0x24
	size_t animNameLen; // 0x28
	BYTE animName[24]; // 0x2C
	float hotspotX; // 0x44
	float hotspotY; // 0x48
	DWORD color; // rgba, 0x4C
	BYTE x50[0x10];
	int animValue1; // 0x60
	int animState; // should be preserved when copying, 0x64
    BYTE x68[0x28];
};

void InitHostileGroupFormation();

void InitHostileGroupMembersFix();
