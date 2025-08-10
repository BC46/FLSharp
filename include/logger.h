#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace Logger
{
    void PrintModuleError(LPCSTR functionName, LPCSTR moduleName);
}
