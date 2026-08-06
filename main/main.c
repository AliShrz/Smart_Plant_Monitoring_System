#include <stdio.h>

#include "esp_check.h"
#include "esp_log.h"
// #include "soil_moisture.h"
// #include "i2c_bus.h"
// #include "aht20.h"
// #include "bmp280.h"
// #include "bh1750.h"
#include "display.h"
#include "display_ui.h"
#include "wifi_manager.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
// #include "esp_lcd_panel_ops.h"
#include "display_font_5x7.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define SSID "HUAWEI-2.4G-k4JK_ext"
#define PASS "3jBc8cpR" // 3jBc8cpR

static const char *TAG = "main";

void app_main(void)
{
    esp_err_t ret;
    
    // ESP_ERROR_CHECK(nvs_flash_init());

    // ESP_ERROR_CHECK(esp_netif_init());

    // ESP_ERROR_CHECK(esp_event_loop_create_default());

    // ret = wifi_manager_init();
    // if (ret != ESP_OK)
    // {
    //     ESP_LOGE(TAG, "Failed to initialize WiFi: %s", esp_err_to_name(ret));
    //     return;
    // }

    // ret = wifi_manager_connect(SSID, PASS);
    // if (ret != ESP_OK)
    // {
    //     ESP_LOGE(TAG, "Failed to connect to WiFi: %s", esp_err_to_name(ret));
    //     return;
    // }

    // esp_ip4_addr_t ip;

    // if (wifi_manager_get_ip(&ip) == ESP_OK)
    // {
    //     ESP_LOGI(TAG, "IP: " IPSTR, IP2STR(&ip));
    // }

    // ESP_LOGI(
    //     TAG,
    //     "RSSI: %d dBm",
    //     wifi_manager_get_rssi());

    // /*******************/

    // ret = soil_moisture_init();
    // if (ret != ESP_OK)
    // {
    //     ESP_LOGE(TAG, "Failed to initialize soil moisture sensor: %s", esp_err_to_name(ret));
    //     return;
    // }

    ret = display_init();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize display: %s", esp_err_to_name(ret));
        return;
    }
    
    // i2c_master_bus_handle_t bus_handle;

    // i2c_bus_config_t bus_config =
    // {
    //     .port = I2C_NUM_0,
    //     .sda = GPIO_NUM_21,
    //     .scl = GPIO_NUM_22,
    //     .enable_internal_pullup = true,
    //     .glitch_ignore_cnt = 0,
    // };

    // ret = i2c_bus_init(&bus_config, &bus_handle);
    // if (ret != ESP_OK)
    // {
    //     ESP_LOGE(TAG, "Failed to initialize I2C bus: %s", esp_err_to_name(ret));
    //     return;
    // }

    // ret = aht20_init(bus_handle);
    // if (ret != ESP_OK)
    // {
    //     ESP_LOGE(TAG, "Failed to initialize AHT20 sensor: %s",
    //             esp_err_to_name(ret));
    //     return;
    // }

    // ret = bmp280_init(bus_handle);
    // if (ret != ESP_OK)
    // {
    //     ESP_LOGE(TAG, "Failed to initialize BMP280 sensor: %s",
    //             esp_err_to_name(ret));
    //     return;
    // }

    // ret = bh1750_init(bus_handle);
    // if (ret != ESP_OK)
    // {
    //     ESP_LOGE(TAG, "Failed to initialize BH1750 sensor: %s",
    //             esp_err_to_name(ret));
    //     return;
    // }

    // soil_moisture_data_t soil_moisture_data;
    // aht20_data_t sensor_data;
    // bmp280_data_t bmp280_data;
    // bh1750_data_t bh1750_data;

    display_fill(COLOR_BLACK); // Draw a blank bitmap (black screen)
    vTaskDelay(pdMS_TO_TICKS(1000));

    display_fill(COLOR_WHITE); // Draw a blank bitmap (white screen)
    vTaskDelay(pdMS_TO_TICKS(1000));

    ret = display_ui_init();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize display UI: %s", esp_err_to_name(ret));
        return;
    }
    
    display_ui_data_t ui = {
        .plant_id = 1,

        .time = "2:13 AM",

        .date = "06 Aug 2026",

        .wifi_connected = true,
        };

    // int8_t wifi_rssi;
    // char ip_string[16];
    // uint8_t count = 0;

    while (1)
    {


    ret = display_ui_show(&ui);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to show display UI: %s", esp_err_to_name(ret));
        return;
    }

    // ret =     display_draw_hline(
    //     0,
    //     24,
    //     127,
    //     COLOR_BLACK);
    // if (ret != ESP_OK)
    // {
    //     ESP_LOGE(TAG, "Failed to draw horizontal line: %s", esp_err_to_name(ret));
    //     return;
    // }
    else {
        ESP_LOGI(TAG, "Display UI updated successfully.");
    }


        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}