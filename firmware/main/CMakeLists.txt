set(srcs
    "main.c"
    "factory_reset.c"
    "nvs_init.c"
    "ota_update.c"
    "platform.c"
    "zip.c"
    "system.c"
    "${COMPONENT_DIR}/platform_gen.c"
)

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "./")

add_custom_command(
    OUTPUT "${COMPONENT_DIR}/platform_gen.c"
    COMMAND echo extmods_init: ${EXTMODS_INIT} && python3 platform_generator.py -b ./ \"${EXTMODS_INIT}\"
    WORKING_DIRECTORY ${COMPONENT_DIR}
)