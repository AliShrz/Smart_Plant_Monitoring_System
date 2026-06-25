idf_component_register(
    SRCS "soil_moisture.c"
    INCLUDE_DIRS "."
    PRIV_REQUIRES esp_adc
)