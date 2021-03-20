# Set location of base MicroPython directory.
get_filename_component(MICROPY_DIR ${PROJECT_DIR}/components/micropython/micropython ABSOLUTE)
get_filename_component(MICROPY_PORT_DIR ${PROJECT_DIR}/components/micropython/micropython/ports/esp32 ABSOLUTE)
# Include core source components.
include(${MICROPY_DIR}/py/py.cmake)

#include(${MICROPY_DIR}/extmod/extmod.cmake)
# CMake fragment for MicroPython extmod component
#COPIED EXTMOD CMAKE file to here. 

set(MICROPY_EXTMOD_DIR "${MICROPY_DIR}/extmod")
set(MICROPY_OOFATFS_DIR "${MICROPY_DIR}/lib/oofatfs")

set(MICROPY_SOURCE_EXTMOD
    ${MICROPY_EXTMOD_DIR}/machine_i2c.c
    ${MICROPY_EXTMOD_DIR}/machine_mem.c
    ${MICROPY_EXTMOD_DIR}/machine_pulse.c
    ${MICROPY_EXTMOD_DIR}/machine_signal.c
    ${MICROPY_EXTMOD_DIR}/machine_spi.c
    ${MICROPY_EXTMOD_DIR}/modbluetooth.c
    ${MICROPY_EXTMOD_DIR}/modbtree.c
    ${MICROPY_EXTMOD_DIR}/modframebuf.c
    ${MICROPY_EXTMOD_DIR}/modonewire.c
    ${MICROPY_EXTMOD_DIR}/moduasyncio.c
    ${MICROPY_EXTMOD_DIR}/modubinascii.c
    ${MICROPY_EXTMOD_DIR}/moducryptolib.c
    ${MICROPY_EXTMOD_DIR}/moductypes.c
    ${MICROPY_EXTMOD_DIR}/moduhashlib.c
    ${MICROPY_EXTMOD_DIR}/moduheapq.c
    ${MICROPY_EXTMOD_DIR}/modujson.c
    ${MICROPY_EXTMOD_DIR}/modurandom.c
    ${MICROPY_EXTMOD_DIR}/modure.c
    ${MICROPY_EXTMOD_DIR}/moduselect.c
    ${MICROPY_EXTMOD_DIR}/modussl_axtls.c
    ${MICROPY_EXTMOD_DIR}/modussl_mbedtls.c
    ${MICROPY_EXTMOD_DIR}/modutimeq.c
    ${MICROPY_EXTMOD_DIR}/moduwebsocket.c
    ${MICROPY_EXTMOD_DIR}/moduzlib.c
    ${MICROPY_EXTMOD_DIR}/modwebrepl.c
    ${MICROPY_EXTMOD_DIR}/uos_dupterm.c
    ${MICROPY_EXTMOD_DIR}/utime_mphal.c
    #${MICROPY_EXTMOD_DIR}/vfs.c
    #${MICROPY_EXTMOD_DIR}/vfs_blockdev.c
    #${MICROPY_EXTMOD_DIR}/vfs_fat.c
    #${MICROPY_EXTMOD_DIR}/vfs_fat_diskio.c
    #${MICROPY_EXTMOD_DIR}/vfs_fat_file.c
    #${MICROPY_EXTMOD_DIR}/vfs_lfs.c
    #${MICROPY_EXTMOD_DIR}/vfs_posix.c
    #${MICROPY_EXTMOD_DIR}/vfs_posix_file.c
    #${MICROPY_EXTMOD_DIR}/vfs_reader.c
    ${MICROPY_EXTMOD_DIR}/virtpin.c
    ${MICROPY_EXTMOD_DIR}/nimble/modbluetooth_nimble.c
    ${COMPONENT_DIR}/vfs_fat/vfs_native_file.c
    ${COMPONENT_DIR}/vfs_fat/vfs_native_misc.c
    ${COMPONENT_DIR}/vfs_fat/vfs_native.c
    ${COMPONENT_DIR}/vfs_fat/vfs_reader.c
    ${COMPONENT_DIR}/vfs_fat/vfs.c
)

set(MICROPY_FROZEN_MANIFEST ${COMPONENT_DIR}/manifest.py)

set(MICROPY_QSTRDEFS_PORT
    ${MICROPY_PORT_DIR}/qstrdefsport.h
)

