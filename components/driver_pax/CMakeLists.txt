# This file is required by ESP-IDF to use pax-graphics as a component.
idf_component_register(
	SRCS
		"src/modpax.c"
	
	INCLUDE_DIRS
		src
	
	REQUIRES
		pax-graphics
		micropython
		driver_framebuffer
)
