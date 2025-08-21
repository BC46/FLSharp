#include "logger.h"
#include "Dacom.h"

#define NAKED __declspec(naked)

#ifdef ASM_FDUMP
NAKED void FDUMP_Asm(DumpSeverity severity, LPCSTR fmt, ...)
{
    #define FL_FDUMP_IMPORT_ADDR 0x5C6D18

    __asm {
        mov eax, dword ptr ds:[FL_FDUMP_IMPORT_ADDR]
        jmp dword ptr ds:[eax]
    }
}

#define FDUMP_FUNC FDUMP_Asm
#else
#define FDUMP_FUNC FDUMP
#endif

void Logger::PrintModuleError(LPCSTR functionName, LPCSTR moduleName)
{
    FDUMP_FUNC(DumpSeverity::SEV_ERROR, "FLSharp (%s): Could not get module handle %s.", functionName, moduleName);
}

void Logger::PrintV10Warning(LPCSTR moduleName)
{
    FDUMP_FUNC(DumpSeverity::SEV_WARNING, "FLSharp: %s may be v1.0 while v1.1 is assumed. "
        "Please install the official 1.1 patch, or proceed at your own risk.", moduleName);
}
