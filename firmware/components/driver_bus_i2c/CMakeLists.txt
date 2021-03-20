set(includes
    "include"
    )

if(CONFIG_DRIVER_I2C_ENABLE)
    set(srcs
        "driver_i2c.c"
        "modi2c.c"
    )
else()
    set(srcs "")
endif()


idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS ${includes}
                       REQUIRES micropython)