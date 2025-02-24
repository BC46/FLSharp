#define WIN32_LEAN_AND_MEAN
#include <windows.h>

struct NeuralNetZoneMessage
{
    UINT prefixMessageSoundId;
    UINT messageSoundId;
    DWORD flag;
    bool testHighByte;
};

struct SystemZone
{
    DWORD x00;
    DWORD currentZoneFlags; // 0x04

    void HandleZoneMessages(DWORD enteredZoneFlags);
};

void QueueNeuralNetMessage(UINT soundId, DWORD unk);

void InitMoreZoneMessages();
