#include "base_info.h"
#include "Freelancer.h"
#include "utils.h"
#include <cstdio>

// Prints the header name as bold text.
void PrintInfoCategoryHeader_Hook(UINT headerIds, RenderDisplayList &rdl)
{
	WCHAR headerName[128];
	GetFlString(headerIds, headerName, _countof(headerName));

	LPCWSTR rdlBoldTextFmt = L"<RDL><PUSH/><TRA bold=\"true\"/><TEXT>%s</TEXT><TRA bold=\"false\"/><POP/></RDL>";
	swprintf_s(FL_BUFFER_2, FL_BUFFER_LEN, rdlBoldTextFmt, headerName);
	AppendXmlWsToRdl(FL_BUFFER_2, rdl);
}

void InitBaseInfoSpacingFix()
{
    Patch<WORD>(0x476177 + 1, 0x9CA4);                  // removes bold style for the category header
    Hook(0x476203, PrintInfoCategoryHeader_Hook, 5);    // ensure the header is printed manually
    Patch<BYTE>(0x476208 + 1, 0x54);                    // use 5 spaces for the first line
}
