#include "infocards.h"
#include "Common.h"
#include "utils.h"
#include "map"

std::map<UINT, UINT> msnBaseIdsInfoMap;

// Parses the MissionCreatedSolars.ini file and for every base stores its ids_info in a map.
void ParseMsnCreatedSolars(LPCSTR iniPath)
{
    INI_Reader reader;

    if (!reader.open(iniPath))
        return;

    while (reader.read_header())
    {
        if (!reader.is_header("MissionCreatedSolar"))
            continue;

        UINT baseId = 0, idsInfo = 0;

        while (reader.read_value())
        {
            if (reader.is_value("base"))
            {
                baseId = reader.get_value_id();
            }
            else if (reader.is_value("ids_info"))
            {
                // TODO: Maybe just use get_value_int? FL uses get_value_uint though.
                idsInfo = reader.get_value_uint();
            }
        }

        msnBaseIdsInfoMap.insert({ baseId, idsInfo });
    }

    reader.close();
}

// Function which Freelancer calls to obtain the ids infocard of the selected object in the Current Info window.
int GetInfocard_Hook(CObject* selectedObj, const int &id, UINT &idsInfo)
{
    // Is the selected object a solar?
    if (CSolar* solar = CSolar::cast(selectedObj))
    {
        // Is the selected object a dynamic base?
        if (solar->baseId && solar->is_dynamic())
        {
            // Try to find the idsInfo in the map.
            const auto it = msnBaseIdsInfoMap.find(solar->baseId);

            if (it != msnBaseIdsInfoMap.end())
            {
                // Found it!
                idsInfo = it->second;
                return 0; // 0 = OK
            }
        }
    }

    // If one of the above conditions are not met, get the infocard by calling the original function.
    return Reputation::Vibe::GetInfocard(id, idsInfo);
}

// In Freelancer, when opening the Current Info window on a dynamic solar, it won't display its infocard.
// Presumably this happens because they are not stored by the server.
// A workaround is to first parse MissionCreatedSolars.ini and store the values.
// Then hook the get infocard function for the Current Info window, check if the selected object is a dynamic solar,
// if so, return the stored ids_info.
void InitDynamicSolarInfocards()
{
    // Get the relative path to MissionCreatedSolars.ini dynamically.
    // TODO: Use GetFileSysPath(relIniPath, iniDataPath, nullptr);
    char relIniPath[MAX_PATH];
    strcpy_s(relIniPath, sizeof(relIniPath), "..\\DATA\\");

    LPCSTR iniDataPath = GetValue<LPCSTR>(0x476C7A); // Universe\\MissionCreatedSolars.ini
    strcat_s(relIniPath, sizeof(relIniPath), iniDataPath);

    ParseMsnCreatedSolars(relIniPath);

    // Add a "push esi" instruction so we can check out the selected CObject in our hook.
    #define GET_INFOCARD_CURRENT_INFO_CALL_ADDR 0x475BD8
    Patch<BYTE>(GET_INFOCARD_CURRENT_INFO_CALL_ADDR, 0x56); // push esi (CObject*)
    Hook(GET_INFOCARD_CURRENT_INFO_CALL_ADDR + 1, GetInfocard_Hook, 5);

    // Fix the stack offset of the return value (shifted by 4 bytes due to the added parameter).
    #define GET_INFOCARD_IDS_STACK_OFFSET 0x475BE1
    GetValue<BYTE>(GET_INFOCARD_IDS_STACK_OFFSET) += sizeof(DWORD);

    // Increase the amount of stack bytes cleaned because the GetInfocard hook takes an additional parameter.
    #define GET_INFOCARD_RET_STACK_SIZE 0x475BE4
    GetValue<BYTE>(GET_INFOCARD_RET_STACK_SIZE) += sizeof(DWORD);
}
