#include "exit.h"
#include "utils.h"
#include "logger.h"

// Function which is called to close the DirectPlay connection.
long STDCALL IDirectPlay8Client::Close_Hook()
{
    // According to the ancient DirectPlay documentation, this function can be called to close all the threads.
    CancelAsyncOperation(0, DPNCANCEL_ALL_OPERATIONS);

    // The CancelAsyncOperation call alone is not enough to fix the Close function taking ~45 seconds to finish.
    // Passing DPNCLOSE_IMMEDIATE (0x1) to Close does fix it.
    // However, this parameter is part of the DirectX 9 SDK while Freelancer uses DirectX 8.
    // Still, Freelancer comes bundled with a DirectX 9 installer, so using this parameter shouldn't cause any issues.
    return Close(DPNCLOSE_IMMEDIATE);
}

// Just close the client connection immediately in this hook because CancelAsyncOperation is already called in the original code.
long STDCALL IDirectPlay8Client::Close_Hook2()
{
    return Close(DPNCLOSE_IMMEDIATE);
}

typedef DWORD (WINAPI *CloseDirectPlayConnection)(LPVOID lpParameter);

// Hook function that FL calls to create the thread that closes the DirectPlay connection.
// We will not create a thread, but instead call the routine on the main thread with a couple of patches.
// Not creating a thread fixes the deadlock.
HANDLE WINAPI CreateThread_Hook(LPSECURITY_ATTRIBUTES lpThreadAttributes,
    SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress,
    LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId)
{
    // TODO: Instead patch this function at the LoadLibraryA call at dalib.dll 0x4C82
    // Patch the call to the DirectPlay Close function to ensure all async operations are properly canceled first.
    DWORD gundllHandle = (DWORD) GetModuleHandle("gundll.dll");
    if (gundllHandle)
    {
        #define CLOSE_DP_CLIENT_CONNECTION_F_OF_GUN (0x302EF)
        Patch<WORD>(gundllHandle + CLOSE_DP_CLIENT_CONNECTION_F_OF_GUN, 0x50); // push eax
        Hook(gundllHandle + CLOSE_DP_CLIENT_CONNECTION_F_OF_GUN + 1, &IDirectPlay8Client::Close_Hook, 6);
    }

    // Call the CloseDirectPlayConnection function on the main thread.
    ((CloseDirectPlayConnection) lpStartAddress)(lpParameter);

    return (HANDLE) 0;
}

// In Freelancer, when you close the server list menu (provided that there were servers listed),
// a thread would be created that closes the DirectPlay connection (takes ~45 seconds to execute).
// If you quit the game before the DirectPlay was connection closed, a WaitForSingleObject call would be made
// which actually waited indefinitely for the thread to finish.
// Consequently, the Freelancer process would remain open until forcefully closed (via Task Manager for instance).
// Turns out that whenever the thread calls FreeLibrary after FL's main exit function had already been called,
// that FreeLibrary call would never return, and thus the thread would never finish its task.
// After looking at the few DirectPlay samples I could find online, I came to the conclusion that the problem must be a deadlock.
// Freelancer spins up a thread to close the DirectPlay connection. None of the samples I checked did this; they closed the connection on the main thread.
// Since threads are usually the reason why deadlocks happen, I implemented a solution where the connection is closed on the main thread.
// The long waiting time is fixed by essentially forcing the connection to close. Now the closing time is so short that it will not freeze the main thread.
void InitPostGameDeadlockFix()
{
    DWORD dalibHandle = (DWORD) GetModuleHandle("dalib.dll");

    if (!dalibHandle)
    {
        Logger::PrintModuleError("InitPostGameDeadlockFix", "dalib.dll");
        return;
    }

    #define CREATE_THREAD_DP_CLOSE_CALL_F_OF (0x4D8E + 0xC00)
    Hook(dalibHandle + CREATE_THREAD_DP_CLOSE_CALL_F_OF, CreateThread_Hook, 5);
    PatchBytes(dalibHandle + CREATE_THREAD_DP_CLOSE_CALL_F_OF + 5, { 0xE9, 0xCB, 0x00, 0x00, 0x00 }); // jmp

    // This patch does "push esi" instead of "push 0", so there are not enough bytes to simply patch it to "push 1".
    // Instead, a hook is needed to change the parameter.
    #define CLOSE_DP_CLIENT_CONNECTION_CALL_F_OF1 (0x1615 + 0xC00)
    Patch<WORD>(dalibHandle + CLOSE_DP_CLIENT_CONNECTION_CALL_F_OF1, 0x50); // push eax
    Hook(dalibHandle + CLOSE_DP_CLIENT_CONNECTION_CALL_F_OF1 + 1, &IDirectPlay8Client::Close_Hook2, 6);

    #define CLOSE_DP_CLIENT_CONNECTION_CALL_F_OF2 (0x170C + 0xC00)
    Patch<BYTE>(dalibHandle + CLOSE_DP_CLIENT_CONNECTION_CALL_F_OF2 + 1, 1);

    #define CLOSE_DP_CLIENT_CONNECTION_CALL_F_OF3 (0x1BAC + 0xC00)
    Patch<BYTE>(dalibHandle + CLOSE_DP_CLIENT_CONNECTION_CALL_F_OF3 + 1, 1);
}

bool noQuitMsgRetrieved = true;
bool (*HandleMessages_Original)(WPARAM *msgWParam);

bool HandleMessages_Hook(WPARAM *msgWParam)
{
    bool result = HandleMessages_Original(msgWParam);
    return noQuitMsgRetrieved &= result;
}

// Freelancer has a message handler function which can be called from multiple places.
// Normally it is called by the "main" function. However, if for example you see the "disconnected"
// dialog, the message handler is actually called from somewhere else.
// This is normally not a problem, unless you exit the game while that dialog is showing.
// The message handler function returns false if the "Quit" message was retrieved.
// When the main caller sees that false was returned, it exits from the loop and shuts down the game.
// This does not happen when the disconnected dialog is showing; it will continue handling messages as normal.
// The "Quit" message is only retrieved once, so when the main handler takes over, it will continue handling messages forever,
// despite the the window being closed by the user.
// This hook fixes the problem by always returning false after the message handler returned false at some point.
void InitQuitMessageFix()
{
    #define HANDLE_MESSAGES_ADDR 0x5B0B60

    HandleMessages_Original = Trampoline(HANDLE_MESSAGES_ADDR, HandleMessages_Hook, 5);
}

void CleanupQuitMessageFix()
{
    CleanupTrampoline(HandleMessages_Original);
}
