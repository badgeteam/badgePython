#Include all files that contain MP bindings

#Define the name of your module here
set(mod_name "ssd1306")
set(driver_name "ssd1306")
set(mod_register "ssd1306 DISPLAY")

if(CONFIG_DRIVER_SSD1306_ENABLE)
    message(STATUS "ssd1306 enabled")
    set(EXTMODS_INIT "${EXTMODS_INIT}" "\"${driver_name}\"@\"${mod_register}\"^" CACHE INTERNAL "")
else()
    message(STATUS "ssd1306 disabled")
endif()
