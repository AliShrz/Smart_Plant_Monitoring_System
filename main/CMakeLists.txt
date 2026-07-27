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
)

