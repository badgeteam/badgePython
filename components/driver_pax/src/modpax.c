/*
 * BADGE.TEAM framebuffer driver
 * Implements the PAX C API for BadgePython.
 * Julian Scheffers 2023
 */

#include <sdkconfig.h>

#ifdef CONFIG_DRIVER_PAX_ENABLE

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

typedef struct {
	const pax_font_t *font;
	float font_size;
} font_n_size_t;

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

// Get buffer to use on some operation (assuming buffer is first arg).
static font_n_size_t get_font(mp_uint_t *n_args, const mp_obj_t **args) {
	// Is ther an font?
	if (mp_obj_is_str(**args)) {
		// Yes font; get.
		font_n_size_t out = {
			pax_get_font(mp_obj_str_get_str((*args)[0])),
			mp_obj_get_float((*args)[1]),
		};
		*n_args -= 2;
		*args   += 2;
		return out;
	} else {
		// No font; default.
		return (font_n_size_t) { PAX_FONT_DEFAULT, PAX_FONT_DEFAULT->default_size };
	}
}

#define GET_BUF() get_buf(&n_args,&args)
#define GET_FILL_COLOR(min) get_fill_color(buf,&n_args,&args,min)
#define GET_LINE_COLOR(min) get_line_color(buf,&n_args,&args,min)
#define GET_FONT() get_font(&n_args,&args)



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
	// Forward function call.
	pax_draw_rect(&buf->buf, color, x, y, w, h);
	return mp_const_none;
}

// Rectangle drawing variants.
static mp_obj_t outlineRect(mp_uint_t n_args, const mp_obj_t *args) {
	// Grab arguments.
	buf_n_col_t *buf = GET_BUF();
	pax_col_t  color = GET_FILL_COLOR(4);
	float      x     = mp_obj_get_float(args[0]);
	float      y     = mp_obj_get_float(args[1]);
	float      w     = mp_obj_get_float(args[2]);
	float      h     = mp_obj_get_float(args[3]);
	// Forward function call.
	pax_outline_rect(&buf->buf, color, x, y, w, h);
	return mp_const_none;
}

// Triangle drawing variants.
static mp_obj_t drawTri(mp_uint_t n_args, const mp_obj_t *args) {
	// Grab arguments.
	buf_n_col_t *buf = GET_BUF();
	pax_col_t  color = GET_FILL_COLOR(6);
	float      x0    = mp_obj_get_float(args[0]);
	float      y0    = mp_obj_get_float(args[1]);
	float      x1    = mp_obj_get_float(args[2]);
	float      y1    = mp_obj_get_float(args[3]);
	float      x2    = mp_obj_get_float(args[4]);
	float      y2    = mp_obj_get_float(args[5]);
	// Forward function call.
	pax_draw_tri(&buf->buf, color, x0, y0, x1, y1, x2, y2);
	return mp_const_none;
}

// Triangle drawing variants.
static mp_obj_t outlineTri(mp_uint_t n_args, const mp_obj_t *args) {
	// Grab arguments.
	buf_n_col_t *buf = GET_BUF();
	pax_col_t  color = GET_FILL_COLOR(6);
	float      x0    = mp_obj_get_float(args[0]);
	float      y0    = mp_obj_get_float(args[1]);
	float      x1    = mp_obj_get_float(args[2]);
	float      y1    = mp_obj_get_float(args[3]);
	float      x2    = mp_obj_get_float(args[4]);
	float      y2    = mp_obj_get_float(args[5]);
	// Forward function call.
	pax_draw_tri(&buf->buf, color, x0, y0, x1, y1, x2, y2);
	return mp_const_none;
}

// Circle drawing variants.
static mp_obj_t drawCircle(mp_uint_t n_args, const mp_obj_t *args) {
	// Grab arguments.
	buf_n_col_t *buf = GET_BUF();
	pax_col_t  color = GET_FILL_COLOR(3);
	float      x     = mp_obj_get_float(args[0]);
	float      y     = mp_obj_get_float(args[1]);
	float      r     = mp_obj_get_float(args[2]);
	// Forward function call.
	pax_draw_circle(&buf->buf, color, x, y, r);
	return mp_const_none;
}

// Circle drawing variants.
static mp_obj_t outlineCircle(mp_uint_t n_args, const mp_obj_t *args) {
	// Grab arguments.
	buf_n_col_t *buf = GET_BUF();
	pax_col_t  color = GET_FILL_COLOR(3);
	float      x     = mp_obj_get_float(args[0]);
	float      y     = mp_obj_get_float(args[1]);
	float      r     = mp_obj_get_float(args[2]);
	// Forward function call.
	pax_outline_circle(&buf->buf, color, x, y, r);
	return mp_const_none;
}

