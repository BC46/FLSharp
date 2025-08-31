#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace Logger
{
    void PrintModuleError(LPCSTR functionName, LPCSTR moduleName);
    void PrintFileOpenError(LPCSTR functionName, LPCSTR filePath);
    void PrintV10Warning(LPCSTR moduleName);
    void PrintInvalidFeatureWarning(LPCSTR functionName, LPCSTR featureName, LPCSTR iniPath);
    void PrintInvalidHeaderWarning(LPCSTR functionName, LPCSTR headerName, LPCSTR iniPath);
}
