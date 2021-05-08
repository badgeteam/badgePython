if(CONFIG_DRIVER_ST7789V_ENABLE)
    set(srcs
        "driver_st7789v.c"
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
