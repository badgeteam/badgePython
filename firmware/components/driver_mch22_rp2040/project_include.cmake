#Include all files that contain MP bindings
set(mods
    "${COMPONENT_DIR}/modmch22.c"
)

#Define the name of your module here
set(mod_name "mch22")
set(mod_register "MCH2022 RP2040")

if(CONFIG_DRIVER_MCH22_RP2040_ENABLE)
    message(STATUS "MCH22 enabled")
    set(EXTMODS "${EXTMODS}" "${mods}" CACHE INTERNAL "")
    set(EXTMODS_NAMES "${EXTMODS_NAMES}" "${mod_name}" CACHE INTERNAL "")
    set(EXTMODS_INIT "${EXTMODS_INIT}" "\"${mod_name}\"@\"${mod_register}\"^" CACHE INTERNAL "")
else()
    message(STATUS "mch22 disabled")
endif()