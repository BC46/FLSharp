#include "logger.h"
#include "Dacom.h"

void Logger::PrintModuleError(LPCSTR functionName, LPCSTR moduleName)
{
    FDUMP(DumpSeverity::SEV_ERROR, "FLSharp (%s): Could not get module handle %s.", functionName, moduleName);
}
