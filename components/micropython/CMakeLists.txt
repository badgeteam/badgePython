get_filename_component(MICROPY_DIR ${PROJECT_DIR}/components/micropython/micropython ABSOLUTE)
get_filename_component(MICROPY_PORT_DIR ${PROJECT_DIR}/components/micropython/micropython/ports/esp32 ABSOLUTE)
set(MICROPY_FROZEN_MANIFEST ${PROJECT_DIR}/manifests/${CONFIG_MICROPY_FROZEN_MANIFEST})

# Include core source components.
include(${MICROPY_DIR}/py/py.cmake)

if(NOT CMAKE_BUILD_EARLY_EXPANSION)
    include(${MICROPY_DIR}/py/usermod.cmake)
    include(${MICROPY_DIR}/extmod/extmod.cmake)

    list(REMOVE_ITEM MICROPY_SOURCE_EXTMOD
        ${MICROPY_EXTMOD_DIR}/vfs.c
        ${MICROPY_EXTMOD_DIR}/vfs_blockdev.c
        ${MICROPY_EXTMOD_DIR}/vfs_fat.c
        ${MICROPY_EXTMOD_DIR}/vfs_fat_diskio.c
        ${MICROPY_EXTMOD_DIR}/vfs_fat_file.c
        ${MICROPY_EXTMOD_DIR}/vfs_lfs.c
        ${MICROPY_EXTMOD_DIR}/vfs_posix.c
        ${MICROPY_EXTMOD_DIR}/vfs_posix_file.c
        ${MICROPY_EXTMOD_DIR}/vfs_reader.c    
    )

    list(APPEND MICROPY_SOURCE_EXTMOD
        ${COMPONENT_DIR}/vfs_fat/vfs_native_file.c
        ${COMPONENT_DIR}/vfs_fat/vfs_native_misc.c
        ${COMPONENT_DIR}/vfs_fat/vfs_native.c
        ${COMPONENT_DIR}/vfs_fat/vfs_reader.c
        ${COMPONENT_DIR}/vfs_fat/vfs.c
    )
endif()

set(MICROPY_QSTRDEFS_PORT
    ${MICROPY_PORT_DIR}/qstrdefsport.h
)

set(MICROPY_SOURCE_SHARED
    ${MICROPY_DIR}/shared/readline/readline.c
    ${MICROPY_DIR}/shared/netutils/netutils.c
    ${MICROPY_DIR}/shared/timeutils/timeutils.c
    ${MICROPY_DIR}/shared/runtime/interrupt_char.c
    ${MICROPY_DIR}/shared/runtime/stdout_helpers.c
    ${MICROPY_DIR}/shared/runtime/sys_stdio_mphal.c
    ${MICROPY_DIR}/shared/runtime/pyexec.c
)

set(MICROPY_SOURCE_LIB
    ${MICROPY_DIR}/lib/littlefs/lfs1.c
    ${MICROPY_DIR}/lib/littlefs/lfs1_util.c
    ${MICROPY_DIR}/lib/littlefs/lfs2.c
    ${MICROPY_DIR}/lib/littlefs/lfs2_util.c
    ${MICROPY_DIR}/lib/mbedtls_errors/mp_mbedtls_errors.c
    # ${MICROPY_DIR}/lib/oofatfs/ff.c
    # ${MICROPY_DIR}/lib/oofatfs/ffunicode.c
)
if(IDF_TARGET STREQUAL "esp32c3")
    list(APPEND MICROPY_SOURCE_LIB ${MICROPY_DIR}/shared/runtime/gchelper_generic.c)
endif()

set(MICROPY_SOURCE_DRIVERS
    ${MICROPY_DIR}/drivers/bus/softspi.c
    ${MICROPY_DIR}/drivers/dht/dht.c
)

set(MICROPY_SOURCE_PORT
    ${COMPONENT_DIR}/main.c
    ${COMPONENT_DIR}/modconsts.c
    ${MICROPY_PORT_DIR}/main.c
    ${MICROPY_PORT_DIR}/uart.c
    ${MICROPY_PORT_DIR}/usb.c
    ${MICROPY_PORT_DIR}/usb_serial_jtag.c
    ${MICROPY_PORT_DIR}/gccollect.c
    ${MICROPY_PORT_DIR}/mphalport.c
    #${MICROPY_PORT_DIR}/fatfs_port.c
    ${MICROPY_PORT_DIR}/help.c
    ${MICROPY_PORT_DIR}/modutime.c
    #${MICROPY_PORT_DIR}/moduos.c
    ${COMPONENT_DIR}/vfs_fat/moduos.c
    ${MICROPY_PORT_DIR}/machine_bitstream.c
    ${MICROPY_PORT_DIR}/machine_timer.c
    ${MICROPY_PORT_DIR}/machine_pin.c
    ${MICROPY_PORT_DIR}/machine_touchpad.c
    ${MICROPY_PORT_DIR}/machine_adc.c
    ${MICROPY_PORT_DIR}/machine_dac.c
    ${COMPONENT_DIR}/esp32/machine_i2c.c
    ${MICROPY_PORT_DIR}/machine_i2s.c
    ${MICROPY_PORT_DIR}/machine_uart.c
    ${MICROPY_PORT_DIR}/modmachine.c
    ${MICROPY_PORT_DIR}/modnetwork.c
    ${MICROPY_PORT_DIR}/network_lan.c
    ${MICROPY_PORT_DIR}/network_ppp.c
    ${MICROPY_PORT_DIR}/network_wlan.c
    ${MICROPY_PORT_DIR}/mpnimbleport.c
    ${MICROPY_PORT_DIR}/modsocket.c
    ${MICROPY_PORT_DIR}/modesp.c
    ${MICROPY_PORT_DIR}/esp32_nvs.c
    ${MICROPY_PORT_DIR}/esp32_partition.c
    ${MICROPY_PORT_DIR}/esp32_rmt.c
    ${MICROPY_PORT_DIR}/esp32_ulp.c
    ${MICROPY_PORT_DIR}/modesp32.c
    ${COMPONENT_DIR}/esp32/machine_hw_spi.c
    ${MICROPY_PORT_DIR}/machine_wdt.c
    ${MICROPY_PORT_DIR}/mpthreadport.c
    ${MICROPY_PORT_DIR}/machine_rtc.c
    #${MICROPY_PORT_DIR}/machine_sdcard.c
)

