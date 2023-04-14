#Include all files that contain MP bindings
set(mods
    "${COMPONENT_DIR}/src/modpax.c"
)

#Define the name of your module here
set(mod_name "pax")
set(driver_name "pax2py")
set(mod_register "pax2py PAX")

if(CONFIG_DRIVER_PAX_ENABLE)
    message(STATUS "PAX2Python enabled")
    
    add_definitions(-DPAX_COMPILE_FONT_INDEX=1)
    if (CONFIG_DRIVER_PAX_MCR)
        add_definitions(-DPAX_COMPILE_MCR=1)
    else()
        add_definitions(-DPAX_COMPILE_MCR=0)
    endif()
    
    set(EXTMODS "${EXTMODS}" "${mods}" CACHE INTERNAL "")
    set(EXTMODS_NAMES "${EXTMODS_NAMES}" "${mod_name}" CACHE INTERNAL "")
    set(EXTMODS_INIT "${EXTMODS_INIT}" "\"${driver_name}\"@\"${mod_register}\"^" CACHE INTERNAL "")
else()
    message(STATUS "PAX2Python disabled")
endif()
