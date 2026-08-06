idf_component_register(
    SRCS
        "display_ui.c"

    INCLUDE_DIRS
        "include"

    REQUIRES
        display
        esp_netif
)