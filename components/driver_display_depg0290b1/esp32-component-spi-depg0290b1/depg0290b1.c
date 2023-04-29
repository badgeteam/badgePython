#include <sdkconfig.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <esp_heap_caps.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <nvs.h>
#include <rom/ets_sys.h>
#include <rom/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <soc/gpio_reg.h>
#include <soc/gpio_sig_map.h>
#include <soc/gpio_struct.h>
#include <soc/spi_reg.h>
#include <esp_err.h>
#include <driver/gpio.h>
#include <driver/spi_master.h>

#include "depg0290b1.h"

static const char *TAG = "DEPG0290B1";

// this number should be a multiply of 4
// to transfer complete dwords in one go
#define SPI_TRANSFER_SIZE 128

#define PIN_NUM_EPD_CS    CONFIG_PIN_NUM_DEPG0290B1_CS
#define PIN_NUM_EPD_DC    CONFIG_PIN_NUM_DEPG0290B1_DCX
#define PIN_NUM_EPD_BUSY  CONFIG_PIN_NUM_DEPG0290B1_BUSY
#define PIN_NUM_EPD_RESET CONFIG_PIN_NUM_DEPG0290B1_RESET

static spi_device_handle_t spi_bus = NULL;

// forward declarations
static void driver_spi_pre_transfer_callback(spi_transaction_t *t);

esp_err_t driver_depg0290b1_dev_reset(void) {
    esp_err_t res = gpio_set_level(PIN_NUM_EPD_RESET, 0);
    if (res != ESP_OK) return res;
    ets_delay_us(200000);
    res = gpio_set_level(PIN_NUM_EPD_RESET, 1);
    if (res != ESP_OK) return res;
    ets_delay_us(200000);
    return ESP_OK;
}

bool driver_depg0290b1_dev_is_busy(void)
{
    return gpio_get_level(PIN_NUM_EPD_BUSY);
}

// semaphore to trigger on gde-busy signal
xSemaphoreHandle driver_depg0290b1_dev_intr_trigger = NULL;

void driver_depg0290b1_dev_busy_wait(void) {
    uint16_t timeout = 50;
    while (driver_depg0290b1_dev_is_busy() && (timeout > 0))
    {
        xSemaphoreTake(driver_depg0290b1_dev_intr_trigger, 100 / portTICK_PERIOD_MS);
        //printf("timeout %u\n", timeout);
        timeout--;
    }

    if (timeout < 1) {
        ESP_LOGE(TAG, "Timeout while waiting for EPD busy flag to clear.");
    }
}

void driver_depg0290b1_dev_intr_handler(void *arg) {
    xSemaphoreGiveFromISR(driver_depg0290b1_dev_intr_trigger, NULL);
}

static void driver_spi_pre_transfer_callback(spi_transaction_t *t) {
    uint8_t dc_level = *((uint8_t *) t->user);
    gpio_set_level(PIN_NUM_EPD_DC, (int) dc_level);
}

void driver_spi_send(const uint8_t *data, int len, const uint8_t dc_level) {
    if (len == 0) return;

    spi_transaction_t t = {
        .length = len * 8,  // transaction length is in bits
        .tx_buffer = data,
        .user = (void *) &dc_level,
    };
    esp_err_t ret = spi_device_transmit(spi_bus, &t);
    assert(ret == ESP_OK);
}

void driver_depg0290b1_dev_write_command(uint8_t command) {
    driver_depg0290b1_dev_busy_wait();
    driver_spi_send(&command, 1, 0);
}

void driver_depg0290b1_dev_write_byte(uint8_t data) {
    driver_depg0290b1_dev_busy_wait();
    driver_spi_send(&data, 1, 1);
}

void driver_depg0290b1_dev_write_command_stream(uint8_t command, const uint8_t *data, unsigned int datalen) {
    ESP_LOGI(TAG, "Sending SPI stream of %d bytes", datalen);
    driver_depg0290b1_dev_write_command(command);
    driver_spi_send(data, datalen, 1);
    ESP_LOGI(TAG, "Done");
}

