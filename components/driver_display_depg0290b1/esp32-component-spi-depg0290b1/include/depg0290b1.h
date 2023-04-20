#pragma once

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#include <stdint.h>
#include <esp_err.h>
#include <stdbool.h>

#define DRIVER_DEPG0290B1_WIDTH  296
#define DRIVER_DEPG0290B1_HEIGHT 128
#define DISP_SIZE_X 128
#define DISP_SIZE_Y 296
#define DISP_SIZE_X_B ((DISP_SIZE_X + 7) >> 3)
#define DEPG0290B1_BUFFER_SIZE DRIVER_DEPG0290B1_WIDTH*DRIVER_DEPG0290B1_HEIGHT
#define DRIVER_DEPG0290B1_LUT_MAX_SIZE 70

esp_err_t driver_depg0290b1_dev_init(void);
esp_err_t driver_depg0290b1_dev_reset(void);
bool driver_depg0290b1_dev_is_busy(void);
void driver_depg0290b1_dev_busy_wait(void);
void driver_depg0290b1_dev_write_byte(uint8_t data);
void driver_depg0290b1_dev_write_command(uint8_t command);

static inline void driver_depg0290b1_dev_write_command_p1(uint8_t command, uint8_t para1)
{
	driver_depg0290b1_dev_write_command(command);
	driver_depg0290b1_dev_write_byte(para1);
}

static inline void driver_depg0290b1_dev_write_command_p2(uint8_t command, uint8_t para1,
									  uint8_t para2)
{
	driver_depg0290b1_dev_write_command(command);
	driver_depg0290b1_dev_write_byte(para1);
	driver_depg0290b1_dev_write_byte(para2);
}

static inline void driver_depg0290b1_dev_write_command_p3(uint8_t command, uint8_t para1,
									  uint8_t para2, uint8_t para3)
{
	driver_depg0290b1_dev_write_command(command);
	driver_depg0290b1_dev_write_byte(para1);
	driver_depg0290b1_dev_write_byte(para2);
	driver_depg0290b1_dev_write_byte(para3);
}

static inline void driver_depg0290b1_dev_write_command_p4(uint8_t command, uint8_t para1,
									  uint8_t para2, uint8_t para3,
									  uint8_t para4)
{
	driver_depg0290b1_dev_write_command(command);
	driver_depg0290b1_dev_write_byte(para1);
	driver_depg0290b1_dev_write_byte(para2);
	driver_depg0290b1_dev_write_byte(para3);
	driver_depg0290b1_dev_write_byte(para4);
}

void driver_depg0290b1_dev_write_command_stream(uint8_t command, const uint8_t *data,
										 unsigned int datalen);

void driver_depg0290b1_dev_write_command_stream_u32(uint8_t command, const uint32_t *data,
										 unsigned int datalen);



/** specification of display update instruction */
struct driver_depg0290b1_lut_entry {
	/** the number of cycles the voltages are held; 0 = end of list */
	uint8_t length;

	/** bitmapped value containing voltages for every (old-bit, new-bit) pair:
	 * - bits 0,1: from 0 to 0
	 * - bits 2,3: from 0 to 1
	 * - bits 4,5: from 1 to 0
	 * - bits 6,7: from 1 to 1
	 *
	 * allowed values:
	 * - 0: VSS
	 * - 1: VSH
	 * - 2: VSL
	 */
	uint8_t voltages;
};

/** filters to use on a driver_depg0290b1_lut_entry structure */
enum driver_depg0290b1_lut_flags {
	LUT_FLAG_FIRST    = 1, // do not depend on previous image
	LUT_FLAG_PARTIAL  = 2, // do not touch already correct pixels
	LUT_FLAG_WHITE    = 4, // white only
	LUT_FLAG_BLACK    = 8, // black only
};

/**
 * Generate LUT data for specific depg0290b1 display.
 *
 * @param list screen updata data in 'generic' format.
 * @param flags optional alterations on generated lut data.
 * @param lut output data buffer. should be of size DRIVER_DEPG0290B1_LUT_MAX_SIZE.
 * @return lut length. returns -1 on error.
 */
int driver_depg0290b1_lut_generate(const struct driver_depg0290b1_lut_entry *list, enum driver_depg0290b1_lut_flags flags, uint8_t *lut);


/** Initialize the depg0290b1 display
 * @return ESP_OK on success; any other value indicates an error
 */
esp_err_t driver_depg0290b1_init();

/** driver_depg0290b1_update 'lut' settings */
enum driver_depg0290b1_lut
{
	DRIVER_DEPG0290B1_LUT_CUSTOM  = -1,
	DRIVER_DEPG0290B1_LUT_FULL    =  0,
	DRIVER_DEPG0290B1_LUT_NORMAL  =  1,
	DRIVER_DEPG0290B1_LUT_FASTER  =  2,
	DRIVER_DEPG0290B1_LUT_FASTEST =  3,
	DRIVER_DEPG0290B1_LUT_DEFAULT = DRIVER_DEPG0290B1_LUT_FULL,
	DRIVER_DEPG0290B1_LUT_MAX     = DRIVER_DEPG0290B1_LUT_FASTEST,
};

/** config-settings structure */
struct driver_depg0290b1_update {
	/** lut index */
	int lut;
	/** optional lut flags */
	int lut_flags;
	/** the raw lut data if DRIVER_DEPG0290B1_LUT_CUSTOM is selected */
	const struct driver_depg0290b1_lut_entry *lut_custom;
	/** raw setting for the number of dummy lines */
	int reg_0x3a;
	/** raw setting for the time per line */
	int reg_0x3b;
	/** the start column for partial-screen-updates */
	int y_start;
	/** the end column for partial-screen-updates */
	int y_end;
};

/** refresh the depg0290b1 display with given config-settings
 * @param buf the raw buffer to write to the screen
 * @param upd_conf the config-settings to use
 */
void driver_depg0290b1_update(const uint32_t *buf, const struct driver_depg0290b1_update *upd_conf);

/** driver_depg0290b1_display flags settings */
typedef int driver_depg0290b1_flags_t;

// bitmapped flags:
/** the input buffer is 8 bits per pixel instead of 1 bit per pixel */
#define DISPLAY_FLAG_8BITPIXEL    1
/** rotate the output 180 degrees */
#define DISPLAY_FLAG_ROTATE_180   2
/** update internal buffer; use driver_depg0290b1_update() to push update to screen */
#define DISPLAY_FLAG_NO_UPDATE    4
/** ensure the screen gets a full update */
#define DISPLAY_FLAG_FULL_UPDATE  8

// fields and sizes:
/** the lut is stored in bits 8-11 */
#define DISPLAY_FLAG_LUT_BIT      8
/** the lut is stored in bits 8-11 */
#define DISPLAY_FLAG_LUT_SIZE     4

/** macro for specifying driver_depg0290b1_flags_t lut type */
#define DISPLAY_FLAG_LUT(x) ((1+(x)) << DISPLAY_FLAG_LUT_BIT)

void driver_depg0290b1_display(const uint8_t *img, driver_depg0290b1_flags_t flags);
void driver_depg0290b1_display_part(const uint8_t *img, driver_depg0290b1_flags_t flags, uint16_t y_start, uint16_t y_end);
void driver_depg0290b1_display_greyscale(const uint8_t *img, driver_depg0290b1_flags_t flags, int layers);
void driver_depg0290b1_deep_sleep(void);
void driver_depg0290b1_wakeup(void);
void driver_depg0290b1_set_minimal_update_height(uint16_t height);

#ifdef __cplusplus
}
#endif //__cplusplus
