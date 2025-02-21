#include "config_reader.h"
#include "Common.h"
#include "feature_config.h"

void ReadConfig(LPCSTR path, FeatureManager &manager)
{
    INI_Reader reader;

    if (!reader.open(path))
        return;

    while (reader.read_header())
    {
        while (reader.read_value())
        {
            manager.SetFeatureEnabled(reader.get_name_ptr(), reader.get_value_bool());
        }
    }

    reader.close();
}
