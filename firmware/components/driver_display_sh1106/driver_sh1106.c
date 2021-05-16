#include <sdkconfig.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <driver/gpio.h>

#include <driver_i2c.h>
#include "include/driver_sh1106.h"

static const char *TAG = "sh1106";

static inline esp_err_t i2c_command(uint8_t value)
{
	esp_err_t res = driver_i2c_write_reg(CONFIG_I2C_ADDR_SH1106, 0x00, value);
	if (res != ESP_OK) {
		ESP_LOGE(TAG, "i2c write command(0x%02x): error %d", value, res);
		return res;
	}
	return res;
}

static inline esp_err_t i2c_data(const uint8_t* buffer, uint16_t len)
{
	esp_err_t res = driver_i2c_write_buffer_reg(CONFIG_I2C_ADDR_SH1106, 0x40, buffer, len);
	if (res != ESP_OK) {
		ESP_LOGE(TAG, "i2c write data: error %d", res);
		return res;
	}
	return res;
}

esp_err_t driver_sh1106_reset(void)
{
#if CONFIG_PIN_NUM_SH1106_RESET >= 0
	gpio_set_level(CONFIG_PIN_NUM_SH1106_RESET, false);
	vTaskDelay(10);
	gpio_set_level(CONFIG_PIN_NUM_SH1106_RESET, true);
	vTaskDelay(50);
#endif
	return ESP_OK;
}

esp_err_t driver_sh1106_init(void)
{
	static bool driver_sh1106_init_done = false;
	if (driver_sh1106_init_done) return ESP_OK;
	ESP_LOGD(TAG, "init called");
	esp_err_t res = driver_i2c_init();
	if (res != ESP_OK) return res;
#if CONFIG_PIN_NUM_SH1106_RESET >= 0
	gpio_set_direction(CONFIG_PIN_NUM_SH1106_RESET, GPIO_MODE_OUTPUT);
	driver_sh1106_reset();
#endif
	
	i2c_command(0xae); // sh1106_DISPLAYOFF
	if (res != ESP_OK) return res;
	i2c_command(0xd5); // sh1106_SETDISPLAYCLOCKDIV
	if (res != ESP_OK) return res;
	i2c_command(0x80); // Suggested value 0x80
	if (res != ESP_OK) return res;
	i2c_command(0xa8); // sh1106_SETMULTIPLEX
	if (res != ESP_OK) return res;
	i2c_command(0x3f); // 1/64
	if (res != ESP_OK) return res;
	i2c_command(0xd3); // sh1106_SETDISPLAYOFFSET
	if (res != ESP_OK) return res;
	i2c_command(0x00); // 0 no offset
	if (res != ESP_OK) return res;
	i2c_command(0x40); // sh1106_SETSTARTLINE line #0
	if (res != ESP_OK) return res;
	i2c_command(0x20); // sh1106_MEMORYMODE
	if (res != ESP_OK) return res;
	i2c_command(0x01); // 0x0 act like ks0108 / 0x01 vertical addressing mode
	if (res != ESP_OK) return res;
	i2c_command(0xa1); // sh1106_SEGREMAP | 1
	if (res != ESP_OK) return res;
	i2c_command(0xc8); // sh1106_COMSCANDEC
	if (res != ESP_OK) return res;
	i2c_command(0xda); // sh1106_SETCOMPINS
	if (res != ESP_OK) return res;
	i2c_command(0x12);
	if (res != ESP_OK) return res;
	i2c_command(0x81); // sh1106_SETCONTRAST
	if (res != ESP_OK) return res;
	i2c_command(0xcf);
	if (res != ESP_OK) return res;
	i2c_command(0xd9); // sh1106_SETPRECHARGE
	if (res != ESP_OK) return res;
	i2c_command(0xf1);
	if (res != ESP_OK) return res;
	i2c_command(0xdb); // sh1106_SETVCOMDETECT
	if (res != ESP_OK) return res;
	i2c_command(0x30);
	if (res != ESP_OK) return res;
	i2c_command(0x8d); // sh1106_CHARGEPUMP
	if (res != ESP_OK) return res;
	i2c_command(0x14); // Charge pump on
	if (res != ESP_OK) return res;
	i2c_command(0x2e); // sh1106_DEACTIVATE_SCROLL
	if (res != ESP_OK) return res;
	i2c_command(0xa4); // sh1106_DISPLAYALLON_RESUME
	if (res != ESP_OK) return res;
	i2c_command(0xa6); // sh1106_NORMALDISPLAY
	if (res != ESP_OK) return res;

	i2c_command(0xaf); // sh1106_DISPLAYON
	if (res != ESP_OK) return res;
	
	uint8_t buffer[1024] = {0};
	res = driver_sh1106_write(buffer); //Clear screen
	if (res != ESP_OK) return res;
	
	driver_sh1106_init_done = true;
	ESP_LOGD(TAG, "init done");
	return ESP_OK;
}

#define WIDTH_OFFSET 2

esp_err_t driver_sh1106_write_part(const uint8_t *buffer, int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
	
	uint16_t addr0 = y0/8;
	uint16_t addr1 = y1/8;
	uint16_t length = x1-x0+1;
	uint8_t startx = WIDTH_OFFSET + x0;
	
	esp_err_t res;
	
	for(int i = addr0; i <= addr1; i++) {
		
		res = i2c_command(startx & 0x0F); //Lower column address
		if (res != ESP_OK) return res;
		res = i2c_command(0x10 | (startx >> 4)); //Higher columnd address
		if (res != ESP_OK) return res;

		res = i2c_command(0xB0 | i); //Higher columnd address
		if (res != ESP_OK) return res;

		res = i2c_data(buffer+x0+i*SH1106_WIDTH, length);
		if ( res != ESP_OK) return res;
	}


	return res;
}

esp_err_t driver_sh1106_write(const uint8_t *buffer)
{
	esp_err_t res;

	for(int i = 0; i < SH1106_HEIGHT/8; i++) {
		res = i2c_command(WIDTH_OFFSET & 0x0F); //Lower column address
		if (res != ESP_OK) return res;
		res = i2c_command(0x10 | (WIDTH_OFFSET >> 4)); //Higher columnd address
		if (res != ESP_OK) return res;

		res = i2c_command(0xB0 | i); //Higher columnd address
		if (res != ESP_OK) return res;

		res = i2c_data(&buffer[i*SH1106_WIDTH], SH1106_WIDTH);
		if ( res != ESP_OK) return res;
	}
	
	

	ESP_LOGD(TAG, "i2c write data ok");
	return res;
}
