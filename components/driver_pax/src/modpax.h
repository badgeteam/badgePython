/*
 * BADGE.TEAM framebuffer driver
 * Implements the PAX C API for BadgePython.
 * Julian Scheffers 2023
 */

#pragma once

#include <sdkconfig.h>
#include <pax_gfx.h>

#ifdef __cplusplus
extern "C" {
#endif

// TODO.
#ifndef FB_TYPE_16BPP
	#define FB_TYPE_16BPP
#endif

#ifdef CONFIG_DRIVER_FRAMEBUFFER_DOUBLE_BUFFERED
	#error "PAX does not support double buffering."
#endif

// Detect correct framebuffer type.
#if   defined(FB_TYPE_1BPP)
	#define PAX2PY_BUF_NATIVE PAX_BUF_1_GREY
#elif defined(FB_TYPE_8BPP)
	#define PAX2PY_BUF_NATIVE PAX_BUF_8_GREY
#elif defined(FB_TYPE_12BPP)
	#error "PAX does not support 12bpp pixel buffers."
#elif defined(FB_TYPE_16BPP)
	#define PAX2PY_BUF_NATIVE PAX_BUF_16_565RGB
#elif defined(FB_TYPE_8CBPP)
	#error "I have literally no idea which color format this is."
#elif defined(FB_TYPE_24BPP)
	// #define PAX2PY_BUF_NATIVE PAX_BUF_32_8888ARGB
	#error "PAX does not support 24bpp pixel buffers."
#elif defined(FB_TYPE_32BPP)
	#define PAX2PY_BUF_NATIVE PAX_BUF_32_8888ARGB
#else
	#error "No framebuffer type configured."
#endif

#ifdef __cplusplus
}
#endif