// Circle drawing variants.
static mp_obj_t drawArc(mp_uint_t n_args, const mp_obj_t *args) {
	// Grab arguments.
	buf_n_col_t *buf = GET_BUF();
	pax_col_t  color = GET_FILL_COLOR(5);
	float      x     = mp_obj_get_float(args[0]);
	float      y     = mp_obj_get_float(args[1]);
	float      r     = mp_obj_get_float(args[2]);
	float      a0    = mp_obj_get_float(args[3]);
	float      a1    = mp_obj_get_float(args[4]);
	// Forward function call.
	pax_draw_arc(&buf->buf, color, x, y, r, a0, a1);
	return mp_const_none;
}

// Arc drawing variants.
static mp_obj_t outlineArc(mp_uint_t n_args, const mp_obj_t *args) {
	// Grab arguments.
	buf_n_col_t *buf = GET_BUF();
	pax_col_t  color = GET_FILL_COLOR(5);
	float      x     = mp_obj_get_float(args[0]);
	float      y     = mp_obj_get_float(args[1]);
	float      r     = mp_obj_get_float(args[2]);
	float      a0    = mp_obj_get_float(args[3]);
	float      a1    = mp_obj_get_float(args[4]);
	// Forward function call.
	pax_outline_arc(&buf->buf, color, x, y, r, a0, a1);
	return mp_const_none;
}

// Line drawing variants.
static mp_obj_t drawLine(mp_uint_t n_args, const mp_obj_t *args) {
	// Grab arguments.
	buf_n_col_t *buf = GET_BUF();
	pax_col_t  color = GET_FILL_COLOR(4);
	float      x0    = mp_obj_get_float(args[0]);
	float      y0    = mp_obj_get_float(args[1]);
	float      x1    = mp_obj_get_float(args[2]);
	float      y1    = mp_obj_get_float(args[3]);
	// Forward function call.
	pax_draw_line(&buf->buf, color, x0, y0, x1, y1);
	return mp_const_none;
}


// TODO: draw, outline, draw_image


// Text size getter.
static mp_obj_t stringSize(mp_uint_t n_args, const mp_obj_t *args) {
	// Grab buffer but ignore it.
	buf_n_col_t  *buf  = GET_BUF();
	// Grab arguments.
	font_n_size_t font = GET_FONT();
	const char   *text = mp_obj_str_get_str(*args);
	// Forward function call.
	pax_vec2f     dims = pax_text_size(font.font, font.font_size, text);
	// Pack output.
	mp_obj_t objs[] = {
		mp_obj_new_float(dims.x),
		mp_obj_new_float(dims.y),
	};
	return mp_obj_new_tuple(2, objs);
}

// Text drawing variants.
static mp_obj_t drawString(mp_uint_t n_args, const mp_obj_t *args) {
	// Grab arguments.
	buf_n_col_t  *buf  = GET_BUF();
	pax_col_t     col  = GET_FILL_COLOR(mp_obj_is_str(*args) ? 5 : 3);
	font_n_size_t font = GET_FONT();
	float         x    = mp_obj_get_float(args[0]);
	float         y    = mp_obj_get_float(args[1]);
	const char   *text = mp_obj_str_get_str(args[2]);
	// Forward function call.
	pax_vec2f     dims = pax_draw_text(&buf->buf, col, font.font, font.font_size, x, y, text);
	// Pack output.
	mp_obj_t objs[] = {
		mp_obj_new_float(dims.x),
		mp_obj_new_float(dims.y),
	};
	return mp_obj_new_tuple(2, objs);
}

// Text drawing variants.
static mp_obj_t drawStringCentered(mp_uint_t n_args, const mp_obj_t *args) {
	// Grab arguments.
	buf_n_col_t  *buf  = GET_BUF();
	pax_col_t     col  = GET_FILL_COLOR(mp_obj_is_str(*args) ? 5 : 3);
	font_n_size_t font = GET_FONT();
	float         x    = mp_obj_get_float(args[0]);
	float         y    = mp_obj_get_float(args[1]);
	const char   *text = mp_obj_str_get_str(args[2]);
	// Forward function call.
	pax_vec2f     dims = pax_center_text(&buf->buf, col, font.font, font.font_size, x, y, text);
	// Pack output.
	mp_obj_t objs[] = {
		mp_obj_new_float(dims.x),
		mp_obj_new_float(dims.y),
	};
	return mp_obj_new_tuple(2, objs);
}


