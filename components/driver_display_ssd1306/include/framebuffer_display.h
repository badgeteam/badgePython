#ifndef __FRAMEBUFFER_DISPLAY_H__
#define __FRAMEBUFFER_DISPLAU_H__

#include "driver_ssd1306.h"

//This header file containes the defines used by the framebuffer driver.

#define FB_SIZE SSD1306_BUFFER_SIZE
#define FB_WIDTH SSD1306_WIDTH
#define FB_HEIGHT SSD1306_HEIGHT
#define FB_TYPE_1BPP
#define FB_1BPP_VERT2
#define FB_FLUSH(buffer,eink_flags,x0,y0,x1,y1) driver_ssd1306_write_part(buffer,x0,y0,x1,y1)
#define COLOR_FILL_DEFAULT 0x000000
#define COLOR_TEXT_DEFAULT 0xFFFFFF

#endif
