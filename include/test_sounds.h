#define WIN32_LEAN_AND_MEAN
#include <windows.h>

struct SoundHandle
{
    BYTE data_x04[0x2C];
    UINT unkBytePtr; // I don't know anything about this value (besides it being a pointer to some byte), but it gets nulled when the music stops playing.

    inline bool StoppedPlaying()
    {
        return unkBytePtr == NULL;
    }

    virtual void Vftable_x00();
    virtual void Vftable_x04();
    virtual DWORD __stdcall FreeReference();
};

bool GetBackgroundMusicHandle_Hook(SoundHandle **handle);

void InitTestSounds();
