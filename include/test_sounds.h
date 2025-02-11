#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "vftable.h"

#define JMP_NO_PAUSE_FOR_BGM ((PBYTE) 0x42A3A7)
#define JMP_NO_RESUME_FOR_BGM ((PBYTE) 0x42A3EB)

struct SoundHandle
{
    BYTE data_x04[0x2C];
    int unkBytePtr; // I don't know anything about this value (besides it being a pointer to some byte), but it gets nulled when the music stops playing.

    inline bool FinishedPlaying()
    {
        return unkBytePtr == NULL || unkBytePtr == -1;
    }

    // The Resume and Pause functions have explicit checks that prevent the BGM from being paused and resumed.
    // However, our code is special, so we are allowed to pause and resume the BGM.
    inline void ForcePause()
    {
        *JMP_NO_PAUSE_FOR_BGM = 0x00;
        Pause();
        *JMP_NO_PAUSE_FOR_BGM = 0x2C;
    }

    inline void ForceResume()
    {
        *JMP_NO_RESUME_FOR_BGM = 0x00;
        Resume();
        *JMP_NO_RESUME_FOR_BGM = 0x25;
    }

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

bool GetBackgroundMusicHandle_Hook(SoundHandle **handle);

void StopMusicTestSound_Hook(BYTE soundId);

void InitTestSounds();

void PauseSound(bool &shouldResume, SoundHandle *handle, bool getHandleResult, bool force = false);
void ResumeSound(bool &shouldResume, SoundHandle *handle, bool getHandleResult, bool force = false);
