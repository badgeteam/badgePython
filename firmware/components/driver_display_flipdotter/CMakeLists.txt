if(CONFIG_DRIVER_FLIPDOTTER_ENABLE)
    set(srcs
        "driver_flipdotter.c"
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