// Matrix stack push.
static mp_obj_t pushMatrix(mp_uint_t n_args, const mp_obj_t *args) {
	// Grab arguments.
	buf_n_col_t *buf = GET_BUF();
	pax_push_2d(&buf->buf);
	return mp_const_none;
}

// Matrix stack pop.
static mp_obj_t popMatrix(mp_uint_t n_args, const mp_obj_t *args) {
	// Grab arguments.
	buf_n_col_t *buf = GET_BUF();
	pax_pop_2d(&buf->buf);
	return mp_const_none;
}

// Matrix stack clear.
static mp_obj_t clearMatrix(mp_uint_t n_args, const mp_obj_t *args) {
	// Grab arguments.
	buf_n_col_t *buf = GET_BUF();
	if (n_args) {
		pax_reset_2d(&buf->buf, mp_obj_is_true(*args));
	} else {
		pax_reset_2d(&buf->buf, true);
	}
	return mp_const_none;
}


// Scales the current view.
static mp_obj_t scale(mp_uint_t n_args, const mp_obj_t *args) {
	// Grab arguments.
	buf_n_col_t *buf = GET_BUF();
	if (n_args > 1) {
		float x = mp_obj_get_float(args[0]);
		pax_apply_2d(&buf->buf, matrix_2d_scale(x, x));
	} else {
		float x = mp_obj_get_float(args[0]);
		float y = mp_obj_get_float(args[1]);
		pax_apply_2d(&buf->buf, matrix_2d_scale(x, y));
	}
	return mp_const_none;
}

// Moves around the current view.
static mp_obj_t translate(mp_uint_t n_args, const mp_obj_t *args) {
	// Grab arguments.
	buf_n_col_t *buf = GET_BUF();
	float x = mp_obj_get_float(args[0]);
	float y = mp_obj_get_float(args[1]);
	pax_apply_2d(&buf->buf, matrix_2d_translate(x, y));
	return mp_const_none;
}

// Shears the current view.
// Positive X causes the points above the origin to move to the right.
// Positive Y causes the points to the right of the origin to move down.
static mp_obj_t shear(mp_uint_t n_args, const mp_obj_t *args) {
	// Grab arguments.
	buf_n_col_t *buf = GET_BUF();
	float x = mp_obj_get_float(args[0]);
	float y = mp_obj_get_float(args[1]);
	pax_apply_2d(&buf->buf, matrix_2d_shear(x, y));
	return mp_const_none;
}

// Rotates the current view around the origin, angles in radians.
static mp_obj_t rotate(mp_uint_t n_args, const mp_obj_t *args) {
	// Grab arguments.
	buf_n_col_t *buf = GET_BUF();
	float x = mp_obj_get_float(args[0]);
	pax_apply_2d(&buf->buf, matrix_2d_rotate(x));
	return mp_const_none;
}


// Get the color at the given pixel.
static mp_obj_t getPixel(mp_uint_t n_args, const mp_obj_t *args) {
	// Grab arguments.
	buf_n_col_t *buf = GET_BUF();
	int x = mp_obj_get_int(args[0]);
	int y = mp_obj_get_int(args[1]);
	pax_col_t col = pax_get_pixel(&buf->buf, x, y);
	return mp_obj_new_int(col);
}

// Set the color at the given pixel.
static mp_obj_t setPixel(mp_uint_t n_args, const mp_obj_t *args) {
	// Grab arguments.
	buf_n_col_t *buf = GET_BUF();
	pax_col_t col = mp_obj_get_int_truncated(args[0]);
	int x = mp_obj_get_int(args[1]);
	int y = mp_obj_get_int(args[2]);
	pax_set_pixel(&buf->buf, col, x, y);
	pax_mark_dirty1(&buf->buf, x, y);
	return mp_const_none;
}

// Gets the raw pixel data (before color converion) at the given point.
static mp_obj_t getPixelRaw(mp_uint_t n_args, const mp_obj_t *args) {
	// Grab arguments.
	buf_n_col_t *buf = GET_BUF();
	int x = mp_obj_get_int(args[0]);
	int y = mp_obj_get_int(args[1]);
	pax_col_t col = pax_get_pixel_raw(&buf->buf, x, y);
	return mp_obj_new_int(col);
}

