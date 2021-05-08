if(CONFIG_DRIVER_EINK_ENABLE)
set(srcs
    "driver_eink.c"
    "driver_eink_lut.c"
    "driver_eink_dev.c"
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
