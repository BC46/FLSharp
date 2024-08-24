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

inline void StopSound(BYTE soundId)
{
    #define STOP_SOUND_ADDR 0x5646E0

    typedef void StopSound(BYTE soundId);
    ((StopSound*) STOP_SOUND_ADDR)(soundId);
}

inline void StartSound(BYTE soundId)
{
    #define START_SOUND_ADDR 0x564650

    typedef void StartSound(BYTE soundId);
    ((StartSound*) START_SOUND_ADDR)(soundId);
}

bool GetBackgroundMusicHandle_Hook(SoundHandle **handle);

void StopMusicTestSound_Hook(BYTE soundId);

void InitTestSounds();
