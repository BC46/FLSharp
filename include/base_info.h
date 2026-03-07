#define WIN32_LEAN_AND_MEAN
#include <windows.h>

struct BaseInfoCat
{
    DWORD headerStyleAddr;
    DWORD headerNamePrintAddr;
    DWORD firstSpacingAddr;
};

void InitBaseInfoSpacingFix();