/* This function is dedicated to copy memory allocated with MALLOC_CAP_32BIT
* Such a memory can only be accessed via 32-bit reads and writes,
* any other type of access will generate a fatal LoadStoreError exception.
*/
static void memcpy_u8_u32(uint8_t *dst, const uint32_t *src, size_t size)
{
    while (size-- > 0)
    {
        uint32_t data_src = *src++;
        *dst++ = data_src >> 24;
        *dst++ = data_src >> 16;
        *dst++ = data_src >> 8;
        *dst++ = data_src;
    }
}

/* Send uint32_t stream in chunks of SPI_TRANSFER_SIZE
* to use to the maximum the size of memory allocated in the SPI buffer
*/
void driver_depg0290b1_dev_write_command_stream_u32(uint8_t command, const uint32_t *data, unsigned int datalen) {
    assert(SPI_TRANSFER_SIZE % 4 == 0);

    uint8_t* data_tmpbuf = heap_caps_malloc(SPI_TRANSFER_SIZE, MALLOC_CAP_8BIT);
    ESP_LOGI(TAG, "Sending SPI stream of %d dwords...", datalen);
    if (data_tmpbuf == NULL){
        ESP_LOGE(TAG, "Failed to allocate memory!");
    }
    while (datalen >= SPI_TRANSFER_SIZE / 4){
        memcpy_u8_u32(data_tmpbuf, data, SPI_TRANSFER_SIZE / 4);
        driver_depg0290b1_dev_write_command(command);
        driver_spi_send(data_tmpbuf, SPI_TRANSFER_SIZE, 1);
        data += SPI_TRANSFER_SIZE / 4;
        datalen -= SPI_TRANSFER_SIZE / 4;
    }
    // send remaining data if < SPI_TRANSFER_SIZE / 4
    if (datalen > 0) {
        memcpy_u8_u32(data_tmpbuf, data, datalen);
        driver_depg0290b1_dev_write_command(command);
        driver_spi_send(data_tmpbuf, datalen*4, 1);
    }
    heap_caps_free(data_tmpbuf);
    ESP_LOGI(TAG, "Done");
}

esp_err_t driver_depg0290b1_dev_init() {
    driver_depg0290b1_dev_intr_trigger = xSemaphoreCreateBinary();
    if (driver_depg0290b1_dev_intr_trigger == NULL) return ESP_ERR_NO_MEM;

    esp_err_t res = gpio_isr_handler_add(PIN_NUM_EPD_BUSY, driver_depg0290b1_dev_intr_handler, NULL);
    if (res != ESP_OK) return res;

    gpio_config_t io_conf = {
        .intr_type    = GPIO_INTR_ANYEDGE,
        .mode         = GPIO_MODE_INPUT,
        .pin_bit_mask = 1LL << PIN_NUM_EPD_BUSY,
        .pull_down_en = 0,
        .pull_up_en   = 1,
    };
    res = gpio_config(&io_conf);
    if (res != ESP_OK) return res;

    res = gpio_set_direction(PIN_NUM_EPD_CS, GPIO_MODE_OUTPUT);
    if (res != ESP_OK) return res;

    res = gpio_set_direction(PIN_NUM_EPD_DC, GPIO_MODE_OUTPUT);
    if (res != ESP_OK) return res;

    res = gpio_set_direction(PIN_NUM_EPD_RESET, GPIO_MODE_OUTPUT);
    if (res != ESP_OK) return res;

    res = gpio_set_direction(PIN_NUM_EPD_BUSY, GPIO_MODE_INPUT);
    if (res != ESP_OK) return res;

    static const spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 20 * 1000 * 1000,
        .mode           = 0,  // SPI mode 0
        .spics_io_num   = PIN_NUM_EPD_CS,
        .queue_size     = 1,
        .flags          = (SPI_DEVICE_HALFDUPLEX | SPI_DEVICE_3WIRE),
        .pre_cb         = driver_spi_pre_transfer_callback,
    };

    return spi_bus_add_device(VSPI_HOST, &devcfg, &spi_bus);
}

