/*
 * BADGE.TEAM PAX driver
 * Implements the PAX API for BadgePython.
 * Julian Scheffers 2023
 */

#include <sdkconfig.h>

#ifdef CONFIG_DRIVER_PAX_ENABLE

#ifndef NO_QSTR
#include <pax_gfx.h>
#include <pax_codecs.h>
#include "modpax.h"
#include "fonts.h"
#endif

#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "py/objarray.h"

#include "vfs.h"
#include "vfs_native.h"

#include <string.h>

#ifdef CONFIG_DRIVER_PAX_COMPAT
// Imported framebuffer from legacy framebuffer driver.
extern uint8_t *framebuffer;
#else
// Statically allocated framebuffer.
static uint8_t *framebuffer;
#endif

/* ==== TYPEDEFS ==== */
// Holder for pax_buf_t and pax_col_t.
typedef struct {
	pax_buf_t buf;
	pax_col_t fill_color, line_color;
} buf_n_col_t;

// Holder for pax_font_t and float.
typedef struct {
	const pax_font_t *font;
	float font_size;
} font_n_size_t;

// Lookup table from buffer definitions to pax_buf_type_t.
static const pax_buf_type_t buf_type_lut[] = {
	PAX_BUF_1_PAL,
	PAX_BUF_2_PAL,
	PAX_BUF_4_PAL,
	PAX_BUF_8_PAL,
	PAX_BUF_16_PAL,
	
	PAX_BUF_1_GREY,
	PAX_BUF_2_GREY,
	PAX_BUF_4_GREY,
	PAX_BUF_8_GREY,
	
	PAX_BUF_8_332RGB,
	PAX_BUF_16_565RGB,
	
	PAX_BUF_4_1111ARGB,
	PAX_BUF_8_2222ARGB,
	PAX_BUF_16_4444ARGB,
	PAX_BUF_32_8888ARGB,
};
static const size_t buf_type_lut_len = sizeof(buf_type_lut) / sizeof(pax_buf_type_t);

// Data holder(tm) for Buffer class.
typedef struct {
	mp_obj_base_t base;
	buf_n_col_t   ctx;
} Buffer_obj_t;

// Type for Buffer class.
extern const mp_obj_type_t Buffer_type;

static const mp_rom_map_elem_t Buffer_locals_table[];
static const mp_rom_map_elem_t pax_module_globals_table[];

// Global buffer instance.
buf_n_col_t global_pax_buf;

/* ==== HELPER FUNCTIONS ==== */
// Is this the pax module?
static bool is_pax_module(mp_const_obj_t obj) {
	return mp_obj_equal(obj, MP_OBJ_FROM_PTR(&pax_module));
}

// Get buffer to use on some operation (assuming buffer is first arg).
static buf_n_col_t *get_buf(mp_uint_t *n_args, const mp_obj_t **args) {
	if (!*n_args) return &global_pax_buf;
	// Check for buffer class.
	if (mp_obj_is_obj(**args) && mp_obj_get_type(**args) == &Buffer_type) {
		Buffer_obj_t *self = MP_OBJ_TO_PTR(**args);
		--*n_args;
		++*args;
		return &self->ctx;
	} else {
		if (is_pax_module(**args)) {
			// Passed in the pax, CONSUME!
			--*n_args;
			++*args;
		}
		return &global_pax_buf;
	}
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
		if (!out.font) {
			mp_raise_ValueError("Unkonwn font");
		}
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



/* ==== CLASS FUNCTION DEFINITIONS ==== */
// Constructor for Buffer class.
static mp_obj_t Buffer_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
	// Create a BOX to hold the buffer.
	mp_arg_check_num(n_args, n_kw, 2, 3, true);
	Buffer_obj_t *self = m_new_obj(Buffer_obj_t);
	self->base.type = &Buffer_type;
	
	// Grab arguments.
	int w = mp_obj_get_int(args[0]);
	int h = mp_obj_get_int(args[1]);
	pax_buf_type_t buf_type;
	if (n_args == 3) {
		int raw_type = mp_obj_get_int(args[2]);
		if (raw_type < 0 || raw_type >= buf_type_lut_len) {
			mp_raise_ValueError("Not a valid buffer type");
		}
		buf_type = buf_type_lut[raw_type];
	} else {
		buf_type = PAX2PY_BUF_NATIVE;
	}
	
	// Initialise the buffer.
	pax_buf_init(&self->ctx.buf, NULL, w, h, buf_type);
	pax_background(&self->ctx.buf, 0x00000000);
	self->ctx.fill_color = 0xffffffff;
	self->ctx.line_color = 0xffffffff;
	
	// Forward the BOX.
	return MP_OBJ_FROM_PTR(self);
}

// Destructor for Buffer class.
static mp_obj_t Buffer_del(mp_uint_t n_args, const mp_obj_t *args) {
	Buffer_obj_t *self = MP_OBJ_TO_PTR(args[0]);
	
	// Destroy PAX ctx.
	pax_join();
	pax_buf_destroy(&self->ctx.buf);
	
	return mp_const_none;
}

// Printener for Buffer class.
static void Buffer_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
	Buffer_obj_t *self = MP_OBJ_TO_PTR(self_in);
	int w = pax_buf_get_width (&self->ctx.buf);
	int h = pax_buf_get_height(&self->ctx.buf);
	
	mp_print_str(print, "Buffer(");
	mp_obj_print_helper(print, mp_obj_new_int(w), PRINT_REPR);
	mp_print_str(print, ", ");
	mp_obj_print_helper(print, mp_obj_new_int(h), PRINT_REPR);
	mp_print_str(print, ")");
}

