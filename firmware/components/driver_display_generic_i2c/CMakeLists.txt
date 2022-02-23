if(CONFIG_DRIVER_DISPLAY_I2C_ENABLE)
    set(srcs
        "driver_i2c_display.c"
    )
else()
    set(srcs "")
endif()


set(includes
    "include"
    )

set(IDF_COMPONENTS
    driver_framebuffer
    buses)

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS ${includes}
                       REQUIRES ${IDF_COMPONENTS})
