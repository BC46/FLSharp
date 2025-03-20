#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "vftable.h"

struct SoundHandle
{
    BYTE data_x04[0x2C];
    int unkBytePtr; // I don't know anything about this value (besides it being a pointer to some byte), but it gets nulled when the music stops playing.

    inline bool FinishedPlaying()
    {
        return unkBytePtr == NULL || unkBytePtr == -1;
    }

    void ForcePause();
    void ForceResume();

    virtual void Vftable_x00();
    virtual void Vftable_x04();
    virtual DWORD __stdcall FreeReference();
    virtual void Vftable_x0C();
    FILL_VFTABLE(1)
    FILL_VFTABLE(2)
    FILL_VFTABLE(3)
    FILL_VFTABLE(4)
    FILL_VFTABLE(5)
    virtual void Vftable_x60();
    virtual void Vftable_x64();
    virtual void Pause();
    virtual void Resume();
    virtual bool IsPaused();
};

bool GetBackgroundMusicHandle(SoundHandle **pHandle);
bool GetBackgroundAmbienceHandle(SoundHandle **pHandle);

bool GetBackgroundMusicHandle_Hook(SoundHandle **handle);

void StopMusicTestSound_Hook(BYTE soundId);

void InitTestSounds();

void PauseSound(bool &shouldResume, SoundHandle *handle, bool getHandleResult, bool force = false);
void ResumeSound(bool &shouldResume, SoundHandle *handle, bool getHandleResult, bool force = false);
