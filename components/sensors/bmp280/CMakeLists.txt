idf_component_register(
    SRCS
        "bmp280.c"

    INCLUDE_DIRS
        "include"

    REQUIRES
        i2c_bus
        esp_driver_i2c
)