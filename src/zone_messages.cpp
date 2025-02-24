#include "zone_messages.h"
#include "utils.h"
#include "fl_func.h"

#define WARNING_SOUND_ID (*(PDWORD) 0x674BE0)
#define DANGER_SOUND_ID (*(PDWORD) 0x674BF0)

#define INCREASED_DRAG_SOUND_ID (*(PDWORD) 0x674BD8)
#define SENSOR_EFFICIENCY_SOUND_ID (*(PDWORD) 0x674BDC)
#define VOLATILE_GASES_SOUND_ID (*(PDWORD) 0x674BE4)
#define MINES_DETECTED_SOUND_ID (*(PDWORD) 0x674BE8)

#define TEST_ZONE_MESSAGE_CUTSCENE_ADDR 0x489A8F
#define DUPLICATE_NN_MESSAGE_CHECK_ADDR 0x489B84

FL_FUNC(void QueueNeuralNetMessage(UINT soundId, DWORD unk), 0x489E80)

// Allows for duplicate neural net messages to be queued.
void ForceQueueNeuralNetMessage(UINT soundId)
{
    //*(PBYTE) TEST_ZONE_MESSAGE_CUTSCENE_ADDR = 0x00;
    *(PBYTE) DUPLICATE_NN_MESSAGE_CHECK_ADDR = 0xEB;

    QueueNeuralNetMessage(soundId, 0);

    //*(PBYTE) TEST_ZONE_MESSAGE_CUTSCENE_ADDR = 0xAE;
    *(PBYTE) DUPLICATE_NN_MESSAGE_CHECK_ADDR = 0x74;
}

void SystemZone::HandleZoneMessages(DWORD enteredZoneFlags)
{
    static const NeuralNetZoneMessage zoneMessages[] =
    {
        NeuralNetZoneMessage { DANGER_SOUND_ID,     MINES_DETECTED_SOUND_ID,    0x1000,     true    },
        NeuralNetZoneMessage { DANGER_SOUND_ID,     VOLATILE_GASES_SOUND_ID,    0x4000,     true    },
        NeuralNetZoneMessage { WARNING_SOUND_ID,    SENSOR_EFFICIENCY_SOUND_ID, 0x100000,   false   },
        NeuralNetZoneMessage { WARNING_SOUND_ID,    INCREASED_DRAG_SOUND_ID,    0x80000,    false   }
    };

    for (int i = 0; i < sizeof(zoneMessages) / sizeof(NeuralNetZoneMessage); ++i)
    {
        const NeuralNetZoneMessage &zoneMessage = zoneMessages[i];

        // TODO: do something with testHighByte?
        if ((enteredZoneFlags & zoneMessage.flag) != 0 && (this->currentZoneFlags & zoneMessage.flag) == 0)
        {
            ForceQueueNeuralNetMessage(zoneMessage.prefixMessageSoundId);
            ForceQueueNeuralNetMessage(zoneMessage.messageSoundId);
        }
    }
}

void InitMoreZoneMessages()
{
    #define RADIATION_DAMAGE_PREFIX_MESSAGE_QUEUE_CALL_ADDR 0x4F5211
    #define RADIATION_DAMAGE_MESSAGE_QUEUE_CALL_ADDR 0x4F521E
    #define ZONE_MESSAGE_CHECK_HOOK_ADDR RADIATION_DAMAGE_MESSAGE_QUEUE_CALL_ADDR + 5 + 6
    #define NN_MESSAGE_BOX_SIZE_ADDR 0x489B72

    //ReadWriteProtect(TEST_ZONE_MESSAGE_CUTSCENE_ADDR, sizeof(BYTE));
    ReadWriteProtect(NN_MESSAGE_BOX_SIZE_ADDR, sizeof(BYTE));
    ReadWriteProtect(DUPLICATE_NN_MESSAGE_CHECK_ADDR, sizeof(BYTE));

    // The default size is 4, but we may add up to 8 more because of our code, so increase it by 8, unless the value has already been increased before.
    *(PBYTE) NN_MESSAGE_BOX_SIZE_ADDR = max(8 + 4, *(PBYTE) NN_MESSAGE_BOX_SIZE_ADDR);

    Patch_BYTE(0x4F51C8, 0x5D);
    Patch_BYTE(0x4F51D1, 0x54);
    Patch_BYTE(0x4F51D6, 0x4C);
    Patch_BYTE(0x4F51E1, 0x41);
    Patch_BYTE(0x4F51F6, 0x2C);
    Patch_BYTE(0x4F5204, 0x1E);

    Hook(RADIATION_DAMAGE_PREFIX_MESSAGE_QUEUE_CALL_ADDR, ForceQueueNeuralNetMessage, 5);
    Hook(RADIATION_DAMAGE_MESSAGE_QUEUE_CALL_ADDR, ForceQueueNeuralNetMessage, 5);

    BYTE preHookPatch[] = { 0x83, 0xC4, 0x10, 0x89, 0xF9, 0x56 }; // add esp, 0x10 + mov ecx, edi + push esi
    BYTE postHookPatch[] = { 0xE9, 0x97, 0x00, 0x00, 0x00 }; // jmp
    Patch(RADIATION_DAMAGE_MESSAGE_QUEUE_CALL_ADDR + 5, preHookPatch, sizeof(preHookPatch));
    Hook(ZONE_MESSAGE_CHECK_HOOK_ADDR, &SystemZone::HandleZoneMessages, 5);
    Patch(ZONE_MESSAGE_CHECK_HOOK_ADDR + 5, postHookPatch, sizeof(postHookPatch));

    Patch_BYTE(DUPLICATE_NN_MESSAGE_CHECK_ADDR, 0xEB);

    // TODO: hook 0x004284F0
    // while *(00671F58) > 0, then call 0x00489E00. Then call the original function
    //AddMessageToInbox_Original = Trampoline(0x00489B50, &NN_Messaging::AddMessageToInbox_Hook, 6);
}
