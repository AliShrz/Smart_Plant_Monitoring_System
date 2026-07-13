#pragma once
#include "esp_err.h"
#include "esp_check.h"
#include "display_font.h"

static esp_err_t spi_bus_init(void);

static esp_err_t lcd_io_init(void);

static esp_err_t lcd_panel_init(void);

static esp_err_t backlight_init(void);

static esp_err_t panel_reset(void);

static esp_err_t panel_initialize(void);

static esp_err_t panel_display_on(void);

static esp_err_t panel_display_off(void);

static esp_err_t backlight_on(void);

static esp_err_t backlight_off(void);

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

static esp_err_t display_draw_circle_points(int x_center, int y_center, int x, int y, uint16_t color);

esp_err_t display_draw_circle(int x_center, int y_center, int radius, uint16_t color);

static esp_err_t display_fill_circle_lines(int x_center, int y_center, int x, int y, uint16_t color);

esp_err_t display_fill_circle(int x_center, int y_center, int radius, uint16_t color);

esp_err_t display_draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, uint16_t color);

esp_err_t display_fill_triangle(int x1, int y1, int x2, int y2, int x3, int y3, uint16_t color);

esp_err_t display_draw_char( int x, int y, char c, const display_font_t *font, uint16_t color, uint16_t background_color);
