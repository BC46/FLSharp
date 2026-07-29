#include "Freelancer.h"

struct HUD_Target
{
    BYTE x00[0x4D0];
    FlUiElement* targetScanButton; // 0x4D0
    FlUiElement* targetTractorButton; // 0x4D4
    FlUiElement* targetCloseButton; // 0x4D8
    FlUiElement* targetPreviousButton; // 0x4DC
    FlUiElement* targetNextButton; // 0x4E0
    FlUiElement* targetCommButton; // 0x4E4
    FlUiElement* tradeRequestGroupButton; // 0x4E8
    BYTE x4EC[0x120];
    FlUiElement* formationList; // 0x60C
    BYTE x610[0x8];
    bool isPlayerInFormation; // 0x618
};

struct Target
{
    BYTE x00[0x38];
    HUD_Target hudTarget; // 0x38

    DWORD HandleSubtargetHotkey(bool previous);
    DWORD HandleFormationListHotkey();
};

void InitSubTargetFix();

void InitFormationListFix();
