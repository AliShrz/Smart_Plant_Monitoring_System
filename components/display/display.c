#include "display.h"

#include "driver/spi_master.h"
#include "esp_lcd_io_spi.h"
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

static esp_err_t spi_bus_init(void)
{
    spi_bus_config_t bus_config = {
    .sclk_io_num = PIN_NUM_CLK,
    .mosi_io_num = PIN_NUM_MOSI,
    .miso_io_num = -1,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,
    .max_transfer_sz = LCD_H_RES * 80 * sizeof(uint16_t), // transfer 80 lines of pixels
    };
    ESP_ERROR_CHECK(spi_bus_initialize((esp_lcd_spi_bus_handle_t)LCD_HOST, &bus_config, SPI_DMA_CH_AUTO)); // Enable the DMA feature
    return ESP_OK;
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

    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi(LCD_HOST, &io_config, &io_handle)
    );
    return ESP_OK;

}

