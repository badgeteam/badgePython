#include "include/platform.h"
#include "include/platform_gen.h"
#include "driver_framebuffer.h"
#include <driver/gpio.h>
#include <esp_event.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <esp_spi_flash.h>
#include <esp_partition.h>
#include <esp_vfs.h>
#include <esp_vfs_fat.h>
#include <esp_log.h>
#include "buses.h"
#include <driver/uart.h>

#define TAG "platform"

esp_err_t isr_init() {
  esp_err_t res = gpio_install_isr_service(0);
  if (res == ESP_FAIL) {
    ESP_LOGW(TAG, "Failed to install gpio isr service. Ignoring this.");
    res = ESP_OK;
  }
  if (res != ESP_OK) {
    ESP_LOGE(TAG, "Failed to install gpio isr service: %d", res);
  }
  return res;
}

bool fbReady = false;

void fatal_error(const char *message) {
  printf("A fatal error occurred while initializing the driver for '%s'.\n", message);
  if (fbReady) {
#ifdef CONFIG_DRIVER_FRAMEBUFFER_ENABLE
#if defined(CONFIG_DRIVER_EINK_ENABLE) || defined(CONFIG_DRIVER_ILI9341_ENABLE)
    driver_framebuffer_fill(NULL, COLOR_WHITE);
    uint16_t y =
        driver_framebuffer_print(NULL, "Fatal error\n", 0, 0, 1, 1, COLOR_BLACK, &roboto_12pt7b);
    y = driver_framebuffer_print(NULL, "Failure while starting driver.\n", 0, y, 1, 1, COLOR_BLACK,
                                 &roboto_12pt7b);
    y = driver_framebuffer_print(NULL, message, 0, y, 1, 1, COLOR_BLACK, &roboto_12pt7b);
    y = driver_framebuffer_print(NULL, "\n\nRestarting in 10 seconds...\n", 0, y, 1, 1, COLOR_BLACK,
                                 &roboto_12pt7b);
    driver_framebuffer_flush(0);
#endif
#if defined(CONFIG_DRIVER_SSD1306_ENABLE) || defined(CONFIG_DRIVER_ERC12846_ENABLE)
    driver_framebuffer_fill(NULL, COLOR_BLACK);
    uint16_t y =
        driver_framebuffer_print(NULL, "Fatal error\n", 0, 0, 2, 2, COLOR_WHITE, &ipane7x5);
    y = driver_framebuffer_print(NULL, message, 0, y + 5, 1, 1, COLOR_WHITE, &ipane7x5);
    driver_framebuffer_flush(0);
#endif
#endif
  }
  restart();
}

void platform_init()
{
	if (isr_init() != ESP_OK) restart();
  //Init UART0 temporarly until upstream uPy is fixed. TODO: Remove this when uPy 1.19 is released
  uart_config_t uartcfg = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0
  };
  uart_param_config(0, &uartcfg);
  uart_driver_install(0, 129, 0, 0, NULL, 0);
  uart_isr_free(0);


  //Static inits can be performed here
  start_buses();

  //Init generated modules
  platform_gen_init();
  
	fflush(stdout);
	vTaskDelay(100 / portTICK_PERIOD_MS); //Give things time to settle.
}
