idf_component_register(SRCS "modwpa2enterprise.c"
                       INCLUDE_DIRS "."
                       REQUIRES lwip micropython wpa_supplicant)
