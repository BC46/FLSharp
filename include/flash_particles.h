#include "Common.h"
#include "utils.h"

void InitFlashParticlesFix();

struct ParticleParams
{};

struct EffectInstance
{
    virtual void Vftable_x00();
    virtual void FreeEngineEffect();

    inline void Dealloc()
    {
        FreeAleEffect();
        FreeEngineEffect();
        FreeHeapMemory();
    }

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

struct GunHandler
{
    DWORD vftable;
    CELauncher* gun; // 0x04
    BYTE x08[0x20];
    union { // 0x28
        EffectInstance* currentFlashParticle;
        EffectInstance** flashParticles;
    };

    void CreateFlashParticlesArray();
    void PlayAllFlashParticles(ID_String* idString);
    void __cdecl PlayFlashParticleForBarrel(ID_String* idString, UINT barrelIndex);

    void Destructor_Hook();

private:
    typedef void (GunHandler::*Destructor)();
};
