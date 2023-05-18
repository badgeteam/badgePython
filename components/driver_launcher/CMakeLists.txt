if(CONFIG_DRIVER_LAUNCHER_ENABLE)
    set(srcs
        "modlauncher.c"
    )
else()
    set(srcs "")
endif()

set(include
    "include"
    )

idf_component_register(
  SRCS "${srcs}"
  INCLUDE_DIRS "${include}"
  REQUIRES micropython
)
