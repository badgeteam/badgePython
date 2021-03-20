set(srcs
    "driver_framebuffer.cpp"
    "driver_framebuffer_dirty.cpp"
    "driver_framebuffer_drawing.cpp"
    "driver_framebuffer_orientation.cpp"
    "driver_framebuffer_text.cpp"
    "driver_framebuffer_window.cpp"

    "png/adler32.c"
    "png/crc32.c"
    "png/deflate_reader.c"
    "png/file_reader.c"
    "png/flash_reader.c"
    "png/mem_reader.c"
    "png/png_reader.c"
)

set(includes
    "include"
    "png"
    )

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS ${includes}
                       REQUIRES spi_flash)
