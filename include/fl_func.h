#ifndef FL_FUNC_H
#define FL_FUNC_H

#define NAKED __declspec(naked)
#define NOINLINE __declspec(noinline)

#define FL_FUNC(func, addr) \
    NAKED NOINLINE func \
    { \
        __asm mov eax, addr \
        __asm jmp eax \
    }

#endif //FL_FUNC_H
