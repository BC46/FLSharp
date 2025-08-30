#include "dll_crash.h"
#include "utils.h"
#include "logger.h"

#define SKIP_DLL_LOAD_FILE_OFFSET_SERVER 0x63F54
#define FDUMP_DLL_INSTANCE_FAILED_1_FILE_OFFSET_SERVER 0x63D50
#define CREATE_DLL_INSTANCE_FAILED_1_FILE_OFFSET_SERVER 0x63D6B
#define CREATE_DLL_INSTANCE_FAILED_2_FILE_OFFSET_SERVER 0x63E6D

#define NAKED __declspec(naked)

DWORD skipDllLoadAddr;

NAKED void CreateDllInstanceFail_Hook()
{
    __asm {
        call [edx]              // overwritten instruction #1
        add esp, 0x14           // overwritten instruction #2
        jmp [skipDllLoadAddr]   // skip the DLL loading code
    }
}

// If you try to load a DLL which doesn't exist via Freelancer.ini (Initial MP DLL or Initial SP DLL),
// the game logs an error to the Spew and then crashes. This code fixes the crash to ensure that the game at least still runs.
void InitMissingDllCrashFix()
{
    // E.g. console.dll enforces the server library to load without causing any issues, so should be fine
    DWORD serverHandle = GetUnloadedModuleHandle("server.dll");

    if (!serverHandle)
    {
        Logger::PrintModuleError("InitMissingDllCrashFix", "server.dll");
        return;
    }

    skipDllLoadAddr = serverHandle + SKIP_DLL_LOAD_FILE_OFFSET_SERVER;

    // mov edx, [FDUMP] <- mov ecx, [FDUMP] to ensure that we can use the same hook for instance 1.
    Patch<BYTE>(serverHandle + FDUMP_DLL_INSTANCE_FAILED_1_FILE_OFFSET_SERVER, 0x15);

    const DWORD dllInstanceFailedOffsets[] = {
        CREATE_DLL_INSTANCE_FAILED_1_FILE_OFFSET_SERVER, CREATE_DLL_INSTANCE_FAILED_2_FILE_OFFSET_SERVER, };

    for (const auto& offset : dllInstanceFailedOffsets)
        Hook(serverHandle + offset, CreateDllInstanceFail_Hook, 5, true);
}
