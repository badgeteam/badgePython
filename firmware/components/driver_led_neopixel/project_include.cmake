#Include all files that contain MP bindings
set(mods
    "${COMPONENT_DIR}/modneopixel.c"
)

#Define the name of your module here
set(mod_name "neopixel")
set(mod_register "NEOPIXEL BUS")

if(CONFIG_DRIVER_NEOPIXEL_ENABLE)
    message(STATUS "neopixel enabled")
    set(EXTMODS "${EXTMODS}" "${mods}" CACHE INTERNAL "")
    set(EXTMODS_NAMES "${EXTMODS_NAMES}" "${mod_name}" CACHE INTERNAL "")
    set(EXTMODS_INIT "${EXTMODS_INIT}" "\"${mod_name}\"@\"${mod_register}\"^" CACHE INTERNAL "")
else()
    message(STATUS "neopixel disabled")
endif()