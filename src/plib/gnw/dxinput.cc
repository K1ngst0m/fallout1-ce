#include "plib/gnw/dxinput.h"

#include "plib/gnw/svga.h"

namespace fallout {

static bool dxinput_mouse_init();
static void dxinput_mouse_exit();
static bool dxinput_keyboard_init();
static void dxinput_keyboard_exit();

static int gMouseWheelDeltaX = 0;
static int gMouseWheelDeltaY = 0;

// 0x4E0400
bool dxinput_init()
{
    if (!SDL_InitSubSystem(SDL_INIT_EVENTS)) {
        return false;
    }

    if (!dxinput_mouse_init()) {
        goto err;
    }

    if (!dxinput_keyboard_init()) {
        goto err;
    }

    return true;

err:

    dxinput_keyboard_exit();
    dxinput_mouse_exit();

    return false;
}

// 0x4E0478
void dxinput_exit()
{
    dxinput_keyboard_exit();
    dxinput_mouse_exit();
    SDL_QuitSubSystem(SDL_INIT_EVENTS);
}

// 0x4E04E8
bool dxinput_acquire_mouse(InputRuntimeCaptureRequestSource source)
{
    input_runtime_request_mouse_capture(source);
    return true;
}

// 0x4E0514
bool dxinput_unacquire_mouse()
{
    input_runtime_release_mouse_capture();
    return true;
}

// 0x4E053C
bool dxinput_get_mouse_state(MouseData* mouseState)
{
    float x;
    float y;
    SDL_MouseButtonFlags buttons = SDL_GetRelativeMouseState(&x, &y);
    mouseState->x = static_cast<int>(x);
    mouseState->y = static_cast<int>(y);
    mouseState->buttons[0] = (buttons & SDL_BUTTON_MASK(SDL_BUTTON_LEFT)) != 0;
    mouseState->buttons[1] = (buttons & SDL_BUTTON_MASK(SDL_BUTTON_RIGHT)) != 0;
    mouseState->wheelX = gMouseWheelDeltaX;
    mouseState->wheelY = gMouseWheelDeltaY;

    gMouseWheelDeltaX = 0;
    gMouseWheelDeltaY = 0;

    return true;
}

// 0x4E05A8
bool dxinput_acquire_keyboard()
{
    return true;
}

// 0x4E05D4
bool dxinput_unacquire_keyboard()
{
    return true;
}

// 0x4E05FC
bool dxinput_flush_keyboard_buffer()
{
    SDL_FlushEvents(SDL_EVENT_KEY_DOWN, SDL_EVENT_TEXT_INPUT);
    return true;
}

// 0x4E0650
bool dxinput_read_keyboard_buffer(KeyboardData* keyboardData)
{
    (void)keyboardData;
    return true;
}

// 0x4E070C
bool dxinput_mouse_init()
{
#ifdef __EMSCRIPTEN__
    // Relative mouse mode requires pointer lock which the browser may not
    // grant automatically.  Skip for now — mouse input handling is M003 scope.
    return true;
#else
    if (gSdlWindow == nullptr) {
        return false;
    }

    input_runtime_reset();
    return true;
#endif
}

// 0x4E078C
void dxinput_mouse_exit()
{
    input_runtime_set_mouse_capture_applied(false);
}

// 0x4E07B8
bool dxinput_keyboard_init()
{
    return true;
}

// 0x4E0874
void dxinput_keyboard_exit()
{
}

void handleMouseEvent(SDL_Event* event)
{
    // Mouse movement and buttons are accumulated in SDL itself and will be
    // processed later in `mouseDeviceGetData` via `SDL_GetRelativeMouseState`.

    if (event->type == SDL_EVENT_MOUSE_WHEEL) {
        gMouseWheelDeltaX += event->wheel.x;
        gMouseWheelDeltaY += event->wheel.y;
    }
}

} // namespace fallout
