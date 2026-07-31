#include <stdio.h>

#include "soil_moisture.h"
#include "i2c_bus.h"
#include "aht20.h"
#include "bmp280.h"
#include "bh1750.h"
#include "display.h"
// #include "esp_lcd_panel_ops.h"
#include "display_font_5x7.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define COLOR_BLACK 0x0000
#define COLOR_WHITE 0xFFFF
#define COLOR_RED   0xF800
#define COLOR_GREEN 0x07E0
#define COLOR_BLUE  0x001F
#define COLOR_YELLOW 0xFFE0
#define COLOR_CYAN 0x07FF
#define COLOR_MAGENTA 0xF81F
#define COLOR_ORANGE 0xFD20
#define COLOR_PURPLE 0x780F

static const char *TAG = "main";

void app_main(void)
{
    esp_err_t ret = soil_moisture_init();
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

    display_fill(COLOR_BLACK); // Draw a blank bitmap (black screen)
    vTaskDelay(pdMS_TO_TICKS(1000));

    display_fill(COLOR_WHITE); // Draw a blank bitmap (white screen)
    vTaskDelay(pdMS_TO_TICKS(1000));
    
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


    aht20_data_t sensor_data;
    bmp280_data_t bmp280_data;
    bh1750_data_t bh1750_data;

    while (1)
    {

        int soil_moisture = soil_moisture_read_raw();
        // printf("Raw value: %d\n", value);

        display_printf(2, 10, &display_font_5x7, COLOR_BLACK, COLOR_WHITE, DISPLAY_BACKGROUND_SOLID, "Soil Moisture: %d", soil_moisture);

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

        // display_draw_string(15, 50, "Soil Moisture:", &display_font_5x7, COLOR_BLACK, COLOR_WHITE,DISPLAY_BACKGROUND_TRANSPARENT);
        // display_draw_string(25, 70, "Raw Value:", &display_font_5x7, COLOR_BLACK, COLOR_WHITE,DISPLAY_BACKGROUND_TRANSPARENT);




        vTaskDelay(pdMS_TO_TICKS(500));
    }
}