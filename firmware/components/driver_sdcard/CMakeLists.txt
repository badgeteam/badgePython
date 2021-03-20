set(srcs
    "driver_sdcard.c"
)

set(includes
    "include"
    )

set(IDF_COMPONENTS
    fatfs
    wear_levelling
    spiffs
)

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS ${includes}
                       REQUIRES ${IDF_COMPONENTS})
