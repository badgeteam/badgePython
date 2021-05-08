 if(CONFIG_DRIVER_ERC12864_ENABLE)
    set(srcs
        "driver_erc12864.c"
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
