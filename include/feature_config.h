#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string.h>
#include <map>

struct FlSharpFeature
{
    void (*initFunc)();         // feature's init function
    void (*cleanupFunc)();      // feature's cleanup function
    bool (*applyPredicate)();   // function that determines whether the feature must be applied from a technical perspective
    bool enabled;               // value determined by the user so they can choose whether they want it to be applied
};

struct CmpStr
{
    bool operator()(LPCSTR a, LPCSTR b) const
    {
        return stricmp(a, b) < 0;
    }
};

class FeatureManager
{
public:
    void RegisterFeature(LPCSTR name, void (*initFunc)(), void (*cleanupFunc)(), bool (*applyPredicate)());
    void SetFeatureEnabled(LPCSTR name, bool enabled);
    void InitFeatures();
    void CleanupFeatures();

private:
    std::map<LPCSTR, FlSharpFeature, CmpStr> features;
};

bool ApplyAlways();
bool ApplyOnlyOnClient();
