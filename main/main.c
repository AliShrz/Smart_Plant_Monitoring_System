#include <stdio.h>

#include "soil_moisture.h"
#include "i2c_bus.h"
#include "aht20.h"
#include "bmp280.h"
#include "bh1750.h"
#include "display.h"
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
    
    ESP_ERROR_CHECK(nvs_flash_init());

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ret = wifi_manager_init();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize WiFi: %s", esp_err_to_name(ret));
        return;
    }

    ret = wifi_manager_connect(SSID, PASS);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to connect to WiFi: %s", esp_err_to_name(ret));
        return;
    }


    ret = soil_moisture_init();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize soil moisture sensor: %s", esp_err_to_name(ret));
        return;
    }

    ret = display_init();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize display: %s", esp_err_to_name(ret));
        return;
    }
    
    i2c_master_bus_handle_t bus_handle;

    i2c_bus_config_t bus_config =
    {
        .port = I2C_NUM_0,
        .sda = GPIO_NUM_21,
        .scl = GPIO_NUM_22,
        .enable_internal_pullup = true,
        .glitch_ignore_cnt = 0,
    };

    ret = i2c_bus_init(&bus_config, &bus_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize I2C bus: %s", esp_err_to_name(ret));
        return;
    }

    ret = aht20_init(bus_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize AHT20 sensor: %s",
                esp_err_to_name(ret));
        return;
    }

    ret = bmp280_init(bus_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize BMP280 sensor: %s",
                esp_err_to_name(ret));
        return;
    }

    ret = bh1750_init(bus_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to initialize BH1750 sensor: %s",
                esp_err_to_name(ret));
        return;
    }

    soil_moisture_data_t soil_moisture_data;
    aht20_data_t sensor_data;
    bmp280_data_t bmp280_data;
    bh1750_data_t bh1750_data;

    display_fill(COLOR_BLACK); // Draw a blank bitmap (black screen)
    vTaskDelay(pdMS_TO_TICKS(1000));

    display_fill(COLOR_WHITE); // Draw a blank bitmap (white screen)
    vTaskDelay(pdMS_TO_TICKS(1000));

    while (1)
    {

        ret = soil_moisture_read(&soil_moisture_data);
        if (ret == ESP_OK)
        {
            display_printf(2, 10, &display_font_5x7, COLOR_BLACK, COLOR_WHITE, DISPLAY_BACKGROUND_SOLID, "Soil Moisture: %.2f %%", soil_moisture_data.moisture_percentage);
            printf("Raw value: %.2f %%\n", soil_moisture_data.moisture_percentage);
        }
        else
        {
            ESP_LOGE(TAG,
                    "Failed to read soil moisture: %s",
                    esp_err_to_name(ret));
        }


        ret = aht20_read(&sensor_data);

        if (ret == ESP_OK)
        {
            display_printf(2, 30, &display_font_5x7, COLOR_BLACK, COLOR_WHITE, DISPLAY_BACKGROUND_SOLID, "Temperature: %.2f C", sensor_data.temperature);
            display_printf(2, 50, &display_font_5x7, COLOR_BLACK, COLOR_WHITE, DISPLAY_BACKGROUND_SOLID, "Humidity: %.2f %%", sensor_data.humidity);
            printf(
                "Temperature: %.2f C, Humidity: %.2f %%\n",
                sensor_data.temperature,
                sensor_data.humidity
            );
        }
        else
        {
            ESP_LOGE(TAG, "Failed to read AHT20: %s", esp_err_to_name(ret));
        }

        ret = bmp280_read(&bmp280_data);
        if (ret == ESP_OK)
        {
            float hpa_pressure = bmp280_data.pressure / 100.0; // Convert Pa to hPa
            float pressure_atm = bmp280_data.pressure / 101325.0f; // Convert Pa to atm
            display_printf(2, 70, &display_font_5x7, COLOR_BLACK, COLOR_WHITE, DISPLAY_BACKGROUND_SOLID, "Pressure: %.2f hPa", hpa_pressure);
            display_printf(2, 90, &display_font_5x7, COLOR_BLACK, COLOR_WHITE, DISPLAY_BACKGROUND_SOLID, "Temperature: %.2f C", bmp280_data.temperature);
            display_printf(2, 110, &display_font_5x7, COLOR_BLACK, COLOR_WHITE, DISPLAY_BACKGROUND_SOLID, "Atmosphere: %.2f atm", pressure_atm);
            printf(
                "Pressure: %.2f hPa, Temperature: %.2f C, Atmosphere: %.2f atm\n",
                hpa_pressure,
                bmp280_data.temperature,
                pressure_atm
            );
        }
        else
        {
            ESP_LOGE(TAG, "Failed to read BMP280: %s", esp_err_to_name(ret));
        }

        ret = bh1750_read(&bh1750_data);
        if (ret == ESP_OK)
        {
            display_printf(2, 130, &display_font_5x7, COLOR_BLACK, COLOR_WHITE, DISPLAY_BACKGROUND_SOLID, "Lux: %.2f lx", bh1750_data.lux);
            printf("Lux: %.2f lx\n", bh1750_data.lux);
        }
        else
        {
            ESP_LOGE(TAG, "Failed to read BH1750: %s", esp_err_to_name(ret));
        }

        if (wifi_manager_is_connected())
        {
            ESP_LOGI(TAG, "Wi-Fi Connected");
            display_printf(2, 150, &display_font_5x7, COLOR_BLACK, COLOR_WHITE, DISPLAY_BACKGROUND_SOLID, "WiFi connected");
        }
        else
        {
            ESP_LOGW(TAG, "Wi-Fi Disconnected");
            display_printf(2, 150, &display_font_5x7, COLOR_BLACK, COLOR_WHITE, DISPLAY_BACKGROUND_SOLID, "WiFi disconnected");
        }


        vTaskDelay(pdMS_TO_TICKS(500));
    }
}