set(MICROPY_SOURCE_LIB
    #${MICROPY_DIR}/lib/littlefs/lfs1.c
    #${MICROPY_DIR}/lib/littlefs/lfs1_util.c
    #${MICROPY_DIR}/lib/littlefs/lfs2.c
    #${MICROPY_DIR}/lib/littlefs/lfs2_util.c
    ${MICROPY_DIR}/lib/mbedtls_errors/mp_mbedtls_errors.c
    ${MICROPY_DIR}/lib/mp-readline/readline.c
    ${MICROPY_DIR}/lib/netutils/netutils.c
    #${MICROPY_DIR}/lib/oofatfs/ff.c
    #${MICROPY_DIR}/lib/oofatfs/ffunicode.c
    ${MICROPY_DIR}/lib/timeutils/timeutils.c
    ${MICROPY_DIR}/lib/utils/interrupt_char.c
    #${MICROPY_DIR}/lib/utils/stdout_helpers.c
    ${MICROPY_DIR}/lib/utils/sys_stdio_mphal.c
    ${MICROPY_DIR}/lib/utils/pyexec.c
)

set(MICROPY_SOURCE_DRIVERS
    ${MICROPY_DIR}/drivers/bus/softspi.c
    ${MICROPY_DIR}/drivers/dht/dht.c
)

set(MICROPY_SOURCE_PORT
    ${COMPONENT_DIR}/main.c
    ${COMPONENT_DIR}/vfs_fat/moduos.c
    ${MICROPY_PORT_DIR}/uart.c
    ${MICROPY_PORT_DIR}/gccollect.c
    ${MICROPY_PORT_DIR}/mphalport.c
    #${MICROPY_PORT_DIR}/fatfs_port.c
    ${MICROPY_PORT_DIR}/help.c
    ${MICROPY_PORT_DIR}/modutime.c
    #${MICROPY_PORT_DIR}/moduos.c
    ${MICROPY_PORT_DIR}/machine_timer.c
    ${MICROPY_PORT_DIR}/machine_pin.c
    ${MICROPY_PORT_DIR}/machine_touchpad.c
    ${MICROPY_PORT_DIR}/machine_adc.c
    ${MICROPY_PORT_DIR}/machine_dac.c
    ${MICROPY_PORT_DIR}/machine_i2c.c
    ${MICROPY_PORT_DIR}/machine_pwm.c
    ${MICROPY_PORT_DIR}/machine_uart.c
    ${MICROPY_PORT_DIR}/modmachine.c
    ${MICROPY_PORT_DIR}/modnetwork.c
    ${MICROPY_PORT_DIR}/network_lan.c
    ${MICROPY_PORT_DIR}/network_ppp.c
    ${MICROPY_PORT_DIR}/mpnimbleport.c
    ${MICROPY_PORT_DIR}/modsocket.c
    ${MICROPY_PORT_DIR}/modesp.c
    ${MICROPY_PORT_DIR}/esp32_nvs.c
    ${MICROPY_PORT_DIR}/esp32_partition.c
    ${MICROPY_PORT_DIR}/esp32_rmt.c
    ${MICROPY_PORT_DIR}/esp32_ulp.c
    ${MICROPY_PORT_DIR}/modesp32.c
    ${MICROPY_PORT_DIR}/espneopixel.c
    ${MICROPY_PORT_DIR}/machine_hw_spi.c
    ${MICROPY_PORT_DIR}/machine_wdt.c
    ${MICROPY_PORT_DIR}/mpthreadport.c
    ${MICROPY_PORT_DIR}/machine_rtc.c
    #${MICROPY_PORT_DIR}/machine_sdcard.c
)

set(MICROPY_SOURCE_QSTR
    ${MICROPY_SOURCE_PY}
    ${MICROPY_SOURCE_EXTMOD}
    ${MICROPY_SOURCE_LIB}
    ${MICROPY_SOURCE_PORT}
    ${COMPONENT_DIR}/vfs_fat/
    ${EXTMODS}
    ${COMPONENT_DIR}/mpconfigoverrides.h
)

