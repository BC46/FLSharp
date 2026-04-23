#include "cgun_wrapper.h"
#include "DALib.h"
#include "utils.h"
#include "fl_func.h"

FL_FUNC(void exit_Original(int const status), dword ptr ds:[0x5C713C])

void exit_Hook(int const status)
{
    // Call the original function with WaitForSingleObject.
    CGunWrapper::Shutdown();

    // Call the original exit function.
    exit_Original(status);
}

bool noQuitMsgRetrieved = true;
bool (*HandleMessages_Original)(WPARAM *msgWParam);

bool HandleMessages_Hook(WPARAM *msgWParam)
{
    bool result = HandleMessages_Original(msgWParam);
    return noQuitMsgRetrieved &= result;
}


// In Freelancer, when you close the server list menu (provided that there were servers listed),
// a thread would be created that closes the DirectPlay connection (takes 15-30 seconds to execute).
// If you quit the game before the DirectPlay was connection closed, a WaitForSingleObject call would be made
// which actually waited indefinitely for the thread to finish.
// Consequently, the Freelancer process would remain open until forcefully closed (via Task Manager for instance).
// Turns out that whenever the thread calls FreeLibrary after FL's main exit function had already been called,
// that FreeLibrary call would never return, and thus the thread would never finish its task.
// I believe this was caused by a deadlock. Yet, I could not explain why this deadlock would occur under these circumstances,
// nor was I able to come up with a "clean fix" for it. So now instead of calling the function with WaitForSingleObject after the exit,
// I call it before the exit. The thread still takes a very long time to close the DirectPlay connection (which I think is a bug too),
// but at least there is no more deadlock and the Freelancer process will eventually close, as it should.

// TODO: If anyone would like to look into this further: in dpnet.dll there's a function called "DN_Close" (locate it by downloading the debug symbols from Microsoft).
// I believe this function is supposed to represent IDirectPlay8Client::Close. It is this exact function that takes ~40 seconds to return on my end.
// This seems strange since in all online examples I could find that closed some DirectPlay connection, it is always done on the main thread.
// Surely, something must have been done incorrectly in one of the DirectPlay calls. Since DA couldn't figure out what,
// they took the band-aid approach and closed the connection on a separate thread.
// Otherwise the screen freezes for 40 seconds every time the server list menu is closed.

// TODO Idea: File offset 0x30896 in gundll.dll. This is a call to IDirectPlay8Client::Connect.
// phAsyncHandle
//      A DPNHANDLE. When the method returns, phAsyncHandle will point to a handle that you can pass to IDirectPlay8Client::CancelAsyncOperation to cancel the operation.
//      This parameter must be set to NULL if you set the DPNCONNECT_SYNC flag in dwFlags.
// What happens if you call IDirectPlay8Client::CancelAsyncOperation before? See dplay.doc in Downloads folder.
// IDirectPlay8Client::CancelAsyncOperation
// Cancels asynchronous requests. Many methods of the IDirectPlay8Client interface run asynchronously by default. Depending on the situation, you might want to cancel requests before they are processed. All the methods of this interface that can be run asynchronously return a hAsyncHandle parameter.
// Specific requests are canceled by passing the hAsyncHandle of the request in this method’s hAsyncHandle parameter. You can cancel all pending asynchronous operations by calling this method, specifying NULL in the hAsyncHandle parameter, and specifying DPNCANCEL_ALL_OPERATIONS in the dwFlags parameter. If a specific handle is provided to this method, no flags should be set.
// DirectPlayClient->CancelAsyncOperation( NULL, DPNCANCEL_ALL_OPERATIONS ); Find where DirectPlayClient is

// gundll.dll: file offset 0x8376. change xor esi, esi in the function call to mov esi, 1. This fixes the 30 second timer
// Test if this works in Win XP too
// dalib.dll: file offset 0x4C82 = load library call of gundll.dll. Use this to set hooks
// dxcheckOK( DirectPlayClient->Close(DPNCLOSE_IMMEDIATE) ); // WARNING DPNCLOSE_IMMEDIATE is a DP feature from DirectX 9 (released shortly after FL came out)
// 		SafeRelease( DirectPlayClient );

void InitPostGameDeadlockFix()
{
    #define CGUNWRAPPER_SHUTDOWN_CALL_ADDR 0x5B2190
    #define FL_EXE_EXIT_CALL_ADDR 0x5B81C6

    Nop(CGUNWRAPPER_SHUTDOWN_CALL_ADDR, 5); // nop out the post-game CGunWrapper::Shutdown() call; call it in the exit hook instead
    Hook(FL_EXE_EXIT_CALL_ADDR, exit_Hook, 6);

    #define HANDLE_MESSAGES_ADDR 0x5B0B60
    HandleMessages_Original = Trampoline(HANDLE_MESSAGES_ADDR, HandleMessages_Hook, 5);
}

void CleanupPostGameDeadlockFix()
{
    CleanupTrampoline(HandleMessages_Original);
}
