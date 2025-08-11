#include "version_check.h"
#include "Dacom.h"
#include "utils.h"

#define DACOM_VERSION_MS_FUNC_OFFSET (0x281D - 0x2720)

// Function that returns the third value of the FILEVERSION/PRODUCTVERSION in a DLL's Version Info.
UINT32 GetDllProductBuildVersion(LPCSTR dllName)
{
    if (!GetUnloadedModuleHandle(dllName))
        return NULL;

    // Hack the DACOM_GetDllVersion function such that it returns the value we're after as the "major".
    // Basically instead of returning the high word of dwProductVersionMS, return the high word of dwProductVersionLS.
    // This is the only value that can be used to distinguish 1.0 DLLs from 1.1 DLLs.
    DWORD dacomVersionMsAddr = ((DWORD) DACOM_GetDllVersion) + DACOM_VERSION_MS_FUNC_OFFSET;
    BYTE& dacomVersionMs = GetValue<BYTE>(dacomVersionMsAddr);
    dacomVersionMs += sizeof(UINT32);

    UINT32 productBuild = 0, minor, build;
    DACOM_GetDllVersion(dllName, productBuild, minor, build);

    // Restore the patch.
    dacomVersionMs -= sizeof(UINT32);

    return productBuild;
}
