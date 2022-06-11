if(CONFIG_DRIVER_HEAPSPACE_ENABLE)
set(srcs
    "modheapspace.c"
)
else()
set(srcs
    ""
)
endif()
set(includes
    ""
    )

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS ${includes}
                       REQUIRES lwip micropython)
