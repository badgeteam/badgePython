#Include all files that contain MP bindings

#Define the name of your module here
set(mod_name "depg0290b1")
set(driver_name "depg0290b1")
set(mod_register "depg0290b1 DISPLAY")

if(CONFIG_DRIVER_DEPG0290B1_ENABLE)
    message(STATUS "depg0290b1 enabled")
    set(EXTMODS_INIT "${EXTMODS_INIT}" "\"${driver_name}\"@\"${mod_register}\"^" CACHE INTERNAL "")
else()
    message(STATUS "depg0290b1 disabled")
endif()
