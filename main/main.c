#include <stdio.h>

#include "soil_moisture.h"
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



void app_main(void)
{
    soil_moisture_init();

    esp_err_t ret = display_init();
    if (ret != ESP_OK)
    {
        printf("Failed to initialize display: %s\n", esp_err_to_name(ret));
        return;
    }

    display_fill(COLOR_BLACK); // Draw a blank bitmap (black screen)
    vTaskDelay(pdMS_TO_TICKS(1000));

    display_fill(COLOR_WHITE); // Draw a blank bitmap (white screen)
    vTaskDelay(pdMS_TO_TICKS(1000));

    while (1)
    {

        int value = soil_moisture_read_raw();
        // printf("Raw value: %d\n", value);

        // display_draw_string(15, 50, "Soil Moisture:", &display_font_5x7, COLOR_BLACK, COLOR_WHITE,DISPLAY_BACKGROUND_TRANSPARENT);
        // display_draw_string(25, 70, "Raw Value:", &display_font_5x7, COLOR_BLACK, COLOR_WHITE,DISPLAY_BACKGROUND_TRANSPARENT);

        display_printf(5, 90, &display_font_5x7, COLOR_BLACK, COLOR_WHITE, DISPLAY_BACKGROUND_SOLID, "Soil Moisture: %d", value);


        vTaskDelay(pdMS_TO_TICKS(500));
    }
}