// full, includes inverting
const struct driver_depg0290b1_lut_entry driver_depg0290b1_lut_full[] = {
	{ .length = 23, .voltages = 0x02, },
	{ .length =  4, .voltages = 0x01, },
	{ .length = 11, .voltages = 0x11, },
	{ .length =  4, .voltages = 0x12, },
	{ .length =  6, .voltages = 0x22, },
	{ .length =  5, .voltages = 0x66, },
	{ .length =  4, .voltages = 0x69, },
	{ .length =  5, .voltages = 0x59, },
	{ .length =  1, .voltages = 0x58, },
	{ .length = 14, .voltages = 0x99, },
	{ .length =  1, .voltages = 0x88, },
	{ .length = 0 }
};

// full, no inversion
const struct driver_depg0290b1_lut_entry driver_depg0290b1_lut_normal[] = {
	{ .length =  3, .voltages = 0x10, },
	{ .length =  5, .voltages = 0x18, },
	{ .length =  1, .voltages = 0x08, },
	{ .length =  8, .voltages = 0x18, },
	{ .length =  2, .voltages = 0x08, },
	{ .length = 0 }
};

// full, no inversion, needs 2 updates for full update
const struct driver_depg0290b1_lut_entry driver_depg0290b1_lut_faster[] = {
	{ .length =  1, .voltages = 0x10, },
	{ .length =  8, .voltages = 0x18, },
	{ .length =  1, .voltages = 0x08, },
	{ .length = 0 }
};

// full, no inversion, needs 4 updates for full update
const struct driver_depg0290b1_lut_entry driver_depg0290b1_lut_fastest[] = {
	{ .length =  1, .voltages = 0x10, },
	{ .length =  5, .voltages = 0x18, },
	{ .length =  1, .voltages = 0x08, },
	{ .length = 0 }
};

static uint8_t
driver_depg0290b1_lut_conv(uint8_t voltages, enum driver_depg0290b1_lut_flags flags)
{
	if (flags & LUT_FLAG_FIRST)
	{
		voltages |= voltages >> 4;
		voltages &= 15;
		if ((voltages & 3) == 3) // reserved
			voltages ^= 2; // set to '1': VSH (black)
		if ((voltages & 12) == 12) // reserved
			voltages ^= 4; // set to '2': VSL (white)
		voltages |= voltages << 4;
	}

	if (flags & LUT_FLAG_PARTIAL)
		voltages &= 0x3c; // only keep 0->1 and 1->0

	if (flags & LUT_FLAG_WHITE)
		voltages &= 0xcc; // only keep 0->1 and 1->1

	if (flags & LUT_FLAG_BLACK)
		voltages &= 0x33; // only keep 0->0 and 1->0

	return voltages;
}

int driver_depg0290b1_lut_generate(const struct driver_depg0290b1_lut_entry *list, enum driver_depg0290b1_lut_flags flags, uint8_t *lut) {
	ESP_LOGD(TAG, "flags = %d.", flags);

	memset(lut, 0, 70);

	int pos = 0;
	int spos = 0;
	while (list->length != 0)
	{
		int len = list->length;
		if (pos == 7)
		{
			ESP_LOGE(TAG, "lut overflow.");
			return -1; // full
		}
		uint8_t voltages = driver_depg0290b1_lut_conv(list->voltages, flags);

		lut[0*7 + pos] |= ((voltages >> 0) & 3) << ((3-spos)*2);
		lut[1*7 + pos] |= ((voltages >> 2) & 3) << ((3-spos)*2);
		lut[2*7 + pos] |= ((voltages >> 4) & 3) << ((3-spos)*2);
		lut[3*7 + pos] |= ((voltages >> 6) & 3) << ((3-spos)*2);
		lut[5*7 + pos*5 + spos] = len;
		lut[5*7 + pos*5 + spos] = len;

		spos++;
		if (spos == 2)
		{
			spos = 0;
			pos++;
		}

		list = &list[1];
	}

	return 70;
}

