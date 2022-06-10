set(includes
    "include"
    )

if(CONFIG_DRIVER_FSOVERBUS_ENABLE)
    set(srcs
        "backend.c"
        "driver_fsoverbus.c"
        "filefunctions.c"
        "packetutils.c"
        "specialfunctions.c"
        "uart_backend.c"
    )
else()
    set(srcs "")
endif()


idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS ${includes}
                       REQUIRES driver_rtcmem)