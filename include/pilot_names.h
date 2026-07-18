#define WIN32_LEAN_AND_MEAN
#include <windows.h>

struct StrBuffer
{
	LPWSTR str;
	size_t capacity; // in number of words
};

enum NameType : DWORD
{
	FactionAndDesignation = 0,
	PilotName = 1,
	Unk = 2
};

void InitPilotNamesFix();