static const uint8_t xlat_curve[256] = {
    0x00,0x01,0x01,0x02,0x02,0x03,0x03,0x03,0x04,0x04,0x05,0x05,
    0x06,0x06,0x07,0x07,0x08,0x08,0x09,0x09,0x0a,0x0a,0x0a,0x0b,
    0x0b,0x0c,0x0c,0x0d,0x0d,0x0e,0x0e,0x0f,0x0f,0x10,0x10,0x11,
    0x11,0x12,0x12,0x13,0x13,0x14,0x15,0x15,0x16,0x16,0x17,0x17,
    0x18,0x18,0x19,0x19,0x1a,0x1a,0x1b,0x1b,0x1c,0x1d,0x1d,0x1e,
    0x1e,0x1f,0x1f,0x20,0x20,0x21,0x22,0x22,0x23,0x23,0x24,0x25,
    0x25,0x26,0x26,0x27,0x27,0x28,0x29,0x29,0x2a,0x2a,0x2b,0x2c,
    0x2c,0x2d,0x2e,0x2e,0x2f,0x2f,0x30,0x31,0x31,0x32,0x33,0x33,
    0x34,0x35,0x35,0x36,0x37,0x37,0x38,0x39,0x39,0x3a,0x3b,0x3b,
    0x3c,0x3d,0x3e,0x3e,0x3f,0x40,0x40,0x41,0x42,0x43,0x43,0x44,
    0x45,0x46,0x46,0x47,0x48,0x49,0x49,0x4a,0x4b,0x4c,0x4c,0x4d,
    0x4e,0x4f,0x50,0x50,0x51,0x52,0x53,0x54,0x55,0x55,0x56,0x57,
    0x58,0x59,0x5a,0x5b,0x5b,0x5c,0x5d,0x5e,0x5f,0x60,0x61,0x62,
    0x63,0x64,0x65,0x66,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,
    0x6e,0x6f,0x70,0x71,0x72,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,
    0x7b,0x7c,0x7d,0x7e,0x80,0x81,0x82,0x83,0x84,0x86,0x87,0x88,
    0x89,0x8a,0x8c,0x8d,0x8e,0x90,0x91,0x92,0x93,0x95,0x96,0x98,
    0x99,0x9a,0x9c,0x9d,0x9f,0xa0,0xa2,0xa3,0xa5,0xa6,0xa8,0xa9,
    0xab,0xac,0xae,0xb0,0xb1,0xb3,0xb5,0xb6,0xb8,0xba,0xbc,0xbe,
    0xbf,0xc1,0xc3,0xc5,0xc7,0xc9,0xcb,0xcd,0xcf,0xd1,0xd3,0xd6,
    0xd8,0xda,0xdc,0xdf,0xe1,0xe3,0xe6,0xe8,0xeb,0xed,0xf0,0xf3,
    0xf5,0xf8,0xfb,0xfe,
};

static uint32_t *driver_depg0290b1_tmpbuf = NULL;
static uint32_t *driver_depg0290b1_oldbuf = NULL;
static bool driver_depg0290b1_have_oldbuf = false;

static void memcpy_u32(uint32_t *dst, const uint32_t *src, size_t size)
{
	while (size-- > 0) {
		*dst++ = *src++;
	}
}

static void memset_u32(uint32_t *dst, uint32_t value, size_t size)
{
	while (size-- > 0) {
		*dst++ = value;
	}
}

static void driver_depg0290b1_create_bitplane(const uint8_t *img, uint32_t *buf, int bit, driver_depg0290b1_flags_t flags) {
	int x, y;
	int pos, dx, dy;
	if (flags & DISPLAY_FLAG_ROTATE_180) {
		pos = DISP_SIZE_Y-1;
		dx = DISP_SIZE_Y;
		dy = -DISP_SIZE_Y*DISP_SIZE_X - 1;
	} else {
		pos = (DISP_SIZE_X-1)*DISP_SIZE_Y;
		dx = -DISP_SIZE_Y;
		dy = DISP_SIZE_Y*DISP_SIZE_X + 1;
	}
	for (y = 0; y < DISP_SIZE_Y; y++) {
		for (x = 0; x < DISP_SIZE_X;) {
			int x_bits;
			uint32_t res = 0;
			for (x_bits=0; x_bits<32; x_bits++)
			{
				res <<= 1;
				if (flags & DISPLAY_FLAG_8BITPIXEL)
				{
					uint8_t pixel = img[pos];
					pos += dx;
					int j = xlat_curve[pixel];
					if ((j & bit) != 0)
						res++;
				}
				else
				{
					uint8_t pixel = img[pos >> 3] >> (pos & 7);
					pos += dx;
					if ((pixel & 1) != 0)
						res++;
				}
				x++;
			}
			*buf++ = res;
		}
		pos += dy;
	}
}

