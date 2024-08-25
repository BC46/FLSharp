#include "test_sounds.h"
#include "utils.h"
#include "Freelancer.h"

#define INTERFACE_VOLUME_SOUND_ID   0x21
#define AMBIENCE_VOLUME_SOUND_ID    0x22

bool interfaceTestSoundAvailable = false, ambienceTestSoundAvailable = false;
bool shouldResumeBGM = false, shouldResumeBGA = false;

void EnsureTestSoundsPlay()
{
    #define INDEPENDENT_INTERFACE_VOLUME_VAL ((PBYTE) 0x4B1503)
    #define INDEPENDENT_AMBIENCE_VOLUME_VAL ((PBYTE) 0x4B1554)

    // Test if the interface and ambience volume controls are independent from the sound effects and music, respectively.
    // If these custom edits are applied, then the respective test sounds will never play.
    // Hence patch Freelancer.exe to make the sounds actually play.

    if (*INDEPENDENT_INTERFACE_VOLUME_VAL == 0x83)
    {
        Patch_WORD(0x4B1533, 0x00FA);
        Patch_BYTE(0x4B154E, 0xDF);
    }

    if (*INDEPENDENT_AMBIENCE_VOLUME_VAL == 0x84)
    {
        Patch_BYTE(0x4B1584, 0xA9);
        Patch_BYTE(0x4B159F, 0x8E);
    }
}

// Checks whether test sound entries exist for the interface and ambience sounds.
// This is important, because if these do not exist, then the game should not attempt to play these.
// A crash will occur if otherwise.
void SetTestSoundAvailability(bool &interfaceSounds, bool &ambienceSounds)
{
    INI_Reader reader;
    // The actual path is defined in Freelancer.ini, but looking it up is a bit cumbersome, hence it's assumed.
    if (!reader.open("..\\DATA\\AUDIO\\interface_sounds.ini"))
    {
        interfaceSounds = false;
        ambienceSounds = false;
        return;
    }

    while (reader.read_header())
    {
        if (!reader.is_header("Sound"))
            continue;

        while (reader.read_value())
        {
            if (reader.is_value("nickname"))
            {
                if (stricmp(reader.get_value_string(), "ui_interface_test") == 0)
                    interfaceSounds = true;
                else if (stricmp(reader.get_value_string(), "ui_ambiance_test") == 0)
                    ambienceSounds = true;
            }

            // There's not much point in breaking early as these values are found near the end of the ini file.
        }
    }

    reader.close();
}

bool inline GetBackgroundMusicHandle(SoundHandle **pHandle)
{
    #define GET_BGM_INSTANCE_ADDR 0x428BA0

    typedef bool GetBackgroundMusicHandle(SoundHandle **pHandle);
    return ((GetBackgroundMusicHandle*) GET_BGM_INSTANCE_ADDR)(pHandle);
}

bool inline GetBackgroundAmbienceHandle(SoundHandle **pHandle)
{
    #define GET_BGA_INSTANCE_ADDR 0x428BC0

    typedef bool GetBackgroundAmbienceHandle(SoundHandle **pHandle);
    return ((GetBackgroundAmbienceHandle*) GET_BGA_INSTANCE_ADDR)(pHandle);
}

// There exists a bug in the game where if for example you are docked at a planet and its music has stopped playing,
// you will not hear any test music while adjusting the music volume in the options menu.
// FL tests if there currently exists background music, but not if it has actually ever stopped playing.
// The hook below makes it so that it only returns the handle if the music has stopped playing.
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
        bgm = NULL;
    }

    // Pause the background ambience.
    SoundHandle *bga = NULL;
    PauseSound(shouldResumeBGA, bga, GetBackgroundAmbienceHandle(&bga));

    return false;
}

// Hook of code section that stops the test sounds when the user stops adjusting the volume sliders.
// The point of hooking and reimplementing this section is to add stops for more test sounds besides the three that already exist.
// If you were to force new test sounds to play, then without adding respective StopSound entries below they would play indefinitely.
void NN_Preferences::VolumeSliderAdjustEnd_Hook(PVOID adjustedScrollElement)
{
    #define DIALOGUE_VOLUME_IDS         1409
    #define SFX_VOLUME_IDS              1336
    #define MUSIC_VOLUME_IDS            1337
    #define INTERFACE_VOLUME_IDS        1411
    #define AMBIENCE_VOLUME_IDS         1412
    #define DIALOGUE_VOLUME_SOUND_ID    0x1E
    #define SFX_VOLUME_SOUND_ID         0x1F
    #define MUSIC_VOLUME_SOUND_ID       0x20

    for (int i = 0; i < 14; ++i)
    {
        if (this->scrollElements[i] != adjustedScrollElement)
            continue;

        switch (this->audioOptions[i].idsName)
        {
            case DIALOGUE_VOLUME_IDS:
                StopSound(DIALOGUE_VOLUME_SOUND_ID);
                break;
            case SFX_VOLUME_IDS:
                StopSound(SFX_VOLUME_SOUND_ID);
                break;
            case MUSIC_VOLUME_IDS:
                StopSound(MUSIC_VOLUME_SOUND_ID);
                break;
            case INTERFACE_VOLUME_IDS:
                StopSound(INTERFACE_VOLUME_SOUND_ID);
                break;
            case AMBIENCE_VOLUME_IDS:
                StopSound(AMBIENCE_VOLUME_SOUND_ID);
                break;
        }
    }

    SoundHandle *bgm = NULL, *bga = NULL;
    ResumeSound(shouldResumeBGM, bgm, GetBackgroundMusicHandle(&bgm), true);
    ResumeSound(shouldResumeBGA, bga, GetBackgroundAmbienceHandle(&bga));
}

