#include "plib/gnw/input_runtime.h"

namespace fallout {

static InputRuntimeState gInputRuntimeState = {
    false,
    false,
    true,
    true,
    true,
    false,
    INPUT_RUNTIME_CAPTURE_REQUEST_SOURCE_NONE,
};

static void input_runtime_refresh_state()
{
    gInputRuntimeState.canApplyMouseCapture = gInputRuntimeState.isActive
        && gInputRuntimeState.hasFocus;
    gInputRuntimeState.hasPendingMouseCaptureSync = gInputRuntimeState.mouseCaptureRequested != gInputRuntimeState.mouseCaptureApplied
        || (gInputRuntimeState.mouseCaptureRequested && !gInputRuntimeState.canApplyMouseCapture);
}

void input_runtime_reset()
{
    gInputRuntimeState.mouseCaptureRequested = false;
    gInputRuntimeState.mouseCaptureApplied = false;
    gInputRuntimeState.isActive = true;
    gInputRuntimeState.hasFocus = true;
    gInputRuntimeState.mouseCaptureRequestSource = INPUT_RUNTIME_CAPTURE_REQUEST_SOURCE_NONE;
    input_runtime_refresh_state();
}

InputRuntimeState input_runtime_get_state()
{
    return gInputRuntimeState;
}

void input_runtime_request_mouse_capture(InputRuntimeCaptureRequestSource source)
{
    gInputRuntimeState.mouseCaptureRequested = true;
    gInputRuntimeState.mouseCaptureRequestSource = source != INPUT_RUNTIME_CAPTURE_REQUEST_SOURCE_NONE
        ? source
        : INPUT_RUNTIME_CAPTURE_REQUEST_SOURCE_EXPLICIT;
    input_runtime_refresh_state();
}

void input_runtime_release_mouse_capture()
{
    gInputRuntimeState.mouseCaptureRequested = false;
    gInputRuntimeState.mouseCaptureRequestSource = INPUT_RUNTIME_CAPTURE_REQUEST_SOURCE_NONE;
    input_runtime_refresh_state();
}

void input_runtime_set_mouse_capture_applied(bool applied)
{
    gInputRuntimeState.mouseCaptureApplied = applied;
    input_runtime_refresh_state();
}

void input_runtime_set_active(bool active)
{
    gInputRuntimeState.isActive = active;
    input_runtime_refresh_state();
}

void input_runtime_set_focused(bool focused)
{
    gInputRuntimeState.hasFocus = focused;
    input_runtime_refresh_state();
}

bool input_runtime_should_apply_mouse_capture()
{
    return gInputRuntimeState.mouseCaptureRequested && gInputRuntimeState.canApplyMouseCapture;
}

bool input_runtime_has_pending_mouse_capture_sync()
{
    return gInputRuntimeState.hasPendingMouseCaptureSync;
}

bool input_runtime_is_startup_capture_adapter_active()
{
    return gInputRuntimeState.mouseCaptureRequestSource == INPUT_RUNTIME_CAPTURE_REQUEST_SOURCE_LINUX_STARTUP_ADAPTER;
}

} // namespace fallout
