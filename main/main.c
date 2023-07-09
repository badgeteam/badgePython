#include "include/nvs_init.h"
#include "include/platform.h"
#include "include/system.h"
#include "include/ota_update.h"
#include "include/factory_reset.h"
#include "driver_framebuffer.h"
#include "include/micropython_main.h"

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"

#define TAG "MAIN"

extern void micropython_entry(void);

extern esp_err_t unpack_first_boot_zip(void);

void nvs_write_zip_status(bool status)
{
	nvs_handle my_handle;
	esp_err_t res = nvs_open("system", NVS_READWRITE, &my_handle);
	if (res != ESP_OK) {
		printf("NVS seems unusable! Please erase flash and try flashing again. (1)\n");
		halt();
	}
	res = nvs_set_u8(my_handle, "preseed", status);
	if (res != ESP_OK) {
		printf("NVS seems unusable! Please erase flash and try flashing again. (2)\n");
		halt();
	}
}

void app_main()
{
	size_t mp_task_heap_size = mp_preallocate_heap();
    ESP_LOGI(TAG, "Heap size: %d", mp_task_heap_size);
	
	logo();
	bool is_first_boot = nvs_init();

	fflush(stdout);
	platform_init();

#ifndef CONFIG_FW_DISABLE_OTA_AND_FIRSTBOOT
	if (is_first_boot) {
		#ifdef CONFIG_DRIVER_FRAMEBUFFER_ENABLE
			driver_framebuffer_fill(NULL, COLOR_BLACK);
			driver_framebuffer_print(NULL, "Extracting ZIP...\n", 0, 0, 1, 1, COLOR_WHITE, &roboto_12pt7b);
			driver_framebuffer_flush(0);
		#endif
		printf("Attempting to unpack FAT initialization ZIP file...\b");
		if (unpack_first_boot_zip() != ESP_OK) { //Error
			#ifdef CONFIG_DRIVER_FRAMEBUFFER_ENABLE
				driver_framebuffer_fill(NULL, COLOR_BLACK);
				driver_framebuffer_print(NULL, "ZIP error!\n", 0, 0, 1, 1, COLOR_WHITE, &roboto_12pt7b);
				driver_framebuffer_flush(0);
			#endif
			printf("An error occured while unpacking the ZIP file!");
			nvs_write_zip_status(false);
		} else {
			nvs_write_zip_status(true);
		}
		esp_restart();
	}
#endif

	// Reset AppFS file descriptor so that BadgePython will be loaded again after reboot
	// (The launcher sets magic value 0xA5, the bootloader changes this to 0xA6, we change it back)
	uint32_t appfs_reg = REG_READ(RTC_CNTL_STORE0_REG);
	if ((appfs_reg & 0xFF000000) == 0xA6000000) { // Check that we started from AppFS before writing
		REG_WRITE(RTC_CNTL_STORE0_REG, 0xA5000000 | (appfs_reg & 0x00FFFFFF));
	}

	 int magic = get_magic();
	
	 switch(magic) {
#ifndef CONFIG_FW_DISABLE_OTA_AND_FIRSTBOOT
	 	case MAGIC_OTA:
	 	  // This triggers an Over-the-Air firmware update
	 		badge_ota_update();
	 		break;
	 	case MAGIC_FACTORY_RESET:
	 	  // This clears any FAT data partitions
	 		factory_reset();
	 		break;
#endif
	 	default:
	 	 	// This starts the MicroPython FreeRTOS task
       		start_mp();
			while(1){}
	 }
}
