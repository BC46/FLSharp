#define WIN32_LEAN_AND_MEAN
#include <windows.h>

struct SoundHandle
{
    typedef DWORD (__stdcall *FreeReferenceFunc)(SoundHandle *handle);

    struct SoundHandle_VFTable
    {
        BYTE funcs[0x08];
        FreeReferenceFunc freeReferenceFunc;
    };

    SoundHandle_VFTable* vftable; // Weird struct; it has a vftable, yet none of the functions in there honor the __thiscall calling convention.
    BYTE data_x04[0x2C];
    UINT unkBytePtr; // I don't know anything about this value (besides it being a pointer to some byte), but it gets nulled when the music stops playing.

    inline bool StoppedPlaying()
    {
        return unkBytePtr == NULL;
    }

    inline DWORD FreeReference()
    {
        return ((FreeReferenceFunc) vftable->freeReferenceFunc)(this);
    }
};

bool GetBackgroundMusicHandle_Hook(SoundHandle **handle);

void InitTestSounds();
