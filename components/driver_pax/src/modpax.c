/*
 * BADGE.TEAM framebuffer driver
 * Implements the PAX C API for BadgePython.
 * Julian Scheffers 2023
 */

#ifndef NO_QSTR
#include <pax_gfx.h>
#include "modpax.h"
#endif

#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "py/objarray.h"

// extern uint8_t* framebuffer;
pax_buf_t buf;

static mp_obj_t pax2py_debug(mp_uint_t n_args, const mp_obj_t *args) {
	printf("The DEBUG.\n");
	return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_VAR(pax2py_debug_obj, 0, pax2py_debug);

void driver_pax2py_init() {
	pax_buf_type_t type = PAX2PY_BUF_NATIVE;
	pax_buf_init(&buf, NULL, 320, 240, type);
}



static const mp_rom_map_elem_t pax_module_globals_table[] = {
	{MP_ROM_QSTR(MP_QSTR_debug), MP_ROM_PTR(&pax2py_debug_obj)},
};

static MP_DEFINE_CONST_DICT(pax_module_globals, pax_module_globals_table);

const mp_obj_module_t pax_module = {
	.base = {&mp_type_module},
	.globals = (mp_obj_dict_t *)&pax_module_globals,
};
