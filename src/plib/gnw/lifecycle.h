#ifndef FALLOUT_PLIB_GNW_LIFECYCLE_H_
#define FALLOUT_PLIB_GNW_LIFECYCLE_H_

namespace fallout {

typedef enum LifecyclePhase {
    LIFECYCLE_PHASE_IDLE,
    LIFECYCLE_PHASE_STARTUP,
    LIFECYCLE_PHASE_RUNNING,
    LIFECYCLE_PHASE_SHUTDOWN,
} LifecyclePhase;

typedef enum LifecycleRequest {
    LIFECYCLE_REQUEST_NONE,
    LIFECYCLE_REQUEST_QUIT,
    LIFECYCLE_REQUEST_STARTUP_FAILURE,
} LifecycleRequest;

typedef enum LifecycleReason {
    LIFECYCLE_REASON_NONE,
    LIFECYCLE_REASON_WINDOW_CLOSE,
    LIFECYCLE_REASON_WINDOW_SYSTEM,
} LifecycleReason;

typedef struct LifecycleState {
    LifecyclePhase phase;
    LifecycleRequest request;
    LifecycleReason reason;
    int code;
} LifecycleState;

void lifecycle_reset();
void lifecycle_set_phase(LifecyclePhase phase);
LifecycleState lifecycle_get_state();
bool lifecycle_is_quit_requested();
bool lifecycle_has_startup_failure();
void lifecycle_request_quit(LifecycleReason reason);
void lifecycle_report_startup_failure(LifecycleReason reason, int code);

} // namespace fallout

#endif /* FALLOUT_PLIB_GNW_LIFECYCLE_H_ */
