idf_component_register(
  SRCS "buses.c"
  INCLUDE_DIRS include
  REQUIRES
        micropython
        driver
        freertos
)
