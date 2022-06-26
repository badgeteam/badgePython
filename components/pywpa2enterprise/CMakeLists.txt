idf_component_register(SRCS "modwpa2enterprise.c"
                       INCLUDE_DIRS "." "include"
                       REQUIRES lwip micropython wpa_supplicant)
