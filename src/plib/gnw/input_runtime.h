#ifndef FALLOUT_PLIB_GNW_INPUT_RUNTIME_H_
#define FALLOUT_PLIB_GNW_INPUT_RUNTIME_H_

namespace fallout {

typedef enum InputRuntimeCaptureRequestSource {
    INPUT_RUNTIME_CAPTURE_REQUEST_SOURCE_NONE,
    INPUT_RUNTIME_CAPTURE_REQUEST_SOURCE_EXPLICIT,
    // Temporary Linux desktop-default bridge. Remove this source once the host
    // owns capture policy directly and no longer relies on GNW_mouse_init to
    // request startup capture on behalf of desktop runtime builds.
    INPUT_RUNTIME_CAPTURE_REQUEST_SOURCE_LINUX_STARTUP_ADAPTER,
} InputRuntimeCaptureRequestSource;

typedef struct InputRuntimeState {
    bool mouseCaptureRequested;
    bool mouseCaptureApplied;
    bool isActive;
    bool hasFocus;
    bool canApplyMouseCapture;
    bool hasPendingMouseCaptureSync;
    InputRuntimeCaptureRequestSource mouseCaptureRequestSource;
} InputRuntimeState;

void input_runtime_reset();
InputRuntimeState input_runtime_get_state();
void input_runtime_request_mouse_capture(InputRuntimeCaptureRequestSource source);
void input_runtime_release_mouse_capture();
void input_runtime_set_mouse_capture_applied(bool applied);
void input_runtime_set_active(bool active);
void input_runtime_set_focused(bool focused);
bool input_runtime_should_apply_mouse_capture();
bool input_runtime_has_pending_mouse_capture_sync();
bool input_runtime_is_startup_capture_adapter_active();

} // namespace fallout

#endif /* FALLOUT_PLIB_GNW_INPUT_RUNTIME_H_ */
