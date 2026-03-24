#include <catch2/catch_test_macros.hpp>

#include "plib/gnw/input_runtime.h"

namespace {

class ScopedInputRuntimeReset {
public:
    ScopedInputRuntimeReset()
    {
        fallout::input_runtime_reset();
    }

    ~ScopedInputRuntimeReset()
    {
        fallout::input_runtime_reset();
    }
};

TEST_CASE("Input runtime starts in the neutral active-and-focus-ready state")
{
    ScopedInputRuntimeReset reset;

    fallout::InputRuntimeState state = fallout::input_runtime_get_state();

    REQUIRE_FALSE(state.mouseCaptureRequested);
    REQUIRE_FALSE(state.mouseCaptureApplied);
    REQUIRE(state.isActive);
    REQUIRE(state.hasFocus);
    REQUIRE(state.canApplyMouseCapture);
    REQUIRE_FALSE(state.hasPendingMouseCaptureSync);
    REQUIRE(state.mouseCaptureRequestSource == fallout::INPUT_RUNTIME_CAPTURE_REQUEST_SOURCE_NONE);
    REQUIRE_FALSE(fallout::input_runtime_should_apply_mouse_capture());
    REQUIRE_FALSE(fallout::input_runtime_has_pending_mouse_capture_sync());
    REQUIRE_FALSE(fallout::input_runtime_is_startup_capture_adapter_active());
}

TEST_CASE("Input runtime keeps request and applied capture as separate observable states")
{
    ScopedInputRuntimeReset reset;

    fallout::input_runtime_request_mouse_capture(fallout::INPUT_RUNTIME_CAPTURE_REQUEST_SOURCE_EXPLICIT);

    fallout::InputRuntimeState state = fallout::input_runtime_get_state();

    REQUIRE(state.mouseCaptureRequested);
    REQUIRE_FALSE(state.mouseCaptureApplied);
    REQUIRE(state.canApplyMouseCapture);
    REQUIRE(state.hasPendingMouseCaptureSync);
    REQUIRE(state.mouseCaptureRequestSource == fallout::INPUT_RUNTIME_CAPTURE_REQUEST_SOURCE_EXPLICIT);
    REQUIRE(fallout::input_runtime_should_apply_mouse_capture());
    REQUIRE(fallout::input_runtime_has_pending_mouse_capture_sync());

    fallout::input_runtime_set_mouse_capture_applied(true);

    state = fallout::input_runtime_get_state();

    REQUIRE(state.mouseCaptureRequested);
    REQUIRE(state.mouseCaptureApplied);
    REQUIRE_FALSE(state.hasPendingMouseCaptureSync);
    REQUIRE_FALSE(fallout::input_runtime_has_pending_mouse_capture_sync());

    fallout::input_runtime_release_mouse_capture();

    state = fallout::input_runtime_get_state();

    REQUIRE_FALSE(state.mouseCaptureRequested);
    REQUIRE(state.mouseCaptureApplied);
    REQUIRE(state.hasPendingMouseCaptureSync);
    REQUIRE(state.mouseCaptureRequestSource == fallout::INPUT_RUNTIME_CAPTURE_REQUEST_SOURCE_NONE);
    REQUIRE_FALSE(fallout::input_runtime_should_apply_mouse_capture());

    fallout::input_runtime_set_mouse_capture_applied(false);

    state = fallout::input_runtime_get_state();

    REQUIRE_FALSE(state.mouseCaptureRequested);
    REQUIRE_FALSE(state.mouseCaptureApplied);
    REQUIRE_FALSE(state.hasPendingMouseCaptureSync);
}

TEST_CASE("Input runtime does not reintroduce pending sync when request state already matches applied capture")
{
    ScopedInputRuntimeReset reset;

    fallout::input_runtime_request_mouse_capture(fallout::INPUT_RUNTIME_CAPTURE_REQUEST_SOURCE_EXPLICIT);
    fallout::input_runtime_set_mouse_capture_applied(true);
    fallout::input_runtime_request_mouse_capture(fallout::INPUT_RUNTIME_CAPTURE_REQUEST_SOURCE_EXPLICIT);

    fallout::InputRuntimeState state = fallout::input_runtime_get_state();

    REQUIRE(state.mouseCaptureRequested);
    REQUIRE(state.mouseCaptureApplied);
    REQUIRE(state.mouseCaptureRequestSource == fallout::INPUT_RUNTIME_CAPTURE_REQUEST_SOURCE_EXPLICIT);
    REQUIRE_FALSE(state.hasPendingMouseCaptureSync);
    REQUIRE_FALSE(fallout::input_runtime_has_pending_mouse_capture_sync());

    fallout::input_runtime_release_mouse_capture();
    fallout::input_runtime_set_mouse_capture_applied(false);
    fallout::input_runtime_release_mouse_capture();

    state = fallout::input_runtime_get_state();

    REQUIRE_FALSE(state.mouseCaptureRequested);
    REQUIRE_FALSE(state.mouseCaptureApplied);
    REQUIRE(state.mouseCaptureRequestSource == fallout::INPUT_RUNTIME_CAPTURE_REQUEST_SOURCE_NONE);
    REQUIRE_FALSE(state.hasPendingMouseCaptureSync);
    REQUIRE_FALSE(fallout::input_runtime_has_pending_mouse_capture_sync());
}

TEST_CASE("Input runtime keeps capture requests pending while inactive")
{
    ScopedInputRuntimeReset reset;

    fallout::input_runtime_request_mouse_capture(fallout::INPUT_RUNTIME_CAPTURE_REQUEST_SOURCE_EXPLICIT);
    fallout::input_runtime_set_mouse_capture_applied(true);
    fallout::input_runtime_set_active(false);

    fallout::InputRuntimeState state = fallout::input_runtime_get_state();

    REQUIRE_FALSE(state.canApplyMouseCapture);
    REQUIRE(state.hasPendingMouseCaptureSync);
    REQUIRE_FALSE(fallout::input_runtime_should_apply_mouse_capture());
    REQUIRE(fallout::input_runtime_has_pending_mouse_capture_sync());

    fallout::input_runtime_set_mouse_capture_applied(false);

    state = fallout::input_runtime_get_state();

    REQUIRE(state.mouseCaptureRequested);
    REQUIRE_FALSE(state.mouseCaptureApplied);
    REQUIRE(state.hasPendingMouseCaptureSync);

    fallout::input_runtime_set_active(true);

    state = fallout::input_runtime_get_state();

    REQUIRE(state.isActive);
    REQUIRE(state.canApplyMouseCapture);
    REQUIRE(state.hasPendingMouseCaptureSync);
    REQUIRE(fallout::input_runtime_should_apply_mouse_capture());

    fallout::input_runtime_set_mouse_capture_applied(true);

    REQUIRE_FALSE(fallout::input_runtime_has_pending_mouse_capture_sync());
}

TEST_CASE("Input runtime keeps capture requests pending while focus is lost")
{
    ScopedInputRuntimeReset reset;

    fallout::input_runtime_request_mouse_capture(fallout::INPUT_RUNTIME_CAPTURE_REQUEST_SOURCE_EXPLICIT);
    fallout::input_runtime_set_mouse_capture_applied(true);
    fallout::input_runtime_set_focused(false);

    fallout::InputRuntimeState state = fallout::input_runtime_get_state();

    REQUIRE_FALSE(state.hasFocus);
    REQUIRE_FALSE(state.canApplyMouseCapture);
    REQUIRE(state.hasPendingMouseCaptureSync);
    REQUIRE_FALSE(fallout::input_runtime_should_apply_mouse_capture());

    fallout::input_runtime_set_mouse_capture_applied(false);
    fallout::input_runtime_set_focused(true);

    state = fallout::input_runtime_get_state();

    REQUIRE(state.hasFocus);
    REQUIRE(state.canApplyMouseCapture);
    REQUIRE(state.hasPendingMouseCaptureSync);
    REQUIRE(fallout::input_runtime_should_apply_mouse_capture());

    fallout::input_runtime_set_mouse_capture_applied(true);

    REQUIRE_FALSE(fallout::input_runtime_has_pending_mouse_capture_sync());
}

TEST_CASE("Input runtime clears pending sync when focus-loss suppression is followed by release")
{
    ScopedInputRuntimeReset reset;

    fallout::input_runtime_request_mouse_capture(fallout::INPUT_RUNTIME_CAPTURE_REQUEST_SOURCE_EXPLICIT);
    fallout::input_runtime_set_mouse_capture_applied(true);
    fallout::input_runtime_set_focused(false);
    fallout::input_runtime_set_mouse_capture_applied(false);

    fallout::InputRuntimeState state = fallout::input_runtime_get_state();

    REQUIRE(state.mouseCaptureRequested);
    REQUIRE_FALSE(state.mouseCaptureApplied);
    REQUIRE_FALSE(state.canApplyMouseCapture);
    REQUIRE(state.hasPendingMouseCaptureSync);
    REQUIRE_FALSE(fallout::input_runtime_should_apply_mouse_capture());

    fallout::input_runtime_release_mouse_capture();

    state = fallout::input_runtime_get_state();

    REQUIRE_FALSE(state.mouseCaptureRequested);
    REQUIRE_FALSE(state.mouseCaptureApplied);
    REQUIRE(state.mouseCaptureRequestSource == fallout::INPUT_RUNTIME_CAPTURE_REQUEST_SOURCE_NONE);
    REQUIRE_FALSE(state.hasPendingMouseCaptureSync);
    REQUIRE_FALSE(fallout::input_runtime_has_pending_mouse_capture_sync());
}

TEST_CASE("Input runtime exposes the temporary Linux startup capture adapter until explicit ownership takes over")
{
    ScopedInputRuntimeReset reset;

    fallout::input_runtime_request_mouse_capture(fallout::INPUT_RUNTIME_CAPTURE_REQUEST_SOURCE_LINUX_STARTUP_ADAPTER);

    fallout::InputRuntimeState state = fallout::input_runtime_get_state();

    REQUIRE(state.mouseCaptureRequestSource == fallout::INPUT_RUNTIME_CAPTURE_REQUEST_SOURCE_LINUX_STARTUP_ADAPTER);
    REQUIRE(fallout::input_runtime_is_startup_capture_adapter_active());
    REQUIRE(state.hasPendingMouseCaptureSync);

    fallout::input_runtime_set_mouse_capture_applied(true);
    fallout::input_runtime_request_mouse_capture(fallout::INPUT_RUNTIME_CAPTURE_REQUEST_SOURCE_EXPLICIT);

    state = fallout::input_runtime_get_state();

    REQUIRE(state.mouseCaptureRequestSource == fallout::INPUT_RUNTIME_CAPTURE_REQUEST_SOURCE_EXPLICIT);
    REQUIRE_FALSE(fallout::input_runtime_is_startup_capture_adapter_active());
    REQUIRE_FALSE(state.hasPendingMouseCaptureSync);
}

} // namespace
