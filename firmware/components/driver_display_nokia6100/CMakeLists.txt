if(CONFIG_DRIVER_NOKIA6100_ENABLE)
    set(srcs
        "driver_nokia6100.c"
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
