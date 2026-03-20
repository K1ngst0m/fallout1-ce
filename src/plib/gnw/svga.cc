#include "plib/gnw/svga.h"

#include "plib/gnw/gnw.h"
#include "plib/gnw/grbuf.h"
#include "plib/gnw/mouse.h"
#include "plib/gnw/winmain.h"

namespace fallout {

static bool createRenderer(int width, int height);
static void destroyRenderer();

// screen rect
Rect scr_size;

// 0x6ACA18
ScreenBlitFunc* scr_blit = GNW95_ShowRect;

SDL_Window* gSdlWindow = NULL;
SDL_Surface* gSdlSurface = NULL;
SDL_Renderer* gSdlRenderer = NULL;
SDL_Texture* gSdlTexture = NULL;
SDL_Surface* gSdlTextureSurface = NULL;

FpsLimiter sharedFpsLimiter;

// 0x4CB310
void GNW95_SetPaletteEntries(unsigned char* palette, int start, int count)
{
    SDL_Palette* surfacePalette = gSdlSurface != NULL ? SDL_GetSurfacePalette(gSdlSurface) : NULL;
    if (surfacePalette != NULL) {
        SDL_Color colors[256];

        if (count != 0) {
            for (int index = 0; index < count; index++) {
                colors[index].r = palette[index * 3] << 2;
                colors[index].g = palette[index * 3 + 1] << 2;
                colors[index].b = palette[index * 3 + 2] << 2;
                colors[index].a = 255;
            }
        }

        SDL_SetPaletteColors(surfacePalette, colors, start, count);
        SDL_BlitSurface(gSdlSurface, NULL, gSdlTextureSurface, NULL);
    }
}

// 0x4CB568
void GNW95_SetPalette(unsigned char* palette)
{
    SDL_Palette* surfacePalette = gSdlSurface != NULL ? SDL_GetSurfacePalette(gSdlSurface) : NULL;
    if (surfacePalette != NULL) {
        SDL_Color colors[256];

        for (int index = 0; index < 256; index++) {
            colors[index].r = palette[index * 3] << 2;
            colors[index].g = palette[index * 3 + 1] << 2;
            colors[index].b = palette[index * 3 + 2] << 2;
            colors[index].a = 255;
        }

        SDL_SetPaletteColors(surfacePalette, colors, 0, 256);
        SDL_BlitSurface(gSdlSurface, NULL, gSdlTextureSurface, NULL);
    }
}

// 0x4CB850
void GNW95_ShowRect(unsigned char* src, unsigned int srcPitch, unsigned int a3, unsigned int srcX, unsigned int srcY, unsigned int srcWidth, unsigned int srcHeight, unsigned int destX, unsigned int destY)
{
    buf_to_buf(src + srcPitch * srcY + srcX, srcWidth, srcHeight, srcPitch, (unsigned char*)gSdlSurface->pixels + gSdlSurface->pitch * destY + destX, gSdlSurface->pitch);

    SDL_Rect srcRect;
    srcRect.x = destX;
    srcRect.y = destY;
    srcRect.w = srcWidth;
    srcRect.h = srcHeight;

    SDL_Rect destRect;
    destRect.x = destX;
    destRect.y = destY;
    SDL_BlitSurface(gSdlSurface, &srcRect, gSdlTextureSurface, &destRect);
}

bool svga_init(VideoOptions* video_options)
{
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");

    if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
        return false;
    }

    Uint32 windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_HIGH_PIXEL_DENSITY;

    if (video_options->fullscreen) {
        windowFlags |= SDL_WINDOW_FULLSCREEN;
    }

    gSdlWindow = SDL_CreateWindow(GNW95_title,
        video_options->width * video_options->scale,
        video_options->height * video_options->scale,
        windowFlags);
    if (gSdlWindow == NULL) {
        return false;
    }

    if (!createRenderer(video_options->width, video_options->height)) {
        destroyRenderer();

        SDL_DestroyWindow(gSdlWindow);
        gSdlWindow = NULL;

        return false;
    }

