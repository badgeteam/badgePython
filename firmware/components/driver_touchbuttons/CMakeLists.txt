set(includes
    "include"
    )

if(CONFIG_DRIVER_IO_TOUCHBUTTONS_ENABLE)
    set(srcs
        "driver_touchbuttons.c"
        "modtouchbuttons.c"
    )
else()
    set(srcs "")
endif()


idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS ${includes}
                       REQUIRES micropython)