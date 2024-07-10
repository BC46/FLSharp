#include "resolutions_asm.h"

#define SELECTED_HEIGHT_OF 0x980
#define ACTIVE_HEIGHT_OF 0x984

int horzRes, vertRes, tempHeight;

__declspec(naked) void CurrentResInfoWrite1()
{
    __asm {
        mov [ebp+0x8B8], ebx
        mov [ebp+SELECTED_HEIGHT_OF], ebx
        mov [ebp+ACTIVE_HEIGHT_OF], ebx
        ret
    }
}

__declspec(naked) void CurrentResInfoWrite2()
{
    __asm {
        mov [ebx+0x330], eax
        mov [ebx+SELECTED_HEIGHT_OF], edi
        mov [ebx+ACTIVE_HEIGHT_OF], edi
        ret
    }
}

__declspec(naked) void CurrentResInfoWrite3()
{
    __asm {
        mov [edi+0x330], eax
        mov [edi+SELECTED_HEIGHT_OF], ecx
        ret
    }
}

__declspec(naked) void CurrentResInfoWrite4()
{
    __asm {
        mov [ebp+0x8B8], eax
        mov eax, [esp+0x20]
        mov [ebp+ACTIVE_HEIGHT_OF], eax
        ret
    }
}

// Selected to active
__declspec(naked) void CurrentResInfoWrite5()
{
    __asm {
        mov [ebp+0x8B8], eax
        mov eax, [ebp+SELECTED_HEIGHT_OF]
        mov [ebp+ACTIVE_HEIGHT_OF], eax
        ret
    }
}

// Active to selected
__declspec(naked) void CurrentResInfoWrite6()
{
    __asm {
        mov [esi+0x330], eax
        mov eax, [esi+ACTIVE_HEIGHT_OF]
        mov [esi+ACTIVE_HEIGHT_OF], eax
        ret
    }
}

__declspec(naked) void CurrentResInfoWrite7()
{
    __asm {
        mov [ebx+0x330], eax
        mov [ebx+SELECTED_HEIGHT_OF], edx
        mov [ebx+ACTIVE_HEIGHT_OF], edx
        ret
    }
}

__declspec(naked) void CurrentResInfoCheck1()
{
    __asm {
        mov eax, [ebp+ACTIVE_HEIGHT_OF]
        cmp eax, [ebp+SELECTED_HEIGHT_OF]
        jne notequal
        mov cl, byte ptr ss:[ebp+0x8BC]
        push 0x04B1F6D
        ret
    notequal:
        push 0x04B1F75
        ret
    }
}

__declspec(naked) void CurrentResInfoCheck2()
{
    __asm {
        cmp [esp+0x88], edi
        jne notequal
        push edi
        push eax
        cmp ecx, 0x20
        lea edx, [esp+0xAC]
        sete cl
        push 0x4B103A
        ret
    notequal:
        push 0x4B1075
        ret
    }
}

__declspec(naked) void CurrentResInfoCheck3()
{
    __asm {
        mov ecx, [esi+ACTIVE_HEIGHT_OF]
        cmp ecx, [esi+SELECTED_HEIGHT_OF]
        jne notequal
        mov cl, byte ptr ss:[esi+0x8BC]
        push 0x4B2580
        ret
    notequal:
        push 0x4B2588
        ret
    }
}

__declspec(naked) void CurrentResInfoCheck4()
{
    __asm {
        cmp edi, [esp+0x48]
        jne notequal
        push 0xFFFFFFFF
        push edi
        push esi
        lea ecx, [esp+0x20]
        push 0x4B1C9B
        ret
    notequal:
        push 0x4B1CB7
        ret
    }
}

__declspec(naked) void CurrentResInfoCheck5()
{
    __asm {
        mov edi, [ebp+ACTIVE_HEIGHT_OF]
        cmp edi, [ebp+SELECTED_HEIGHT_OF]
        jne notequal
        mov cl, byte ptr ss:[ebp+0x8BC]
        push 0x4B0754
        ret
    notequal:
        push 0x4B0760
        ret
    }
}

__declspec(naked) void CurrentResInfoCheck6()
{
    __asm {
        mov eax, [ebp+ACTIVE_HEIGHT_OF]
        cmp eax, [ebx-0x4]
        jne notequal
        mov ecx, [ebx]
        xor eax, eax
        cmp ecx, 0x20
        movzx ecx, byte ptr ss:[ebp+0x8BC]
        sete al
        push 0x4B0797
        ret
    notequal:
        push 0x4B07D1
        ret
    }
}

__declspec(naked) void CurrentResInfoCheck7()
{
    __asm {
        cmp [ecx-8], edx
        jne notequal
        mov ebx, [ecx-4]
        cmp ebx, [tempHeight]
        jne notequal
        push 0x4ACEE7
        ret
    notequal:
        push 0x4ACEEB
        ret
    }
}

__declspec(naked) void DefaultResSet1()
{
    __asm {
        mov edx, [horzRes]
        mov ebx, [vertRes]
        mov [tempHeight], ebx
        push 0x4ACEB0
        ret
    }
}

__declspec(naked) void DefaultResSet2()
{
    __asm {
        mov edx, 0x320
        mov [tempHeight], 0x258
        cmp eax, esi
        jbe conditionmet
        push 0x4ACEC4
        ret
    conditionmet:
        push 0x4ACEC9
        ret
    }
}

void SetHorzRes(int value)
{
    horzRes = value;
}

void SetVertRes(int value)
{
    vertRes = value;
}
