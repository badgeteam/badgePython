if(CONFIG_DRIVER_ILI9341_ENABLE)
    set(srcs
        "driver_ili9341.c"
    )
else()
    set(srcs "")
endif()


set(includes
    "include"
    )

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS ${includes}
                       REQUIRES driver_framebuffer)
