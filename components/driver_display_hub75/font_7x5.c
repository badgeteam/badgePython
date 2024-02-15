//
// Created by Tom on 12/07/2019.
//

#include "esp_log.h"
#include "include/font_7x5.h"
#include "include/compositor.h"

#ifdef CONFIG_DRIVER_HUB75_ENABLE

void renderCharCol(uint8_t ch, Color color, int x, int y) {
    for(int py = y; py<y+7; py++) {
        if(py >= 0 && py < CONFIG_HUB75_HEIGHT && x >= 0 && x < CONFIG_HUB75_WIDTH) {
            if((ch & (1<<(py-y))) != 0) compositor_setPixel(x, py, color);
        }
    }
}

void renderChar_7x5(uint8_t charId, Color color, int *x, int y, int endX, int *offset, float micro_frame) {
    uint8_t orig_alpha = color.RGB[0];

    for(int i = 0; i<5; i++) {
        if(*offset == 0) {
            uint8_t cs = font_7x5[charId*5+i];
            if(endX > 0 && *x >= endX) return;

            if(*x >= 1 && micro_frame < 0.5f) {
                // Left neighbour
                color.RGB[0] = (uint8_t)((0.5f - micro_frame) * 255); // max 255/2
                renderCharCol(cs, color, (*x)-1, y);
            }

            // Current column
            color.RGB[0] = orig_alpha;
            renderCharCol(cs, color, *x, y);

            if(*x < (endX-1) && micro_frame > 0.5f) {
                // Right neighbour
                color.RGB[0] = (uint8_t)((micro_frame - 0.5f) * 255); // max 255/2
                renderCharCol(cs, color, (*x)+1, y);
            }

            (*x)++;
        } else {
            (*offset)--;
        }
    }
}

int getCharWidth_7x5(uint8_t charId) {
    return 5;
}

#endif
