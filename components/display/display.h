#pragma once
#include "esp_err.h"
#include "esp_check.h"
#include "display_font.h"

typedef enum
{
    DISPLAY_BACKGROUND_SOLID,
    DISPLAY_BACKGROUND_TRANSPARENT
} display_background_mode_t;

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



esp_err_t display_init(void);

esp_err_t display_fill(uint16_t color);

esp_err_t display_draw_pixel(int x, int y, uint16_t color);

esp_err_t display_fill_rect(int x_start, int y_start, int x_end, int y_end, uint16_t color);

esp_err_t display_draw_hline(int x_start, int y, int x_end, uint16_t color);

esp_err_t display_draw_vline(int x, int y_start, int y_end, uint16_t color);

esp_err_t display_draw_rect(int x_start, int y_start, int x_end, int y_end, uint16_t color);

esp_err_t display_draw_line_dda(int x_start, int y_start, int x_end, int y_end, uint16_t color);

esp_err_t display_draw_line_int(int x_start, int y_start, int x_end, int y_end, uint16_t color);

esp_err_t display_draw_line_bresenham(int x_start, int y_start, int x_end, int y_end, uint16_t color);

esp_err_t display_draw_line(int x_start, int y_start, int x_end, int y_end, uint16_t color);

esp_err_t display_draw_circle(int x_center, int y_center, int radius, uint16_t color);

esp_err_t display_fill_circle(int x_center, int y_center, int radius, uint16_t color);

esp_err_t display_draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, uint16_t color);

esp_err_t display_fill_triangle(int x1, int y1, int x2, int y2, int x3, int y3, uint16_t color);

esp_err_t display_draw_char( int x, int y, char c, const display_font_t *font, uint16_t color, uint16_t background_color, display_background_mode_t background_mode);

esp_err_t display_draw_string( int x, int y, const char *text, const display_font_t *font, uint16_t color, uint16_t background_color, display_background_mode_t background_mode);

esp_err_t display_printf( int x, int y, const display_font_t *font, uint16_t color, uint16_t background_color, display_background_mode_t background_mode, const char *format, ...);
