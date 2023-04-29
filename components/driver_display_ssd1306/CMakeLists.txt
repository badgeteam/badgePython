if(CONFIG_DRIVER_SSD1306_ENABLE)
    set(srcs
        "driver_ssd1306.c"
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
                       REQUIRES driver_framebuffer)
