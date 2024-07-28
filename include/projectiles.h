#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Common.h"

#define PROJECTILES_PER_FIRE_CALL_ADDR 0x534D0F

void InitProjectilesSoundFix();
void InitProjectilesServerFix();
