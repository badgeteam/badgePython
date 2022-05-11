#ifndef NO_QSTR
#include "rp2040.h"

#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/runtime.h"
#endif

#define RP2040_ADDR 0x17
#define GPIO_INT_RP2040  34

static RP2040 rp2040;

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

STATIC const mp_rom_map_elem_t rp2040_module_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR_buttons), MP_ROM_PTR(&buttons_obj)},
};

STATIC MP_DEFINE_CONST_DICT(rp2040_module_globals, rp2040_module_globals_table);

//===================================
const mp_obj_module_t rp2040_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&rp2040_module_globals,
};