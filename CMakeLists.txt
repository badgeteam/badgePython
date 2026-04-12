# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

message(STATUS "Clearing cache")
unset(EXTMODS CACHE)
unset(EXTMODS_NAMES CACHE)
unset(EXTMODS_INIT CACHE)

set(EXTRA_COMPONENT_DIRS ./obsolete_components)

set(MP_ROOT ${PROJECT_DIR}/components/micropython/micropython)
set(MP_PORT_DIR ${PROJECT_DIR}/components/micropython/micropython/ports/esp32)
set(MICROPY_BOARD_DIR ${MP_PORT_DIR}/boards/ESP32_GENERIC_P4)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(badgepython)
