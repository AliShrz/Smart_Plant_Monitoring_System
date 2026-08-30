idf_component_register(
    SRCS
        "main.c"

    INCLUDE_DIRS
        "."

    REQUIRES
        display
        soil_moisture
        aht20
        bmp280
        bh1750
        nvs_flash
        esp_netif
        esp_event
        wifi
        display_ui
        time
        system_state
        cloud
)

