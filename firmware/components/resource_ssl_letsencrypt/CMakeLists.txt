set(srcs
    "letsencrypt.c"
)

set(includes
    "./"
    )

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS ${includes}
                       )
