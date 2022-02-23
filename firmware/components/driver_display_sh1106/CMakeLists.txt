if(CONFIG_DRIVER_SH1106_ENABLE)
    set(srcs
        "driver_sh1106.c"
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
