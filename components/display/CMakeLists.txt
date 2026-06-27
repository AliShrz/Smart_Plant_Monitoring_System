idf_component_register(
    SRCS "display.c"
    INCLUDE_DIRS "."
    PRIV_REQUIRES esp_lcd driver
)