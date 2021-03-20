set(srcs
    "driver_hub75.c"
    "compositor.c"
    "font_6x3.c"
    "font_7x5.c"
    "i2s_parallel.c"
    "val2pwm.c"
)

set(includes
    "include"
    )

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS ${includes}
                       )
