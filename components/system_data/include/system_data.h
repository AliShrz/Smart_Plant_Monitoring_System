#include <stdbool.h>
#include <stdint.h>


typedef struct
{
    uint8_t plant_id;

    char time[12];   // "2:13 AM"

    char date[16];  // "13 Aug 2024"

    uint8_t soil_moisture_percent;

    float temperature_c;

    float humidity_percent;

    float pressure_hpa;

    float light_lux;

    bool wifi_connected;

    int8_t wifi_rssi;

    char wifi_ip[16];

} system_data_t;