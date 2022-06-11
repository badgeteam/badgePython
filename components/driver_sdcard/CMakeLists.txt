if(CONFIG_DRIVER_SDCARD_ENABLE)
set(srcs
    "driver_sdcard.c"
    "modsdcard.c"
)
else()
set(srcs "")
endif()

set(includes
    "include"
    )

set(IDF_COMPONENTS
    fatfs
    wear_levelling
    spiffs
    micropython
)

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS ${includes}
                       REQUIRES ${IDF_COMPONENTS})
