#include "base_info.h"
#include "Freelancer.h"
#include "utils.h"
#include <cstdio>

// Prints the header name in bold.
void PrintInfoCategoryHeader_Hook(UINT headerIds, RenderDisplayList &rdl)
{
	WCHAR headerName[128];
	GetFlString(headerIds, headerName, _countof(headerName));

	LPCWSTR rdlBoldTextFmt = L"<RDL><PUSH/><TRA bold=\"true\"/><TEXT>%s</TEXT><TRA bold=\"false\"/><POP/></RDL>";
	swprintf_s(FL_BUFFER_2, FL_BUFFER_LEN, rdlBoldTextFmt, headerName);
	AppendXmlWsToRdl(FL_BUFFER_2, rdl);
}

// If you open the "Current Information" window of a base, it shows which ships,
// equipment, and commodities it is selling/buying. Every item displayed under each category
// is preceded by a number of spaces. The first entry has 5 spaces and all the others 4.
// The different number of spaces was done to fix a misalignment visible on lower 4:3 resolutions.
// However, the misalignment still happens on higher resolutions; it is caused by the bold header category text.
// This is because the bold closing tag is not added in the correct place (at least I think).
// Hence the spaces which are added for the first entry are bold and are thus wider than normal on some resolutions.
// This hook fixes it by making sure all entries use 5 spaces and printing the bold header text correctly.
void InitBaseInfoSpacingFix()
{
    // Stores for each category, the headerStyleAddr and headerNamePrintAddr, respectively.
    // The address of the first spacing string is always 6 bytes in front of headerNamePrintAddr.
    const BaseInfoCat baseInfoCategories[] = {
        { 0x476177, 0x476203 }, // Ships For Sale (ids 0x669/1641)
        { 0x476388, 0x476414 }, // Commodities Selling (ids 0x668/1640)
        { 0x4765F4, 0x476684 }, // Commodities Buying (ids 0x667/1639)
        { 0x476939, 0x4769E7 }  // Equipment For Sale (ids 0x66A/1642)
    };

    for (const auto &baseInfoCat : baseInfoCategories)
    {
        Patch<WORD>(baseInfoCat.headerStyleAddr + 1, 0x9CA4);                   // remove the bold style for the category header
        Hook(baseInfoCat.headerNamePrintAddr, PrintInfoCategoryHeader_Hook, 5); // ensure the header is printed manually, in bold
        Patch<BYTE>(baseInfoCat.headerNamePrintAddr + 6, 0x54);                 // use 5 spaces for the first line
    }
}
