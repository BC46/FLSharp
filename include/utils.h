#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void Patch(LPVOID vOffset, LPVOID mem, UINT len);

void Nop(LPVOID vOffset, UINT len);

void Hook(DWORD location, DWORD hookFunc, UINT instrLen);