#ifdef CONFIG_DRIVER_PAX_EXPERIMENTAL
// Encodes a PNG file or bytes object from a Buffer.
static mp_obj_t encodePNG(mp_uint_t n_args, const mp_obj_t *args) {
	buf_n_col_t *buf = GET_BUF();
	
	if (n_args == 0 || n_args == 4) {
		// Get the WINDOW.
		int x, y, w, h;
		if (n_args == 4) {
			x = mp_obj_get_int(args[0]);
			y = mp_obj_get_int(args[1]);
			w = mp_obj_get_int(args[2]);
			h = mp_obj_get_int(args[3]);
		} else {
			x = 0;
			y = 0;
			w = pax_buf_get_width(&buf->buf);
			h = pax_buf_get_height(&buf->buf);
		}
		
		// Encode a PNG bytes.
		size_t pnglen;
		void  *pngbuf;
		bool res = pax_encode_png_buf(&buf->buf, &pngbuf, &pnglen, x, y, w, h);
		if (!res) {
			mp_raise_ValueError("Error while encoding PNG data");
		}
		
		// Unfortunately, micropyhon insists on copying the data.
		mp_obj_t bytes = mp_obj_new_bytes(pngbuf, pnglen);
		free(pngbuf);
		return bytes;
		
	} else if (n_args == 1 || n_args == 5) {
		// Convert given path to physical path.
		const char *path = mp_obj_str_get_str(*args);
		char fullname[128] = {'\0'};
		int res = physicalPathN(path, fullname, sizeof(fullname));
		if (res) {
			mp_raise_msg_varg(&mp_type_OSError, "Error opening %s: %s", path, strerror(errno));
		}
		
		// Open file for writing.
		FILE *fd = fopen(fullname, "wb");
		if (!fd) {
			mp_raise_msg_varg(&mp_type_OSError, "Error opening %s: %s", fullname, strerror(errno));
		}
		
		// Get the WINDOW.
		int x, y, w, h;
		if (n_args == 5) {
			x = mp_obj_get_int(args[1]);
			y = mp_obj_get_int(args[2]);
			w = mp_obj_get_int(args[3]);
			h = mp_obj_get_int(args[4]);
		} else {
			x = 0;
			y = 0;
			w = pax_buf_get_width(&buf->buf);
			h = pax_buf_get_height(&buf->buf);
		}
		
		// Encode a PNG file.
		res = pax_encode_png_fd(&buf->buf, fd, x, y, w, h);
		fclose(fd);
		
		if (res) {
			mp_raise_ValueError(pax_desc_err(pax_last_error));
		} else {
			return mp_const_none;
		}
		
	} else {
		mp_raise_ValueError("Expected 0 to 5 arguments: (buffer), (path), (x, y, width, height)");
	}
}
#endif // CONFIG_DRIVER_PAX_EXPERIMENTAL



/* ==== GLOBAL FUNCTION DEFINITIONS ==== */
// Get a list of all available fonts.
static mp_obj_t fontList(mp_uint_t n_args, const mp_obj_t *args) {
	mp_obj_t objs[pax_n_fonts];
	for (size_t i = 0; i < pax_n_fonts; i++) {
		objs[i] = mp_obj_new_str(pax_fonts_index[i]->name, strlen(pax_fonts_index[i]->name));
	}
	return mp_obj_new_list(pax_n_fonts, objs);
}

// Get info about some font.
static mp_obj_t fontInfo(mp_uint_t n_args, const mp_obj_t *args) {
	// Look up the font.
	const char *name = mp_obj_str_get_str(*args);
	const pax_font_t *font = pax_get_font(name);
	int detail = n_args == 2 ? mp_obj_get_int(args[1]) : 0;
	if (!font) {
		mp_raise_ValueError("Unkonwn font");
	}
	
	// Collect generic infos.
	mp_obj_t info = mp_obj_new_dict(4);
	mp_obj_dict_store(info, MP_OBJ_NEW_QSTR(MP_QSTR_name), mp_obj_new_str(font->name, strlen(font->name)));
	mp_obj_dict_store(info, MP_OBJ_NEW_QSTR(MP_QSTR_defaultSize), mp_obj_new_int(font->default_size));
	mp_obj_dict_store(info, MP_OBJ_NEW_QSTR(MP_QSTR_antialiased), mp_obj_new_bool(font->recommend_aa));
	
	// Collect range infos.
	if (detail >= 1) {
		mp_obj_t ranges = mp_obj_new_list(0, NULL);
		for (size_t i = 0; i < font->n_ranges; i++) {
			const pax_font_range_t *range = &font->ranges[i];
			mp_obj_t rinfo = mp_obj_new_dict(7);
			
			// Set generic range infos.
			bool mono = range->type == PAX_FONT_TYPE_BITMAP_MONO;
			mp_obj_dict_store(rinfo, MP_OBJ_NEW_QSTR(MP_QSTR_monospace), mp_obj_new_bool(mono));
			mp_obj_dict_store(rinfo, MP_OBJ_NEW_QSTR(MP_QSTR_start), mp_obj_new_int(range->start));
			mp_obj_dict_store(rinfo, MP_OBJ_NEW_QSTR(MP_QSTR_end), mp_obj_new_int(range->end));
			
			// Set detailed range infos.
			if (mono && detail >= 2) {
				mp_obj_dict_store(rinfo, MP_OBJ_NEW_QSTR(MP_QSTR_width), mp_obj_new_int(range->bitmap_mono.width));
				mp_obj_dict_store(rinfo, MP_OBJ_NEW_QSTR(MP_QSTR_height), mp_obj_new_int(range->bitmap_mono.height));
				mp_obj_dict_store(rinfo, MP_OBJ_NEW_QSTR(MP_QSTR_bpp), mp_obj_new_int(range->bitmap_mono.bpp));
				
			} else if (detail >= 2) {
				// Grab individual widths.
				mp_obj_t widths = mp_obj_new_list(0, NULL);
				for (size_t i = 0; i < range->end - range->start + 1; i++) {
					mp_obj_list_append(widths, mp_obj_new_int(range->bitmap_var.dims[i].measured_width));
				}
				
				mp_obj_dict_store(rinfo, MP_OBJ_NEW_QSTR(MP_QSTR_widths), widths);
				mp_obj_dict_store(rinfo, MP_OBJ_NEW_QSTR(MP_QSTR_height), mp_obj_new_int(range->bitmap_var.height));
				mp_obj_dict_store(rinfo, MP_OBJ_NEW_QSTR(MP_QSTR_bpp), mp_obj_new_int(range->bitmap_var.bpp));
			}
			mp_obj_list_append(ranges, rinfo);
		}
		mp_obj_dict_store(info, MP_OBJ_NEW_QSTR(MP_QSTR_ranges), ranges);
	}
	
	return info;
}

