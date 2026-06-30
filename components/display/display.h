#pragma once
#include "esp_err.h"
#include "esp_check.h"

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