set(MICROPY_SOURCE_QSTR
    ${MICROPY_SOURCE_PY}
    ${MICROPY_SOURCE_EXTMOD}
    ${MICROPY_SOURCE_USERMOD}
    ${MICROPY_SOURCE_SHARED}
    ${MICROPY_SOURCE_LIB}
    ${MICROPY_SOURCE_PORT}
    ${MICROPY_SOURCE_BOARD}
    ${COMPONENT_DIR}/modconsts.c
    ${EXTMODS}
    ${COMPONENT_DIR}/mpconfigoverrides.h
)

set(IDF_COMPONENTS
    app_update
    bootloader_support
    bt
    driver
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
    buses
    wpa_supplicant
)

if(IDF_VERSION_MINOR GREATER_EQUAL 1 OR IDF_VERSION_MAJOR GREATER_EQUAL 5)
    list(APPEND IDF_COMPONENTS esp_netif)
endif()

if(IDF_VERSION_MINOR GREATER_EQUAL 2 OR IDF_VERSION_MAJOR GREATER_EQUAL 5)
    list(APPEND IDF_COMPONENTS esp_system)
    list(APPEND IDF_COMPONENTS esp_timer)
endif()

if(IDF_VERSION_MINOR GREATER_EQUAL 3 OR IDF_VERSION_MAJOR GREATER_EQUAL 5)
    list(APPEND IDF_COMPONENTS esp_hw_support)
    list(APPEND IDF_COMPONENTS esp_pm)
    list(APPEND IDF_COMPONENTS hal)
endif()

if(IDF_TARGET STREQUAL "esp32")
    list(APPEND IDF_COMPONENTS esp32)
elseif(IDF_TARGET STREQUAL "esp32c3")
    list(APPEND IDF_COMPONENTS esp32c3)
    list(APPEND IDF_COMPONENTS riscv)
elseif(IDF_TARGET STREQUAL "esp32s2")
    list(APPEND IDF_COMPONENTS esp32s2)
    list(APPEND IDF_COMPONENTS tinyusb)
elseif(IDF_TARGET STREQUAL "esp32s3")
    list(APPEND IDF_COMPONENTS esp32s3)
    list(APPEND IDF_COMPONENTS tinyusb)
endif()

# Register the main IDF component.
idf_component_register(
    SRCS
        ${MICROPY_SOURCE_PY}
        ${MICROPY_SOURCE_EXTMOD}
        ${MICROPY_SOURCE_SHARED}
        ${MICROPY_SOURCE_LIB}
        ${MICROPY_SOURCE_DRIVERS}
        ${MICROPY_SOURCE_PORT}
        ${MICROPY_SOURCE_BOARD}
    INCLUDE_DIRS
        ${MICROPY_INC_CORE}
        ${MICROPY_INC_USERMOD}
        ${MICROPY_PORT_DIR}
        ${MICROPY_BOARD_DIR}
        ${CMAKE_BINARY_DIR}
        "./"
        "vfs_fat/"
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
    DEPENDS ${EXTMODS}
)

# Set the MicroPython target as the current (main) IDF component target.
set(MICROPY_TARGET ${COMPONENT_TARGET})

# Define mpy-cross flags, for use with frozen code.
set(MICROPY_CROSS_FLAGS -march=xtensawin)

# Set compile options for this port.
target_compile_definitions(${MICROPY_TARGET} PUBLIC
    ${MICROPY_DEF_CORE}
    MICROPY_ESP_IDF_4=1
)

# Disable some warnings to keep the build output clean.
target_compile_options(${MICROPY_TARGET} PUBLIC
    -Wno-clobbered
    -Wno-deprecated-declarations
    -Wno-missing-field-initializers
)

# Add additional extmod and usermod components.
target_link_libraries(${MICROPY_TARGET} micropy_extmod_btree)
target_link_libraries(${MICROPY_TARGET} usermod)


# Collect all of the include directories and compile definitions for the IDF components.
foreach(comp ${IDF_COMPONENTS})
    micropy_gather_target_properties(__idf_${comp})
endforeach()

if(IDF_VERSION_MINOR GREATER_EQUAL 2 OR IDF_VERSION_MAJOR GREATER_EQUAL 5)
    # These paths cannot currently be found by the IDF_COMPONENTS search loop above,
    # so add them explicitly.
    list(APPEND MICROPY_CPP_INC_EXTRA ${IDF_PATH}/components/soc/soc/${IDF_TARGET}/include)
    list(APPEND MICROPY_CPP_INC_EXTRA ${IDF_PATH}/components/soc/soc/include)
    if(IDF_VERSION_MINOR GREATER_EQUAL 3)
        list(APPEND MICROPY_CPP_INC_EXTRA ${IDF_PATH}/components/tinyusb/additions/include)
        list(APPEND MICROPY_CPP_INC_EXTRA ${IDF_PATH}/components/tinyusb/tinyusb/src)
    endif()
endif()

# Include the main MicroPython cmake rules.
include(${MICROPY_DIR}/py/mkrules.cmake)
