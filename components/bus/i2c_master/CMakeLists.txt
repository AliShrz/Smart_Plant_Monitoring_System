idf_component_register(
    SRCS
        "i2c_bus.c"

    INCLUDE_DIRS
        "include"

    PRIV_REQUIRES
        esp_driver_i2c
)