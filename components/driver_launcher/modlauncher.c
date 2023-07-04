#ifndef NO_QSTR
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include <driver/uart.h>

#include <esp_system.h>
#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"
#endif

#define TAG "LAUNCHER_UPY"

void driver_launcher_init() {

}

static mp_obj_t driver_launcher_return_to_launcher() {
    REG_WRITE(RTC_CNTL_STORE0_REG, 0);
    esp_restart();
    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_0(launcher_return_to_launcher_obj, driver_launcher_return_to_launcher);

STATIC const mp_rom_map_elem_t launcher_module_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR_exit_python), MP_ROM_PTR(&launcher_return_to_launcher_obj)},
};

STATIC MP_DEFINE_CONST_DICT(launcher_module_globals, launcher_module_globals_table);

//===================================
const mp_obj_module_t launcher_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&launcher_module_globals,
};
