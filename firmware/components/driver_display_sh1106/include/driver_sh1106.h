#ifndef DRIVER_SH1106_H
#define DRIVER_SH1106_H

#include <stdbool.h>
#include <stdint.h>
#include <esp_err.h>

#define SH1106_WIDTH  128
#define SH1106_HEIGHT 64

#define SH1106_BUFFER_SIZE (SH1106_WIDTH * SH1106_HEIGHT) / 8

__BEGIN_DECLS

extern esp_err_t driver_sh1106_init(void);
extern esp_err_t driver_sh1106_write_part(const uint8_t *buffer, int16_t x0, int16_t y0, int16_t x1, int16_t y1);
extern esp_err_t driver_sh1106_write(const uint8_t *buffer);

__END_DECLS

#endif // DRIVER_SSD1306_H
