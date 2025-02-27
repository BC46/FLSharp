#include "cgun_wrapper.h"
#include "DALib.h"
#include "utils.h"
#include "stdlib.h"
#include "fl_func.h"

DWORD exitCallAddress = NULL;

FL_FUNC(void exit_Original(int const status), exitCallAddress)

void exit_Hook(int const status)
{
    // Call the original function with WaitForSingleObject.
    CGunWrapper::Shutdown();

    // Call the original exit function.
    exit_Original(status);
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
void InitPostGameDeadlockFix()
{
    #define CGUNWRAPPER_SHUTDOWN_CALL_ADDR 0x5B2190
    #define FL_EXE_EXIT_CALL_ADDR 0x5B81C6

    Nop(CGUNWRAPPER_SHUTDOWN_CALL_ADDR, 5); // nop out the post-game CGunWrapper::Shutdown() call; call it in the exit hook instead
    exitCallAddress = *((PDWORD) 0x5C713C);
    Hook(FL_EXE_EXIT_CALL_ADDR, exit_Hook, 6);
}
