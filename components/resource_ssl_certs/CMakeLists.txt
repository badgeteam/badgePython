set(srcs
    "pinned_certs.c"
)

set(includes
    "./"
    )

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS ${includes}
                       )
