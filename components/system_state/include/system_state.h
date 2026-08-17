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
    int8_t wifi_rssi;
    char wifi_ip[16];

} system_data_t;

typedef struct
{
    bool i2c_bus_init;
    bool nvs_init;
    bool netif_init;
    bool event_loop_init;
} core_status_t;

typedef struct
{
    bool soil_moisture_init;
    bool aht20_init;
    bool bmp280_init;
    bool bh1750_init;

} sensor_status_t;

typedef struct
{
    bool wifi_init;
    bool wifi_connected;
} wifi_status_t;

typedef struct
{
    bool display_init;
    bool display_ui_init;
} display_status_t;

typedef struct
{
    bool time_init;
    bool time_synced;
} time_status_t;

typedef struct
{
    core_status_t core;
    sensor_status_t sensors;
    wifi_status_t wifi;
    display_status_t display;
    time_status_t time;

} system_status_t;

typedef struct
{
    system_data_t data;
    system_status_t status;

} system_state_t;