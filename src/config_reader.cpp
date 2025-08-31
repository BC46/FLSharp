#include "config_reader.h"
#include "Common.h"
#include "feature_config.h"
#include "logger.h"

void ReadConfig(LPCSTR path, FeatureManager &manager)
{
    INI_Reader reader;

    if (!reader.open(path))
        return;

    while (reader.read_header())
    {
        while (reader.read_value())
        {
            if (!manager.SetFeatureEnabled(reader.get_name_ptr(), reader.get_value_bool()))
            {
                Logger::PrintInvalidFeatureWarning("ReadConfig", reader.get_name_ptr(), reader.get_file_name());
            }
        }
    }

    reader.close();
}
