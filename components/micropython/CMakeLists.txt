# Component-level cmake file for building MicroPython on ESP32.

cmake_minimum_required(VERSION 3.12)

if(NOT DEFINED MICROPY_DIR)
    get_filename_component(MICROPY_DIR ${PROJECT_DIR}/external/micropython ABSOLUTE)
endif()

# Include core source components.
include(${MICROPY_DIR}/ports/esp32/main_${IDF_TARGET}/CMakeLists.txt)
