#include <stdio.h>

// #include "soil_moisture.h"
#include "display.h"
// #include "esp_lcd_panel_ops.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define COLOR_BLACK 0x0000
#define COLOR_WHITE 0xFFFF
#define COLOR_RED   0xF800
#define COLOR_GREEN 0x07E0
#define COLOR_BLUE  0x001F


void app_main(void)
{
    // soil_moisture_init();

    esp_err_t ret = display_init();
    if (ret != ESP_OK)
    {
        printf("Failed to initialize display: %s\n", esp_err_to_name(ret));
        return;
    }

    while (1)
    {
        // int value = soil_moisture_read_raw();
        // printf("Raw value: %d\n", value);
        display_fill(COLOR_BLACK); // Draw a blank bitmap (black screen)
        vTaskDelay(pdMS_TO_TICKS(1000));

        display_fill(COLOR_WHITE); // Draw a blank bitmap (white screen)
        vTaskDelay(pdMS_TO_TICKS(1000));

        display_fill(COLOR_RED); // Draw a blank bitmap (red screen)
        vTaskDelay(pdMS_TO_TICKS(1000));

        display_fill(COLOR_GREEN); // Draw a blank bitmap (green screen)
        vTaskDelay(pdMS_TO_TICKS(1000));

        display_fill(COLOR_BLUE); // Draw a blank bitmap (blue screen)
        vTaskDelay(pdMS_TO_TICKS(1000));
        
    }
}