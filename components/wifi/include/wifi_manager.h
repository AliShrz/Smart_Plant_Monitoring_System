#pragma once

#include <stdbool.h>
#include "esp_netif_ip_addr.h"
#include "esp_err.h"

/*
Public API
──────────
wifi_manager_init()
wifi_manager_connect()
wifi_manager_disconnect()
wifi_manager_is_connected()
wifi_manager_get_ip()
wifi_manager_get_rssi()
wifi_manager_deinit()
*/



esp_err_t wifi_manager_init(void);

esp_err_t wifi_manager_connect(const char *ssid, const char *password);

esp_err_t wifi_manager_disconnect(void);

bool wifi_manager_is_connected(void);

int8_t wifi_manager_get_rssi(void);

esp_err_t wifi_manager_get_ip(esp_ip4_addr_t *ip);

esp_err_t wifi_manager_deinit(void);


