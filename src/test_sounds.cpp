#include "test_sounds.h"
#include "utils.h"
#include "Freelancer.h"
#include "fl_func.h"

#define INTERFACE_VOLUME_SOUND_ID   0x21
#define AMBIENCE_VOLUME_SOUND_ID    0x22

bool shouldResumeBGM = false, shouldResumeBGA = false;

FL_FUNC(FlSound* GetSound(const ID_String& ids), 0x42AE40)

// Checks whether a test sound exists.
// This is important, because if it does not exist, then the game should not attempt to play it.
// A crash will occur if otherwise.
bool IsTestSoundAvailable(LPCSTR nickname)
{
    // Generates a Spew warning if the sound is not defined.
    // I think it is useful because the warning will only appear if FL wants to plays the test sound
    // while the slider is being dragged but the sound is not defined.
    // It's a hint to the modder that something is missing.
    // Moverover, this function is called at most once for each sound, so it won't spam the Spew.
    FlSound* sound = GetSound(ID_String{ CreateID(nickname) });
    return sound != nullptr;
}

bool IsInterfaceTestSoundAvailable()
{
    static bool result = IsTestSoundAvailable("ui_interface_test");
    return result;
}

bool IsAmbienceTestSoundAvailable()
{
    static bool result = IsTestSoundAvailable("ui_ambiance_test");
    return result;
}

void EnsureTestSoundsPlay()
{
    #define INDEPENDENT_INTERFACE_VOLUME_VAL_ADDR 0x4B1503
    #define INDEPENDENT_AMBIENCE_VOLUME_VAL_ADDR 0x4B1554

    // Test if the interface and ambience volume controls are independent from the sound effects and music, respectively.
    // If these custom edits are applied, then the respective test sounds will never play.
    // Hence patch Freelancer.exe to make the sounds actually play.
    if (GetValue<BYTE>(INDEPENDENT_INTERFACE_VOLUME_VAL_ADDR) == 0x83)
    {
        Patch<WORD>(0x4B1533, 0x00FA);
        Patch<BYTE>(0x4B154E, 0xDF);
    }

    if (GetValue<BYTE>(INDEPENDENT_AMBIENCE_VOLUME_VAL_ADDR) == 0x84)
    {
        Patch<BYTE>(0x4B1584, 0xA9);
        Patch<BYTE>(0x4B159F, 0x8E);
    }
}

FL_FUNC(bool GetBackgroundMusicHandle(SoundHandle **pHandle), 0x428BA0);
FL_FUNC(bool GetBackgroundAmbienceHandle(SoundHandle **pHandle), 0x428BC0);

// There exists a bug in the game where if for example you are docked at a planet and its music has stopped playing,
// you will not hear any test music while adjusting the music volume in the options menu.
// FL tests if there currently exists background music, but not if it has actually ever stopped playing.
// The hook below makes it so that it only returns the handle if the music is still playing.
// As a result, you'll now hear the iconic Tau music when the BGM stopped playing; this way you can more easily fine tune the volume to your liking.
bool GetBackgroundMusicHandle_Hook(SoundHandle **pBgm)
{
    if (GetBackgroundMusicHandle(pBgm))
    {
        SoundHandle *bgm = *pBgm;

        bool bgmPlaying = !(bgm->FinishedPlaying() || bgm->IsPaused());
        if (bgmPlaying)
        {
            // Handle is freed by the caller.
            return true;
        }

        bgm->FreeReference();
        bgm = nullptr;
    }

    // Pause the background ambience.
    PauseSound(shouldResumeBGA, GetBackgroundAmbienceHandle);

    return false;
}

// Hook of code section that stops the test sounds when the user stops adjusting the volume sliders.
// The point of hooking and reimplementing this section is to add stops for more test sounds besides the three that already exist.
// If you were to force new test sounds to play, then without adding respective StopSound entries below they would play indefinitely.
void NN_Preferences::VolumeSliderAdjustEnd_Hook(PVOID adjustedScrollElement)
{
    // For every known test sound, store its IDS name and sound ID.
    static const TestSound testSounds[] =
    {
        { 1409, 0x1E }, // dialogue
        { 1336, 0x1F }, // sfx
        { 1337, 0x20 }, // music
        { 1411, INTERFACE_VOLUME_SOUND_ID }, // interface
        { 1412, AMBIENCE_VOLUME_SOUND_ID } // ambience
    };

    for (int i = 0; i < _countof(scrollElements); ++i)
    {
        if (this->scrollElements[i] != adjustedScrollElement)
            continue;

        for (const auto& testSound : testSounds)
        {
            if (this->audioOptions[i].idsName == testSound.idsName)
            {
                StopSound(testSound.soundId);
                break;
            }
        }
    }

    ResumeSound(shouldResumeBGM, GetBackgroundMusicHandle, true);
    ResumeSound(shouldResumeBGA, GetBackgroundAmbienceHandle);
}

