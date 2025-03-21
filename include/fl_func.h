#pragma once

#define NAKED __declspec(naked)
#define NOINLINE __declspec(noinline)

#define FL_FUNC(func, addr) \
    NAKED NOINLINE func \
    { \
        __asm mov eax, addr \
        __asm jmp eax \
    }
