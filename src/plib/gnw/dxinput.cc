#include "plib/gnw/dxinput.h"

#include "plib/gnw/svga.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace fallout {

static bool dxinput_mouse_init();
static void dxinput_mouse_exit();
static bool dxinput_keyboard_init();
static void dxinput_keyboard_exit();

static int gMouseWheelDeltaX = 0;
static int gMouseWheelDeltaY = 0;

#ifdef __EMSCRIPTEN__
// Event-driven mouse state for Emscripten — updated by JS pointer event
// workaround (via exported fallout_push_mouse_*), consumed by dxinput_get_mouse_state.
float gEmMouseX = 0.0f;
float gEmMouseY = 0.0f;
SDL_MouseButtonFlags gEmMouseButtons = 0;

static int emscriptenMouseMaxX()
{
    int width = screenGetWidth();
    return width > 0 ? width - 1 : 0;
}

static int emscriptenMouseMaxY()
{
    int height = screenGetHeight();
    return height > 0 ? height - 1 : 0;
}

static float clampEmscriptenMouseCoordinate(float value, int maxValue)
{
    if (value < 0.0f) {
        return 0.0f;
    }

    if (value > maxValue) {
        return static_cast<float>(maxValue);
    }

    return value;
}

static void clampEmscriptenMousePosition()
{
    gEmMouseX = clampEmscriptenMouseCoordinate(gEmMouseX, emscriptenMouseMaxX());
    gEmMouseY = clampEmscriptenMouseCoordinate(gEmMouseY, emscriptenMouseMaxY());
}
#endif

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

#ifdef __EMSCRIPTEN__
    // Read from event-driven tracking updated by JS pointer event workaround.
    static float prevX = -1.0f;
    static float prevY = -1.0f;
    x = gEmMouseX;
    y = gEmMouseY;
    SDL_MouseButtonFlags buttons = gEmMouseButtons;
    if (prevX < 0.0f) {
        prevX = x;
        prevY = y;
        mouseState->x = 0;
        mouseState->y = 0;
    } else {
        mouseState->x = static_cast<int>(x - prevX);
        mouseState->y = static_cast<int>(y - prevY);
        prevX = x;
        prevY = y;
    }
#else
    SDL_MouseButtonFlags buttons = SDL_GetRelativeMouseState(&x, &y);
    mouseState->x = static_cast<int>(x);
    mouseState->y = static_cast<int>(y);
#endif

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
#ifdef __EMSCRIPTEN__
    // Under Emscripten + Asyncify, SDL_GetMouseState / SDL_GetRelativeMouseState
    // may return stale data because the Asyncify yield/resume cycle can desync
    // SDL's internal mouse state.  Track position and buttons directly from
    // the events that GNW95_process_message already dispatches to us.
    switch (event->type) {
    case SDL_EVENT_MOUSE_MOTION:
        gEmMouseX = event->motion.x;
        gEmMouseY = event->motion.y;
        clampEmscriptenMousePosition();
        break;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        gEmMouseX = event->button.x;
        gEmMouseY = event->button.y;
        clampEmscriptenMousePosition();
        if (event->button.button == SDL_BUTTON_LEFT)
            gEmMouseButtons |= SDL_BUTTON_MASK(SDL_BUTTON_LEFT);
        if (event->button.button == SDL_BUTTON_RIGHT)
            gEmMouseButtons |= SDL_BUTTON_MASK(SDL_BUTTON_RIGHT);
        break;
    case SDL_EVENT_MOUSE_BUTTON_UP:
        gEmMouseX = event->button.x;
        gEmMouseY = event->button.y;
        clampEmscriptenMousePosition();
        if (event->button.button == SDL_BUTTON_LEFT)
            gEmMouseButtons &= ~SDL_BUTTON_MASK(SDL_BUTTON_LEFT);
        if (event->button.button == SDL_BUTTON_RIGHT)
            gEmMouseButtons &= ~SDL_BUTTON_MASK(SDL_BUTTON_RIGHT);
        break;
    case SDL_EVENT_MOUSE_WHEEL:
        gMouseWheelDeltaX += event->wheel.x;
        gMouseWheelDeltaY += event->wheel.y;
        break;
    }
#else
    // On native platforms, mouse state is read via SDL_GetRelativeMouseState
    // in dxinput_get_mouse_state.  Only wheel events need manual accumulation.
    if (event->type == SDL_EVENT_MOUSE_WHEEL) {
        gMouseWheelDeltaX += event->wheel.x;
        gMouseWheelDeltaY += event->wheel.y;
    }
#endif
}

} // namespace fallout

#ifdef __EMSCRIPTEN__
// Exported so JavaScript can push pointer events directly into the engine,
// bypassing SDL3's experimental Emscripten pointer event handler that only
// processes pen input (pointerType == "pen"), not mouse input.
extern "C" {

EMSCRIPTEN_KEEPALIVE
void fallout_push_mouse_move(float x, float y) {
    fallout::gEmMouseX = x;
    fallout::gEmMouseY = y;
    fallout::clampEmscriptenMousePosition();
}

EMSCRIPTEN_KEEPALIVE
void fallout_push_mouse_move_relative(float dx, float dy) {
    fallout::gEmMouseX += dx;
    fallout::gEmMouseY += dy;
    fallout::clampEmscriptenMousePosition();
}

EMSCRIPTEN_KEEPALIVE
int fallout_get_mouse_bounds_width() {
    return fallout::screenGetWidth();
}

EMSCRIPTEN_KEEPALIVE
int fallout_get_mouse_bounds_height() {
    return fallout::screenGetHeight();
}

EMSCRIPTEN_KEEPALIVE
float fallout_get_mouse_x() {
    return fallout::gEmMouseX;
}

EMSCRIPTEN_KEEPALIVE
float fallout_get_mouse_y() {
    return fallout::gEmMouseY;
}

EMSCRIPTEN_KEEPALIVE
int fallout_get_mouse_buttons() {
    return fallout::gEmMouseButtons;
}

EMSCRIPTEN_KEEPALIVE
void fallout_push_mouse_button(float x, float y, int button, int down) {
    fallout_push_mouse_move(x, y);

    SDL_MouseButtonFlags mask = 0;
    if (button == 0) mask = SDL_BUTTON_MASK(SDL_BUTTON_LEFT);
    else if (button == 2) mask = SDL_BUTTON_MASK(SDL_BUTTON_RIGHT);
    if (down) {
        fallout::gEmMouseButtons |= mask;
    } else {
        fallout::gEmMouseButtons &= ~mask;
    }
}

} // extern "C"
#endif
