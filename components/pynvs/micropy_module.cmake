add_library(driver_nvs INTERFACE)

target_sources(driver_nvs INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/modnvs.c"
    )

target_include_directories(driver_nvs INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
    )

target_link_libraries(usermod INTERFACE driver_nvs)

message(STATUS "rtcmem enabled")
