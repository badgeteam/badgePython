if(CONFIG_DRIVER_MPR121_ENABLE)
    set(srcs
        "driver_mpr121.c"
        "modmpr121.c"
    )
    set(includes
    "include"
    )
else()
    set(srcs "")
    set(includes
    ""
    )
endif()

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS ${includes}
                       REQUIRES micropython buses nvs_flash)
