#include "plib/gnw/winmain.h"

#include <stdlib.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "game/main.h"
#include "plib/gnw/gnw.h"
#include "plib/gnw/svga.h"

namespace fallout {

// 0x53A290
bool GNW95_isActive = false;

// 0x6B0760
char GNW95_title[256];

int main(int argc, char* argv[])
{
    int rc;

    SDL_HideCursor();

    GNW95_isActive = true;
    rc = gnw_main(argc, argv);

    return rc;
}

} // namespace fallout

int main(int argc, char* argv[])
{
    return fallout::main(argc, argv);
}
