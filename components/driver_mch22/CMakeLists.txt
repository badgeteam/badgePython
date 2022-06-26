if(CONFIG_DRIVER_MCH22_RP2040_ENABLE)
    set(srcs
        "esp32-component-mch2022-rp2040/rp2040.c"
        "esp32-component-spi-ice40/ice40.c"
        "modmch22.c"
    )
else()
    set(srcs "")
endif()

set(include
    "include"
    "esp32-component-mch2022-rp2040/include"
    "esp32-component-spi-ice40/include"
    )

idf_component_register(
  SRCS "${srcs}"
  INCLUDE_DIRS "${include}"
  REQUIRES micropython buses driver_display_ili9341
)
