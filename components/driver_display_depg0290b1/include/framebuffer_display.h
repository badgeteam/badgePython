#pragma once

#include "depg0290b1.h"

#define DEPG0290B1_WIDTH  296
#define DEPG0290B1_HEIGHT 128

#define FB_SIZE (DEPG0290B1_WIDTH * DEPG0290B1_HEIGHT)
#define FB_WIDTH DEPG0290B1_WIDTH
#define FB_HEIGHT DEPG0290B1_HEIGHT
#define FB_TYPE_8BPP
#define FB_ALPHA_ENABLED
#define FB_FLUSH_GS(buffer,eink_flags) driver_depg0290b1_display_greyscale(buffer,eink_flags,16);
#define FB_FLUSH(buffer,eink_flags,x0,y0,x1,y1) driver_depg0290b1_display_part(buffer,eink_flags,x0,x1);
#define COLOR_FILL_DEFAULT 0xFFFFFF
#define COLOR_TEXT_DEFAULT 0x000000
