if(CONFIG_DRIVER_LAUNCHER_ENABLE)
    set(srcs
        "modlauncher.c"
    )
else()
    set(srcs "")
endif()

idf_component_register(
  SRCS "${srcs}"
  REQUIRES micropython
)
