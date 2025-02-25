#include "sound_ref_leaks.h"
#include "fl_func.h"
#include "utils.h"

FL_FUNC(void CleanUpNNMessageInboxQueue(), 0x489E00)

void (*ShrinkSoundPool_Original)();

// The ShrinkSoundPool_Original is what checks for sound reference leaks.
// We make sure all sound references from the NN message queue are cleaned up prior to this.
void ShrinkSoundPool_Hook()
{
    #define NN_MESSAGE_INBOX_QUEUE_SIZE (*(int*) 0x671F58)

    while (int lastQueueSize = NN_MESSAGE_INBOX_QUEUE_SIZE)
    {
        CleanUpNNMessageInboxQueue();

        // Sometimes one of the sounds can't be cleaned up for reasons which I can't explain.
        // If that's the case, just break because otherwise there'll be an infinite loop.
        if (lastQueueSize == NN_MESSAGE_INBOX_QUEUE_SIZE)
            break;
    }

    ShrinkSoundPool_Original();
}

// There is an issue in the game where if you quit to the main menu while one of the neural net sounds is playing,
// the sound references are not cleaned up and there is a sound reference leak.
// This hook ensures that before the sound reference pool is shrunk, all sounds in the neural net message queue are cleaned first.
// Or at least it tries to because sometimes a sound can't be cleaned up and thus a reference will remain.
void InitSoundRefLeaksFix()
{
    ShrinkSoundPool_Original = Trampoline(0x4284F0, ShrinkSoundPool_Hook, 7);
    FixRelAddressInGateway(ShrinkSoundPool_Original, 3, 0x4284F0);
}

void CleanupSoundRefLeaksFix()
{
    CleanupTrampoline(ShrinkSoundPool_Original);
}
