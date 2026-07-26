idf_component_register(
    SRCS 
        "display.c"
        "display_font.c"
        "display_font_5x7.c"
    INCLUDE_DIRS 
        "."
    PRIV_REQUIRES 
        esp_lcd driver
)