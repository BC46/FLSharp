#include "infocards.h"
#include "Common.h"
#include "utils.h"
#include "cmpstr.h"

std::map<UINT, UINT> msnBaseIdsInfoMap;
std::map<UINT, UINT> msnNicknameIdsInfoMap;

void ParseEntries(std::map<UINT, UINT>& map, INI_Reader& reader, const std::map<LPCSTR, InfocardEntry, CmpStr>& entries)
{
    while (reader.read_header())
    {
        const auto it = entries.find(reader.get_header_ptr());

        if (it == entries.end())
            continue;

        UINT key = 0, value = 0;

        while (reader.read_value())
        {
            if (reader.is_value(it->second.key))
            {
                key = reader.get_value_id();
            }
            else if (reader.is_value(it->second.value))
            {
                value = reader.get_value_int();
            }
        }

        it->second.map.insert({ key, value });
    }
}

// Parses the MissionCreatedSolars.ini file and for every base stores its ids_info in a map.
void ParseMsnCreatedSolars(LPCSTR iniPath)
{
    INI_Reader reader;

    if (!reader.open(iniPath))
        return;

    std::map<LPCSTR, InfocardEntry, CmpStr> entries = {
        { "MissionCreatedSolar", { msnBaseIdsInfoMap, "base", "ids_info" } },
        { "MissionCreatedNonDockableSolar", { msnNicknameIdsInfoMap, "nickname", "ids_info" } }
    };

    ParseEntries(msnBaseIdsInfoMap, reader, entries);
    reader.close();
}

bool FindInMap(std::map<UINT, UINT>& map, UINT key, UINT& foundValue)
{
    auto it = map.find(key);
    if (it == map.end())
        return false;

    foundValue = it->second;
    return true;
}

// Function which Freelancer calls to obtain the ids infocard of the selected object in the Current Info window.
int GetInfocard_Hook(CObject* selectedObj, const int &id, UINT &idsInfo)
{
    // Is the selected object a solar?
    if (const CSolar* solar = CSolar::cast(selectedObj))
    {
        if (solar->is_dynamic())
        {
            // Try to find the idsInfo in the base map.
            if (solar->is_base() && FindInMap(msnBaseIdsInfoMap, solar->baseId, idsInfo))
                return S_OK;

            // Otherwise try the nickname map.
            if (FindInMap(msnNicknameIdsInfoMap, solar->nickname, idsInfo))
                return S_OK;
        }
    }

    // If the above approaches failed, get the infocard by calling the original function.
    return Reputation::Vibe::GetInfocard(id, idsInfo);
}

// In Freelancer, when opening the Current Info window on a dynamic solar, it won't display its infocard.
// Presumably this happens because they are not stored by the server.
// A workaround is to first parse MissionCreatedSolars.ini and store the values.
// Then hook the get infocard function for the Current Info window, check if the selected object is a dynamic solar,
// if so, return the stored ids_info.
void InitDynamicSolarInfocards()
{
    // Get the full path to MissionCreatedSolars.ini dynamically.
    char fullIniPath[MAX_PATH];
    strcpy_s(fullIniPath, sizeof(fullIniPath), "..\\DATA\\");
    LPCSTR relIniPath = GetValue<LPCSTR>(0x476C7A); // Universe\\MissionCreatedSolars.ini
    strcat_s(fullIniPath, sizeof(fullIniPath), relIniPath);

    ParseMsnCreatedSolars(fullIniPath);

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
