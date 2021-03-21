#include "include/driver_rtcmem.h"

#include <stdio.h>
#include <string.h>

#include "sdkconfig.h"

#include "py/obj.h"
#include "py/objstr.h"
#include "py/runtime.h"

#include "mphalport.h"
#include "modmachine.h"

// ====== RTC memory functions ============================

//--------------------------------------------------------------------------------
STATIC mp_obj_t esp_rtcmem_write(mp_obj_t _pos, mp_obj_t _val) {
	int pos = mp_obj_get_int(_pos);
	int val = mp_obj_get_int(_val);
	if (driver_rtcmem_int_write(pos, val) != ESP_OK) return mp_const_false;
	return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp_rtcmem_write_obj, esp_rtcmem_write);

//----------------------------------------------------------------
STATIC mp_obj_t esp_rtcmem_read(mp_obj_t _pos) {
	int pos = mp_obj_get_int(_pos);
	int value;
	if (driver_rtcmem_int_read(pos, &value) != ESP_OK) return mp_const_none;	
	return mp_obj_new_int(value);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp_rtcmem_read_obj, esp_rtcmem_read);

//--------------------------------------------------------------------------
STATIC mp_obj_t esp_rtcmem_write_string(mp_obj_t str_in) {
	const char *str = mp_obj_str_get_str(str_in);
	if (driver_rtcmem_string_write(str) != ESP_OK) return mp_const_false;
	return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp_rtcmem_write_string_obj, esp_rtcmem_write_string);

//--------------------------------------------------------
STATIC mp_obj_t esp_rtcmem_read_string() {
	const char* str;
	if (driver_rtcmem_string_read(&str) != ESP_OK) return mp_const_none;
	return mp_obj_new_str(str, strlen(str));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp_rtcmem_read_string_obj, esp_rtcmem_read_string);

//--------------------------------------------------
STATIC mp_obj_t esp_rtcmem_clear() {
    driver_rtcmem_clear();
	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp_rtcmem_clear_obj, esp_rtcmem_clear);


//=========================================================
STATIC const mp_rom_map_elem_t rtcmem_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_write),			(mp_obj_t)&esp_rtcmem_write_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_read),			(mp_obj_t)&esp_rtcmem_read_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_clear),			(mp_obj_t)&esp_rtcmem_clear_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_write_string),	(mp_obj_t)&esp_rtcmem_write_string_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_read_string),		(mp_obj_t)&esp_rtcmem_read_string_obj},
};
STATIC MP_DEFINE_CONST_DICT(rtcmem_module_globals, rtcmem_module_globals_table);

//===================================
const mp_obj_module_t rtcmem_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&rtcmem_module_globals,
};