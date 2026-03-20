#ifndef FALLOUT_PLIB_GNW_SVGA_TYPES_H_
#define FALLOUT_PLIB_GNW_SVGA_TYPES_H_

namespace fallout {

// NOTE: These typedefs always appear in this order in implementation files
// with extended debug info. `mouse.c` lacks them, implying it does not include
// `svga.h`.

typedef void(UpdatePaletteFunc)();
typedef void(ResetModeFunc)();
typedef int(SetModeFunc)();
typedef void(ScreenTransBlitFunc)(unsigned char* srcBuf, unsigned int srcW, unsigned int srcH, unsigned int subX, unsigned int subY, unsigned int subW, unsigned int subH, unsigned int dstX, unsigned int dstY, unsigned char trans);
typedef void(ScreenBlitFunc)(unsigned char* srcBuf, unsigned int srcW, unsigned int srcH, unsigned int subX, unsigned int subY, unsigned int subW, unsigned int subH, unsigned int dstX, unsigned int dstY);

typedef struct VideoOptions {
    int width;
    int height;
    bool fullscreen;
    int scale;
} VideoOptions;

} // namespace fallout

#endif /* FALLOUT_PLIB_GNW_SVGA_TYPES_H_ */
