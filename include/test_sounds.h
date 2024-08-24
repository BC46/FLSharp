#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define JMP_NO_PAUSE_FOR_BGM ((PBYTE) 0x42A3A7)
#define JMP_NO_RESUME_FOR_BGM ((PBYTE) 0x42A3EB)

#define DEFINE_DUMMY_VFTABLE_FUNCS(tensPlace) \
    virtual void Vftable_x ##tensPlace## 0(); \
    virtual void Vftable_x ##tensPlace## 4(); \
    virtual void Vftable_x ##tensPlace## 8(); \
    virtual void Vftable_x ##tensPlace## C(); \

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
    DEFINE_DUMMY_VFTABLE_FUNCS(1)
    DEFINE_DUMMY_VFTABLE_FUNCS(2)
    DEFINE_DUMMY_VFTABLE_FUNCS(3)
    DEFINE_DUMMY_VFTABLE_FUNCS(4)
    DEFINE_DUMMY_VFTABLE_FUNCS(5)
    virtual void Vftable_x60();
    virtual void Vftable_x64();
    virtual void Pause();
    virtual void Resume();
    virtual bool IsPaused();
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

void PauseSound(bool &shouldResume, SoundHandle *handle, bool getHandleResult, bool force = false);
void ResumeSound(bool &shouldResume, SoundHandle *handle, bool getHandleResult, bool force = false);
