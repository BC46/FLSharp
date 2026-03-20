#pragma once

#define IMPORT __declspec(dllimport)

class CGunWrapper
{
public:
    IMPORT static void __cdecl Shutdown();
};

class CDPClient
{
};

#define FL_CDP_CLIENT ((CDPClient*) 0x67E7BC)
