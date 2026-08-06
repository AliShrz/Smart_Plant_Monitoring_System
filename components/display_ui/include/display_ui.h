#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "esp_err.h"
#include "esp_netif_ip_addr.h"

#include "display.h"

typedef struct
{
    uint8_t soil_moisture_percent;

    float temperature_c;

    float humidity_percent;

    float pressure_hpa;

    float light_lux;

    bool wifi_connected;

    int8_t wifi_rssi;

    esp_ip4_addr_t wifi_ip;

} display_ui_data_t;


esp_err_t display_ui_init(void);

esp_err_t display_ui_show(const display_ui_data_t *data);

esp_err_t display_ui_deinit(void);