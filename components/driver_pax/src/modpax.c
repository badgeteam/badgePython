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

extern uint8_t *framebuffer;

typedef struct {
	pax_col_t fill_color, line_color;
	pax_buf_t buf;
} buf_n_col_t;

buf_n_col_t global_pax_buf;

/* ==== HELPER FUNCTIONS ==== */
// Get buffer to use on some operation (assuming buffer is first arg).
static buf_n_col_t *get_buf(mp_uint_t *n_args, const mp_obj_t **args) {
	// No-op for now.
	return &global_pax_buf;
}

// Get color to use on some operation (assuming color is first arg).
static pax_col_t get_fill_color(buf_n_col_t *buf, mp_uint_t *n_args, const mp_obj_t **args, int min) {
	if (*n_args > min) {
		pax_col_t col = mp_obj_get_int_truncated(**args);
		--*n_args;
		++*args;
		return col;
	} else {
		return buf->fill_color;
	}
}

// Get color to use on some operation (assuming color is first arg).
static pax_col_t get_line_color(buf_n_col_t *buf, mp_uint_t *n_args, const mp_obj_t **args, int min) {
	if (*n_args > min) {
		pax_col_t col = mp_obj_get_int_truncated(**args);
		--*n_args;
		++*args;
		return col;
	} else {
		return buf->line_color;
	}
}

#define GET_BUF() get_buf(&n_args,&args)
#define GET_FILL_COLOR(min) get_fill_color(buf,&n_args,&args,min)
#define GET_LINE_COLOR(min) get_line_color(buf,&n_args,&args,min)



/* ==== FUNCTION DEFINITIONS ==== */
// For debugging purposes.
static mp_obj_t pax2py_debug(mp_uint_t n_args, const mp_obj_t *args) {
	printf("You gave me %d arguments.\n", n_args);
	for (size_t i = 0; i < n_args; i++) {
		printf("  %s\n", mp_obj_get_type_str(args[i]));
	}
	return mp_const_none;
}

// Fill color accessor.
static mp_obj_t fillColor(mp_uint_t n_args, const mp_obj_t *args) {
	buf_n_col_t *buf = GET_BUF();
	if (n_args) buf->fill_color = mp_obj_get_int_truncated(args[0]);
	return mp_obj_new_int(buf->fill_color);
}

// Line color accessor.
static mp_obj_t lineColor(mp_uint_t n_args, const mp_obj_t *args) {
	buf_n_col_t *buf = GET_BUF();
	if (n_args) buf->line_color = mp_obj_get_int_truncated(args[0]);
	return mp_obj_new_int(buf->line_color);
}

// Rectangle drawing variants.
static mp_obj_t drawRect(mp_uint_t n_args, const mp_obj_t *args) {
	// Grab arguments.
	buf_n_col_t *buf = GET_BUF();
	pax_col_t  color = GET_FILL_COLOR(4);
	float      x     = mp_obj_get_float(args[0]);
	float      y     = mp_obj_get_float(args[1]);
	float      w     = mp_obj_get_float(args[2]);
	float      h     = mp_obj_get_float(args[3]);
	printf("pax_draw_rect(0x%08x, %f, %f, %f, %f)\n", color, x, y, w, h);
	// Forward "things".
	pax_draw_rect(&buf->buf, color, x, y, w, h);
	return mp_const_none;
}



/* ==== OBJECT DEFINITIONS ==== */
static MP_DEFINE_CONST_FUN_OBJ_VAR(pax2py_debug_obj, 0, pax2py_debug);

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(fillColor_obj, 0, 1, fillColor);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lineColor_obj, 0, 1, lineColor);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(drawRect_obj, 4, 5, drawRect);



/* ==== MODULE DEFINITION ==== */
static const mp_rom_map_elem_t pax_module_globals_table[] = {
	{MP_ROM_QSTR(MP_QSTR_debug), MP_ROM_PTR(&pax2py_debug_obj)},
	{MP_ROM_QSTR(MP_QSTR_fillColor), MP_ROM_PTR(&fillColor_obj)},
	{MP_ROM_QSTR(MP_QSTR_lineColor), MP_ROM_PTR(&lineColor_obj)},
	{MP_ROM_QSTR(MP_QSTR_drawRect), MP_ROM_PTR(&drawRect_obj)},
};

static MP_DEFINE_CONST_DICT(pax_module_globals, pax_module_globals_table);

const mp_obj_module_t pax_module = {
	.base = {&mp_type_module},
	.globals = (mp_obj_dict_t *)&pax_module_globals,
};



/* ==== MODULE INITIALISER ==== */
extern void driver_framebuffer_set_dirty_area(int16_t x0, int16_t y0, int16_t x1, int16_t y1, bool force);
void driver_framebuffer_pre_flush_callback() {
	pax_recti dirty = pax_get_dirty(&global_pax_buf.buf);
	if (pax_is_dirty(&global_pax_buf.buf)) {
		driver_framebuffer_set_dirty_area(
			dirty.x,
			dirty.y,
			dirty.x + dirty.w - 1,
			dirty.y + dirty.h - 1,
			false
		);
	}
}

extern esp_err_t driver_framebuffer_init();
esp_err_t driver_pax2py_init() {
	// Get framebuffer from the other place(tm).
	driver_framebuffer_init();
	
	// Create global PAX context.
	pax_buf_type_t type = PAX2PY_BUF_NATIVE;
	pax_buf_init(&global_pax_buf.buf, framebuffer, 320, 240, type);
	global_pax_buf.fill_color = 0xffffffff;
	global_pax_buf.line_color = 0xffffffff;
	
	return 0;
}
