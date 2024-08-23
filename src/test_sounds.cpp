#include "test_sounds.h"
#include "utils.h"
#include "Freelancer.h"

// There exists a bug in the game where if for example you are docked at a planet and its music has stopped playing,
// you will not hear any test music while adjusting the music volume in the options menu.
// FL tests if there currently exists background music, but not if it has actually ever stopped playing.
// The hook below makes it so that it only returns the handle if the music has stopped playing.
// As a result, you'll now hear the iconic Tau music when the BGM stopped playing; this way you can more easily fine tune the volume to your liking.
bool GetBackgroundMusicHandle_Hook(SoundHandle **pHandle)
{
    #define GET_BGM_INSTANCE_ADDR 0x428BA0

    typedef bool GetBackgroundMusicHandle(SoundHandle **pHandle);
    bool getHandleResult = ((GetBackgroundMusicHandle*) GET_BGM_INSTANCE_ADDR)(pHandle);

    if (!getHandleResult)
        return false;

    SoundHandle *handle = *pHandle;

    if (handle->StoppedPlaying())
    {
        handle->FreeReference();
        handle = NULL;

        return false;
    }

    return true;
}

// Hook of code section that stops the test sounds when the user stops adjusting the volume sliders.
// The point of hooking and reimplementing this section is to add stops for more test sounds besides the three that already exist.
// If you were to force new test sounds to play, then without adding respective StopSound entries below they would play indefinitely.
void NN_Preferences::VolumeSliderAdjustEnd_Hook(PVOID adjustedScrollElement)
{
    #define DIALOGUE_VOLUME_IDS         1409
    #define SFX_VOLUME_IDS              1336
    #define MUSIC_VOLUME_IDS            1337
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
        }
    }
}

void InitTestSounds()
{
    #define GET_BGM_INSTANCE_CALL_ADDR 0x4B17A1
    #define VOLUME_SLIDER_ADJUST_END_CALL 0x4ACBAB

    // Boilerplate code for setting the volume slider adjust end hook.
    BYTE patches[] = { 0xEB, 0x70, 0x51, 0x89, 0xE9 };
    Patch(VOLUME_SLIDER_ADJUST_END_CALL - 0x70 - 0x2, patches,     2); // jmp 0x04ACBAB
    Patch(VOLUME_SLIDER_ADJUST_END_CALL,              patches + 2, 3); // push ecx + mov ecx, ebp

    Hook(VOLUME_SLIDER_ADJUST_END_CALL + 3, NN_Preferences::VolumeSliderAdjustEnd_Hook, 5);
    Hook(GET_BGM_INSTANCE_CALL_ADDR, GetBackgroundMusicHandle_Hook, 5);
}

