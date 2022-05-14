#ifndef NO_QSTR
#include "rp2040.h"

#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/runtime.h"
#endif

#define TAG "RP2040_UPY"

#define RP2040_ADDR 0x17
#define GPIO_INT_RP2040  34

static RP2040 rp2040;
static mp_obj_t touch_callback = mp_const_none;



void driver_rp2040_init() {
    rp2040.i2c_bus = 0;
    rp2040.i2c_address = RP2040_ADDR;
    rp2040.pin_interrupt = GPIO_INT_RP2040;

    rp2040_init(&rp2040);
}

static mp_obj_t buttons() {
    uint16_t value;
    rp2040_read_buttons(&rp2040, &value);
    return mp_obj_new_int(value);
}
static MP_DEFINE_CONST_FUN_OBJ_0(buttons_obj, buttons);

static void button_handler(uint16_t touch_state) {
    if (touch_callback != mp_const_none) {
        // uPy scheduler tends to get full for unknown reasons, so to not lose any interrupts,
        // we try until the schedule succeeds.
        bool succeeded = mp_sched_schedule(touch_callback, mp_obj_new_int(touch_state));
        while (!succeeded) {
            ESP_LOGW(TAG, "Failed to call touch callback, retrying");
            succeeded = mp_sched_schedule(touch_callback, mp_obj_new_int(touch_state));
        }
    }
}

static mp_obj_t set_handler(mp_obj_t handler) {
    touch_callback = handler;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(set_handler_obj, set_handler);

static mp_obj_t get_brightness() {
    uint8_t value;
    rp2040_get_lcd_backlight(&rp2040, &value);
    return mp_obj_new_int(value);
}
static MP_DEFINE_CONST_FUN_OBJ_0(get_brightness_obj, get_brightness);

static mp_obj_t set_brightness(mp_obj_t brightness) {
    uint8_t value = mp_obj_get_int(brightness);
    rp2040_set_lcd_backlight(&rp2040, value);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(set_brightness_obj, set_brightness);

STATIC const mp_rom_map_elem_t rp2040_module_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR_buttons), MP_ROM_PTR(&buttons_obj)},
    {MP_ROM_QSTR(MP_QSTR_get_brightness), MP_ROM_PTR(&get_brightness_obj)},
    {MP_ROM_QSTR(MP_QSTR_set_brightness), MP_ROM_PTR(&set_brightness_obj)},
    {MP_ROM_QSTR(MP_QSTR_set_handler), MP_ROM_PTR(&set_handler_obj)},
};

STATIC MP_DEFINE_CONST_DICT(rp2040_module_globals, rp2040_module_globals_table);

//===================================
const mp_obj_module_t rp2040_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&rp2040_module_globals,
};