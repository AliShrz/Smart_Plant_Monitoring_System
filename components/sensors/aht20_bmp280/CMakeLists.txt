idf_component_register(
    SRCS
        "aht20_bmp280.c"

    INCLUDE_DIRS
        "."

    REQUIRES
        i2c_master
)