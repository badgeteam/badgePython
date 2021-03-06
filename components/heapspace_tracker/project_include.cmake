#Include all files that contain MP bindings
set(mods
    "${COMPONENT_DIR}/modheapspace.c"
)

#Define the name of your module here
set(mod_name "heapspace")
set(mod_register "heapspace")

if(CONFIG_DRIVER_HEAPSPACE_ENABLE)
    message(STATUS "heapspace enabled")
    set(EXTMODS "${EXTMODS}" "${mods}" CACHE INTERNAL "")
    set(EXTMODS_NAMES "${EXTMODS_NAMES}" "${mod_name}" CACHE INTERNAL "")
endif()
