if(CONFIG_DRIVER_DEPG0290B1_ENABLE)
    set(srcs
        "esp32-component-spi-depg0290b1/depg0290b1.c"
    )
    set(includes
    "include"
    "esp32-component-spi-depg0290b1/include"
    )
else()
    set(srcs "")
    set(includes
    ""
    )
endif()

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS ${includes}
                       REQUIRES driver_framebuffer)