// Sets the raw pixel data (before color converion) at the given point.
static mp_obj_t setPixelRaw(mp_uint_t n_args, const mp_obj_t *args) {
	// Grab arguments.
	buf_n_col_t *buf = GET_BUF();
	pax_col_t col = mp_obj_get_int_truncated(args[0]);
	int x = mp_obj_get_int(args[1]);
	int y = mp_obj_get_int(args[2]);
	pax_set_pixel_raw(&buf->buf, col, x, y);
	pax_mark_dirty1(&buf->buf, x, y);
	return mp_const_none;
}

// Overlays the color at the given point (for transparent drawing).
static mp_obj_t mergePixel(mp_uint_t n_args, const mp_obj_t *args) {
	// Grab arguments.
	buf_n_col_t *buf = GET_BUF();
	pax_col_t col = mp_obj_get_int_truncated(args[0]);
	int x = mp_obj_get_int(args[1]);
	int y = mp_obj_get_int(args[2]);
	pax_merge_pixel(&buf->buf, col, x, y);
	pax_mark_dirty1(&buf->buf, x, y);
	return mp_const_none;
}


// Whether there has been drawing since the last flush call.
static mp_obj_t isDirty(mp_uint_t n_args, const mp_obj_t *args) {
	// Grab arguments.
	buf_n_col_t *buf = GET_BUF();
	return mp_obj_new_bool(pax_is_dirty(&buf->buf));
}

// Gets the rectangle in which it is dirty.
static mp_obj_t getDirtyRect(mp_uint_t n_args, const mp_obj_t *args) {
	// Grab arguments.
	buf_n_col_t *buf = GET_BUF();
	pax_recti dirty = pax_get_dirty(&buf->buf);
	mp_obj_t objs[] = {
		mp_obj_new_float(dirty.x),
		mp_obj_new_float(dirty.y),
		mp_obj_new_float(dirty.w),
		mp_obj_new_float(dirty.h),
	};
	return mp_obj_new_tuple(4, objs);
}


// Apply a clip rectangle to the buffer.
// Anything outside of the clip will not be drawn.
// This is an operation that ignores matrix transforms (translate, rotate, etc.).
static mp_obj_t clip(mp_uint_t n_args, const mp_obj_t *args) {
	// Grab arguments.
	buf_n_col_t *buf = GET_BUF();
	float x = mp_obj_get_float(args[0]);
	float y = mp_obj_get_float(args[1]);
	float w = mp_obj_get_float(args[2]);
	float h = mp_obj_get_float(args[3]);
	pax_clip(&buf->buf, x, y, w, h);
	return mp_const_none;
}

// Disable clipping.
// Any effects of previous clip calls are nullified.
static mp_obj_t noClip(mp_uint_t n_args, const mp_obj_t *args) {
	// Grab arguments.
	buf_n_col_t *buf = GET_BUF();
	pax_noclip(&buf->buf);
	return mp_const_none;
}

// Obtain a copy of the current clip rect.
static mp_obj_t getClipRect(mp_uint_t n_args, const mp_obj_t *args) {
	// Grab arguments.
	buf_n_col_t *buf = GET_BUF();
	pax_recti clip = pax_get_clip(&buf->buf);
	mp_obj_t objs[] = {
		mp_obj_new_float(clip.x),
		mp_obj_new_float(clip.y),
		mp_obj_new_float(clip.w),
		mp_obj_new_float(clip.h),
	};
	return mp_obj_new_tuple(4, objs);
}



/* ==== OBJECT DEFINITIONS ==== */
static MP_DEFINE_CONST_FUN_OBJ_VAR(pax2py_debug_obj, 0, pax2py_debug);

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(fillColor_obj,			0, 1, fillColor);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lineColor_obj,			0, 1, lineColor);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(drawRect_obj,			4, 5, drawRect);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(outlineRect_obj,			4, 5, outlineRect);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(drawTri_obj,				6, 7, drawTri);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(outlineTri_obj,			6, 7, outlineTri);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(drawCircle_obj,			3, 4, drawCircle);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(outlineCircle_obj,		3, 4, outlineCircle);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(drawArc_obj,				5, 6, drawArc);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(outlineArc_obj,			5, 6, outlineArc);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(drawLine_obj,			4, 5, drawLine);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(stringSize_obj,			1, 3, stringSize);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(drawString_obj,			3, 6, drawString);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(drawStringCentered_obj,	3, 6, drawStringCentered);

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pushMatrix_obj,			0, 0, pushMatrix);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(popMatrix_obj,			0, 0, popMatrix);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(clearMatrix_obj,			0, 1, clearMatrix);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(scale_obj,				1, 2, scale);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(translate_obj,			2, 2, translate);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(shear_obj,				2, 2, shear);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(rotate_obj,				1, 1, rotate);

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(getPixel_obj,			2, 2, getPixel);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(setPixel_obj,			3, 3, setPixel);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(getPixelRaw_obj,			2, 2, getPixelRaw);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(setPixelRaw_obj,			3, 3, setPixelRaw);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mergePixel_obj,			3, 3, mergePixel);

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(isDirty_obj,				0, 0, isDirty);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(getDirtyRect_obj,		0, 0, getDirtyRect);

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(clip_obj,				4, 4, clip);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(noClip_obj,				0, 0, noClip);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(getClipRect_obj,			0, 0, getClipRect);

