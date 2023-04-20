#Include all files that contain MP bindings
set(mods
    "${COMPONENT_DIR}/modmpr121.c"
)

#Define the name of your module here
set(driver_name "mpr121")
set(mod_name "mpr121")
set(mod_register "mpr121")

if(CONFIG_DRIVER_MPR121_ENABLE)
    message(STATUS "mpr121 enabled")
    set(EXTMODS "${EXTMODS}" "${mods}" CACHE INTERNAL "")
    set(EXTMODS_NAMES "${EXTMODS_NAMES}" "${mod_name}" CACHE INTERNAL "")
    set(EXTMODS_INIT "${EXTMODS_INIT}" "\"${driver_name}\"@\"${mod_register}\"^" CACHE INTERNAL "")
else()
    message(STATUS "mpr121 disabled")
endif()
