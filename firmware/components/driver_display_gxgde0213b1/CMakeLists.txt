if(CONFIG_DRIVER_GXGDE0213B1_ENABLE)
    set(srcs
        "driver_gxgde0213b1.c"
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
