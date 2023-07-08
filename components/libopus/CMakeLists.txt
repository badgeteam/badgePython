include("${PROJECT_DIR}/external/opus/cmake/OpusFunctions.cmake")

set(IDF_COMPONENTS
    
    )

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS ${includes}
                       REQUIRES ${IDF_COMPONENTS})


idf_build_set_property(COMPILE_OPTIONS "-DVAR_ARRAYS" APPEND)
idf_build_set_property(COMPILE_OPTIONS "-DHAVE_CONFIG_H" APPEND)
