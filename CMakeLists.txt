# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
# cmake_minimum_required(VERSION 3.22)

# include($ENV{IDF_PATH}/tools/cmake/project.cmake)
# project(plant_monitor)


cmake_minimum_required(VERSION 3.22)

set(EXTRA_COMPONENT_DIRS

    # Display
    components/display
    components/display_ui

    # Bus
    components/bus/i2c_bus

    # Sensors
    components/sensors/soil_moisture
    components/sensors/aht20
    components/sensors/bmp280
    components/sensors/bh1750

    # WiFi
    components/wifi

    # time
    components/time

    # Data flow
    components/system_data
)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(plant_monitor)