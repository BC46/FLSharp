#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <map>

struct InfocardEntry
{
    std::map<UINT, UINT>& map;
    LPCSTR key;
    LPCSTR value;
};

void InitDynamicSolarInfocards();
