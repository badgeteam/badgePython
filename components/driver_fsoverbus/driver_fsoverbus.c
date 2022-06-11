#include <sdkconfig.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <esp_err.h>
#include <esp_log.h>
#include <driver/uart.h>
#include <driver/gpio.h>
#include <esp_vfs.h>
#include <dirent.h>
#include <esp_intr_alloc.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/timers.h"

#include "include/driver_fsoverbus.h"
#include "include/filefunctions.h"
#include "include/packetutils.h"
#include "include/specialfunctions.h"
#include "include/fsob_backend.h"

//TEMP
#include <driver/spi_master.h>


#ifdef CONFIG_DRIVER_FSOVERBUS_ENABLE

void vTimeoutFunction( TimerHandle_t xTimer );

#define TAG "FSoverBus"

TimerHandle_t timeout;
RingbufHandle_t buf_handle[2];

uint8_t command_in[1024];

#define min(a,b) (((a) < (b)) ? (a) : (b))

//Function lookup tables

int (*specialfunction[])(uint8_t *data, uint16_t command, uint32_t message_id, uint32_t size, uint32_t received, uint32_t length) = {execfile, heartbeat, pythonstdin};
int specialfunction_size = 3;
                         //                                                                                  4096    4097      4098       4099     4100      4101    4102
int (*filefunction[])(uint8_t *data, uint16_t command, uint32_t message_id, uint32_t size, uint32_t received, uint32_t length) = {getdir, readfile, writefile, delfile, duplfile, mvfile, makedir};
int filefunction_size = 7;

void handleFSCommand(uint8_t *data, uint16_t command, uint32_t message_id, uint32_t size, uint32_t received, uint32_t length) {
    static uint32_t write_pos;
    if(received == length) { //First data of the packet
        write_pos = 0;
    }

    if(length > 0) {
        memcpy(&command_in[write_pos], data, length);
        write_pos += length;
    }

    int return_val = 0;
    if(command < 4096) {
        if(command < specialfunction_size) {
            return_val = specialfunction[command](command_in, command, message_id, size, received, length);
        }
    } else if(command < 8192) {
        if((command-4096) < filefunction_size) {
            return_val = filefunction[command-4096](command_in, command, message_id, size, received, length);
        }
    }
    if(return_val) {    //Function has indicated that next payload should write at start of buffer.
        write_pos = 0;
    }
}

void vTimeoutFunction( TimerHandle_t xTimer ) {
    ESP_LOGI(TAG, "Saw no message for 1s assuming task crashed. Resetting...");
    fsob_reset();
}

void fsob_stop_timeout() {
     xTimerStop(timeout, 1);
}

void fsob_start_timeout() {
    xTimerStart(timeout, 1);
}

esp_err_t driver_fsoverbus_init(void) { 
    fsob_init();

    ESP_LOGI(TAG, "fs over bus registered.");
    
    timeout = xTimerCreate("FSoverBUS_timeout", 100, false, 0, vTimeoutFunction);
    return ESP_OK;
} 

#endif