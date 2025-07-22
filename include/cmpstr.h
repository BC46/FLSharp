#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

struct CmpStr
{
    bool operator()(LPCSTR a, LPCSTR b) const
    {
        return _stricmp(a, b) < 0;
    }
};
