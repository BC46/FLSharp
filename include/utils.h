#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <time.h>

void Patch(DWORD vOffset, LPVOID mem, UINT len);

template <typename Type>
inline void Patch(DWORD vOffset, Type value)
{
    Patch(vOffset, (LPVOID) &value, sizeof(Type));
}

void Nop(DWORD vOffset, UINT len);

inline void ReadWriteProtect(DWORD location, DWORD size)
{
    DWORD _;
    VirtualProtect((PVOID) location, size, PAGE_EXECUTE_READWRITE, &_);
}

template <typename Func>
Func SetRelPointer(DWORD location, Func hookFunc)
{
    DWORD originalPointer;

    // Set and calculate the relative offset for the hook function
    ReadWriteProtect(location, sizeof(DWORD));
    originalPointer = location + (*(PDWORD) location) + 4;

    DWORD hookFuncLocation = *((PDWORD) &hookFunc);
    *(PDWORD) location = hookFuncLocation - (location + 4);

    return GetFuncDef<Func>(originalPointer);
}

template <typename Func>
void Hook(DWORD location, Func hookFunc, UINT instrLen, bool jmp = false)
{
    // Set the opcode for the call or jmp instruction
    Patch<BYTE>(location, jmp ? 0xE9 : 0xE8); // 0xE9 = jmp, 0xE8 = call

    // Set the relative address
    SetRelPointer(location + 1, hookFunc);

    // Nop out excess bytes
    if (instrLen > 5)
        Nop((location + 5), instrLen - 5);
}

template <typename Func>
Func Trampoline(DWORD location, Func hookFunc, UINT instrLen)
{
    // Allocate memory for gateway function.
    PBYTE gatewayFunc = (PBYTE) VirtualAlloc(NULL, instrLen + 5, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

    // Copy the instruction(s) that will be overwritten by setting the hooks to the gateway code.
    ReadWriteProtect(location, instrLen);
    memcpy(gatewayFunc, (PVOID) location, instrLen);

    // Jmp from location to hook function.
    Hook(location, hookFunc, 5, true);
    // Jmp from gateway to original function.
    Hook((DWORD) (gatewayFunc + instrLen), GetFuncDef<Func>(location + instrLen), 5, true);

    // Nop out excess bytes.
    if (instrLen > 5)
        Nop((location + 5), instrLen - 5);

    // Return handle for calling the gateway function which in turn calls the original function.
    return GetFuncDef<Func>((DWORD) gatewayFunc);
}

template <typename Func>
void CleanupTrampoline(Func trampolineFunc)
{
    VirtualFree((LPVOID) *((PDWORD) &trampolineFunc), 0, MEM_RELEASE);
}

template <typename Func>
void SetPointer(DWORD location, Func hookFunc)
{
    ReadWriteProtect(location, sizeof(PDWORD));
    *(Func*) location = hookFunc;
}

template <typename Type>
inline Type& GetValue(DWORD location)
{
    ReadWriteProtect(location, sizeof(Type));
    return *(Type*) location;
}

template <class Func>
inline Func GetFuncDef(DWORD funcAddr)
{
    return *(Func*) &funcAddr;
}

double getTimeElapsed(const clock_t &lastUpdate);
