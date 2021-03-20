set(srcs
    "driver_rtcmem.c"
)

set(includes
    "include"
    )

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS ${includes}
                       REQUIRES lwip)
