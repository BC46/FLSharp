#include "Common.h"
#include "utils.h"

void InitFlashParticlesFix();

struct EffectInstance
{
    virtual void Vftable_x00();
    virtual void FreeEngineEffect();

    // Dealloc function which the game calls before creating a new flash effect instance for the same barrel/launcher.
    inline void Dealloc()
    {
        FreeAleEffect();
        FreeEngineEffect();
        FreeHeapMemory();
    }

    // Dealloc function which the game calls right after quitting the play session.
    inline void PostGameDealloc()
    {
        ResetBaseWatcher();
        FreeEngineEffect();
        FreeHeapMemory();
    }

private:
    struct WatcherInfo
    {
        float data[12];
    };

    typedef void (EffectInstance::*FreeAleEffectFunc)();
    typedef int (EffectInstance::*FreeHeapMemoryFunc)();
    typedef void (EffectInstance::*ResetBaseWatcherFunc)(int unk1, int unk2, WatcherInfo* watcherInfo);

    inline void FreeAleEffect()
    {
        #define FREE_ALE_EFFECT_ADDR 0x4F8110
        FreeAleEffectFunc freeAleEffect = GetFuncDef<FreeAleEffectFunc>(FREE_ALE_EFFECT_ADDR);
        (this->*freeAleEffect)();
    }

    inline int FreeHeapMemory()
    {
        #define FREE_HEAP_MEMORY_EFFECT_ADDR 0x4F7A90
        FreeHeapMemoryFunc freeHeapMemory = GetFuncDef<FreeHeapMemoryFunc>(FREE_HEAP_MEMORY_EFFECT_ADDR);
        return (this->*freeHeapMemory)();
    }

    inline void ResetBaseWatcher()
    {
        WatcherInfo watcherInfo = { 0 };
        watcherInfo.data[0] = watcherInfo.data[4] = watcherInfo.data[8] = 1.0f;

        #define RESET_BASE_WATCHER_ADDR 0x4F7D20
        ResetBaseWatcherFunc resetBaseWatcher = GetFuncDef<ResetBaseWatcherFunc>(RESET_BASE_WATCHER_ADDR);
        (this->*resetBaseWatcher)(0, -1, &watcherInfo);
    }
};

struct LauncherHandler
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

    void CreateFlashParticlesArray();
    void PlayAllFlashParticles(ID_String* idString);
    // PlayFlashParticleForBarrel must be __cdecl because this code jumps to a vanilla FL function which does ret instead of ret n at the end.
    // Therefore, the caller must clean the stack.
    void __cdecl PlayFlashParticleForBarrel(ID_String* idString, UINT barrelIndex);

    void Destructor_Hook();

private:
    typedef void (LauncherHandler::*Destructor)();
};
