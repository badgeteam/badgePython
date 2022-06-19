idf_component_register(SRCS "modwpa2enterprise.c" "wifi_connect.c" "wifi_connection.c"
                       INCLUDE_DIRS "." "include"
                       REQUIRES lwip micropython wpa_supplicant)
