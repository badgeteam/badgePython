# This file is required by ESP-IDF to use pax-graphics as a component.
idf_component_register(
	SRCS
		"src/modpax.c"
	
	INCLUDE_DIRS
		"src"
	
	REQUIRES
		pax-graphics
		pax-codecs
		micropython
		driver_display_ili9341
		driver_framebuffer
)
