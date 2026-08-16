idf_component_register(
    SRCS
        "display_ui.c"
        "display_ui_widgets.c"

    INCLUDE_DIRS
        "include"

    REQUIRES
        display
        esp_netif
        system_data
)