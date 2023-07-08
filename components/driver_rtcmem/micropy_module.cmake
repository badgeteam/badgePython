add_library(driver_rtcmem INTERFACE)

target_sources(driver_rtcmem INTERFACE
    "${CMAKE_CURRENT_LIST_DIR}/modrtcmem.c"
    )

target_include_directories(driver_rtcmem INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
    )

target_link_libraries(usermod INTERFACE driver_rtcmem)

message(STATUS "rtcmem enabled")
