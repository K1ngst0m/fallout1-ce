#include <catch2/catch_test_macros.hpp>

#include "plib/gnw/lifecycle.h"

namespace {

class ScopedLifecycleReset {
public:
    ScopedLifecycleReset()
    {
        fallout::lifecycle_reset();
    }

    ~ScopedLifecycleReset()
    {
        fallout::lifecycle_reset();
    }
};

TEST_CASE("Lifecycle starts in the neutral idle state")
{
    ScopedLifecycleReset reset;

    fallout::LifecycleState state = fallout::lifecycle_get_state();

    REQUIRE(state.phase == fallout::LIFECYCLE_PHASE_IDLE);
    REQUIRE(state.request == fallout::LIFECYCLE_REQUEST_NONE);
    REQUIRE(state.reason == fallout::LIFECYCLE_REASON_NONE);
    REQUIRE(state.code == 0);
    REQUIRE_FALSE(fallout::lifecycle_is_quit_requested());
    REQUIRE_FALSE(fallout::lifecycle_has_startup_failure());
}

TEST_CASE("Lifecycle reset clears previously recorded state")
{
    ScopedLifecycleReset reset;

    fallout::lifecycle_set_phase(fallout::LIFECYCLE_PHASE_RUNNING);
    fallout::lifecycle_request_quit(fallout::LIFECYCLE_REASON_WINDOW_CLOSE);

    fallout::lifecycle_reset();

    fallout::LifecycleState state = fallout::lifecycle_get_state();

    REQUIRE(state.phase == fallout::LIFECYCLE_PHASE_IDLE);
    REQUIRE(state.request == fallout::LIFECYCLE_REQUEST_NONE);
    REQUIRE(state.reason == fallout::LIFECYCLE_REASON_NONE);
    REQUIRE(state.code == 0);
}

TEST_CASE("Lifecycle records quit requests without discarding phase")
{
    ScopedLifecycleReset reset;

    fallout::lifecycle_set_phase(fallout::LIFECYCLE_PHASE_RUNNING);
    fallout::lifecycle_request_quit(fallout::LIFECYCLE_REASON_WINDOW_CLOSE);

    fallout::LifecycleState state = fallout::lifecycle_get_state();

    REQUIRE(state.phase == fallout::LIFECYCLE_PHASE_RUNNING);
    REQUIRE(state.request == fallout::LIFECYCLE_REQUEST_QUIT);
    REQUIRE(state.reason == fallout::LIFECYCLE_REASON_WINDOW_CLOSE);
    REQUIRE(state.code == 0);
    REQUIRE(fallout::lifecycle_is_quit_requested());
    REQUIRE_FALSE(fallout::lifecycle_has_startup_failure());
}

TEST_CASE("Lifecycle records startup failures with reason and detail code")
{
    ScopedLifecycleReset reset;

    fallout::lifecycle_set_phase(fallout::LIFECYCLE_PHASE_STARTUP);
    fallout::lifecycle_report_startup_failure(fallout::LIFECYCLE_REASON_WINDOW_SYSTEM, 17);

    fallout::LifecycleState state = fallout::lifecycle_get_state();

    REQUIRE(state.phase == fallout::LIFECYCLE_PHASE_STARTUP);
    REQUIRE(state.request == fallout::LIFECYCLE_REQUEST_STARTUP_FAILURE);
    REQUIRE(state.reason == fallout::LIFECYCLE_REASON_WINDOW_SYSTEM);
    REQUIRE(state.code == 17);
    REQUIRE_FALSE(fallout::lifecycle_is_quit_requested());
    REQUIRE(fallout::lifecycle_has_startup_failure());
}

TEST_CASE("Lifecycle startup failures replace earlier quit requests")
{
    ScopedLifecycleReset reset;

    fallout::lifecycle_set_phase(fallout::LIFECYCLE_PHASE_STARTUP);
    fallout::lifecycle_request_quit(fallout::LIFECYCLE_REASON_WINDOW_CLOSE);
    fallout::lifecycle_report_startup_failure(fallout::LIFECYCLE_REASON_WINDOW_SYSTEM, 11);

    fallout::LifecycleState state = fallout::lifecycle_get_state();

    REQUIRE(state.phase == fallout::LIFECYCLE_PHASE_STARTUP);
    REQUIRE(state.request == fallout::LIFECYCLE_REQUEST_STARTUP_FAILURE);
    REQUIRE(state.reason == fallout::LIFECYCLE_REASON_WINDOW_SYSTEM);
    REQUIRE(state.code == 11);
    REQUIRE_FALSE(fallout::lifecycle_is_quit_requested());
    REQUIRE(fallout::lifecycle_has_startup_failure());
}

TEST_CASE("Lifecycle quit requests clear stale startup failure detail codes")
{
    ScopedLifecycleReset reset;

    fallout::lifecycle_set_phase(fallout::LIFECYCLE_PHASE_RUNNING);
    fallout::lifecycle_report_startup_failure(fallout::LIFECYCLE_REASON_WINDOW_SYSTEM, 11);
    fallout::lifecycle_request_quit(fallout::LIFECYCLE_REASON_WINDOW_CLOSE);

    fallout::LifecycleState state = fallout::lifecycle_get_state();

    REQUIRE(state.phase == fallout::LIFECYCLE_PHASE_RUNNING);
    REQUIRE(state.request == fallout::LIFECYCLE_REQUEST_QUIT);
    REQUIRE(state.reason == fallout::LIFECYCLE_REASON_WINDOW_CLOSE);
    REQUIRE(state.code == 0);
    REQUIRE(fallout::lifecycle_is_quit_requested());
    REQUIRE_FALSE(fallout::lifecycle_has_startup_failure());
}

TEST_CASE("Lifecycle phase changes preserve a recorded quit request")
{
    ScopedLifecycleReset reset;

    fallout::lifecycle_set_phase(fallout::LIFECYCLE_PHASE_RUNNING);
    fallout::lifecycle_request_quit(fallout::LIFECYCLE_REASON_WINDOW_CLOSE);
    fallout::lifecycle_set_phase(fallout::LIFECYCLE_PHASE_SHUTDOWN);

    fallout::LifecycleState state = fallout::lifecycle_get_state();

    REQUIRE(state.phase == fallout::LIFECYCLE_PHASE_SHUTDOWN);
    REQUIRE(state.request == fallout::LIFECYCLE_REQUEST_QUIT);
    REQUIRE(state.reason == fallout::LIFECYCLE_REASON_WINDOW_CLOSE);
    REQUIRE(state.code == 0);
    REQUIRE(fallout::lifecycle_is_quit_requested());
}

} // namespace