// Make sure to stop the new test sounds too.
void StopMusicTestSound_Hook(BYTE soundId)
{
    StopSound(soundId); // soundId should always be 0x20 here
    StopSound(INTERFACE_VOLUME_SOUND_ID);
    StopSound(AMBIENCE_VOLUME_SOUND_ID);

    ResumeSound(shouldResumeBGM, GetBackgroundMusicHandle, true);
    ResumeSound(shouldResumeBGA, GetBackgroundAmbienceHandle);
}

// Prevent the interface test sound from starting if it isn't available (prevent crashes)
void StartInterfaceTestSound_Hook(BYTE soundId)
{
    if (IsInterfaceTestSoundAvailable())
        StartSound(soundId); // soundId should always be 0x21 here
}

void StartAmbienceTestSound_Hook(BYTE soundId) // soundId should always be 0x22 here
{
    SoundHandle *bga = nullptr;
    if (GetBackgroundAmbienceHandle(&bga))
    {
        bool bgaPlaying = !(bga->FinishedPlaying() || bga->IsPaused());

        bga->FreeReference();

        if (bgaPlaying)
            return;
    }

    if (!IsAmbienceTestSoundAvailable())
        return;

    StartSound(soundId);

    // Pause the background music.
    PauseSound(shouldResumeBGM, GetBackgroundMusicHandle, true);
}

void PauseSound(bool &shouldResume, GetSoundHandleFunc getHandle, bool force)
{
    SoundHandle *handle = nullptr;
    if (!getHandle(&handle))
        return;

    if (!handle->IsPaused())
    {
        if (force)
            handle->ForcePause();
        else
            handle->Pause();

        shouldResume = true;
    }

    handle->FreeReference();
}

void ResumeSound(bool &shouldResume, GetSoundHandleFunc getHandle, bool force)
{
    SoundHandle *handle = nullptr;
    if (!getHandle(&handle))
        return;

    if (shouldResume && handle->IsPaused())
    {
        if (force)
            handle->ForceResume();
        else
            handle->Resume();
    }

    shouldResume = false;
    handle->FreeReference();
}

// The Resume and Pause functions have explicit checks that prevent the BGM from being paused and resumed.
// However, our code is special, so we are allowed to pause and resume the BGM.
void SoundHandle::ForcePause()
{
    static BYTE& jmpNoPauseForBgm = GetValue<BYTE>(0x42A3A7);
    BYTE jmpNoPauseForBgmOriginal = jmpNoPauseForBgm;
    jmpNoPauseForBgm = 0x00;
    Pause();
    jmpNoPauseForBgm = jmpNoPauseForBgmOriginal;
}

void SoundHandle::ForceResume()
{
    static BYTE& jmpNoResumeForBgm = GetValue<BYTE>(0x42A3EB);
    BYTE jmpNoResumeForBgmOriginal = jmpNoResumeForBgm;
    jmpNoResumeForBgm = 0x00;
    Resume();
    jmpNoResumeForBgm = jmpNoResumeForBgmOriginal;
}

// Improves the way FL handles test sounds in the options menu.
// For instance, provide better support for playing the interface and ambience test sounds.
// Mute background music accordingly when adjusting the ambience volume.
// Also allow the test background music to play if the current planetscape background music has stopped playing.
void InitTestSounds()
{
    #define GET_BGM_INSTANCE_CALL_ADDR 0x4B17A1
    #define VOLUME_SLIDER_ADJUST_END_CALL 0x4ACBAB
    #define STOP_MUSIC_TEST_SOUND_1 0x4ADD81
    #define STOP_MUSIC_TEST_SOUND_2 0x4B0689
    #define STOP_MUSIC_TEST_SOUND_3 0x4B0903
    #define START_INTERFACE_TEST_SOUND 0x4B1967
    #define START_AMBIENCE_TEST_SOUND 0x4B1949

    EnsureTestSoundsPlay();

    // Boilerplate code for setting the volume slider adjust end hook.
    PatchBytes(VOLUME_SLIDER_ADJUST_END_CALL - 0x70 - 0x2, { 0xEB, 0x70 });         // jmp 0x04ACBAB
    PatchBytes(VOLUME_SLIDER_ADJUST_END_CALL,              { 0x51, 0x89, 0xE9 });   // push ecx + mov ecx, ebp

    Hook(VOLUME_SLIDER_ADJUST_END_CALL + 3, &NN_Preferences::VolumeSliderAdjustEnd_Hook, 5);
    Hook(GET_BGM_INSTANCE_CALL_ADDR, GetBackgroundMusicHandle_Hook, 5);
    Hook(STOP_MUSIC_TEST_SOUND_1, StopMusicTestSound_Hook, 5);
    Hook(STOP_MUSIC_TEST_SOUND_2, StopMusicTestSound_Hook, 5);
    Hook(STOP_MUSIC_TEST_SOUND_3, StopMusicTestSound_Hook, 5);
    Hook(START_INTERFACE_TEST_SOUND, StartInterfaceTestSound_Hook, 5);
    Hook(START_AMBIENCE_TEST_SOUND, StartAmbienceTestSound_Hook, 5);
}