// static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(_obj,	3, 6, _______);



/* ==== MODULE DEFINITION ==== */
static const mp_rom_map_elem_t pax_module_globals_table[] = {
	{MP_ROM_QSTR(MP_QSTR_debug),				MP_ROM_PTR(&pax2py_debug_obj)},
	{MP_ROM_QSTR(MP_QSTR_fillColor),			MP_ROM_PTR(&fillColor_obj)},
	{MP_ROM_QSTR(MP_QSTR_fillColour),			MP_ROM_PTR(&fillColor_obj)},
	{MP_ROM_QSTR(MP_QSTR_lineColor),			MP_ROM_PTR(&lineColor_obj)},
	{MP_ROM_QSTR(MP_QSTR_lineColour),			MP_ROM_PTR(&lineColor_obj)},
	{MP_ROM_QSTR(MP_QSTR_drawRect),				MP_ROM_PTR(&drawRect_obj)},
	{MP_ROM_QSTR(MP_QSTR_drawRectangle),		MP_ROM_PTR(&drawRect_obj)},
	{MP_ROM_QSTR(MP_QSTR_outlineRect),			MP_ROM_PTR(&outlineRect_obj)},
	{MP_ROM_QSTR(MP_QSTR_outlineRectangle),		MP_ROM_PTR(&outlineRect_obj)},
	{MP_ROM_QSTR(MP_QSTR_drawTri),				MP_ROM_PTR(&drawTri_obj)},
	{MP_ROM_QSTR(MP_QSTR_drawTriangle),			MP_ROM_PTR(&drawTri_obj)},
	{MP_ROM_QSTR(MP_QSTR_outlineTri),			MP_ROM_PTR(&outlineTri_obj)},
	{MP_ROM_QSTR(MP_QSTR_outlineTriangle),		MP_ROM_PTR(&outlineTri_obj)},
	{MP_ROM_QSTR(MP_QSTR_drawCircle),			MP_ROM_PTR(&drawCircle_obj)},
	{MP_ROM_QSTR(MP_QSTR_outlineCircle),		MP_ROM_PTR(&outlineCircle_obj)},
	{MP_ROM_QSTR(MP_QSTR_drawArc),				MP_ROM_PTR(&drawArc_obj)},
	{MP_ROM_QSTR(MP_QSTR_outlineArc),			MP_ROM_PTR(&outlineArc_obj)},
	{MP_ROM_QSTR(MP_QSTR_drawLine),				MP_ROM_PTR(&drawLine_obj)},
	{MP_ROM_QSTR(MP_QSTR_stringSize),			MP_ROM_PTR(&stringSize_obj)},
	{MP_ROM_QSTR(MP_QSTR_drawString),			MP_ROM_PTR(&drawString_obj)},
	{MP_ROM_QSTR(MP_QSTR_drawStringCentered),	MP_ROM_PTR(&drawStringCentered_obj)},
	{MP_ROM_QSTR(MP_QSTR_pushMatrix),			MP_ROM_PTR(&pushMatrix_obj)},
	{MP_ROM_QSTR(MP_QSTR_popMatrix),			MP_ROM_PTR(&popMatrix_obj)},
	{MP_ROM_QSTR(MP_QSTR_clearMatrix),			MP_ROM_PTR(&clearMatrix_obj)},
	{MP_ROM_QSTR(MP_QSTR_scale),				MP_ROM_PTR(&scale_obj)},
	{MP_ROM_QSTR(MP_QSTR_translate),			MP_ROM_PTR(&translate_obj)},
	{MP_ROM_QSTR(MP_QSTR_shear),				MP_ROM_PTR(&shear_obj)},
	{MP_ROM_QSTR(MP_QSTR_rotate),				MP_ROM_PTR(&rotate_obj)},
	{MP_ROM_QSTR(MP_QSTR_getPixel),				MP_ROM_PTR(&getPixel_obj)},
	{MP_ROM_QSTR(MP_QSTR_setPixel),				MP_ROM_PTR(&setPixel_obj)},
	{MP_ROM_QSTR(MP_QSTR_getPixelRaw),			MP_ROM_PTR(&getPixelRaw_obj)},
	{MP_ROM_QSTR(MP_QSTR_setPixelRaw),			MP_ROM_PTR(&setPixelRaw_obj)},
	{MP_ROM_QSTR(MP_QSTR_mergePixel),			MP_ROM_PTR(&mergePixel_obj)},
	{MP_ROM_QSTR(MP_QSTR_isDirty),				MP_ROM_PTR(&isDirty_obj)},
	{MP_ROM_QSTR(MP_QSTR_getDirtyRect),			MP_ROM_PTR(&getDirtyRect_obj)},
	{MP_ROM_QSTR(MP_QSTR_clip),					MP_ROM_PTR(&clip_obj)},
	{MP_ROM_QSTR(MP_QSTR_noClip),				MP_ROM_PTR(&noClip_obj)},
	{MP_ROM_QSTR(MP_QSTR_getClipRect),			MP_ROM_PTR(&getClipRect_obj)},
	// {MP_ROM_QSTR(MP_QSTR_),	MP_ROM_PTR(&_obj)},
};