#ifdef CONFIG_DRIVER_PAX_COMPAT
extern bool driver_framebuffer_flush(uint32_t flags);
static mp_obj_t flush(mp_uint_t n_args, const mp_obj_t *args) {
	int flags = n_args ? mp_obj_get_int_truncated(*args) : 0;
	driver_framebuffer_flush(flags);
	return mp_const_none;
}
#else
extern esp_err_t driver_ili9341_write_partial(const uint8_t *buffer, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
// #include "driver_ili9341.h"
mp_obj_t pax2py_flush(mp_uint_t n_args, const mp_obj_t *args) {
	int flags = n_args ? mp_obj_get_int_truncated(*args) : 0;
	
	if (flags & PAX2PY_FLAG_FORCE) {
		pax_mark_dirty0(&global_pax_buf.buf);
	}
	
	// Check dirty area.
	if (!pax_is_dirty(&global_pax_buf.buf)) return mp_const_none;
	if (flags & PAX2PY_FLAG_FULL) {
		pax_mark_dirty0(&global_pax_buf.buf);
	}
	pax_recti dirty = pax_get_dirty(&global_pax_buf.buf);
	
	// Flush the thingy.
	driver_ili9341_write_partial(framebuffer, dirty.x, dirty.y, dirty.x+dirty.w-1, dirty.y+dirty.h-1);
	return mp_const_none;
}
#endif

// Decodes a PNG file or bytes object into a Buffer.
static mp_obj_t decodePNG(mp_uint_t n_args, const mp_obj_t *args) {
	pax_buf_t decd;
	
	if (mp_obj_is_str(*args)) {
		// Convert given path to physical path.
		const char *path = mp_obj_str_get_str(*args);
		char fullname[128] = {'\0'};
		int res = physicalPathN(path, fullname, sizeof(fullname));
		if (res) {
			mp_raise_msg_varg(&mp_type_OSError, "Error opening %s: %s", path, strerror(errno));
		}
		
		// Open file for reading.
		FILE *fd = fopen(fullname, "rb");
		if (!fd) {
			mp_raise_msg_varg(&mp_type_ValueError, "Error opening %s: %s", fullname, strerror(errno));
		}
		
		// Decode PNG.
		res = pax_decode_png_fd(&decd, fd, PAX_BUF_16_4444ARGB, 0);
		fclose(fd);
		
		// Error handling.
		if (!res) {
			if (pax_last_error == PAX_ERR_NOMEM) {
				mp_raise_msg(&mp_type_MemoryError, "Out of memory");
			} else {
				mp_raise_ValueError("Error reading PNG data");
			}
		}
		
	} else if (mp_obj_get_type(*args) == &mp_type_bytes) {
		// Get raw data.
		size_t len = 0;
		const char *data = mp_obj_str_get_data(*args, &len);
		
		// Decode PNG.
		int res = pax_decode_png_buf(&decd, data, len, PAX_BUF_16_4444ARGB, 0);
		
		// Error handling.
		if (!res) {
			if (pax_last_error == PAX_ERR_NOMEM) {
				mp_raise_msg(&mp_type_MemoryError, "Out of memory");
			} else {
				mp_raise_ValueError("Error reading PNG data");
			}
		}
	}
	
	// Create a BOX to hold the buffer.
	Buffer_obj_t *self = m_new_obj_maybe(Buffer_obj_t);
	if (!self) {
		pax_buf_destroy(&decd);
		mp_raise_msg(&mp_type_MemoryError, "Out of memory");
	}
	self->base.type = &Buffer_type;
	
	// Hand over buffer data.
	self->ctx.buf        = decd;
	self->ctx.fill_color = 0xffffffff;
	self->ctx.line_color = 0xffffffff;
	
	// Forward the BOX.
	return MP_OBJ_FROM_PTR(self);
}

// Gets info about some sort of PNG.
static mp_obj_t infoPNG(mp_uint_t n_args, const mp_obj_t *args) {
	pax_png_info_t info;
	
	if (mp_obj_is_str(*args)) {
		// Convert given path to physical path.
		const char *path = mp_obj_str_get_str(*args);
		char fullname[128] = {'\0'};
		int res = physicalPathN(path, fullname, sizeof(fullname));
		if (res) {
			mp_raise_msg_varg(&mp_type_OSError, "Error opening %s: %s", path, strerror(errno));
		}
		
		// Open file for reading.
		FILE *fd = fopen(fullname, "rb");
		if (!fd) {
			mp_raise_msg_varg(&mp_type_ValueError, "Error opening %s: %s", fullname, strerror(errno));
		}
		
		// Decode PNG.
		res = pax_info_png_fd(&info, fd);
		fclose(fd);
		
		// Error handling.
		if (!res) {
			mp_raise_ValueError("Error reading PNG data");
		}
		
	} else if (mp_obj_get_type(*args) == &mp_type_bytes) {
		// Get raw data.
		size_t len = 0;
		const char *data = mp_obj_str_get_data(*args, &len);
		
		// Decode PNG.
		int res = pax_info_png_buf(&info, data, len);
		
		// Error handling.
		if (!res) {
			mp_raise_ValueError("Error reading PNG data");
		}
	}
	
	// Pack infos.
	mp_obj_t dict = mp_obj_new_dict(4);
	mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_width),      info.width);
	mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_height),     info.height);
	mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_bit_depth),  info.bit_depth);
	mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_color_type), info.color_type);
	return dict;
}



