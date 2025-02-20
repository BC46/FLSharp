#include "feature_config.h"
#include "Common.h"

void FeatureManager::RegisterFeature(LPCSTR name, void (*initFunc)(), void (*cleanupFunc)(), bool (*applyPredicate)())
{
    features[name] = FlSharpFeature{ initFunc, cleanupFunc, applyPredicate, true };
}

void FeatureManager::SetFeatureEnabled(LPCSTR name, bool enabled)
{
    std::map<LPCSTR, FlSharpFeature>::iterator it = features.find(name);

    if (it != features.end())
    {
        it->second.enabled = enabled;
    }
}

void FeatureManager::InitFeatures()
{
    std::map<LPCSTR, FlSharpFeature>::iterator it;

    for (it = features.begin(); it != features.end(); ++it)
    {
        FlSharpFeature& feature = it->second;

        if (feature.enabled && feature.applyPredicate() && feature.initFunc)
            feature.initFunc();
    }
}

void FeatureManager::CleanupFeatures()
{
    std::map<LPCSTR, FlSharpFeature>::iterator it;

    for (it = features.begin(); it != features.end(); ++it)
    {
        FlSharpFeature& feature = it->second;

        if (feature.enabled && feature.applyPredicate() && feature.cleanupFunc)
            feature.cleanupFunc();
    }
}

bool ApplyAlways()
{
    return true;
}

bool ApplyOnlyOnClient()
{
    return !IsMPServer();
}