    gSdlSurface = SDL_CreateSurface(video_options->width,
        video_options->height,
        SDL_PIXELFORMAT_INDEX8);
    if (gSdlSurface == NULL) {
        destroyRenderer();

        SDL_DestroyWindow(gSdlWindow);
        gSdlWindow = NULL;

        return false;
    }

    SDL_Palette* palette = SDL_CreatePalette(256);
    if (palette == NULL || !SDL_SetSurfacePalette(gSdlSurface, palette)) {
        if (palette != NULL) {
            SDL_DestroyPalette(palette);
        }

        SDL_DestroySurface(gSdlSurface);
        gSdlSurface = NULL;
        destroyRenderer();

        SDL_DestroyWindow(gSdlWindow);
        gSdlWindow = NULL;

        return false;
    }

    SDL_DestroyPalette(palette);

    SDL_Color colors[256];
    for (int index = 0; index < 256; index++) {
        colors[index].r = index;
        colors[index].g = index;
        colors[index].b = index;
        colors[index].a = 255;
    }

    SDL_SetPaletteColors(SDL_GetSurfacePalette(gSdlSurface), colors, 0, 256);

    scr_size.ulx = 0;
    scr_size.uly = 0;
    scr_size.lrx = video_options->width - 1;
    scr_size.lry = video_options->height - 1;

    mouse_blit_trans = NULL;
    scr_blit = GNW95_ShowRect;
    mouse_blit = GNW95_ShowRect;

    return true;
}

void svga_exit()
{
    destroyRenderer();

    if (gSdlWindow != NULL) {
        SDL_DestroyWindow(gSdlWindow);
        gSdlWindow = NULL;
    }

    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

int screenGetWidth()
{
    // TODO: Make it on par with _xres;
    return rectGetWidth(&scr_size);
}

int screenGetHeight()
{
    // TODO: Make it on par with _yres.
    return rectGetHeight(&scr_size);
}

static bool createRenderer(int width, int height)
{
    gSdlRenderer = SDL_CreateRenderer(gSdlWindow, NULL);
    if (gSdlRenderer == NULL) {
        return false;
    }

    if (!SDL_SetRenderLogicalPresentation(gSdlRenderer, width, height, SDL_LOGICAL_PRESENTATION_STRETCH)) {
        return false;
    }

    gSdlTexture = SDL_CreateTexture(gSdlRenderer, SDL_PIXELFORMAT_XRGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
    if (gSdlTexture == NULL) {
        return false;
    }

    SDL_PropertiesID textureProperties = SDL_GetTextureProperties(gSdlTexture);
    if (textureProperties == 0) {
        return false;
    }

    SDL_PixelFormat format = static_cast<SDL_PixelFormat>(SDL_GetNumberProperty(textureProperties, SDL_PROP_TEXTURE_FORMAT_NUMBER, SDL_PIXELFORMAT_UNKNOWN));
    if (format == SDL_PIXELFORMAT_UNKNOWN) {
        return false;
    }

    gSdlTextureSurface = SDL_CreateSurface(width, height, format);
    if (gSdlTextureSurface == NULL) {
        return false;
    }

    return true;
}

static void destroyRenderer()
{
    if (gSdlTextureSurface != NULL) {
        SDL_DestroySurface(gSdlTextureSurface);
        gSdlTextureSurface = NULL;
    }

    if (gSdlTexture != NULL) {
        SDL_DestroyTexture(gSdlTexture);
        gSdlTexture = NULL;
    }

    if (gSdlRenderer != NULL) {
        SDL_DestroyRenderer(gSdlRenderer);
        gSdlRenderer = NULL;
    }
}

void handleWindowSizeChanged()
{
    destroyRenderer();
    createRenderer(screenGetWidth(), screenGetHeight());
}

void renderPresent()
{
    SDL_UpdateTexture(gSdlTexture, NULL, gSdlTextureSurface->pixels, gSdlTextureSurface->pitch);
    SDL_RenderClear(gSdlRenderer);
    SDL_RenderTexture(gSdlRenderer, gSdlTexture, NULL, NULL);
    SDL_RenderPresent(gSdlRenderer);
}

} // namespace fallout
