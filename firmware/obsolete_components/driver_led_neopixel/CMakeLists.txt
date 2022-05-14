set(includes
    "include"
    )

if(CONFIG_DRIVER_NEOPIXEL_ENABLE)
    set(srcs
        "driver_neopixel.c"
        "modneopixel.c"
    )
else()
    set(srcs "")
endif()


idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS ${includes}
                       REQUIRES micropython)