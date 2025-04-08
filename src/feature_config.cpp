#include "feature_config.h"
#include "Common.h"

void FeatureManager::RegisterFeature(LPCSTR name, void (*initFunc)(), void (*cleanupFunc)(), bool (*applyPredicate)())
{
    // Enable the feature by default.
    FlSharpFeature feature { initFunc, cleanupFunc, applyPredicate, true };
    features.insert({ name, feature });
}

void FeatureManager::SetFeatureEnabled(LPCSTR name, bool enabled)
{
    const auto it = features.find(name);

    if (it != features.end())
    {
        it->second.enabled = enabled;
    }
}

void FeatureManager::InitFeatures()
{
    for (const auto& it : features)
    {
        const FlSharpFeature& feature = it.second;

        if (feature.enabled && feature.initFunc && feature.applyPredicate())
            feature.initFunc();
    }
}

void FeatureManager::CleanupFeatures()
{
    for (const auto& it : features)
    {
        const FlSharpFeature& feature = it.second;

        if (feature.enabled && feature.cleanupFunc && feature.applyPredicate())
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
