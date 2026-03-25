#include "plib/gnw/winmain.h"

#include <stdlib.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "game/amutex.h"
#include "game/main.h"
#include "plib/gnw/gnw.h"
#include "plib/gnw/svga.h"

namespace fallout {

// 0x53A290
bool GNW95_isActive = false;

// 0x6B0760
char GNW95_title[256];

static int main_immediate(int argc, char* argv[])
{
    if (!autorun_mutex_create()) {
        return -1;
    }

    int rc = -1;

    if (main_init_system(argc, argv) == 0) {
        rc = main_run_system();
        main_exit_system();
    }

    autorun_mutex_destroy();

    return rc;
}

} // namespace fallout

int main(int argc, char* argv[])
{
#ifndef __EMSCRIPTEN__
    SDL_HideCursor();
#endif
    fallout::GNW95_isActive = true;

    int rc = fallout::main_immediate(argc, argv);

    SDL_Quit();

    return rc == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
