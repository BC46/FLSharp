#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define IMPORT __declspec(dllimport)

enum DumpSeverity
{
    SEV_ERROR   = 0x100001,
    SEV_WARNING = 0x100002,
    SEV_NOTICE  = 0x100003
};

typedef int (*FDUMP_HANDLER)(DumpSeverity severity, LPCSTR fmt, ...);

extern "C"
{
    IMPORT FDUMP_HANDLER FDUMP;
    IMPORT int DACOM_GetDllVersion(LPCSTR dllName, UINT32& major, UINT32& minor, UINT32& build);
}