static void driver_depg0290b1_set_ram_area(uint8_t x_start, uint8_t x_end, uint16_t y_start, uint16_t y_end) {
	// set RAM X - address Start / End position
	driver_depg0290b1_dev_write_command_p2(0x44, x_start, x_end);
	// set RAM Y - address Start / End position
	driver_depg0290b1_dev_write_command_p4(0x45, y_start & 0xff, y_start >> 8, y_end & 0xff, y_end >> 8);
}

static void driver_depg0290b1_set_ram_pointer(uint8_t x_addr, uint16_t y_addr) {
	// set RAM X address counter
	driver_depg0290b1_dev_write_command_p1(0x4e, x_addr);
	// set RAM Y address counter
	driver_depg0290b1_dev_write_command_p2(0x4f, y_addr & 0xff, y_addr >> 8);
}

static void driver_depg0290b1_write_bitplane(const uint32_t *buf) {
	driver_depg0290b1_set_ram_area(0, DISP_SIZE_X_B - 1, 0, DISP_SIZE_Y - 1);
	driver_depg0290b1_set_ram_pointer(0, 0);
	driver_depg0290b1_dev_write_command_stream_u32(0x24, buf, DISP_SIZE_X_B * DISP_SIZE_Y/4);
}

void driver_depg0290b1_update(const uint32_t *buf, const struct driver_depg0290b1_update *upd_conf) {
	// generate lut data
	const struct driver_depg0290b1_lut_entry *lut_entries;

	if (upd_conf->lut == DRIVER_DEPG0290B1_LUT_CUSTOM) {
		lut_entries = upd_conf->lut_custom;
	} else if (upd_conf->lut >= 0 && upd_conf->lut <= DRIVER_DEPG0290B1_LUT_MAX) {
		const struct driver_depg0290b1_lut_entry *lut_lookup[DRIVER_DEPG0290B1_LUT_MAX + 1] = {
			driver_depg0290b1_lut_full,
			driver_depg0290b1_lut_normal,
			driver_depg0290b1_lut_faster,
			driver_depg0290b1_lut_fastest,
		};
		lut_entries = lut_lookup[upd_conf->lut];
	} else {
		lut_entries = driver_depg0290b1_lut_full;
	}

	uint8_t lut[DRIVER_DEPG0290B1_LUT_MAX_SIZE];
	int lut_len = driver_depg0290b1_lut_generate(lut_entries, upd_conf->lut_flags, lut);
	assert( lut_len >= 0 );

	driver_depg0290b1_dev_write_command_stream(0x32, lut, lut_len);

	if (buf == NULL)
		buf = driver_depg0290b1_tmpbuf;

	driver_depg0290b1_write_bitplane(buf);

	if (driver_depg0290b1_have_oldbuf) {
		driver_depg0290b1_dev_write_command_stream_u32(0x26, driver_depg0290b1_oldbuf, DISP_SIZE_X_B * DISP_SIZE_Y/4);
	}

	// write number of overscan lines
	driver_depg0290b1_dev_write_command_p1(0x3a, upd_conf->reg_0x3a);

	// write time to write every line
	driver_depg0290b1_dev_write_command_p1(0x3b, upd_conf->reg_0x3b);

	uint16_t y_len = upd_conf->y_end - upd_conf->y_start;
	
	// configure length of update
	driver_depg0290b1_dev_write_command_p3(0x01, y_len & 0xff, y_len >> 8, 0x00);

	// configure starting-line of update
	driver_depg0290b1_dev_write_command_p2(0x0f, upd_conf->y_start & 0xff, upd_conf->y_start >> 8);

	// bitmapped enabled phases of the update: (in this order)
	//   80 - enable clock signal
	//   40 - enable CP
	//   20 - load temperature value
	//   10 - load LUT
	//   08 - initial display
	//   04 - pattern display
	//   02 - disable CP
	//   01 - disable clock signal
	driver_depg0290b1_dev_write_command_p1(0x22, 0xc7);

	// start update
	driver_depg0290b1_dev_write_command(0x20);

	memcpy_u32(driver_depg0290b1_oldbuf, buf, DISP_SIZE_X_B * DISP_SIZE_Y/4);
	driver_depg0290b1_have_oldbuf = true;
}

