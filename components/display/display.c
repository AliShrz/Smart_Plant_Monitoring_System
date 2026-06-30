#include "display.h"

#include "driver/spi_master.h"
#include "esp_lcd_io_spi.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_st7735.h"
#include "driver/gpio.h"
// #include "esp_check.h"

// SPI Host
#define LCD_HOST    SPI2_HOST

// SPI Pins
#define PIN_NUM_MOSI   23
#define PIN_NUM_CLK    18
#define PIN_NUM_CS     5
#define PIN_NUM_DC     27
#define PIN_NUM_RST    26
#define PIN_NUM_BCKL   25

// Display Resolution
#define LCD_H_RES    128
#define LCD_V_RES    160

// LCD Configuration
#define LCD_PIXEL_CLOCK_HZ (10 * 1000 * 1000)
#define LCD_CMD_BITS 8
#define LCD_PARAM_BITS 8
#define LCD_SPI_MODE 0
#define LCD_TRANSACTION_QUEUE_DEPTH 2

static esp_lcd_panel_io_handle_t io_handle = NULL;
static esp_lcd_panel_handle_t panel_handle = NULL;

#define DISPLAY_BUFFER_LINES    20

static uint16_t display_buffer[LCD_H_RES * DISPLAY_BUFFER_LINES];

#define MIN(a, b) ((a) < (b) ? (a) : (b))

static esp_err_t spi_bus_init(void)
{
    spi_bus_config_t bus_config = {
    .sclk_io_num = PIN_NUM_CLK,
    .mosi_io_num = PIN_NUM_MOSI,
    .miso_io_num = -1,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,
    .max_transfer_sz =
    LCD_H_RES *
    DISPLAY_BUFFER_LINES *
    sizeof(uint16_t), // transfer 80 lines of pixels
    };
    return spi_bus_initialize(LCD_HOST, &bus_config, SPI_DMA_CH_AUTO); // Enable the DMA feature;
}

static esp_err_t lcd_io_init(void)
{
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = PIN_NUM_DC,
        .cs_gpio_num = PIN_NUM_CS,
        .pclk_hz = LCD_PIXEL_CLOCK_HZ,
        .lcd_cmd_bits = LCD_CMD_BITS,
        .lcd_param_bits = LCD_PARAM_BITS,
        .spi_mode = LCD_SPI_MODE,
        .trans_queue_depth = LCD_TRANSACTION_QUEUE_DEPTH,
        };
    return esp_lcd_new_panel_io_spi(LCD_HOST, &io_config, &io_handle);

}

static esp_err_t lcd_panel_init(void)
{
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = PIN_NUM_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR,
        .bits_per_pixel = 16,
    };
    return esp_lcd_new_panel_st7735(io_handle, &panel_config, &panel_handle);
}

static esp_err_t backlight_init(void)
{
    gpio_config_t io_config = {
        .pin_bit_mask = (1ULL << PIN_NUM_BCKL),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    return gpio_config(&io_config);
}

static esp_err_t panel_reset(void)
{
    return esp_lcd_panel_reset(panel_handle);
}

static esp_err_t panel_initialize(void)
{
    return esp_lcd_panel_init(panel_handle);
}

static esp_err_t panel_display_on(void)
{
    return esp_lcd_panel_disp_on_off(panel_handle, true);
}

static esp_err_t panel_display_off(void)
{
    return esp_lcd_panel_disp_on_off(panel_handle, false);
}

static esp_err_t backlight_on(void)
{
    return gpio_set_level(PIN_NUM_BCKL, 1);
}

static esp_err_t backlight_off(void)
{
    return gpio_set_level(PIN_NUM_BCKL, 0);
}

esp_err_t display_init(void)
{
    esp_err_t ret;

    ret = spi_bus_init();
    if (ret != ESP_OK) return ret;

    ret = lcd_io_init();
    if (ret != ESP_OK) return ret;

    ret = lcd_panel_init();
    if (ret != ESP_OK) return ret;

    ret = panel_reset();
    if (ret != ESP_OK) return ret;

    ret = panel_initialize();
    if (ret != ESP_OK) return ret;

    ret = backlight_init();
    if (ret != ESP_OK) return ret;

    ret = panel_display_on();
    if (ret != ESP_OK) return ret;

    ret = backlight_on();
    if (ret != ESP_OK) return ret;

    return ESP_OK;
}

static inline uint16_t display_format_color(uint16_t color)
{
    return (color >>8) | (color << 8);
}

esp_err_t display_fill(uint16_t color)
{
    if (panel_handle == NULL)
    {
    return ESP_ERR_INVALID_STATE;
    }

    uint16_t lcd_color = display_format_color(color);
    
    for (size_t i = 0; i < LCD_H_RES * DISPLAY_BUFFER_LINES; i++)
    {
        display_buffer[i] = lcd_color;
    }
    int current_y = 0;

    while (current_y < LCD_V_RES)
    {
        int lines_to_send = MIN(DISPLAY_BUFFER_LINES, LCD_V_RES - current_y);
        esp_err_t ret = esp_lcd_panel_draw_bitmap(panel_handle,0, current_y, LCD_H_RES, current_y + lines_to_send, display_buffer);
        if (ret != ESP_OK) 
        {
            return ret;
        }
        current_y += lines_to_send;
    }

    return ESP_OK;
}