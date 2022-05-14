if(CONFIG_DRIVER_NEOPIXEL_ENABLE)
    set(srcs
        "esp32-component-mch2022-rp2040/rp2040.c"
        "modrp2040.c"
    )
else()
    set(srcs "")
endif()

set(include
    "include"
    "esp32-component-mch2022-rp2040/include"
    )

idf_component_register(
  SRCS "${srcs}"
  INCLUDE_DIRS "${include}"
  REQUIRES micropython buses
)