set(IDF_COMPONENTS
    app_update
    bootloader_support
    bt
    driver
    esp32
    esp_common
    esp_eth
    esp_event
    esp_ringbuf
    esp_rom
    esp_wifi
    freertos
    heap
    log
    lwip
    mbedtls
    mdns
    newlib
    nvs_flash
    sdmmc
    soc
    spi_flash
    tcpip_adapter
    ulp
    vfs
    xtensa
    fatfs
    wear_levelling
    spiffs
    driver_sdcard
)

if(IDF_VERSION_MINOR GREATER_EQUAL 1)
    list(APPEND IDF_COMPONENTS esp_netif)
endif()

if(IDF_VERSION_MINOR GREATER_EQUAL 2)
    list(APPEND IDF_COMPONENTS esp_system)
    list(APPEND IDF_COMPONENTS esp_timer)
endif()

if(IDF_VERSION_MINOR GREATER_EQUAL 3)
    list(APPEND IDF_COMPONENTS esp_hw_support)
    list(APPEND IDF_COMPONENTS esp_pm)
    list(APPEND IDF_COMPONENTS hal)
endif()

# Register the main IDF component.
idf_component_register(
    SRCS
        ${MICROPY_SOURCE_PY}
        ${MICROPY_SOURCE_EXTMOD}
        ${MICROPY_SOURCE_LIB}
        ${MICROPY_SOURCE_DRIVERS}
        ${MICROPY_SOURCE_PORT}
        ${COMPONENT_DIR}/mpconfigoverrides.h
    INCLUDE_DIRS
        ${MICROPY_DIR}
        ${MICROPY_PORT_DIR}
        ${MICROPY_BOARD_DIR}
        ${CMAKE_BINARY_DIR}
        ${COMPONENT_DIR}
        ${COMPONENT_DIR}/vfs_fat
    REQUIRES
        ${IDF_COMPONENTS}
)

add_custom_command(
    OUTPUT
        ${COMPONENT_DIR}/mpconfigoverrides.h
    COMMAND echo extmods: ${EXTMODS} &&
            python3 mpconfigover_generator.py -b ./ \"${EXTMODS_NAMES}\" &&
            cp mpconfigport.h micropython/ports/esp32/
    WORKING_DIRECTORY ${COMPONENT_DIR}
)

# Set the MicroPython target as the current (main) IDF component target.
set(MICROPY_TARGET ${COMPONENT_TARGET})

# Define mpy-cross flags, for use with frozen code.
set(MICROPY_CROSS_FLAGS -march=xtensawin)

# Set compile options for this port.
target_compile_definitions(${MICROPY_TARGET} PUBLIC
    MICROPY_ESP_IDF_4=1
)

# Disable some warnings to keep the build output clean.
target_compile_options(${MICROPY_TARGET} PUBLIC
    -Wno-clobbered
    -Wno-deprecated-declarations
    -Wno-missing-field-initializers
)

# Collect all of the include directories and compile definitions for the IDF components.
foreach(comp ${IDF_COMPONENTS})
    get_target_property(type __idf_${comp} TYPE)
    set(_inc OFF)
    set(_def OFF)
    if(${type} STREQUAL STATIC_LIBRARY)
        get_target_property(_inc __idf_${comp} INCLUDE_DIRECTORIES)
        get_target_property(_def __idf_${comp} COMPILE_DEFINITIONS)
    elseif(${type} STREQUAL INTERFACE_LIBRARY)
        get_target_property(_inc __idf_${comp} INTERFACE_INCLUDE_DIRECTORIES)
        get_target_property(_def __idf_${comp} INTERFACE_COMPILE_DEFINITIONS)
    endif()
    if(_inc)
        list(APPEND MICROPY_CPP_INC_EXTRA ${_inc})
    endif()
    if(_def)
        list(APPEND MICROPY_CPP_DEF_EXTRA ${_def})
    endif()
endforeach()

if(IDF_VERSION_MINOR GREATER_EQUAL 2)
    # These paths cannot currently be found by the IDF_COMPONENTS search loop above,
    # so add them explicitly.
    list(APPEND MICROPY_CPP_INC_EXTRA ${IDF_PATH}/components/soc/soc/${IDF_TARGET}/include)
    list(APPEND MICROPY_CPP_INC_EXTRA ${IDF_PATH}/components/soc/soc/include)
endif()

# Include the main MicroPython cmake rules.
include(${MICROPY_DIR}/py/mkrules.cmake)
