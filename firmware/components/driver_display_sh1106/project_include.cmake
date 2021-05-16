#Include all files that contain MP bindings

#Define the name of your module here
set(mod_name "sh1106")
set(driver_name "sh1106")
set(mod_register "sh1106 DISPLAY")

if(CONFIG_DRIVER_FRAMEBUFFER_ENABLE)
    message(STATUS "sh1106 enabled")
    set(EXTMODS_INIT "${EXTMODS_INIT}" "\"${driver_name}\"@\"${mod_register}\"^" CACHE INTERNAL "")
else()
    message(STATUS "sh1106 disabled")
endif()