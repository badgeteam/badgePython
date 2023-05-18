#Include all files that contain MP bindings
set(mods
    "${COMPONENT_DIR}/modlauncher.c"
)

#Define the name of your module here
set(mod_name "launcher")
set(mod_register "launcher")

if(CONFIG_DRIVER_LAUNCHER_ENABLE)
    message(STATUS "Launcher API enabled")
    set(EXTMODS "${EXTMODS}" "${mods}" CACHE INTERNAL "")
    set(EXTMODS_NAMES "${EXTMODS_NAMES}" "${mod_name}" CACHE INTERNAL "")
    set(EXTMODS_INIT "${EXTMODS_INIT}" "\"${mod_name}\"@\"${mod_register}\"^" CACHE INTERNAL "")
else()
    message(STATUS "Launcher API disabled")
endif()
