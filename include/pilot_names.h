#define WIN32_LEAN_AND_MEAN
#include <windows.h>

struct StrBuffer
{
	LPWSTR str;
	size_t capacity;
};

enum NameType : DWORD
{
	FactionAndDesignation = 0,
	PilotName = 1,
	Unk = 2
};

void InitPilotNamesFix();
