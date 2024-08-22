#include "test_sounds.h"
#include "utils.h"

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

void InitTestSounds()
{
    #define GET_BGM_INSTANCE_CALL_ADDR 0x4B17A1

    Hook(GET_BGM_INSTANCE_CALL_ADDR, GetBackgroundMusicHandle_Hook, 5);
}

