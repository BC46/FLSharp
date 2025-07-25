#pragma once

#include "Common.h"
#include "utils.h"

void InitFlashParticlesFix();

struct EffectInstance
{
    virtual void Vftable_x00();
    virtual void FreeEngineEffect();

    // Dealloc function which the game calls to clean up the effects when e.g. a ship or solar gets destroyed.
    inline void EngineDealloc()
    {
        FreeEngineEffect();
        FreeHeapMemory();
    }

    // Dealloc function which the game calls before creating a new flash effect instance for the same barrel/launcher.
    inline void GeneralDealloc()
    {
        FreeAleEffect();
        EngineDealloc();
    }

    // Dealloc function which the game calls right after quitting the play session.
    inline void PostGameDealloc()
    {
        ResetBaseWatcher();
        EngineDealloc();
    }

    inline void DoFreeHeapMemory()
    {
        FreeHeapMemory();
    }

private:
    struct WatcherInfo
    {
        float data[12];
    };

    void FreeAleEffect();
    int FreeHeapMemory();
    void SetBaseWatcher(int unk1, int unk2, WatcherInfo* watcherInfo);

    inline void ResetBaseWatcher()
    {
        WatcherInfo watcherInfo = { 0 };
        watcherInfo.data[0] = watcherInfo.data[4] = watcherInfo.data[8] = 1.0f;

        SetBaseWatcher(0, -1, &watcherInfo);
    }
};

EffectInstance** CreateFlashParticlesArray(UINT barrelAmount);

struct CliLauncher
{
    DWORD vftable;
    CELauncher* launcher; // 0x04
    BYTE x08[0x20];
    // After playing the flash particle on a launcher, the effect instance is stored in 0x28.
    // We need to keep track of more than one effect instance if the launcher has multiple barrels.
    // Expanding the struct's memory is not feasible due to there existing many variations of this struct,
    // which each have their own unique constructor and object size. Hence we dynamically manage this array at the same offset as currentFlashParticle.
    union { // 0x28
        EffectInstance* currentFlashParticle;
        EffectInstance** flashParticlesArr;
    };

    void PlayAllFlashParticles(const ID_String& idString);
    // PlayFlashParticleForBarrel must be __cdecl because this code jumps to a vanilla FL function which does ret instead of ret n at the end.
    // Therefore, the caller must clean the stack.
    void __cdecl PlayFlashParticleForBarrel(const ID_String& idString, UINT barrelIndex);

    void CleanFlashParticlesPostGame_Hook();
    void CleanFlashParticlesEngine_Hook();
    void CleanFlashParticlesMemory_Hook();

    void CleanFlashParticlesArr(void (EffectInstance::*deallocFunc)());
};
