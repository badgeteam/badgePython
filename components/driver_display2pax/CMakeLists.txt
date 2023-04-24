if(CONFIG_DRIVER_FRAMEBUFFER_COMPAT_ENABLE)
set(srcs
    "moddisplay.c"
)
else()
set(srcs)
endif()

set(IDF_COMPONENTS
    driver_pax
    pax-graphics
    pax-codecs
    )

idf_component_register(SRCS "${srcs}"
                    #    INCLUDE_DIRS ${includes}
                       REQUIRES ${IDF_COMPONENTS})