uint16_t minimal_update_height = DISP_SIZE_Y;

void driver_depg0290b1_set_minimal_update_height(uint16_t height)
{
	if (height > DISP_SIZE_Y) height = DISP_SIZE_Y;
	minimal_update_height = height;
}

void driver_depg0290b1_display_part(const uint8_t *img, driver_depg0290b1_flags_t flags, uint16_t y_start, uint16_t y_end) {
	int lut_mode = (flags >> DISPLAY_FLAG_LUT_BIT) & ((1 << DISPLAY_FLAG_LUT_SIZE)-1);

	uint32_t *buf = driver_depg0290b1_tmpbuf;
	if (img == NULL) {
		memset_u32(buf, 0, DISP_SIZE_X_B * DISP_SIZE_Y/4);
	} else {
		driver_depg0290b1_create_bitplane(img, buf, 0x80, flags);
	}

	if ((flags & DISPLAY_FLAG_NO_UPDATE) != 0) {
		return;
	}

	int lut_flags = 0;
	if (!driver_depg0290b1_have_oldbuf || (flags & DISPLAY_FLAG_FULL_UPDATE)) {
		// old image not known (or full update requested); do full update
		lut_flags |= LUT_FLAG_FIRST;
	} else if (lut_mode - 1 != DRIVER_DEPG0290B1_LUT_FULL) {
		// old image is known; prefer to do a partial update
		lut_flags |= LUT_FLAG_PARTIAL;
	}
	
	/*
	This piece of code makes the height of the updated area at least a certain
	size wide. This is done to make sure the pixels involved change enough to be
	noticable and it is a big workaround. The default minimum area is the height
	of the display, resulting in full updates.
	 */
	
	if (y_end-y_start+1 < minimal_update_height) {
		y_end += minimal_update_height+y_start-y_end;
		if (y_end >= DISP_SIZE_Y) {
			y_start -= y_end % DISP_SIZE_Y;
			y_end -= y_end % DISP_SIZE_Y;
		}
	}
	
	struct driver_depg0290b1_update depg0290b1_upd = {
		.lut       = lut_mode > 0 ? lut_mode - 1 : DRIVER_DEPG0290B1_LUT_DEFAULT,
		.lut_flags = lut_flags,
		.reg_0x3a  = 26,   // 26 dummy lines per gate
		.reg_0x3b  = 0x08, // 62us per line
		.y_start   = y_start,
		.y_end     = y_end,
	};
	driver_depg0290b1_update(buf, &depg0290b1_upd);
}

void driver_depg0290b1_display(const uint8_t *img, driver_depg0290b1_flags_t flags)
{
	driver_depg0290b1_display_part(img, flags, 0, 295);
}

void driver_depg0290b1_display_greyscale(const uint8_t *img, driver_depg0290b1_flags_t flags, int layers)
{
	// start with black.
	driver_depg0290b1_display(NULL, flags | DISPLAY_FLAG_FULL_UPDATE);
	layers = 5;

	driver_depg0290b1_have_oldbuf = false;

	for (int layer = 0; layer < layers; layer++) {
		int bit = 128 >> layer;
		int t = bit;
		// depg: 128, 64, 32, 16, 8

		int p = 4;

		while ((t & 1) == 0 && (p > 1)) {
			t >>= 1;
			p >>= 1;
		}

		if (driver_depg0290b1_have_oldbuf == false && p == 1 && t > 1 && layer+1 < layers) {
			driver_depg0290b1_create_bitplane(img, driver_depg0290b1_oldbuf, bit, flags);
			driver_depg0290b1_have_oldbuf = true;
			continue;
		}

		for (int j = 0; j < p; j++) {
			int y_start = 0 + j * (DISP_SIZE_Y / p);
			int y_end = y_start + (DISP_SIZE_Y / p) - 1;

			uint32_t *buf = driver_depg0290b1_tmpbuf;
			driver_depg0290b1_create_bitplane(img, buf, bit, flags);

			// clear borders
			memset_u32(buf, 0, y_start * DISP_SIZE_X_B/4);
			memset_u32(&buf[(y_end+1) * DISP_SIZE_X_B/4], 0, (DISP_SIZE_Y-y_end-1) * DISP_SIZE_X_B/4);

			struct driver_depg0290b1_lut_entry lut[4];

			if (driver_depg0290b1_have_oldbuf) {
				// LUT:
				//   Use old state as previous layer;
				//   Do nothing when bits are not set;
				//   Make pixel whiter when bit is set;
				//   Duration is <t> cycles.
				lut[0].length = t;
				lut[0].voltages = 0x80;
				lut[1].length = t;
				lut[1].voltages = 0xa0;
				lut[2].length = t;
				lut[2].voltages = 0xa8;
				lut[3].length = 0;

			} else {
				// LUT:
				//   Ignore old state;
				//   Do nothing when bit is not set;
				//   Make pixel whiter when bit is set;
				//   Duration is <t> cycles.
				lut[0].length = t;
				lut[0].voltages = 0x88;
				lut[1].length = 0;
			}

			/* update display */
			struct driver_depg0290b1_update depg0290b1_upd = {
				.lut        = DRIVER_DEPG0290B1_LUT_CUSTOM,
				.lut_custom = lut,
				.reg_0x3a   = 0, // no dummy lines per gate
				.reg_0x3b   = 0, // 30us per line
				.y_start    = y_start,
				.y_end      = y_end + 1,
			};
			driver_depg0290b1_update(buf, &depg0290b1_upd);
			driver_depg0290b1_have_oldbuf = false;
		}
	}
}

