#include "vftable.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define STDCALL __stdcall

void InitIncreaseLightLimit();

// Redefining these structs because I don't want the project to depend on the DirectX SDK...
struct D3DCAPS8
{
    BYTE x00[0xA0];
    DWORD MaxActiveLights; // 0xA0
};

struct IDirect3DDevice8
{
    long STDCALL GetDeviceCaps_Hook(D3DCAPS8* pCaps);

    FILL_VFTABLE(0)
    virtual void Vftable_x10();
    virtual void Vftable_x14();
    virtual void Vftable_x18();
    virtual long STDCALL GetDeviceCaps(D3DCAPS8* pCaps); // 0x1C
};