static MP_DEFINE_CONST_DICT(pax_module_globals, pax_module_globals_table);

const mp_obj_module_t pax_module = {
	.base = {&mp_type_module},
	.globals = (mp_obj_dict_t *)&pax_module_globals,
};



/* ==== MODULE INITIALISER ==== */
#ifdef CONFIG_DRIVER_PAX_COMPAT
extern void driver_framebuffer_set_dirty_area(int16_t x0, int16_t y0, int16_t x1, int16_t y1, bool force);
#endif
#ifdef CONFIG_DRIVER_FRAMEBUFFER_ENABLE
void driver_framebuffer_pre_flush_callback() {
#ifdef CONFIG_DRIVER_PAX_COMPAT
	#if defined(CONFIG_DRIVER_PAX_MCR)
	pax_join();
	#endif
	// pax_recti dirty = pax_get_dirty(&global_pax_buf.buf);
	// if (pax_is_dirty(&global_pax_buf.buf)) {
	// 	driver_framebuffer_set_dirty_area(
	// 		dirty.x,
	// 		dirty.y,
	// 		dirty.x + dirty.w - 1,
	// 		dirty.y + dirty.h - 1,
	// 		false
	// 	);
	// }
	pax_mark_clean(&global_pax_buf.buf);
#endif
}
#endif

extern esp_err_t driver_framebuffer_init();
esp_err_t driver_pax2py_init() {
	#ifdef CONFIG_DRIVER_PAX_COMPAT
	// Get framebuffer from the other place(tm).
	driver_framebuffer_init();
	#endif
	
	// Create global PAX context.
	pax_buf_type_t type = PAX2PY_BUF_NATIVE;
	pax_buf_init(
		&global_pax_buf.buf,
	#ifdef CONFIG_DRIVER_PAX_COMPAT
		framebuffer,
	#else
		NULL,
	#endif
		CONFIG_DRIVER_PAX_FBWIDTH, CONFIG_DRIVER_PAX_FBHEIGHT,
		type
	);
	global_pax_buf.fill_color = 0xffffffff;
	global_pax_buf.line_color = 0xffffffff;
	
	// Apply settings.
	#if   defined(CONFIG_DRIVER_PAX_ROT_1)
	pax_buf_set_rotation(&global_pax_buf.buf, 1);
	#elif defined(CONFIG_DRIVER_PAX_ROT_2)
	pax_buf_set_rotation(&global_pax_buf.buf, 2);
	#elif defined(CONFIG_DRIVER_PAX_ROT_3)
	pax_buf_set_rotation(&global_pax_buf.buf, 3);
	#endif
	
	#ifdef CONFIG_DRIVER_PAX_REVERSE_ENDIANNESS
	pax_buf_reversed(&global_pax_buf.buf, true);
	#endif
	
	#if defined(CONFIG_DRIVER_PAX_MCR_OPT)
	pax_enable_multicore(1);
	#endif
	
	return 0;
}

#endif // CONFIG_DRIVER_PAX_ENABLE