/* ==== COMMON FUNCTION DEFINITIONS ==== */
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


// Get buf width.
static mp_obj_t getWidth(mp_uint_t n_args, const mp_obj_t *args) {
	buf_n_col_t *buf = GET_BUF();
	return mp_obj_new_int(pax_buf_get_width(&buf->buf));
}

// Get buf height.
static mp_obj_t getHeight(mp_uint_t n_args, const mp_obj_t *args) {
	buf_n_col_t *buf = GET_BUF();
	return mp_obj_new_int(pax_buf_get_height(&buf->buf));
}

// Get buf type.
static mp_obj_t getType(mp_uint_t n_args, const mp_obj_t *args) {
	buf_n_col_t *buf = GET_BUF();
	return mp_obj_new_int(pax_buf_get_type(&buf->buf));
}


// Background fill.
static mp_obj_t background(mp_uint_t n_args, const mp_obj_t *args) {
	// Grab arguments.
	buf_n_col_t *buf = GET_BUF();
	pax_col_t  color = mp_obj_get_int_truncated(*args);
	// Forward function call.
	pax_background(&buf->buf, color);
	return mp_const_none;
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

// Rectangle drawing variants.
static mp_obj_t drawRoundRect(mp_uint_t n_args, const mp_obj_t *args) {
	// Grab arguments.
	buf_n_col_t *buf = GET_BUF();
	pax_col_t  color = GET_FILL_COLOR(5);
	float      x     = mp_obj_get_float(args[0]);
	float      y     = mp_obj_get_float(args[1]);
	float      w     = mp_obj_get_float(args[2]);
	float      h     = mp_obj_get_float(args[3]);
	float      r     = mp_obj_get_float(args[4]);
	// Forward function call.
	pax_draw_round_rect(&buf->buf, color, x, y, w, h, r);
	return mp_const_none;
}

// Rectangle drawing variants.
static mp_obj_t outlineRoundRect(mp_uint_t n_args, const mp_obj_t *args) {
	// Grab arguments.
	buf_n_col_t *buf = GET_BUF();
	pax_col_t  color = GET_FILL_COLOR(5);
	float      x     = mp_obj_get_float(args[0]);
	float      y     = mp_obj_get_float(args[1]);
	float      w     = mp_obj_get_float(args[2]);
	float      h     = mp_obj_get_float(args[3]);
	float      r     = mp_obj_get_float(args[4]);
	// Forward function call.
	pax_outline_round_rect(&buf->buf, color, x, y, w, h, r);
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
static mp_obj_t drawHollowCircle(mp_uint_t n_args, const mp_obj_t *args) {
	// Grab arguments.
	buf_n_col_t *buf = GET_BUF();
	pax_col_t  color = GET_FILL_COLOR(4);
	float      x     = mp_obj_get_float(args[0]);
	float      y     = mp_obj_get_float(args[1]);
	float      r0    = mp_obj_get_float(args[2]);
	float      r1    = mp_obj_get_float(args[3]);
	// Forward function call.
	pax_draw_hollow_circle(&buf->buf, color, x, y, r0, r1);
	return mp_const_none;
}

// Circle drawing variants.
static mp_obj_t outlineHollowCircle(mp_uint_t n_args, const mp_obj_t *args) {
	// Grab arguments.
	buf_n_col_t *buf = GET_BUF();
	pax_col_t  color = GET_FILL_COLOR(4);
	float      x     = mp_obj_get_float(args[0]);
	float      y     = mp_obj_get_float(args[1]);
	float      r0    = mp_obj_get_float(args[2]);
	float      r1    = mp_obj_get_float(args[3]);
	// Forward function call.
	pax_outline_hollow_circle(&buf->buf, color, x, y, r0, r1);
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

// Circle drawing variants.
static mp_obj_t drawHollowArc(mp_uint_t n_args, const mp_obj_t *args) {
	// Grab arguments.
	buf_n_col_t *buf = GET_BUF();
	pax_col_t  color = GET_FILL_COLOR(6);
	float      x     = mp_obj_get_float(args[0]);
	float      y     = mp_obj_get_float(args[1]);
	float      r0    = mp_obj_get_float(args[2]);
	float      r1    = mp_obj_get_float(args[3]);
	float      a0    = mp_obj_get_float(args[4]);
	float      a1    = mp_obj_get_float(args[5]);
	// Forward function call.
	pax_draw_hollow_arc(&buf->buf, color, x, y, r0, r1, a0, a1);
	return mp_const_none;
}

// Arc drawing variants.
static mp_obj_t outlineHollowArc(mp_uint_t n_args, const mp_obj_t *args) {
	// Grab arguments.
	buf_n_col_t *buf = GET_BUF();
	pax_col_t  color = GET_FILL_COLOR(6);
	float      x     = mp_obj_get_float(args[0]);
	float      y     = mp_obj_get_float(args[1]);
	float      r0    = mp_obj_get_float(args[2]);
	float      r1    = mp_obj_get_float(args[3]);
	float      a0    = mp_obj_get_float(args[4]);
	float      a1    = mp_obj_get_float(args[5]);
	// Forward function call.
	pax_outline_hollow_arc(&buf->buf, color, x, y, r0, r1, a0, a1);
	return mp_const_none;
}

// Circle drawing variants.
static mp_obj_t drawRoundHollowArc(mp_uint_t n_args, const mp_obj_t *args) {
	// Grab arguments.
	buf_n_col_t *buf = GET_BUF();
	pax_col_t  color = GET_FILL_COLOR(6);
	float      x     = mp_obj_get_float(args[0]);
	float      y     = mp_obj_get_float(args[1]);
	float      r0    = mp_obj_get_float(args[2]);
	float      r1    = mp_obj_get_float(args[3]);
	float      a0    = mp_obj_get_float(args[4]);
	float      a1    = mp_obj_get_float(args[5]);
	// Forward function call.
	pax_draw_round_hollow_arc(&buf->buf, color, x, y, r0, r1, a0, a1);
	return mp_const_none;
}

// Arc drawing variants.
static mp_obj_t outlineRoundHollowArc(mp_uint_t n_args, const mp_obj_t *args) {
	// Grab arguments.
	buf_n_col_t *buf = GET_BUF();
	pax_col_t  color = GET_FILL_COLOR(6);
	float      x     = mp_obj_get_float(args[0]);
	float      y     = mp_obj_get_float(args[1]);
	float      r0    = mp_obj_get_float(args[2]);
	float      r1    = mp_obj_get_float(args[3]);
	float      a0    = mp_obj_get_float(args[4]);
	float      a1    = mp_obj_get_float(args[5]);
	// Forward function call.
	pax_outline_round_hollow_arc(&buf->buf, color, x, y, r0, r1, a0, a1);
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



// Draw image variants.
static mp_obj_t drawImage(mp_uint_t n_args, const mp_obj_t *args) {
	pax_buf_t *dst, *src;
	
	// Grab buffer #1.
	if (is_pax_module(*args)) {
		dst = &global_pax_buf.buf;
	} else {
		if (!mp_obj_is_obj(args[0]) || mp_obj_get_type(args[0]) != &Buffer_type) {
			mp_raise_TypeError("Expected pax.Buffer type");
		}
		dst = &((Buffer_obj_t *) MP_OBJ_TO_PTR(args[0]))->ctx.buf;
	}
	--n_args;
	++args;
	
	// Grab optional buffer #2.
	if (is_pax_module(*args)) {
		src = &global_pax_buf.buf;
		--n_args;
		++args;
	} else if (mp_obj_is_obj(args[0]) && mp_obj_get_type(args[0]) == &Buffer_type) {
		src = &((Buffer_obj_t *) MP_OBJ_TO_PTR(args[0]))->ctx.buf;
		--n_args;
		++args;
	} else {
		src = dst;
		dst = &global_pax_buf.buf;
	}
	
	// Required args.
	float x = mp_obj_get_float(args[0]);
	float y = mp_obj_get_float(args[1]);
	// Optional args.
	float w, h;
	if (n_args == 4) {
		w = mp_obj_get_float(args[2]);
		h = mp_obj_get_float(args[3]);
	} else {
		w = pax_buf_get_widthf (src);
		h = pax_buf_get_heightf(src);
	}
	
	// Forward function call.
	pax_draw_image_sized(dst, src, x, y, w, h);
	return mp_const_none;
}

// Draw image variants.
static mp_obj_t drawImageOpaque(mp_uint_t n_args, const mp_obj_t *args) {
	pax_buf_t *dst, *src;
	
	// Grab buffer #1.
	if (is_pax_module(*args)) {
		dst = &global_pax_buf.buf;
	} else {
		if (!mp_obj_is_obj(args[0]) || mp_obj_get_type(args[0]) != &Buffer_type) {
			mp_raise_TypeError("Expected pax.Buffer type");
		}
		dst = &((Buffer_obj_t *) MP_OBJ_TO_PTR(args[0]))->ctx.buf;
	}
	dst = &((Buffer_obj_t *) MP_OBJ_TO_PTR(args[0]))->ctx.buf;
	--n_args;
	++args;
	
	// Grab optional buffer #2.
	if (is_pax_module(*args)) {
		src = &global_pax_buf.buf;
		--n_args;
		++args;
	} else if (mp_obj_is_obj(args[0]) && mp_obj_get_type(args[0]) == &Buffer_type) {
		src = &((Buffer_obj_t *) MP_OBJ_TO_PTR(args[0]))->ctx.buf;
		--n_args;
		++args;
	} else {
		src = dst;
		dst = &global_pax_buf.buf;
	}
	
	// Required args.
	float x = mp_obj_get_float(args[1]);
	float y = mp_obj_get_float(args[2]);
	// Optional args.
	float w, h;
	if (n_args == 4) {
		w = mp_obj_get_float(args[3]);
		h = mp_obj_get_float(args[4]);
	} else {
		w = pax_buf_get_widthf (src);
		h = pax_buf_get_heightf(src);
	}
	
	// Forward function call.
	pax_draw_image_sized_op(dst, src, x, y, w, h);
	return mp_const_none;
}



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


// Get a copy of the pixel data from a Buffer.
static mp_obj_t getPixelData(mp_uint_t n_args, const mp_obj_t *args) {
	buf_n_col_t *buf = GET_BUF();
	
	// Get range params.
	int buf_size = PAX_BUF_CALC_SIZE(pax_buf_get_width(&buf->buf), pax_buf_get_height(&buf->buf), pax_buf_get_type(&buf->buf));
	int off      = 0;
	int len      = buf_size;
	if (n_args > 0) {
		off = mp_obj_get_int(args[0]);
	}
	if (n_args > 1) {
		len = mp_obj_get_int(args[1]);
	}
	
	// Bounds check.
	if (off < 0 || off + len > buf_size || len < 0) {
		mp_raise_msg_varg(&mp_type_ValueError, "Buffer index out of range: %d bytes at position %d in buffer of size %d", len, off, buf_size);
	}
	
	// Get some bytes.
	const uint8_t *buf_dat = pax_buf_get_pixels(&buf->buf);
	return mp_obj_new_bytearray(len, buf_dat + off);
}

// Get the size of the pixel data returned by `getPixelData`.
static mp_obj_t getPixelDataLen(mp_uint_t n_args, const mp_obj_t *args) {
	buf_n_col_t *buf = GET_BUF();
	
	// Simple calculator.
	size_t len = PAX_BUF_CALC_SIZE(pax_buf_get_width(&buf->buf), pax_buf_get_height(&buf->buf), pax_buf_get_type(&buf->buf));
	return mp_obj_new_int(len);
}



/* ==== CLASS DEFINITIONS ==== */
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(Buffer_del_obj,				1, 1, Buffer_del);

/* ==== GLOBAL DEFINITIONS ==== */
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(flush_obj,					0, 1, pax2py_flush);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(fontList_obj,				0, 0, fontList);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(fontInfo_obj,				1, 2, fontInfo);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(decodePNG_obj,				1, 1, decodePNG);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(infoPNG_obj,					1, 1, infoPNG);

/* ==== COMMOM DEFINITIONS ==== */
#ifdef CONFIG_DRIVER_PAX_EXPERIMENTAL
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(encodePNG_obj,				0, 6, encodePNG);
#endif // CONFIG_DRIVER_PAX_EXPERIMENTAL

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(fillColor_obj,				0, 2, fillColor);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lineColor_obj,				0, 2, lineColor);

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(width_obj,					0, 1, getWidth);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(height_obj,					0, 1, getHeight);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(type_obj,					0, 1, getType);

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(background_obj,				1, 2, background);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(drawRect_obj,				4, 6, drawRect);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(outlineRect_obj,				4, 6, outlineRect);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(drawRoundRect_obj,			5, 7, drawRoundRect);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(outlineRoundRect_obj,		5, 7, outlineRoundRect);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(drawTri_obj,					6, 8, drawTri);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(outlineTri_obj,				6, 8, outlineTri);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(drawCircle_obj,				3, 5, drawCircle);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(outlineCircle_obj,			3, 5, outlineCircle);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(drawHollowCircle_obj,		4, 6, drawHollowCircle);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(outlineHollowCircle_obj,		4, 6, outlineHollowCircle);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(drawArc_obj,					5, 7, drawArc);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(outlineArc_obj,				5, 7, outlineArc);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(drawHollowArc_obj,			6, 8, drawHollowArc);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(outlineHollowArc_obj,		6, 8, outlineHollowArc);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(drawRoundHollowArc_obj,		6, 8, drawRoundHollowArc);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(outlineRoundHollowArc_obj,	6, 8, outlineRoundHollowArc);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(drawLine_obj,				4, 6, drawLine);

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(drawImage_obj,				3, 6, drawImage);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(drawImageOpaque_obj,			3, 6, drawImageOpaque);

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(stringSize_obj,				1, 4, stringSize);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(drawString_obj,				3, 7, drawString);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(drawStringCentered_obj,		3, 7, drawStringCentered);

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pushMatrix_obj,				0, 1, pushMatrix);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(popMatrix_obj,				0, 1, popMatrix);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(clearMatrix_obj,				0, 2, clearMatrix);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(scale_obj,					1, 3, scale);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(translate_obj,				2, 3, translate);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(shear_obj,					2, 3, shear);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(rotate_obj,					1, 2, rotate);

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(getPixel_obj,				2, 3, getPixel);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(setPixel_obj,				3, 4, setPixel);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(getPixelRaw_obj,				2, 3, getPixelRaw);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(setPixelRaw_obj,				3, 4, setPixelRaw);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mergePixel_obj,				3, 4, mergePixel);

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(isDirty_obj,					0, 1, isDirty);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(getDirtyRect_obj,			0, 1, getDirtyRect);

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(clip_obj,					4, 5, clip);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(noClip_obj,					0, 1, noClip);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(getClipRect_obj,				0, 1, getClipRect);

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(getPixelData_obj,			0, 3, getPixelData);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(getPixelDataLen_obj,			0, 1, getPixelDataLen);



/* ==== MODULE DEFINITION ==== */
// Functions exclusive to Buffer objects.
#ifdef CONFIG_DRIVER_PAX_EXPERIMENTAL
#define MODPAX_CLASS_EXP_ROM
#else
#define MODPAX_CLASS_EXP_ROM
#endif

#define MODPAX_CLASS_ROM \
	MODPAX_CLASS_EXP_ROM \
	{MP_ROM_QSTR(MP_QSTR___del__),				MP_ROM_PTR(&Buffer_del_obj)},


// Functions exclusive to the global(tm).
#ifdef CONFIG_DRIVER_PAX_EXPERIMENTAL
#define MODPAX_GLOBAL_EXP_ROM
#else
#define MODPAX_GLOBAL_EXP_ROM
#endif

#define MODPAX_GLOBAL_ROM \
	MODPAX_GLOBAL_EXP_ROM \
	{MP_ROM_QSTR(MP_QSTR_FLAG_FORCE),			MP_ROM_INT(PAX2PY_FLAG_FORCE)}, \
	{MP_ROM_QSTR(MP_QSTR_FLAG_FULL),			MP_ROM_INT(PAX2PY_FLAG_FULL)}, \
	\
	{MP_ROM_QSTR(MP_QSTR_flush),				MP_ROM_PTR(&flush_obj)}, \
	{MP_ROM_QSTR(MP_QSTR___name__),				MP_ROM_QSTR(MP_QSTR_pax)}, \
	{MP_ROM_QSTR(MP_QSTR_Buffer),				MP_ROM_PTR(&Buffer_type)}, \
	\
	{MP_ROM_QSTR(MP_QSTR_FONT_INFO_BASIC),		MP_ROM_INT(0)}, \
	{MP_ROM_QSTR(MP_QSTR_FONT_INFO_RANGES),		MP_ROM_INT(1)}, \
	{MP_ROM_QSTR(MP_QSTR_FONT_INFO_ALL),		MP_ROM_INT(2)}, \
	\
	{MP_ROM_QSTR(MP_QSTR_fontList),				MP_ROM_PTR(&fontList_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_fontInfo),				MP_ROM_PTR(&fontInfo_obj)}, \
	\
	{MP_ROM_QSTR(MP_QSTR_decodePNG),			MP_ROM_PTR(&decodePNG_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_infoPNG),				MP_ROM_PTR(&infoPNG_obj)}, \
	\
	{MP_ROM_QSTR(MP_QSTR_PAX_BUF_1_PAL),		MP_ROM_INT(0)}, \
	{MP_ROM_QSTR(MP_QSTR_PAX_BUF_2_PAL),		MP_ROM_INT(1)}, \
	{MP_ROM_QSTR(MP_QSTR_PAX_BUF_4_PAL),		MP_ROM_INT(2)}, \
	{MP_ROM_QSTR(MP_QSTR_PAX_BUF_8_PAL),		MP_ROM_INT(3)}, \
	{MP_ROM_QSTR(MP_QSTR_PAX_BUF_16_PAL),		MP_ROM_INT(4)}, \
	\
	{MP_ROM_QSTR(MP_QSTR_PAX_BUF_1_GREY),		MP_ROM_INT(5)}, \
	{MP_ROM_QSTR(MP_QSTR_PAX_BUF_2_GREY),		MP_ROM_INT(6)}, \
	{MP_ROM_QSTR(MP_QSTR_PAX_BUF_4_GREY),		MP_ROM_INT(7)}, \
	{MP_ROM_QSTR(MP_QSTR_PAX_BUF_8_GREY),		MP_ROM_INT(8)}, \
	\
	{MP_ROM_QSTR(MP_QSTR_PAX_BUF_8_332RGB),		MP_ROM_INT(9)}, \
	{MP_ROM_QSTR(MP_QSTR_PAX_BUF_16_565RGB),	MP_ROM_INT(10)}, \
	\
	{MP_ROM_QSTR(MP_QSTR_PAX_BUF_4_1111ARGB),	MP_ROM_INT(11)}, \
	{MP_ROM_QSTR(MP_QSTR_PAX_BUF_8_2222ARGB),	MP_ROM_INT(12)}, \
	{MP_ROM_QSTR(MP_QSTR_PAX_BUF_16_4444ARGB),	MP_ROM_INT(13)}, \
	{MP_ROM_QSTR(MP_QSTR_PAX_BUF_32_8888ARGB),	MP_ROM_INT(14)},


// Function common to Buffer objects and the global(tm).
#ifdef CONFIG_DRIVER_PAX_EXPERIMENTAL
#define MODPAX_COMMON_EXP_ROM \
	{MP_ROM_QSTR(MP_QSTR_encodePNG),			MP_ROM_PTR(&encodePNG_obj)},
#else
#define MODPAX_COMMON_EXP_ROM
#endif

#define MODPAX_COMMON_ROM \
	MODPAX_COMMON_EXP_ROM \
	\
	{MP_ROM_QSTR(MP_QSTR_fillColor),			MP_ROM_PTR(&fillColor_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_fillColour),			MP_ROM_PTR(&fillColor_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_lineColor),			MP_ROM_PTR(&lineColor_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_lineColour),			MP_ROM_PTR(&lineColor_obj)}, \
	\
	{MP_ROM_QSTR(MP_QSTR_width),				MP_ROM_PTR(&width_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_height),				MP_ROM_PTR(&height_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_type),					MP_ROM_PTR(&type_obj)}, \
	\
	{MP_ROM_QSTR(MP_QSTR_background),			MP_ROM_PTR(&background_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_drawRect),				MP_ROM_PTR(&drawRect_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_outlineRect),			MP_ROM_PTR(&outlineRect_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_drawRoundRect),		MP_ROM_PTR(&drawRoundRect_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_outlineRoundRect),		MP_ROM_PTR(&outlineRoundRect_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_drawTri),				MP_ROM_PTR(&drawTri_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_outlineTri),			MP_ROM_PTR(&outlineTri_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_drawCircle),			MP_ROM_PTR(&drawCircle_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_outlineCircle),		MP_ROM_PTR(&outlineCircle_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_drawDonut),			MP_ROM_PTR(&drawHollowCircle_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_outlineDonut),			MP_ROM_PTR(&outlineHollowCircle_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_drawHollowCircle),		MP_ROM_PTR(&drawHollowCircle_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_outlineHollowCircle),	MP_ROM_PTR(&outlineHollowCircle_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_drawArc),				MP_ROM_PTR(&drawArc_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_outlineArc),			MP_ROM_PTR(&outlineArc_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_drawHollowArc),		MP_ROM_PTR(&drawHollowArc_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_outlineHollowArc),		MP_ROM_PTR(&outlineHollowArc_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_drawRoundHollowArc),	MP_ROM_PTR(&drawRoundHollowArc_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_outlineRoundHollowArc),MP_ROM_PTR(&outlineRoundHollowArc_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_drawLine),				MP_ROM_PTR(&drawLine_obj)}, \
	\
	{MP_ROM_QSTR(MP_QSTR_drawImage),			MP_ROM_PTR(&drawImage_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_drawImageOpaque),		MP_ROM_PTR(&drawImageOpaque_obj)}, \
	\
	{MP_ROM_QSTR(MP_QSTR_stringSize),			MP_ROM_PTR(&stringSize_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_drawString),			MP_ROM_PTR(&drawString_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_drawStringCentered),	MP_ROM_PTR(&drawStringCentered_obj)}, \
	\
	{MP_ROM_QSTR(MP_QSTR_pushMatrix),			MP_ROM_PTR(&pushMatrix_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_popMatrix),			MP_ROM_PTR(&popMatrix_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_clearMatrix),			MP_ROM_PTR(&clearMatrix_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_scale),				MP_ROM_PTR(&scale_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_translate),			MP_ROM_PTR(&translate_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_shear),				MP_ROM_PTR(&shear_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_rotate),				MP_ROM_PTR(&rotate_obj)}, \
	\
	{MP_ROM_QSTR(MP_QSTR_getPixel),				MP_ROM_PTR(&getPixel_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_setPixel),				MP_ROM_PTR(&setPixel_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_getPixelRaw),			MP_ROM_PTR(&getPixelRaw_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_setPixelRaw),			MP_ROM_PTR(&setPixelRaw_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_mergePixel),			MP_ROM_PTR(&mergePixel_obj)}, \
	\
	{MP_ROM_QSTR(MP_QSTR_isDirty),				MP_ROM_PTR(&isDirty_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_getDirtyRect),			MP_ROM_PTR(&getDirtyRect_obj)}, \
	\
	{MP_ROM_QSTR(MP_QSTR_clip),					MP_ROM_PTR(&clip_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_noClip),				MP_ROM_PTR(&noClip_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_getClipRect),			MP_ROM_PTR(&getClipRect_obj)}, \
	\
	{MP_ROM_QSTR(MP_QSTR_getPixelData),			MP_ROM_PTR(&getPixelData_obj)}, \
	{MP_ROM_QSTR(MP_QSTR_getPixelDataLen),		MP_ROM_PTR(&getPixelDataLen_obj)},


static const mp_rom_map_elem_t Buffer_locals_table[] = {
	MODPAX_CLASS_ROM
	MODPAX_COMMON_ROM
};

static MP_DEFINE_CONST_DICT(Buffer_locals, Buffer_locals_table);

const mp_obj_type_t Buffer_type = {
	.base        = { &mp_type_type },
	.name        = MP_QSTR_Buffer,
	.print       = Buffer_print,
	.make_new    = Buffer_new,
	.locals_dict = (mp_obj_dict_t *) &Buffer_locals,
};

static const mp_rom_map_elem_t pax_module_globals_table[] = {
	MODPAX_GLOBAL_ROM
	MODPAX_COMMON_ROM
};

static MP_DEFINE_CONST_DICT(pax_module_globals, pax_module_globals_table);

const mp_obj_module_t pax_module = {
	.base    = {&mp_type_module},
	.globals = (mp_obj_dict_t *) &pax_module_globals,
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
	pax_buf_init(
		&global_pax_buf.buf,
	#ifdef CONFIG_DRIVER_PAX_COMPAT
		framebuffer,
	#else
		NULL,
	#endif
		CONFIG_DRIVER_PAX_FBWIDTH, CONFIG_DRIVER_PAX_FBHEIGHT,
		PAX2PY_BUF_NATIVE
	);
	#ifndef CONFIG_DRIVER_PAX_COMPAT
	framebuffer = pax_buf_get_pixels_rw(&global_pax_buf.buf);
	#endif
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
