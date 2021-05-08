if(CONFIG_DRIVER_SSD1306_ENABLE)
    set(srcs
        "driver_ssd1306.c"
    )
else()
    set(srcs "")
endif()


set(includes
    "include"
    )

set(IDF_COMPONENTS
    driver_framebuffer
    driver_bus_i2c)

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS ${includes}
                       REQUIRES ${IDF_COMPONENTS})
