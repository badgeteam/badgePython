#Include all files that contain MP bindings
set(mods
    "${COMPONENT_DIR}/modwpa2enterprise.c"
)

#Define the name of your module here
set(mod_name "wpa2enterprise")
set(mod_register "wpa2enterprise")

message(STATUS "wpa2enterprise enabled")
set(EXTMODS "${EXTMODS}" "${mods}" CACHE INTERNAL "")
set(EXTMODS_NAMES "${EXTMODS_NAMES}" "${mod_name}" CACHE INTERNAL "")
set(EXTMODS_INIT "${EXTMODS_INIT}" "\"${mod_name}\"@\"${mod_register}\"^" CACHE INTERNAL "")
