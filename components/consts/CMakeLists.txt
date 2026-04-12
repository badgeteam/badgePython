idf_component_register(
  SRCS "modconsts.c"
    PRIV_INCLUDE_DIRS
        ${MP_ROOT}
        ${MP_PORT_DIR}
        ${MICROPY_BOARD_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/..
        ${BUILD_DIR}
    REQUIRES
        micropython
        driver
        freertos)