// Make sure to stop the new test sounds too.
void StopMusicTestSound_Hook(BYTE soundId)
{
    StopSound(soundId); // soundId should always be 0x20 here
    StopSound(INTERFACE_VOLUME_SOUND_ID);
    StopSound(AMBIENCE_VOLUME_SOUND_ID);

    SoundHandle *bgm = NULL, *bga = NULL;
    ResumeSound(shouldResumeBGM, bgm, GetBackgroundMusicHandle(&bgm), true);
    ResumeSound(shouldResumeBGA, bga, GetBackgroundAmbienceHandle(&bga));
}

// Prevent the interface test sound from starting if it isn't available (prevent crashes)
void StartInterfaceTestSound_Hook(BYTE soundId)
{
    if (interfaceTestSoundAvailable)
        StartSound(soundId); // soundId should always be 0x21 here
}

void StartAmbienceTestSound_Hook(BYTE soundId) // soundId should always be 0x22 here
{
    SoundHandle *bga = NULL;
    if (GetBackgroundAmbienceHandle(&bga))
    {
        bool bgaPlaying = !(bga->FinishedPlaying() || bga->IsPaused());

        bga->FreeReference();

        if (bgaPlaying)
            return;
    }

    if (!ambienceTestSoundAvailable)
        return;

    StartSound(soundId);

    // Pause the background music.
    SoundHandle *bgm = NULL;
    PauseSound(shouldResumeBGM, bgm, GetBackgroundMusicHandle(&bgm), true);
}

void PauseSound(bool &shouldResume, SoundHandle *handle, bool getHandleResult, bool force)
{
    if (!getHandleResult)
        return;

    if (!handle->IsPaused())
    {
        // Pause the music so the ambience sound can be heard better.
        if (force)
            handle->ForcePause();
        else
            handle->Pause();

        shouldResume = true;
    }

    handle->FreeReference();
}

void ResumeSound(bool &shouldResume, SoundHandle *handle, bool getHandleResult, bool force)
{
    if (!getHandleResult)
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
    SetTestSoundAvailability(interfaceTestSoundAvailable, ambienceTestSoundAvailable);

    // Boilerplate code for setting the volume slider adjust end hook.
    BYTE patches[] = { 0xEB, 0x70, 0x51, 0x89, 0xE9 };
    Patch(VOLUME_SLIDER_ADJUST_END_CALL - 0x70 - 0x2, patches,     2); // jmp 0x04ACBAB
    Patch(VOLUME_SLIDER_ADJUST_END_CALL,              patches + 2, 3); // push ecx + mov ecx, ebp

    Hook(VOLUME_SLIDER_ADJUST_END_CALL + 3, NN_Preferences::VolumeSliderAdjustEnd_Hook, 5);
    Hook(GET_BGM_INSTANCE_CALL_ADDR, GetBackgroundMusicHandle_Hook, 5);
    Hook(STOP_MUSIC_TEST_SOUND_1, StopMusicTestSound_Hook, 5);
    Hook(STOP_MUSIC_TEST_SOUND_2, StopMusicTestSound_Hook, 5);
    Hook(STOP_MUSIC_TEST_SOUND_3, StopMusicTestSound_Hook, 5);
    Hook(START_INTERFACE_TEST_SOUND, StartInterfaceTestSound_Hook, 5);
    Hook(START_AMBIENCE_TEST_SOUND, StartAmbienceTestSound_Hook, 5);

    DWORD _;
    VirtualProtect((PVOID) JMP_NO_PAUSE_FOR_BGM, sizeof(BYTE), PAGE_EXECUTE_READWRITE, &_);
    VirtualProtect((PVOID) JMP_NO_RESUME_FOR_BGM, sizeof(BYTE), PAGE_EXECUTE_READWRITE, &_);
}

