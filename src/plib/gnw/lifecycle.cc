#include "plib/gnw/lifecycle.h"

namespace fallout {

static LifecycleState gLifecycleState = {
    LIFECYCLE_PHASE_IDLE,
    LIFECYCLE_REQUEST_NONE,
    LIFECYCLE_REASON_NONE,
    0,
};

void lifecycle_reset()
{
    gLifecycleState.phase = LIFECYCLE_PHASE_IDLE;
    gLifecycleState.request = LIFECYCLE_REQUEST_NONE;
    gLifecycleState.reason = LIFECYCLE_REASON_NONE;
    gLifecycleState.code = 0;
}

void lifecycle_set_phase(LifecyclePhase phase)
{
    gLifecycleState.phase = phase;
}

LifecycleState lifecycle_get_state()
{
    return gLifecycleState;
}

bool lifecycle_is_quit_requested()
{
    return gLifecycleState.request == LIFECYCLE_REQUEST_QUIT;
}

bool lifecycle_has_startup_failure()
{
    return gLifecycleState.request == LIFECYCLE_REQUEST_STARTUP_FAILURE;
}

void lifecycle_request_quit(LifecycleReason reason)
{
    gLifecycleState.request = LIFECYCLE_REQUEST_QUIT;
    gLifecycleState.reason = reason;
    gLifecycleState.code = 0;
}

void lifecycle_report_startup_failure(LifecycleReason reason, int code)
{
    gLifecycleState.request = LIFECYCLE_REQUEST_STARTUP_FAILURE;
    gLifecycleState.reason = reason;
    gLifecycleState.code = code;
}

} // namespace fallout
