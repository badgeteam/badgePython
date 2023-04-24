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

#define PAX2PY_FLAG_FORCE 1
#define PAX2PY_FLAG_FULL  2

#if   defined(CONFIG_DRIVER_PAX_BUF_1_GREY)
	#define PAX2PY_BUF_NATIVE PAX_BUF_1_GREY
#elif defined(CONFIG_DRIVER_PAX_BUF_2_GREY)
	#define PAX2PY_BUF_NATIVE PAX_BUF_2_GREY
#elif defined(CONFIG_DRIVER_PAX_BUF_4_GREY)
	#define PAX2PY_BUF_NATIVE PAX_BUF_4_GREY
#elif defined(CONFIG_DRIVER_PAX_BUF_8_GREY)
	#define PAX2PY_BUF_NATIVE PAX_BUF_8_GREY
#elif defined(CONFIG_DRIVER_PAX_BUF_16_GREY)
	#define PAX2PY_BUF_NATIVE PAX_BUF_16_GREY
#elif defined(CONFIG_DRIVER_PAX_BUF_1_GREY)
	#define PAX2PY_BUF_NATIVE PAX_BUF_1_GREY
#elif defined(CONFIG_DRIVER_PAX_BUF_2_GREY)
	#define PAX2PY_BUF_NATIVE PAX_BUF_2_GREY
#elif defined(CONFIG_DRIVER_PAX_BUF_4_GREY)
	#define PAX2PY_BUF_NATIVE PAX_BUF_4_GREY
#elif defined(CONFIG_DRIVER_PAX_BUF_8_GREY)
	#define PAX2PY_BUF_NATIVE PAX_BUF_8_GREY
#elif defined(CONFIG_DRIVER_PAX_BUF_8_332RGB)
	#define PAX2PY_BUF_NATIVE PAX_BUF_8_332RGB
#elif defined(CONFIG_DRIVER_PAX_BUF_16_565RGB)
	#define PAX2PY_BUF_NATIVE PAX_BUF_16_565RGB
#elif defined(CONFIG_DRIVER_PAX_BUF_4_1111ARGB)
	#define PAX2PY_BUF_NATIVE PAX_BUF_4_1111ARGB
#elif defined(CONFIG_DRIVER_PAX_BUF_8_2222ARGB)
	#define PAX2PY_BUF_NATIVE PAX_BUF_8_2222ARGB
#elif defined(CONFIG_DRIVER_PAX_BUF_16_4444ARGB)
	#define PAX2PY_BUF_NATIVE PAX_BUF_16_4444ARGB
#elif defined(CONFIG_DRIVER_PAX_BUF_32_8888ARGB)
	#define PAX2PY_BUF_NATIVE PAX_BUF_32_8888ARGB
#else
	#error "No framebuffer type configured."
#endif

#ifdef __cplusplus
}
#endif