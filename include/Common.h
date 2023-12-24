#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define Import __declspec(dllimport)

struct Import CShip
{
    float get_throttle() const;

    BYTE data[0x2DC];
};
