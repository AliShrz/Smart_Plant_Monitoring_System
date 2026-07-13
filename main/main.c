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
    // soil_moisture_init();

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

        // int value = soil_moisture_read_raw();
        // printf("Raw value: %d\n", value);
        // display_fill(COLOR_BLACK); // Draw a blank bitmap (black screen)
        // vTaskDelay(pdMS_TO_TICKS(1000));

        // display_fill(COLOR_WHITE); // Draw a blank bitmap (white screen)
        // vTaskDelay(pdMS_TO_TICKS(1000));

        // display_fill(COLOR_RED); // Draw a blank bitmap (red screen)
        // vTaskDelay(pdMS_TO_TICKS(1000));

        // display_fill(COLOR_GREEN); // Draw a blank bitmap (green screen)
        // vTaskDelay(pdMS_TO_TICKS(1000));

        // display_fill(COLOR_BLUE); // Draw a blank bitmap (blue screen)
        // vTaskDelay(pdMS_TO_TICKS(1000));

        // display_draw_line_dda(0, 0, 127, 159, COLOR_RED); // Draw a red diagonal line
        // vTaskDelay(pdMS_TO_TICKS(1000));

        // display_draw_line_dda(0, 159, 127, 0, COLOR_GREEN); // Draw a green diagonal line
        // vTaskDelay(pdMS_TO_TICKS(1000));

        // display_draw_line_dda(0, 80, 127, 80, COLOR_BLUE); // Draw a blue diagonal line
        // vTaskDelay(pdMS_TO_TICKS(1000));

        // display_draw_line_dda(64, 0, 64, 159, COLOR_BLACK); // Draw a black diagonal line
        // vTaskDelay(pdMS_TO_TICKS(1000));

        // display_draw_pixel(64, 80, COLOR_RED); // Draw a red pixel at (64, 80)
        // vTaskDelay(pdMS_TO_TICKS(1000));

        // display_fill_rect(30, 40, 50, 60, COLOR_BLUE); // Draw a blue rectangle
        // vTaskDelay(pdMS_TO_TICKS(1000));

        // display_draw_hline(10, 100, 100, COLOR_GREEN); // Draw a green horizontal line
        // vTaskDelay(pdMS_TO_TICKS(1000));

        // display_draw_vline(80, 30, 120, COLOR_RED); // Draw a red vertical line
        // vTaskDelay(pdMS_TO_TICKS(1000));

        // display_draw_rect(40, 50, 80, 90, COLOR_BLUE); // Draw a blue rectangle outline
        // vTaskDelay(pdMS_TO_TICKS(1000));
        
        // display_draw_line_int(0, 10, 127, 149, COLOR_MAGENTA); // Draw a red diagonal line
        // vTaskDelay(pdMS_TO_TICKS(1000));

        // display_draw_line_int(50, 0, 100, 130, COLOR_PURPLE); // Draw a purple diagonal line
        // vTaskDelay(pdMS_TO_TICKS(1000));

        // display_draw_line(50, 10, 100, 140, COLOR_RED); // Draw a red diagonal line
        // vTaskDelay(pdMS_TO_TICKS(1000));

        // display_draw_circle(64, 80, 30, COLOR_ORANGE); // Draw an orange circle
        // vTaskDelay(pdMS_TO_TICKS(200));

        // for (int radius = 30; radius > 15; radius--)
        // {
        //     display_draw_circle(64, 80, radius, COLOR_ORANGE); // Draw an orange circle
        //     vTaskDelay(pdMS_TO_TICKS(100));
        // }

        // display_fill_circle(64, 80, 10, COLOR_GREEN); // Draw a green circle
        // vTaskDelay(pdMS_TO_TICKS(1000));

        // display_fill_triangle(50, 50, 100, 100, 75, 125, COLOR_CYAN); // Draw a cyan filled triangle
        // vTaskDelay(pdMS_TO_TICKS(1000));

        // display_draw_triangle(50, 50, 100, 100, 75, 125, COLOR_BLUE); // Draw a blue triangle
        // vTaskDelay(pdMS_TO_TICKS(1000));

        display_draw_char(15, 15, 'A', &display_font_5x7, COLOR_BLUE, COLOR_WHITE); // Draw character 'A' in yellow
        vTaskDelay(pdMS_TO_TICKS(1000));

        display_draw_char(21, 15, 'B', &display_font_5x7, COLOR_BLACK, COLOR_WHITE); // Draw character 'B' in yellow
        vTaskDelay(pdMS_TO_TICKS(1000));

        // display_fill(COLOR_WHITE); // Draw a blank bitmap (white screen)
        // vTaskDelay(pdMS_TO_TICKS(1000));

        // display_draw_pixel(20,20,COLOR_RED);
        // vTaskDelay(pdMS_TO_TICKS(1000));

        // display_draw_hline(20,30,100,COLOR_RED);
        // vTaskDelay(pdMS_TO_TICKS(1000));

        // display_fill_rect(20,40,100,50,COLOR_RED);
        // vTaskDelay(pdMS_TO_TICKS(1000));
    }
}