void driver_depg0290b1_deep_sleep(void) {
	driver_depg0290b1_dev_write_command_p1(0x10, 0x01); // enter deep sleep
}

void driver_depg0290b1_wakeup(void) {
	driver_depg0290b1_dev_write_command_p1(0x10, 0x00); // leave deep sleep
}

esp_err_t driver_depg0290b1_init(void) {
	// initialize spi interface to display
	esp_err_t res = driver_depg0290b1_dev_init();
	if (res != ESP_OK) { ESP_LOGE(TAG, "dev init failure");  return res; }

	// allocate buffers
	driver_depg0290b1_tmpbuf = heap_caps_malloc(DISP_SIZE_X_B * DISP_SIZE_Y, MALLOC_CAP_32BIT);
	if (driver_depg0290b1_tmpbuf == NULL) { ESP_LOGE(TAG, "tmpbuf alloc no mem"); return ESP_ERR_NO_MEM; }

	driver_depg0290b1_oldbuf = heap_caps_malloc(DISP_SIZE_X_B * DISP_SIZE_Y, MALLOC_CAP_32BIT);
	if (driver_depg0290b1_oldbuf == NULL) { ESP_LOGE(TAG, "oldbuf alloc no mem");  return ESP_ERR_NO_MEM; }

	driver_depg0290b1_dev_reset(); // Hardware reset
	driver_depg0290b1_dev_write_command(0x12); // Software reset
	driver_depg0290b1_dev_write_command_p1(0x74, 0x54); // Set analog block control
	driver_depg0290b1_dev_write_command_p1(0x7E, 0x3B); // Set digital block control
	driver_depg0290b1_dev_write_command_p3(0x01, 0x27, 0x01, 0x00); // Set display size and driver output control
	driver_depg0290b1_dev_write_command_p1(0x11, 0x03); // Ram data entry mode (Adress counter is updated in Y direction, Y increment, X increment)
	driver_depg0290b1_dev_write_command_p2(0x44, 0x00, 0x0F); // Set RAM X address (00h to 0Fh)
	driver_depg0290b1_dev_write_command_p4(0x45, 0x00, 0x00, 0x27, 0x01); // Set RAM Y address (0127h to 0000h)
	driver_depg0290b1_dev_write_command_p1(0x3C, 0x01); // Set border waveform for VBD (see datasheet)
	driver_depg0290b1_dev_write_command_p1(0x2C, 0x26); // Set VCOM value (SET VOLTAGE)
	driver_depg0290b1_dev_write_command_p1(0x03, 0x17); // Gate voltage setting (17h = 20 Volt, ranges from 10v to 21v) (SET VOLTAGE)
	driver_depg0290b1_dev_write_command_p3(0x04, 0x41, 0x00, 0x32); // Source voltage setting (15volt, 0 volt and -15 volt) (SET VOLTAGE)
	return ESP_OK;
}
